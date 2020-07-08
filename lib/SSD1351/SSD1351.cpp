#include "SSD1351.h"

SSD1351::SSD1351(void)
{
}

void SSD1351::spiSetCommand()
{
  digitalWrite(oled_dc, LOW);
}
void SSD1351::spiSetData()
{
  digitalWrite(oled_dc, HIGH);
}

void SSD1351::spiWriteCommand(uint8_t cmd)
{
  spiSetCommand();
  SPI.write(cmd);
}

void SSD1351::spiWriteCommand(uint8_t cmd, uint8_t dat)
{
  spiSetCommand();
  SPI.write(cmd);
  spiSetData();
  SPI.write(dat);
}

void SSD1351::spiWriteCommand(uint8_t cmd, uint8_t dat1, uint8_t dat2)
{
  spiSetCommand();
  SPI.write(cmd);
  spiSetData();
  SPI.write(dat1);
  SPI.write(dat2);
}

void SSD1351::spiWriteCommand(uint8_t cmd, uint8_t dat1, uint8_t dat2, uint8_t dat3)
{
  spiSetCommand();
  SPI.write(cmd);
  spiSetData();
  SPI.write(dat1);
  SPI.write(dat2);
  SPI.write(dat3);
}

void SSD1351::setU8g2Font(const uint8_t *font)
{
  fonts.initialize(font);
}

int SSD1351::getUTF8Width(String text)
{

  // return text.length() * fonts.header.FontBoundingBoxWidth;

  int ox = 0;

  for (int i = 0; i < text.length(); i++)
  {
    uint16_t a = text.charAt(i);

    //Unicode 1
    if ((a & 0xE0) == 0xC0)
    {
      i++;
      uint16_t b = text.charAt(i);

      //Unicode 2
      if ((b & 0xC0) == 0x80)
        a = ((a & 0x1F) << 6) | (b & 0x3F);
    }

    grayscaleFonts.readGlyph(a);
    ox += grayscaleFonts.glyph.characterPitch;
  }

  return ox;
}

void SSD1351::drawUTF8(int x, int y, String text)
{
  /*
  int ox = 0;

  for (int i = 0; i < text.length(); i++)
  {

    fonts.readGlyph(text.charAt(i));

    setColor(255, 255, 255);

    for (int px = 0; px < fonts.glyph.bitmapWidth; px++)
      for (int py = 0; py < fonts.glyph.bitmapHeight; py++)
        if (fonts.glyph.pixels[px + py * fonts.glyph.bitmapWidth])
          drawPixel(
              x + px + fonts.header.FontBoundingBoxOffsetX + fonts.glyph.bitmapOffsetX + ox,
              y + py + fonts.header.FontBoundingBoxOffsetY - fonts.glyph.bitmapOffsetY - fonts.glyph.bitmapHeight);

    ox += fonts.header.FontBoundingBoxWidth;
  }*/

  int ox = 0;

  for (int i = 0; i < text.length(); i++)
  {
    if (x + ox > SSD1351_WIDTH)
      return;

    uint16_t a = text.charAt(i);

    //Unicode 1
    if ((a & 0xE0) == 0xC0)
    {
      i++;
      uint16_t b = text.charAt(i);

      //Unicode 2
      if ((b & 0xC0) == 0x80)
        a = ((a & 0x1F) << 6) | (b & 0x3F);
    }

    grayscaleFonts.readGlyph(a);

    for (int px = 0; px < grayscaleFonts.glyph.bitmapWidth; px++)
      for (int py = 0; py < grayscaleFonts.glyph.bitmapHeight; py++)
      {
        drawPixelAlpha(
            x + px + grayscaleFonts.glyph.bitmapOffsetX + ox,
            y + py - grayscaleFonts.glyph.bitmapOffsetY,
            grayscaleFonts.glyph.data[px + py * grayscaleFonts.glyph.bitmapWidth]);
      }
    ox += grayscaleFonts.glyph.characterPitch;
  }
}

