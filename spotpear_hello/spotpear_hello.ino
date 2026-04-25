// Spotpear ESP32-C3 1.44" LCD — Hello World
// Display: ST7735 128x128 via TFT_eSPI
// Serial baud: 115200

#include <SPI.h>
#include <TFT_eSPI.h>

// Pin definitions (matching User_Setup.h / board schematic)
// CLK  → GPIO 3
// MOSI → GPIO 4
// CS   → GPIO 2
// DC   → GPIO 0
// RST  → GPIO 5
// BL   → always on (no GPIO)

// Button pins
#define BTN1  8
#define BTN2  10
#define BOOT  9

TFT_eSPI tft = TFT_eSPI();

// -------------------------------------------------------
// Flux Capacitor — geometry (128x128 screen)
// Three arms meet at a central junction in a Y shape.
// -------------------------------------------------------
#define FC_CX  64    // junction centre
#define FC_CY  60
#define FC_TX  64    // top arm tip
#define FC_TY  14
#define FC_LX  18    // bottom-left tip
#define FC_LY 104
#define FC_RX 110    // bottom-right tip
#define FC_RY 104

static uint8_t  fcPhase = 0;   // 0-59 animation frame counter
static uint32_t fcLast  = 0;

// Palette for cycling background demo
static const uint16_t COLORS[] = {
    TFT_RED, TFT_GREEN, TFT_BLUE,
    TFT_CYAN, TFT_MAGENTA, TFT_YELLOW
};
static const uint8_t NUM_COLORS = sizeof(COLORS) / sizeof(COLORS[0]);

// -------------------------------------------------------
static void drawHello(uint16_t bg, uint16_t fg)
{
    tft.fillScreen(bg);

    // Centered title
    tft.setTextColor(fg, bg);
    tft.setTextDatum(MC_DATUM);          // middle-centre
    tft.setTextSize(2);
    tft.drawString("Hello", 64, 45);
    tft.drawString("World!", 64, 70);

    // Small subtitle
    tft.setTextSize(1);
    tft.drawString("Spotpear ESP32-C3", 64, 100);
    tft.drawString("TFT_eSPI ready", 64, 112);
}

// -------------------------------------------------------
// Flux Capacitor helpers
// -------------------------------------------------------

// Jagged lightning bolt between two points.
// seed drives pseudo-random jitter so each call looks different.
static void fcDrawBolt(int16_t x0, int16_t y0,
                       int16_t x1, int16_t y1,
                       uint16_t col, uint8_t seed)
{
    const int STEPS = 7;
    int16_t px = x0, py = y0;
    for (int i = 1; i < STEPS; i++) {
        int16_t nx = x0 + ((x1 - x0) * i) / STEPS
                        + (int8_t)((seed * 13 + i * 37) % 11) - 5;
        int16_t ny = y0 + ((y1 - y0) * i) / STEPS
                        + (int8_t)((seed *  7 + i * 19) % 9 ) - 4;
        tft.drawLine(px, py, nx, ny, col);
        px = nx; py = ny;
    }
    tft.drawLine(px, py, x1, y1, col);
}

// 3-pixel-wide tube (draw three offset lines)
static void fcDrawTube(int16_t x0, int16_t y0,
                       int16_t x1, int16_t y1, uint16_t col)
{
    tft.drawLine(x0,   y0,   x1,   y1,   col);
    tft.drawLine(x0+1, y0,   x1+1, y1,   col);
    tft.drawLine(x0,   y0+1, x1,   y1+1, col);
}

