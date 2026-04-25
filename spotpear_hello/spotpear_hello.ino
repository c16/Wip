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