void SSD1351::initialize(void)
{

  //display.begin(UCG_FONT_MODE_TRANSPARENT);
  //Init GPIO
  pinMode(oled_cs, OUTPUT);
  pinMode(oled_rst, OUTPUT);
  pinMode(oled_dc, OUTPUT);
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.begin();

#ifdef buffered
#if depth == depth_262k
  buffer = (uint8_t *)malloc(128 * 128 * 3);
#endif
#if depth == depth_65k
  buffer = (uint8_t *)malloc(128 * 128 * 2);
#endif
#endif

  digitalWrite(oled_cs, LOW);
  digitalWrite(oled_rst, LOW);
  delay(500);
  digitalWrite(oled_rst, HIGH);
  delay(500);

  spiWriteCommand(SSD1351_CMD_COMMANDLOCK, 0x12);                  //unlock driver IC
  spiWriteCommand(SSD1351_CMD_COMMANDLOCK, SSD1351_CMD_PRECHARGE); //unlock precharge command
  spiWriteCommand(SSD1351_CMD_DISPLAYOFF);
  spiWriteCommand(SSD1351_CMD_DISPLAYON);
  spiWriteCommand(SSD1351_CMD_SETCOLUMN, 0, 127);
  spiWriteCommand(SSD1351_CMD_SETROW, 0, 127);
  spiWriteCommand(SSD1351_CMD_CLOCKDIV, 0xF1);
  spiWriteCommand(SSD1351_CMD_MUXRATIO, 0x7F);

#if depth == depth_262k
  spiWriteCommand(SSD1351_CMD_SETREMAP, 0xB4); //-----
#endif

#if depth == depth_65k
  spiWriteCommand(SSD1351_CMD_SETREMAP, 0x74); //-----
#endif

  spiWriteCommand(SSD1351_CMD_STARTLINE, 0x00);
  spiWriteCommand(SSD1351_CMD_DISPLAYOFFSET, 0x00);
  spiWriteCommand(SSD1351_CMD_FUNCTIONSELECT, 0x01); //16bit, 5:6:5
  spiWriteCommand(SSD1351_CMD_SETVSL, 0xA0, 0xB5, 0x55);
  spiWriteCommand(SSD1351_CMD_CONTRASTABC, 0xC8, 0x80, 0xC0);
  spiWriteCommand(SSD1351_CMD_CONTRASTMASTER, 0x0F);
  spiWriteCommand(SSD1351_CMD_PRECHARGE, 0x32);
  spiWriteCommand(SSD1351_CMD_DISPLAYENHANCE, 0xA4, 0x00, 0x00);
  spiWriteCommand(SSD1351_CMD_PRECHARGELEVEL, 0x17);
  spiWriteCommand(SSD1351_CMD_PRECHARGE2, 0x01);
  spiWriteCommand(SSD1351_CMD_VCOMH, 0x05);
  spiWriteCommand(SSD1351_CMD_NORMALDISPLAY);

  clearScreen();
  spiWriteCommand(SSD1351_CMD_DISPLAYON);
}

void SSD1351::defineArea(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
#ifdef buffered
  buf_col_min = x;
  buf_col_max = x + w - 1;
  buf_col = x;

  buf_row_min = y;
  buf_row_max = y + h - 1;
  buf_row = y;

#else
  spiWriteCommand(SSD1351_CMD_SETCOLUMN, x, x + w - 1);
  spiWriteCommand(SSD1351_CMD_SETROW, y, y + h - 1);
  spiWriteCommand(SSD1351_CMD_WRITERAM);
  spiSetData();
#endif
}

void SSD1351::bufferWrite(uint8_t dat[3])
{

  uint8_t *pixel = buffer + (buf_col + buf_row * 128) * 3;
  *(pixel + 0) = dat[0];
  *(pixel + 1) = dat[1];
  *(pixel + 2) = dat[2];

  buf_col++;

  if (buf_col > buf_col_max)
  {
    buf_col = buf_col_min;
    buf_row++;

    if (buf_row > buf_row_max)
    {
      buf_row = buf_row_min;
    }
  }
}

void SSD1351::bufferWrite(uint16_t dat)
{
  *(buffer + (buf_col + buf_row * 128) * 2 + 0) = dat >> 8;
  *(buffer + (buf_col + buf_row * 128) * 2 + 1) = dat;

  buf_col++;

  if (buf_col > buf_col_max)
  {
    buf_col = buf_col_min;
    buf_row++;

    if (buf_row > buf_row_max)
    {
      buf_row = buf_row_min;
    }
  }
}

void SSD1351::bufferRead()
{
#if depth == depth_262k
  uint8_t *pixel = buffer + (buf_col + buf_row * 128) * 3;
  color_buffer_262k[0] = *(pixel + 0);
  color_buffer_262k[1] = *(pixel + 1);
  color_buffer_262k[2] = *(pixel + 2);
#endif
#if depth == depth_65k
  uint8_t *pixel = buffer + (buf_col + buf_row * 128) * 2;
  uint16_t b1 = *(pixel + 0);
  uint16_t b2 = *(pixel + 1);
  color_buffer_65k = b1 << 8 | b2;
#endif
}

void SSD1351::writeBuffer()
{

#if depth == depth_262k
  for (int i = 0; i < SSD1351_WIDTH * SSD1351_HEIGHT; i++)
  {
    Serial.write(*(buffer + i * 3 + 0));
    Serial.write(*(buffer + i * 3 + 1));
    Serial.write(*(buffer + i * 3 + 2));
  }
#endif
#if depth == depth_65k
  for (int i = 0; i < SSD1351_WIDTH * SSD1351_HEIGHT; i++)
  {
    Serial.write(*(buffer + i * 2 + 0));
    Serial.write(*(buffer + i * 2 + 1));
  }
#endif
}

