#include "stdlib.h"
#include "SPI.h"
#include "Arduino.h"
#include "stdlib.h"
#include "Fonts.h"
#include "GrayscaleFonts.h"

//Bit depth of display
#define depth_display_65k 65 //5:6:5 (faster, only 2 bytes per pixel)
#define depth_display_262k 262 //6:6:6 (3 bytes per pixel)
#define depth_display depth_display_65k

//Bit depth of buffer
#define depth_buffer_same 0 //same as display (no conversion necessary)
#define depth_buffer_24bit 24 //8:8:8 (experimental)
#define depth_buffer depth_buffer_same

#define buffered

#define SSD1351_WIDTH 128
#define SSD1351_HEIGHT 128
#define SSD1351_FIRSTCOLUMN 0
#define SSD1351_LASTCOLUMN 127
#define SSD1351_FIRSTROW 0
#define SSD1351_LASTROW 127
#define SSD1351_PIXELS 16384

#define SSD1351_CMD_SETCOLUMN 0x15
#define SSD1351_CMD_SETROW 0x75
#define SSD1351_CMD_WRITERAM 0x5C
#define SSD1351_CMD_READRAM 0x5D

#define SSD1351_CMD_SETREMAP 0xA0

/*
#define Horizontal_address_increment 0
#define Vertical_address_increment 1
#define Column_address_0_SEG0 0 
#define Column_address_127_SEG0 2
#define Color_Sequence_ABC 0 
#define Color_Sequence_BCA 4
#define Scan_0_N 0 
#define Scan_N_0 8
*/
#define SSD1351_CMD_STARTLINE 0xA1
#define SSD1351_CMD_DISPLAYOFFSET 0xA2
#define SSD1351_CMD_DISPLAYALLOFF 0xA4
#define SSD1351_CMD_DISPLAYALLON 0xA5
#define SSD1351_CMD_NORMALDISPLAY 0xA6
#define SSD1351_CMD_INVERTDISPLAY 0xA7
#define SSD1351_CMD_FUNCTIONSELECT 0xAB
#define SSD1351_CMD_DISPLAYOFF 0xAE
#define SSD1351_CMD_DISPLAYON 0xAF
#define SSD1351_CMD_PRECHARGE 0xB1
#define SSD1351_CMD_DISPLAYENHANCE 0xB2
#define SSD1351_CMD_CLOCKDIV 0xB3
#define SSD1351_CMD_SETVSL 0xB4
#define SSD1351_CMD_SETGPIO 0xB5
#define SSD1351_CMD_PRECHARGE2 0xB6
#define SSD1351_CMD_SETGRAY 0xB8
#define SSD1351_CMD_USELUT 0xB9
#define SSD1351_CMD_PRECHARGELEVEL 0xBB
#define SSD1351_CMD_VCOMH 0xBE
#define SSD1351_CMD_CONTRASTABC 0xC1
#define SSD1351_CMD_CONTRASTMASTER 0xC7
#define SSD1351_CMD_MUXRATIO 0xCA
#define SSD1351_CMD_COMMANDLOCK 0xFD
#define SSD1351_CMD_HORIZSCROLL 0x96
#define SSD1351_CMD_STOPSCROLL 0x9E
#define SSD1351_CMD_STARTSCROLL 0x9F

/*
//Display-pins
#define PIN_SPI_MOSI 23 //VSPID    DIN - 3
#define PIN_SPI_CLK 18  //VSPICLK  CLK - 4
#define PIN_SPI_CS 5    //VSPICS0  CS  - 5
#define PIN_SPI_DC 21   //         DC  - 6
*/
/*
#define oled_cs   10
#define oled_rst  8
#define oled_dc   7
#define oled_sck  13
#define oled_din  11
*/
#define oled_cs 5
#define oled_rst 255
#define oled_dc 21
#define oled_sck 4
#define oled_din 23

class SSD1351
{

public:
  SSD1351(void);

  void initialize();
  void clearScreen();
  void clearScreen(uint16_t color65k);
  void clearScreen(uint8_t r, uint8_t g, uint8_t b);
  void setColor(uint16_t color65k);
  void setColor(uint8_t r, uint8_t g, uint8_t b);


  void drawPixel(uint8_t x, uint8_t y);
  void drawPixel(uint8_t x, uint8_t y, uint16_t color);
  void drawPixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b);

  void drawPixelAlpha(int x, int y, uint8_t a);

  void drawUTF8(int x, int y, String text);
  int getUTF8Width(String text);
  void setU8g2Font(const uint8_t *font);

  void drawBitmap24bit(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const char *data);
  void drawBitmap65k(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const char *data);

  void drawFrame(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
  void drawBox(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
  void drawRBox(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t r);

  void drawBoxAlpha(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
  void drawRBoxAlpha(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t r);

  void defineArea(uint8_t x, uint8_t y, uint8_t w, uint8_t h);

  void flush();
  void writeBuffer();

  void writeColor();
  void writeColorAlpha(uint8_t a);

  /*
  void Draw_FastHLine(int16_t x, int16_t y, int16_t length);
  void Draw_FastVLine(int16_t x, int16_t y, int16_t length);

  void Draw_Line(int16_t x0, int16_t y0, int16_t x1, int16_t y1);

  void Draw_Rect(int16_t x, int16_t y, int16_t w, int16_t h);
  void Fill_Rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h);

  void Fill_Circle(int16_t x0, int16_t y0, int16_t r);
  void Draw_Circle(int16_t x0, int16_t y0, int16_t r);

  void Draw_RoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r);
  void Draw_Triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2);

  void print_String(uint8_t x, uint8_t y, const uint8_t *text);
  void Display_String_5x8(uint8_t x, uint8_t y, const uint8_t *text);
  void Display_String_8x16(uint8_t x, uint8_t y, const uint8_t *text);
*/

private:
  uint8_t *buffer24bit;
  uint8_t *buffer;
  uint8_t buf_row = 0;
  uint8_t buf_col = 0;
  uint8_t buf_row_min = 0;
  uint8_t buf_row_max = 0;
  uint8_t buf_col_min = 0;
  uint8_t buf_col_max = 0;

  void spiSetCommand();
  void spiSetData();
  void bufferWrite(uint8_t dat[3]);
  void bufferWrite(uint16_t dat);
  void bufferRead();

  uint16_t color_65k;
  uint8_t color_262k[3];
  uint8_t color_24bit[3];

  uint16_t color_buffer_65k;
  uint8_t color_buffer_262k[3];
  uint8_t color_buffer_24bit[3];

  Fonts fonts;
  GrayscaleFonts grayscaleFonts;

  void Write_Line(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
  void FillCircle_Helper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta);
  void DrawCircle_Helper(int16_t x0, int16_t y0, int16_t r, uint8_t corner);

  void spiWriteCommand(uint8_t cmd);
  void spiWriteCommand(uint8_t cmd, uint8_t dat);
  void spiWriteCommand(uint8_t cmd, uint8_t dat1, uint8_t dat2);
  void spiWriteCommand(uint8_t cmd, uint8_t dat1, uint8_t dat2, uint8_t dat3);
};
