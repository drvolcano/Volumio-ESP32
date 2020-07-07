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

#include "GrayscaleFonts.h"

void GrayscaleFonts::readGlyph(uint16_t c)
{
    for (int i = 0; i < 200; i++)

        if (c == Lato_Regular_8_bold[i].c)
        {
            glyph = Lato_Regular_8_bold[i];
            break;
        }
}