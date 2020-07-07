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

#include "Arduino.h"
#include "u8g2_font_t0_12_tf.c"



class Fonts
{
public:
    void initialize(const uint8_t * font);
    void readGlyph(char c);

    struct FontGlyph
    {
        uint16_t unicode;
        uint8_t jumpOffset;
        uint8_t bitmapWidth;
        uint8_t bitmapHeight;
        int8_t bitmapOffsetX;
        int8_t bitmapOffsetY;
        uint8_t characterPitch;
        bool pixels[100];
    };

    struct FontHeader
    {
        uint8_t numberGlyphs;
        uint8_t boundingBoxMode;
        uint8_t bitWidhtM0;
        uint8_t bitWidhtM1;
        uint8_t bitWidhtGlyphBitmapWidth;
        uint8_t bitWidhtGlyphBitmapHeight;
        uint8_t bitWidhtGlyphBitmapOffsetX;
        uint8_t bitWidhtGlyphBitmapOffsetY;
        uint8_t bitWidhtGlyphCharacterPitch;
        uint8_t FontBoundingBoxWidth;
        uint8_t FontBoundingBoxHeight;
        int8_t FontBoundingBoxOffsetX;
        int8_t FontBoundingBoxOffsetY;
        int8_t LetterA_Ascent;
        int8_t LetterA_Descent;
        int8_t LetterBracket_Ascent;
        int8_t LetterBracket_Descent;
        uint16_t ArrayOffsetGlyphUpperA;
        uint16_t ArrayOffsetGlypLowerA;
        uint16_t ArrayOffsetGlyp0x0100;
    };

    FontHeader header;
    FontGlyph glyph;

private:
    const uint8_t * actualfont;

    uint8_t readUnsignedBits(int cnt);
    int8_t readSignedBits(int cnt);

    uint16_t findGlyphPosition(char c);
};