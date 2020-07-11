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

class Encoder
{

public:
    void begin(uint8_t PinA, uint8_t PinB, uint8_t Mode); //Initialize with numbers for both pins.
    void process();                                       //cyclic call of internal functions
    void A_CHANGED();                                     //Call this in ISR for Pin A
    void B_CHANGED();                                     //Call this in ISR for Pin B
    void reset();                                         //Reset value to 0
    void setValue(int value);                             //Set value to a given number
    int getValue() { return newValue; }                   //Get actual value
    bool getChanged() { return changed; }                 //Get if value changed since last process

private:
    uint8_t PIN_A; //number of input pin A
    uint8_t PIN_B; //number of input pin B
    bool ACT_A;    //state of pin A when pin B changed
    bool ACT_B;    //state of pin B when pin A changed
    bool OLD_A;    //old state of pin A
    bool OLD_B;    //old state of pin B
    int cnt = 0;   //internal counter value
    int oldValue = 0;
    int newValue; //Actual value of encoder, readonly
    bool changed;
};