void SSD1351::flush()
{
#ifdef buffered

  spiWriteCommand(SSD1351_CMD_SETCOLUMN, SSD1351_FIRSTCOLUMN, SSD1351_LASTCOLUMN);
  spiWriteCommand(SSD1351_CMD_SETROW, SSD1351_FIRSTROW, SSD1351_LASTROW);
  spiWriteCommand(SSD1351_CMD_WRITERAM);
  spiSetData();

#if depth == depth_262k
  SPI.writeBytes(buffer, 128 * 128 * 3);
#endif

#if depth == depth_65k
  SPI.writeBytes(buffer, 128 * 128 * 2);
#endif

#endif
}

void SSD1351::writeColor()
{
#if depth == depth_262k
#ifdef buffered
  bufferWrite(color_262k);
#else
  SPI.writeBytes(color_262k, 3);
#endif

#endif

#if depth == depth_65k
#ifdef buffered
  bufferWrite(color_65k);
#else
  SPI.write16(color_65k);
#endif
#endif
}

void SSD1351::writeColorAlpha(uint8_t a)
{
#if depth == depth_262k
#ifdef buffered
  bufferRead();

  uint8_t ra = color_262k[0];
  uint8_t ga = color_262k[1];
  uint8_t ba = color_262k[2];

  uint8_t rb = color_buffer_262k[0];
  uint8_t gb = color_buffer_262k[1];
  uint8_t bb = color_buffer_262k[2];

  rb += ((ra - rb) * a >> 8);
  gb += ((ga - gb) * a >> 8);
  bb += ((ba - bb) * a >> 8);

  uint8_t result[3];
  result[0] = rb;
  result[1] = gb;
  result[2] = bb;

  bufferWrite(result);
#else
  SPI.writeBytes(color_262k, 3);
#endif

#endif

#if depth == depth_65k
#ifdef buffered
  bufferRead();

  uint8_t ra = (color_65k >> 11);
  uint8_t ga = ((color_65k >> 5) & 0x3F);
  uint8_t ba = (color_65k & 0x1F);

  uint8_t rb = (color_buffer_65k >> 11);
  uint8_t gb = ((color_buffer_65k >> 5) & 0x3F);
  uint8_t bb = (color_buffer_65k & 0x1F);

  rb += ((ra - rb) * a >> 8) & 0x1F;
  gb += ((ga - gb) * a >> 8) & 0x3F;
  bb += ((ba - bb) * a >> 8) & 0x1F;

  uint16_t result = (rb << 11 | gb << 5 | bb);
  bufferWrite(result);
#else
  SPI.write16(color_65k);
#endif
#endif
}

void SSD1351::setColor(uint16_t color65k)
{
#if depth == depth_65k
  color_65k = color65k;
#endif

#if depth == depth_262k
  color_262k[0] = (color65k >> 11) << 1;
  color_262k[1] = ((color65k >> 5) & 0x3F);
  color_262k[2] = (color65k & 0x1F) << 1;
#endif
}

void SSD1351::setColor(uint8_t r, uint8_t g, uint8_t b)
{
#if depth == depth_65k
  //Convert 3 x 8 bit to 5 + 6 + 5 bit in a row
  //RRRRR----------- (8bit --> 5 bit (-3), shifted 11 left)
  //-----GGGGGG----- (8bit --> 6 bit (-2), shifted 5 left)
  //-----------BBBBB (8bit --> 5 bit (-3), shifted 5 left)

  color_65k = (r >> 3 << 11 | g >> 2 << 5 | b >> 3);
#endif

#if depth == depth_262k
  color_262k[0] = r >> 2;
  color_262k[1] = g >> 2;
  color_262k[2] = b >> 2;
#endif
}

void SSD1351::setFillColor(uint16_t color65k)
{
  color_fill_65k = color65k;
}

void SSD1351::setFillColor(uint8_t r, uint8_t g, uint8_t b)
{
#if depth == depth_65k
  //Convert 3 x 8 bit to 5 + 6 + 5 bit in a row
  //RRRRR----------- (8bit --> 5 bit (-3), shifted 11 left)
  //-----GGGGGG----- (8bit --> 6 bit (-2), shifted 5 left)
  //-----------BBBBB (8bit --> 5 bit (-3), shifted 5 left)

  color_fill_65k = (r >> 3 << 11 | g >> 2 << 5 | b >> 3);
#endif

#if depth == depth_262k
  color_fill_262k[0] = r;
  color_fill_262k[1] = g;
  color_fill_262k[2] = b;
#endif
}

