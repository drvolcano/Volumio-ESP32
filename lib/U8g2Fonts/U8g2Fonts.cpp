/*#################################################################*\

Copyright 2020 Jakob Föger

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

\*#################################################################*/

#include "U8g2Fonts.h"

//#define DEBUG_FONTS

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

uint8_t Fonts::readUnsignedBits(int cnt)
{
  uint8_t b = 0;

  for (int i = 0; i < cnt; i++)
  {
    if (bcnt == 0)
    {
      TEMP = actualfont[buf++];
      bcnt = 8;
    }

    b |= (TEMP & 1) << i;
    TEMP = TEMP >> 1;
    bcnt--;
  }

  return b;
}

int8_t Fonts::readSignedBits(int cnt)
{
  //stolen from u8g2
  //dont know why this works and my attempts not....
  int8_t v, d;
  v = (int8_t)readUnsignedBits(cnt);
  d = 1;
  cnt--;
  d <<= cnt;
  v -= d;

  return v;
}

uint16_t Fonts::findGlyphPosition(char c)
{
  int pos = 23;

  for (int i = 0; i < header.numberGlyphs; i++)
  {
    glyph.unicode = actualfont[pos + 0];
    glyph.jumpOffset = actualfont[pos + 1];

    if (glyph.unicode == c)
    {
      return pos;
    }

    pos += glyph.jumpOffset;
  }

  return -1;
}

void Fonts::readGlyph(char c)
{
  int pos = 23;

  DEBUG_PRINTLN("Looking for: " + String(c) + " " + String((uint16_t)c));

  for (int i = 0; i < header.numberGlyphs; i++)
  {

    char unicode = actualfont[pos + 0];
    uint8_t jumpOffset = actualfont[pos + 1];

    //Start of buffer
    buf = pos + 2;

    if (pos > (header.ArrayOffsetGlyp0x0100 + 23))
    {

      unicode = actualfont[pos + 0] << 8 | actualfont[pos + 1];
      jumpOffset = actualfont[pos + 2];

      //Start of buffer
      buf = pos + 3;
    }

    DEBUG_PRINTLN("Found: " + String(unicode) + " " + String((uint16_t)unicode));

    if (unicode == c)
    {
      bcnt = 0;
      TEMP = 0;

      glyph.unicode = unicode;
      glyph.jumpOffset = jumpOffset;

      glyph.bitmapWidth = readUnsignedBits(header.bitWidhtGlyphBitmapWidth);
      glyph.bitmapHeight = readUnsignedBits(header.bitWidhtGlyphBitmapHeight);
      glyph.bitmapOffsetX = readSignedBits(header.bitWidhtGlyphBitmapOffsetX);
      glyph.bitmapOffsetY = readSignedBits(header.bitWidhtGlyphBitmapOffsetY);
      glyph.characterPitch = readUnsignedBits(header.bitWidhtGlyphCharacterPitch);

      DEBUG_PRINTLN("");
      DEBUG_PRINTLN("offset                                     :" + String(pos));
      DEBUG_PRINTLN("Unicode of character/glyph                 :" + String(glyph.unicode));
      DEBUG_PRINTLN("jump offset to next glyph                  :" + String(glyph.jumpOffset));
      DEBUG_PRINTLN("glyph bitmap width                         :" + String(glyph.bitmapWidth));
      DEBUG_PRINTLN("glyph bitmap height                        :" + String(glyph.bitmapHeight));
      DEBUG_PRINTLN("glyph bitmap x offset                      :" + String(glyph.bitmapOffsetX));
      DEBUG_PRINTLN("glyph bitmap y offset                      :" + String(glyph.bitmapOffsetY));
      DEBUG_PRINTLN("character pitch                            :" + String(glyph.characterPitch));

      int len = 0;
      int rowcnt = 0;

      while (len < glyph.bitmapWidth * glyph.bitmapHeight)
      {
        uint8_t bit0 = readUnsignedBits(header.bitWidhtM0);
        uint8_t bit1 = readUnsignedBits(header.bitWidhtM1);

        do
        {
          for (int c = 0; c < bit0; c++)
          {
            glyph.pixels[len++] = false;

            DEBUG_PRINT("⬛");
            rowcnt++;
            if (rowcnt >= glyph.bitmapWidth)
            {
              rowcnt = 0;
              DEBUG_PRINTLN("");
            }
          }

          for (int c = 0; c < bit1; c++)
          {
            glyph.pixels[len++] = true;
            DEBUG_PRINT("⬜");
            rowcnt++;
            if (rowcnt >= glyph.bitmapWidth)
            {
              rowcnt = 0;
              DEBUG_PRINTLN("");
            }
          }

        } while (readUnsignedBits(1) != 0);
      }

      break;
    }

    pos += jumpOffset;
  }
}

