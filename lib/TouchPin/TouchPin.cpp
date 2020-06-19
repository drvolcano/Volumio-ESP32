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

void TouchPin::begin(uint8_t pinNumber)
{
  pinNr = pinNumber;
  value = touchRead(pinNr);
  lastChange = millis();
}

void TouchPin::process()
{
  unsigned long now = millis();

  //now should always be higher than lastchange.
  //but after overflow this may occur
  if (now < lastChange)
    lastChange = now;

  value = touchRead(pinNr);

  if (value > valueReleased)
    statusPin = false;

  if (value < valuePressed)
    statusPin = true;

  //Only recognize a pin change after a certain time
  if (statusPin != statusDebounced)
    if (now > lastChange + debounceTime)
    {
      statusDebounced = statusPin;
      lastChange = now;
    }

  //shift states ( not --> old, debounced --> now)
  statusOld = statusNow;
  statusNow = statusDebounced;

  //pressed = rising edge (is now but was not before)
  pressed = statusNow && !statusOld;

  //released = falling edge (is not now but was before)
  released = !statusNow && statusOld;
}
