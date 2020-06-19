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

#include "TouchPin.h"

void TouchPin::begin(int pin)
{
  pinnr = pin;
  value = touchRead(pinnr);
  lastchange = millis();
}

void TouchPin::process()
{
  unsigned long now = millis();

  //now should always be higher than lastchange.
  //but after overflow this may occur
  if (now < lastchange)
    lastchange = now;

  value = touchRead(pinnr);

  if (value > valuehigh)
    statusPin = false;

  if (value < valuelow)
    statusPin = true;

 //Only recognize a pin change after a certain time
  if (statusPin != statusDeb)
    if (now > lastchange + debounce)
    {
      statusDeb = statusPin;
      lastchange = now;
    }

 //shift states ( not --> old, debounced --> now)
  statusOld = statusNow;
  statusNow = statusDeb;

  //pressed = rising edge (is now but was not before)
  pressed = statusNow && !statusOld;

  //released = falling edge (is not now but was before)
  released = !statusNow && statusOld;
}
