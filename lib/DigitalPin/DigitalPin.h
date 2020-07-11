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

#pragma once
#include "Arduino.h"
#include "LibDebug.h"

class DigitalPin
{
public:
  bool getPressed() { return pressed; }        //pressed = rising edge (is now but was not before)
  bool getReleased() { return released; }      //released = falling edge (is not now but was before)
  void begin(uint8_t pinNumber, uint8_t Mode); //initialize with pin number.
  void process();                              //cyclic call of internal functions
  int debounceTime = 50;                       //debounce time in milliseconds

private:
  uint8_t pinNr;                //number of input pin
  unsigned long lastChange;     //last time the debounced state changed
  bool statusPin = false;       //actual state of pin (read from input)
  bool statusDebounced = false; //debounced state of pin (used debounce-delay)
  bool statusNow = false;       //actual state after debouncing
  bool statusOld = false;       //state of last processing
  bool pressed = false;         //pressed = rising edge (is now but was not before)
  bool released = false;        //released = falling edge (is not now but was before)
};