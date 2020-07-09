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

#include "Encoder.h"


void Encoder::begin(uint8_t PinA, uint8_t PinB, uint8_t Mode)
{
    PIN_A = PinA;
    PIN_B = PinB;
    pinMode(PIN_A, Mode);
    pinMode(PIN_B, Mode);
    newValue = 0;
    oldValue = 0;
}

void Encoder::A_CHANGED()
{
    //debouncing method: memorize pin B when pin A changes
    ACT_B = digitalRead(PIN_B) == LOW;

    if (ACT_B ^ OLD_B)
    {
        if (ACT_A ^ OLD_B)
            cnt--;
        else
            cnt++;

        OLD_B = ACT_B;
    }
}

void Encoder::B_CHANGED()
{
    //debouncing method: memorize pin A when pin B changes
    ACT_A = digitalRead(PIN_A) == LOW;

    if (ACT_A ^ OLD_A)
    {
        if (ACT_B ^ OLD_A)
            cnt++;
        else
            cnt--;

        OLD_A = ACT_A;
    }
}

void Encoder::reset()
{
    setValue(0);
}

void Encoder::setValue(int value)
{
    //Internal value may have more digits than official value (e.g. factor 2)
    cnt = value << 2;
    newValue = cnt >> 2;
}

void Encoder::process()
{
    newValue = (cnt + 2) >> 2;
    changed = newValue != oldValue;
    oldValue = newValue;
}