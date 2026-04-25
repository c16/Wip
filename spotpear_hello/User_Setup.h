// -------------------------------------------------------
// TFT_eSPI User Setup — Spotpear ESP32-C3 1.44" LCD
// ST7735 128x128, SPI
//
// Copy this file into your TFT_eSPI library folder,
// replacing the existing User_Setup.h there.
// -------------------------------------------------------

#define ST7735_DRIVER

// 128x128 with 2-col / 1-row offset (green tab variant)
#define TFT_WIDTH  128
#define TFT_HEIGHT 128
#define ST7735_GREENTAB128

// SPI pins (ESP32-C3)
#define TFT_MOSI  4   // SDA
#define TFT_SCLK  3   // SCL/CLK
#define TFT_CS    2   // Chip select
#define TFT_DC    0   // Data/Command
#define TFT_RST   5   // Reset
#define TFT_BL   -1   // Backlight always on — no GPIO control

// Use VSPI bus
#define USE_HSPI_PORT

// SPI frequency
#define SPI_FREQUENCY       27000000
#define SPI_READ_FREQUENCY   5000000

// Colour byte order for this panel
#define TFT_RGB_ORDER TFT_BGR

// Fonts to include
#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF
#define SMOOTH_FONT
