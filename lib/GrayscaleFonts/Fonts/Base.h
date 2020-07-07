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

struct GrayscaleFontChar
{
    uint16_t c;
    uint8_t characterPitch;
    uint8_t bitmapWidth;
    uint8_t bitmapHeight;
    int8_t bitmapOffsetX;
    int8_t bitmapOffsetY;
    uint8_t *data;
};