void SSD1351::clearScreen(void)
{

#if depth == depth_262k
  clearScreen(0, 0, 0);
#endif

#if depth == depth_65k
  clearScreen(0);
#endif
}

void SSD1351::clearScreen(uint8_t r, uint8_t g, uint8_t b)
{
  defineArea(SSD1351_FIRSTCOLUMN, SSD1351_FIRSTROW, SSD1351_WIDTH, SSD1351_HEIGHT);
  setColor(r, g, b);

  for (int i = 0; i < SSD1351_PIXELS; i++)
    writeColor();
}

void SSD1351::clearScreen(uint16_t color65k)
{
  defineArea(SSD1351_FIRSTCOLUMN, SSD1351_FIRSTROW, SSD1351_WIDTH, SSD1351_HEIGHT);
  setColor(color65k);

  for (int i = 0; i < SSD1351_PIXELS; i++)
    writeColor();
}

void SSD1351::drawPixel(uint8_t x, uint8_t y)
{
  // Bounds check.
  if ((x > SSD1351_LASTCOLUMN) || (y > SSD1351_LASTROW))
    return;

  defineArea(x, y, 1, 1);
  writeColor();
}

void SSD1351::drawPixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b)
{
  // Bounds check.
  if ((x > SSD1351_LASTCOLUMN) || (y > SSD1351_LASTROW))
    return;

  defineArea(x, y, 1, 1);
  setColor(r, g, b);
  writeColor();
}

void SSD1351::drawPixelAlpha(int x, int y, uint8_t a)
{
  // Bounds check.
  if ((x > SSD1351_LASTCOLUMN) || (y > SSD1351_LASTROW))
    return;

  if ((x < SSD1351_FIRSTCOLUMN) || (y < SSD1351_FIRSTROW))
    return;

  defineArea(x, y, 1, 1);
  writeColorAlpha(a);
}

void SSD1351::drawBitmap24bit(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const char *data)
{
  if ((x + w > SSD1351_WIDTH) || (y + h > SSD1351_HEIGHT))
    return;

  defineArea(x, y, w, h);

  for (uint8_t y = 0; y < h; y++)
    for (uint8_t x = 0; x < w; x++)
    {
      uint8_t r = *(data + (x + (h - 1 - y) * w) * 3 + 2 + 54);
      uint8_t g = *(data + (x + (h - 1 - y) * w) * 3 + 1 + 54);
      uint8_t b = *(data + (x + (h - 1 - y) * w) * 3 + 0 + 54);

      setColor(r, g, b);
      writeColor();
    }
}

void SSD1351::drawBitmap65k(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const char *data)
{
  if ((x + w > SSD1351_WIDTH) || (y + h > SSD1351_HEIGHT))
    return;

  defineArea(x, y, w, h);

  for (uint8_t y = 0; y < h; y++)
    for (uint8_t x = 0; x < w; x++)
    {
      uint8_t a = *(data + (x + y * w) * 2 + 0);
      uint8_t b = *(data + (x + y * w) * 2 + 1);

      setColor(b << 8 | a);
      writeColor();
    }
}

void SSD1351::drawFrame(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
  if ((x + w > SSD1351_WIDTH) || (y + h > SSD1351_HEIGHT))
    return;

  defineArea(x, y, w, 1);

  for (int i = 0; i < w; i++)
    writeColor();

  defineArea(x, y + h - 1, w, 1);

  for (int i = 0; i < w; i++)
    writeColor();

  defineArea(x, y, 1, h);

  for (int i = 0; i < h; i++)
    writeColor();

  defineArea(x + w - 1, y, 1, h);

  for (int i = 0; i < h; i++)
    writeColor();
}

void SSD1351::drawBox(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
  if ((x + w > SSD1351_WIDTH) || (y + h > SSD1351_HEIGHT))
    return;

  defineArea(x, y, w, h);

  for (int i = 0; i < w * h; i++)
    writeColor();
}

void SSD1351::drawRBox(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t r)
{
  if ((x + w > SSD1351_WIDTH) || (y + h > SSD1351_HEIGHT))
    return;

  defineArea(x, y, w, h);

  for (int i = 0; i < w * h; i++)
    writeColor();
}

void SSD1351::drawBoxAlpha(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
  if ((x + w > SSD1351_WIDTH) || (y + h > SSD1351_HEIGHT))
    return;

  defineArea(x, y, w, h);

  for (int i = 0; i < w * h; i++)
    writeColorAlpha(128);
}

void SSD1351::drawRBoxAlpha(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t r)
{
  if ((x + w > SSD1351_WIDTH) || (y + h > SSD1351_HEIGHT))
    return;

  defineArea(x, y, w, h);

  for (int py = 0; py < h; py++)
    for (int px = 0; px < w; px++)
      if (false)
        writeColorAlpha(0);
      else
        writeColorAlpha(128);
}