// Render one animation frame.  ph is 0-59.
static void fcDrawFrame(uint8_t ph)
{
    const float TWO_PI = 6.28318f;
    const float fph    = (float)ph / 60.0f;   // normalised 0..1 per cycle

    // Background & housing
    tft.fillScreen(TFT_BLACK);
    tft.drawRoundRect(4,  4, 120, 120, 5, tft.color565(60, 60, 70));
    tft.drawRoundRect(5,  5, 118, 118, 4, tft.color565(30, 30, 40));

    // Labels
    tft.setTextSize(1);
    tft.setTextColor(tft.color565(200, 160, 0), TFT_BLACK);
    tft.setTextDatum(TC_DATUM);
    tft.drawString("FLUX", 64, 7);
    tft.setTextDatum(BC_DATUM);
    tft.drawString("CAPACITOR", 64, 121);

    // Tube brightness pulses with the phase
    uint8_t glow    = (uint8_t)(110.0f + 110.0f * sinf(fph * TWO_PI));
    uint16_t tubeBase = tft.color565(glow / 6, glow / 6, glow);
    uint16_t tubeHot  = tft.color565(glow / 3, glow / 3, 255);

    // Three arms
    fcDrawTube(FC_CX, FC_CY, FC_TX, FC_TY, tubeBase);
    fcDrawTube(FC_CX, FC_CY, FC_LX, FC_LY, tubeBase);
    fcDrawTube(FC_CX, FC_CY, FC_RX, FC_RY, tubeBase);

    // Travelling energy blips — 3 blips per cycle, moving outward
    float t = fmodf(fph * 3.0f, 1.0f);

    int16_t bx = FC_CX + (int16_t)((FC_TX - FC_CX) * t);
    int16_t by = FC_CY + (int16_t)((FC_TY - FC_CY) * t);
    tft.fillCircle(bx, by, 3, tubeHot);

    bx = FC_CX + (int16_t)((FC_LX - FC_CX) * t);
    by = FC_CY + (int16_t)((FC_LY - FC_CY) * t);
    tft.fillCircle(bx, by, 3, tubeHot);

    bx = FC_CX + (int16_t)((FC_RX - FC_CX) * t);
    by = FC_CY + (int16_t)((FC_RY - FC_CY) * t);
    tft.fillCircle(bx, by, 3, tubeHot);

    // Tip glow caps — strobe every 3 frames
    uint8_t  tipBr  = (ph % 6 < 3) ? 255 : 150;
    uint16_t tipCol = tft.color565(tipBr / 2, tipBr / 2, tipBr);
    tft.fillCircle(FC_TX, FC_TY, 5, tipCol);
    tft.fillCircle(FC_LX, FC_LY, 5, tipCol);
    tft.fillCircle(FC_RX, FC_RY, 5, tipCol);
    // Inner white hot spot
    tft.fillCircle(FC_TX, FC_TY, 2, TFT_WHITE);
    tft.fillCircle(FC_LX, FC_LY, 2, TFT_WHITE);
    tft.fillCircle(FC_RX, FC_RY, 2, TFT_WHITE);

    // Lightning arcs between tips — active ~40% of frames
    if (ph % 12 < 5) {
        uint16_t boltCol = tft.color565(210, 210, 255);
        uint8_t  seed    = ph * 17;
        fcDrawBolt(FC_TX, FC_TY, FC_LX, FC_LY, boltCol, seed);
        fcDrawBolt(FC_LX, FC_LY, FC_RX, FC_RY, boltCol, seed + 23);
        fcDrawBolt(FC_RX, FC_RY, FC_TX, FC_TY, boltCol, seed + 47);
    }

    // Central junction orb — double-speed pulse
    uint8_t  cBr    = (uint8_t)(155.0f + 100.0f * sinf(fph * TWO_PI * 2.0f));
    tft.fillCircle(FC_CX, FC_CY, 8, tft.color565(cBr / 3, cBr / 3, cBr));
    tft.fillCircle(FC_CX, FC_CY, 5, tft.color565(cBr / 2, cBr / 2, 255));
    tft.fillCircle(FC_CX, FC_CY, 2, TFT_WHITE);
}

// -------------------------------------------------------
void setup()
{
    Serial.begin(115200);
    while (!Serial && millis() < 2000) {}   // wait up to 2 s for USB CDC

    Serial.println();
    Serial.println("=== Spotpear ESP32-C3 Hello World ===");
    Serial.printf("Chip: %s  Rev %d\n",
                  ESP.getChipModel(), ESP.getChipRevision());
    Serial.printf("Flash: %u MB  PSRAM: %u B\n",
                  ESP.getFlashChipSize() / (1024 * 1024),
                  ESP.getPsramSize());
    Serial.println("Initialising display…");

    pinMode(BTN1, INPUT_PULLUP);
    pinMode(BTN2, INPUT_PULLUP);
    pinMode(BOOT, INPUT_PULLUP);

    // Pre-initialise FSPI so the spi_t struct (and its mutex) exist before
    // TFT_eSPI calls spiStartBus() → spiDetachBus() internally.
    // Without this, spiDetachBus crashes on an uninitialised struct on
    // arduino-esp32 3.x.  Has no effect on 2.0.17 where it is not needed.
    SPI.begin(3, -1, 4, 2);  // SCLK, MISO, MOSI, CS
    Serial.println("SPI pre-init done");

    tft.init();
    tft.setRotation(2);   // 180° — correct orientation for this board
    tft.fillScreen(TFT_BLACK);

    Serial.println("Display OK");
    Serial.printf("Width: %d  Height: %d\n", tft.width(), tft.height());

    drawHello(TFT_BLACK, TFT_WHITE);
    Serial.println("Hello World drawn — entering loop");
}

// -------------------------------------------------------
void loop()
{
    static uint32_t lastCycle  = 0;
    static uint32_t loopCount  = 0;
    static uint8_t  colorIndex = 0;

    loopCount++;

    // Cycle background colour every 3 seconds
    if (millis() - lastCycle >= 3000) {
        lastCycle = millis();
        colorIndex = (colorIndex + 1) % NUM_COLORS;

        uint16_t bg = COLORS[colorIndex];
        // Pick contrasting text colour
        uint16_t fg = (colorIndex < 3) ? TFT_WHITE : TFT_BLACK;

        drawHello(bg, fg);

        Serial.printf("[%lu ms] Colour cycle %d — loop iterations: %lu\n",
                      millis(), colorIndex, loopCount);
    }

    // Log button presses
    if (!digitalRead(BTN1)) {
        Serial.printf("[%lu ms] Button 1 pressed\n", millis());
        delay(200);  // debounce
    }
    if (!digitalRead(BTN2)) {
        Serial.printf("[%lu ms] Button 2 pressed\n", millis());
        delay(200);
    }

    delay(10);
}

// -------------------------------------------------------
// Alternative loop — shows the animated Flux Capacitor.
// To activate: rename this to loop() and rename the
// existing loop() above to loopHello() (or any other name).
// -------------------------------------------------------
void loopFluxCapacitor()
{
    if (millis() - fcLast >= 80) {   // ~12 fps
        fcLast = millis();
        fcDrawFrame(fcPhase);
        Serial.printf("[%lu ms] FC frame %d\n", millis(), fcPhase);
        fcPhase = (fcPhase + 1) % 60;
    }
}
