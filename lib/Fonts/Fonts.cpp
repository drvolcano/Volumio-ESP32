#include "Fonts.h"

#define DEBUG_FONTS

#ifdef DEBUG_FONTS
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINT(x) Serial.print(x)
#else
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT(x)
#endif

int bcnt = 0;
byte TEMP = 0;
int buf = 0;

int Fonts::readBits(int cnt)
{
  int b = 0;

  for (int i = 0; i < cnt; i++)
  {
    if (bcnt == 0)
    {
      TEMP = u8g2_font_t0_12_tf_2[buf++];
      bcnt = 8;
    }

    b += (TEMP & 1) << i;
    TEMP = TEMP >> 1;
    bcnt--;
  }

  return b;
}

void Fonts::initialize()
{
  //https://github.com/olikraus/u8g2/wiki/u8g2fontformat
  //https://www.mikrocontroller.net/topic/443768
  //https://github.com/olikraus/u8g2/blob/master/csrc/u8g2_font.c
  uint8_t var0 = u8g2_font_t0_12_tf_2[0];
  uint8_t var1 = u8g2_font_t0_12_tf_2[1];
  uint8_t var2 = u8g2_font_t0_12_tf_2[2];
  uint8_t var3 = u8g2_font_t0_12_tf_2[3];
  uint8_t var4 = u8g2_font_t0_12_tf_2[4];
  uint8_t var5 = u8g2_font_t0_12_tf_2[5];
  uint8_t var6 = u8g2_font_t0_12_tf_2[6];
  uint8_t var7 = u8g2_font_t0_12_tf_2[7];
  uint8_t var8 = u8g2_font_t0_12_tf_2[8];
  uint8_t var9 = u8g2_font_t0_12_tf_2[9];
  uint8_t var10 = u8g2_font_t0_12_tf_2[10];
  uint8_t var11 = u8g2_font_t0_12_tf_2[11];
  uint8_t var12 = u8g2_font_t0_12_tf_2[12];
  uint8_t var13 = u8g2_font_t0_12_tf_2[13];
  uint8_t var14 = u8g2_font_t0_12_tf_2[14];
  uint8_t var15 = u8g2_font_t0_12_tf_2[15];
  uint8_t var16 = u8g2_font_t0_12_tf_2[16];
  uint16_t var17 = u8g2_font_t0_12_tf_2[17] << 8 | u8g2_font_t0_12_tf_2[18]; //+23 = pos
  uint16_t var19 = u8g2_font_t0_12_tf_2[19] << 8 | u8g2_font_t0_12_tf_2[20]; //+23 = pos
  uint16_t var21 = u8g2_font_t0_12_tf_2[21] << 8 | u8g2_font_t0_12_tf_2[22]; //+23 = pos

  DEBUG_PRINTLN("");
  DEBUG_PRINTLN("Number of glyphs                           :" + String(var0));
  DEBUG_PRINTLN("Bounding Box Mode                          :" + String(var1));
  DEBUG_PRINTLN("m0 Bit width of Zero-Bit RLE in bitmap     :" + String(var2));
  DEBUG_PRINTLN("m1 Bit width of One-Bit RLE in bitmap      :" + String(var3));
  DEBUG_PRINTLN("Bit width of glyph bitmap width            :" + String(var4));
  DEBUG_PRINTLN("Bit width of glyph bitmap height           :" + String(var5));
  DEBUG_PRINTLN("Bit width of glyph bitmap x offset         :" + String(var6));
  DEBUG_PRINTLN("Bit width of glyph bitmap y offset         :" + String(var7));
  DEBUG_PRINTLN("Bit width of glyph character pitch         :" + String(var8));
  DEBUG_PRINTLN("Font Bounding Box width                    :" + String(var9));
  DEBUG_PRINTLN("Font Bounding Box height                   :" + String(var10));
  DEBUG_PRINTLN("Font Bounding Box x Offset                 :" + String(var11));
  DEBUG_PRINTLN("Font Bounding Box y Offset                 :" + String(var12));
  DEBUG_PRINTLN("Ascent (size above baseline) of letter 'A' :" + String(var13));
  DEBUG_PRINTLN("Descent (size below baseline) of letter 'g':" + String(var14));
  DEBUG_PRINTLN("Ascent of '('                              :" + String(var15));
  DEBUG_PRINTLN("Descent of '('                             :" + String(var16));
  DEBUG_PRINTLN("Array offset of glyph 'A'                  :" + String(var17));
  DEBUG_PRINTLN("Array offset of glyph 'a'                  :" + String(var19));
  DEBUG_PRINTLN("Array offset of glyph 0x0100               :" + String(var21));

  int pos = 23;

  for (int i = 0; i < var0; i++)
  {

    uint8_t v0 = u8g2_font_t0_12_tf_2[pos + 0];
    uint8_t v1 = u8g2_font_t0_12_tf_2[pos + 1];

    //Start of buffer
    buf = pos + 2;
    bcnt = 0;
    TEMP = 0;

    uint8_t v2 = readBits(var4);
    uint8_t v3 = readBits(var5);
    uint8_t v4 = readBits(var6);
    uint8_t v5 = readBits(var7);
    uint8_t v6 = readBits(var8);

    DEBUG_PRINTLN("");
    DEBUG_PRINTLN("offset                                     :" + String(pos));
    DEBUG_PRINTLN("Unicode of character/glyph                 :" + String(v0));
    DEBUG_PRINTLN("jump offset to next glyph                  :" + String(v1));
    DEBUG_PRINTLN("glyph bitmap width                         :" + String(v2));
    DEBUG_PRINTLN("glyph bitmap height                        :" + String(v3));
    DEBUG_PRINTLN("glyph bitmap x offset                      :" + String(v4));
    DEBUG_PRINTLN("glyph bitmap y offset                      :" + String(v5));
    DEBUG_PRINTLN("character pitch                            :" + String(v6));

#ifdef DEBUG_FONTS
    int len = 0;
    int rowcnt = 0;

    while (len < v2 * v3)
    {
      uint8_t bit0 = readBits(var2);
      uint8_t bit1 = readBits(var3);

      do
      {

        for (int c = 0; c < bit0; c++)
        {
          Serial.print("⬛");
          rowcnt++;
          if (rowcnt >= v2)
          {
            rowcnt = 0;
            DEBUG_PRINTLN("");
          }
        }

        for (int c = 0; c < bit1; c++)
        {
          Serial.print("⬜");
          rowcnt++;
          if (rowcnt >= v2)
          {
            rowcnt = 0;
            DEBUG_PRINTLN("");
          }
        }

        len += bit0 + bit1;
      } while (readBits(1) != 0);
    }
#endif

    pos += v1;
  }
}
