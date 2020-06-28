#include "SSD1351.h"

#define depth_262k 1
#define depth_65k 0

#define depth depth_65k
#define buffered

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

void SSD1351::spiWriteData(uint8_t dat)
{
  spiSetData();
  SPI.write(dat);
}

void SSD1351::spiWriteData(uint8_t *dat_p, uint32_t length)
{
  spiSetData();
  SPI.writeBytes(dat_p, length);
}

void SSD1351::initialize(void)
{
  
  fonts.initialize();

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
  *(buffer + (buf_col + buf_row * 128) * 3 + 0) = dat[0];
  *(buffer + (buf_col + buf_row * 128) * 3 + 1) = dat[1];
  *(buffer + (buf_col + buf_row * 128) * 3 + 2) = dat[2];

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

void SSD1351::setColor(uint16_t color65k)
{
  color_65k = color65k;
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

void SSD1351::drawBitmap(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const char *data)
{
  if ((x + w > SSD1351_WIDTH) || (y + h > SSD1351_HEIGHT))
    return;

  defineArea(x, y, w, h);

  for (uint8_t x = 0; x < w; x++)
    for (uint8_t y = 0; y < h; y++)
    {
      uint8_t r = *(data + (y + (99 - x) * 100) * 3 + 2 + 54);
      uint8_t g = *(data + (y + (99 - x) * 100) * 3 + 1 + 54);
      uint8_t b = *(data + (y + (99 - x) * 100) * 3 + 0 + 54);

      setColor(r, g, b);
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

void SSD1351::drawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
  if ((x + w > SSD1351_WIDTH) || (y + h > SSD1351_HEIGHT))
    return;

  defineArea(x, y, w, h);

  for (int i = 0; i < w * h; i++)
    writeColor();
}

/*

// Draw a horizontal line ignoring any screen rotation.
void SSD1351::Draw_FastHLine(int16_t x, int16_t y, int16_t length)
{
  // Bounds check
  if ((x >= SSD1351_WIDTH) || (y >= SSD1351_HEIGHT))
    return;

  // X bounds check
  if (x + length > SSD1351_WIDTH)
  {
    length = SSD1351_WIDTH - x - 1;
  }

  if (length < 0)
    return;

  // set location
  spiWriteCommand(SSD1351_CMD_SETCOLUMN, x, x + length - 1);
  spiWriteCommand(SSD1351_CMD_SETROW, y, y);
  spiWriteCommand(SSD1351_CMD_WRITERAM);
  spiSetData();
  for (uint16_t i = 0; i < length; i++)
    SPI.write16(color_byte);
}

// Draw a vertical line ignoring any screen rotation.
void SSD1351::Draw_FastVLine(int16_t x, int16_t y, int16_t length)
{
  // Bounds check

  uint16_t i;

  if ((x >= SSD1351_WIDTH) || (y >= SSD1351_HEIGHT))
    return;
  // X bounds check
  if (y + length > SSD1351_HEIGHT)
  {
    length = SSD1351_HEIGHT - y - 1;
  }
  if (length < 0)
    return;

  // set location
  spiWriteCommand(SSD1351_CMD_SETCOLUMN, x, x);
  spiWriteCommand(SSD1351_CMD_SETROW, y, y + length - 1);
  spiWriteCommand(SSD1351_CMD_WRITERAM);
  spiSetData();

  for (i = 0; i < length; i++)
    SPI.write16(color_byte);
}






void SSD1351::Draw_Line(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
  // Update in subclasses if desired!
  if (x0 == x1)
  {
    if (y0 > y1)
    {
      uint8_t t = y0;
      y0 = y1;
      y1 = t;
    }
    Draw_FastVLine(x0, y0, y1 - y0 + 1);
  }
  else if (y0 == y1)
  {
    if (x0 > x1)
    {
      {
        uint8_t t = x0;
        x0 = x1;
        x1 = t;
      }
    }
    Draw_FastHLine(x0, y0, x1 - x0 + 1);
  }
  else
    Write_Line(x0, y0, x1, y1);
}

void SSD1351::Write_Line(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{

  int16_t steep = abs(y1 - y0) > abs(x1 - x0);

  if (steep)
  {

    uint8_t t = x0;
    x0 = y0;
    y0 = t;

    t = x1;
    x1 = y1;
    y1 = t;
  }
  if (x0 > x1)
  {
    uint8_t t = x0;
    x0 = x1;
    x1 = t;

    t = y0;
    y0 = y1;
    y1 = t;
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1)
  {
    ystep = 1;
  }
  else
  {
    ystep = -1;
  }

  for (; x0 <= x1; x0++)
  {
    if (steep)
      drawPixel(y0, x0);
    else
      drawPixel(x0, y0);
    err -= dy;

    if (err < 0)
    {
      y0 += ystep;
      err += dx;
    }
  }
}

// Draw a rectangle
void SSD1351::Draw_Rect(int16_t x, int16_t y, int16_t w, int16_t h)
{

  Draw_FastHLine(x, y, w);
  Draw_FastHLine(x, y + h - 1, w);
  Draw_FastVLine(x, y, h);
  Draw_FastVLine(x + w - 1, y, h);
}

void SSD1351::Fill_Rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{

  if ((x >= SSD1351_WIDTH) || (y >= SSD1351_HEIGHT))
    return;

  // Y bounds check
  if (y + h > SSD1351_HEIGHT)
  {
    h = SSD1351_HEIGHT - y - 1;
  }

  // X bounds check
  if (x + w > SSD1351_WIDTH)
  {
    w = SSD1351_WIDTH - x - 1;
  }

  // set location
  spiWriteCommand(SSD1351_CMD_SETCOLUMN, x, x + w - 1);
  spiWriteCommand(SSD1351_CMD_SETROW, y, y + h - 1);
  spiWriteCommand(SSD1351_CMD_WRITERAM);
  spiSetData();

  for (uint16_t i = 0; i < w * h; i++)
  {
    // SPI.write16(color_fill_byte);
  }
}

void SSD1351::Fill_Circle(int16_t x0, int16_t y0, int16_t r)
{

  Draw_FastVLine(x0, y0 - r, 2 * r + 1);
  FillCircle_Helper(x0, y0, r, 3, 0);
}

// Used to do circles and roundrects
void SSD1351::FillCircle_Helper(int16_t x0, int16_t y0, int16_t r, uint8_t corner, int16_t delta)
{

  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  while (x < y)
  {
    if (f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    if (corner & 0x1)
    {
      Draw_FastVLine(x0 + x, y0 - y, 2 * y + 1 + delta);
      Draw_FastVLine(x0 + y, y0 - x, 2 * x + 1 + delta);
    }
    if (corner & 0x2)
    {
      Draw_FastVLine(x0 - x, y0 - y, 2 * y + 1 + delta);
      Draw_FastVLine(x0 - y, y0 - x, 2 * x + 1 + delta);
    }
  }
}

// Draw a circle outline
void SSD1351::Draw_Circle(int16_t x0, int16_t y0, int16_t r)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  drawPixel(x0, y0 + r);
  drawPixel(x0, y0 - r);
  drawPixel(x0 + r, y0);
  drawPixel(x0 - r, y0);

  while (x < y)
  {
    if (f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    drawPixel(x0 + x, y0 + y);
    drawPixel(x0 - x, y0 + y);
    drawPixel(x0 + x, y0 - y);
    drawPixel(x0 - x, y0 - y);
    drawPixel(x0 + y, y0 + x);
    drawPixel(x0 - y, y0 + x);
    drawPixel(x0 + y, y0 - x);
    drawPixel(x0 - y, y0 - x);
  }
}

// Draw a rounded rectangle
void SSD1351::Draw_RoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r)
{
  // smarter version

  Draw_FastHLine(x + r, y, w - 2 * r);         // Top
  Draw_FastHLine(x + r, y + h - 1, w - 2 * r); // Bottom
  Draw_FastVLine(x, y + r, h - 2 * r);         // Left
  Draw_FastVLine(x + w - 1, y + r, h - 2 * r); // Right
  // draw four corners
  DrawCircle_Helper(x + r, y + r, r, 1);
  DrawCircle_Helper(x + w - r - 1, y + r, r, 2);
  DrawCircle_Helper(x + w - r - 1, y + h - r - 1, r, 4);
  DrawCircle_Helper(x + r, y + h - r - 1, r, 8);
}

void SSD1351::DrawCircle_Helper(int16_t x0, int16_t y0, int16_t r, uint8_t corner)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  while (x < y)
  {
    if (f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    if (corner & 0x4)
    {
      drawPixel(x0 + x, y0 + y);
      drawPixel(x0 + y, y0 + x);
    }
    if (corner & 0x2)
    {
      drawPixel(x0 + x, y0 - y);
      drawPixel(x0 + y, y0 - x);
    }
    if (corner & 0x8)
    {
      drawPixel(x0 - y, y0 + x);
      drawPixel(x0 - x, y0 + y);
    }
    if (corner & 0x1)
    {
      drawPixel(x0 - y, y0 - x);
      drawPixel(x0 - x, y0 - y);
    }
  }
}

// Draw a triangle
void SSD1351::Draw_Triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
  Draw_Line(x0, y0, x1, y1);
  Draw_Line(x1, y1, x2, y2);
  Draw_Line(x2, y2, x0, y0);
}

*/