void Fonts::initialize(const uint8_t *font)
{
  actualfont = font;

  //https://github.com/olikraus/u8g2/wiki/u8g2fontformat
  //https://www.mikrocontroller.net/topic/443768
  //https://github.com/olikraus/u8g2/blob/master/csrc/u8g2_font.c

  header.numberGlyphs = actualfont[0];
  header.boundingBoxMode = actualfont[1];
  header.bitWidhtM0 = actualfont[2];
  header.bitWidhtM1 = actualfont[3];
  header.bitWidhtGlyphBitmapWidth = actualfont[4];
  header.bitWidhtGlyphBitmapHeight = actualfont[5];
  header.bitWidhtGlyphBitmapOffsetX = actualfont[6];
  header.bitWidhtGlyphBitmapOffsetY = actualfont[7];
  header.bitWidhtGlyphCharacterPitch = actualfont[8];
  header.FontBoundingBoxWidth = actualfont[9];
  header.FontBoundingBoxHeight = actualfont[10];
  header.FontBoundingBoxOffsetX = (int8_t)actualfont[11];
  header.FontBoundingBoxOffsetY = (int8_t)actualfont[12];
  header.LetterA_Ascent = (int8_t)actualfont[13];
  header.LetterA_Descent = (int8_t)actualfont[14];
  header.LetterBracket_Ascent = (int8_t)actualfont[15];
  header.LetterBracket_Descent = (int8_t)actualfont[16];
  header.ArrayOffsetGlyphUpperA = actualfont[17] << 8 | actualfont[18]; //+23 = pos
  header.ArrayOffsetGlypLowerA = actualfont[19] << 8 | actualfont[20];  //+23 = pos
  header.ArrayOffsetGlyp0x0100 = actualfont[21] << 8 | actualfont[22];  //+23 = pos

  DEBUG_PRINTLN("");
  DEBUG_PRINTLN("Number of glyphs                           :" + String(header.numberGlyphs));
  DEBUG_PRINTLN("Bounding Box Mode                          :" + String(header.boundingBoxMode));
  DEBUG_PRINTLN("m0 Bit width of Zero-Bit RLE in bitmap     :" + String(header.bitWidhtM0));
  DEBUG_PRINTLN("m1 Bit width of One-Bit RLE in bitmap      :" + String(header.bitWidhtM1));
  DEBUG_PRINTLN("Bit width of glyph bitmap width            :" + String(header.bitWidhtGlyphBitmapWidth));
  DEBUG_PRINTLN("Bit width of glyph bitmap height           :" + String(header.bitWidhtGlyphBitmapHeight));
  DEBUG_PRINTLN("Bit width of glyph bitmap x offset         :" + String(header.bitWidhtGlyphBitmapOffsetX));
  DEBUG_PRINTLN("Bit width of glyph bitmap y offset         :" + String(header.bitWidhtGlyphBitmapOffsetY));
  DEBUG_PRINTLN("Bit width of glyph character pitch         :" + String(header.bitWidhtGlyphCharacterPitch));
  DEBUG_PRINTLN("Font Bounding Box width                    :" + String(header.FontBoundingBoxWidth));
  DEBUG_PRINTLN("Font Bounding Box height                   :" + String(header.FontBoundingBoxHeight));
  DEBUG_PRINTLN("Font Bounding Box x Offset                 :" + String(header.FontBoundingBoxOffsetX));
  DEBUG_PRINTLN("Font Bounding Box y Offset                 :" + String(header.FontBoundingBoxOffsetY));
  DEBUG_PRINTLN("Ascent (size above baseline) of letter 'A' :" + String(header.LetterA_Ascent));
  DEBUG_PRINTLN("Descent (size below baseline) of letter 'g':" + String(header.LetterA_Descent));
  DEBUG_PRINTLN("Ascent of '('                              :" + String(header.LetterBracket_Ascent));
  DEBUG_PRINTLN("Descent of '('                             :" + String(header.LetterBracket_Descent));
  DEBUG_PRINTLN("Array offset of glyph 'A'                  :" + String(header.ArrayOffsetGlyphUpperA));
  DEBUG_PRINTLN("Array offset of glyph 'a'                  :" + String(header.ArrayOffsetGlypLowerA));
  DEBUG_PRINTLN("Array offset of glyph 0x0100               :" + String(header.ArrayOffsetGlyp0x0100));

  /*
  int pos = 23;

  for (int i = 0; i < header.numberGlyphs; i++)
  {


    glyph.unicode = actualfont[pos + 0];
    glyph.jumpOffset = actualfont[pos + 1];

    //Start of buffer
    buf = pos + 2;
    bcnt = 0;
    TEMP = 0;

    glyph.bitmapWidth = readBits(header.bitWidhtGlyphBitmapWidth);
    glyph.bitmapHeight = readBits(header.bitWidhtGlyphBitmapHeight);
    glyph.bitmapOffsetX = readBits(header.bitWidhtGlyphBitmapOffsetX);
    glyph.bitmapOffsetY = readBits(header.bitWidhtGlyphBitmapOffsetY);
    glyph.characterPitch = readBits(header.bitWidhtGlyphCharacterPitch);

    DEBUG_PRINTLN("");
    DEBUG_PRINTLN("offset                                     :" + String(pos));
    DEBUG_PRINTLN("Unicode of character/glyph                 :" + String(glyph.unicode));
    DEBUG_PRINTLN("jump offset to next glyph                  :" + String(glyph.jumpOffset));
    DEBUG_PRINTLN("glyph bitmap width                         :" + String(glyph.bitmapWidth));
    DEBUG_PRINTLN("glyph bitmap height                        :" + String(glyph.bitmapHeight));
    DEBUG_PRINTLN("glyph bitmap x offset                      :" + String(glyph.bitmapOffsetX));
    DEBUG_PRINTLN("glyph bitmap y offset                      :" + String(glyph.bitmapOffsetY));
    DEBUG_PRINTLN("character pitch                            :" + String(glyph.characterPitch));

#ifdef DEBUG_FONTS
    int len = 0;
    int rowcnt = 0;

    while (len < glyph.bitmapWidth * glyph.bitmapHeight)
    {
      uint8_t bit0 = readBits(header.bitWidhtM0);
      uint8_t bit1 = readBits(header.bitWidhtM1);

      do
      {

        for (int c = 0; c < bit0; c++)
        {
          Serial.print("⬛");
          rowcnt++;
          if (rowcnt >= glyph.bitmapWidth)
          {
            rowcnt = 0;
            DEBUG_PRINTLN("");
          }
        }

        for (int c = 0; c < bit1; c++)
        {
          Serial.print("⬜");
          rowcnt++;
          if (rowcnt >= glyph.bitmapWidth)
          {
            rowcnt = 0;
            DEBUG_PRINTLN("");
          }
        }

        len += bit0 + bit1;
      } while (readBits(1) != 0);
    }
#endif

    pos += glyph.jumpOffset;
  }*/
}
