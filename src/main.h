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

#include <U8g2lib.h> //enable #define U8G2_16BIT in "u8g2.h"
#include "TouchPin.h"
#include "DigitalPin.h"
#include "Volumio.h"
#include "Encoder.h"
#include "config.h"
#include "define_icons.h"
#include "define_text.h"
#include "define_logo.h"
#include "menu.h"

/*#################################################################*\
|* COM
\*#################################################################*/

#define STX (char)0x02
#define ETX (char)0x03

/*#################################################################*\
|* Hardware
\*#################################################################*/

/*
TouchPin TouchPin0;
TouchPin TouchPin1;
TouchPin TouchPin2;
TouchPin TouchPin3;
TouchPin TouchPin4;
TouchPin TouchPin5;
TouchPin TouchPin6;
TouchPin TouchPin7;
TouchPin TouchPin8;
TouchPin TouchPin9;
*/

Encoder LeftEncoder;
Encoder RightEncoder;

DigitalPin LeftSwitch;
DigitalPin RightSwitch;

Volumio volumio;

/*#################################################################*\
|* Volume
\*#################################################################*/

int startvolume = 0;
int startcnt = 0;

int newvolume = 0;
long laststop = 0;
long lastsetvolume = 0;
long lastsendvolume = 0;
long lastinput =0;

bool waitvolumechange = false;

/*#################################################################*\
|* Display
\*#################################################################*/

bool volumedisplay = false;
bool StatusDisplay = true;
bool ToastDisplay= false;
long ToastStart = 0;
bool NoDisplay= false;

long lastscroll = 0;
int scrollbuffer= 0;
long lastscreenshot =0;
long lastmenuchange =0;


u8g2_uint_t line1_offset; // current offset for the scrolling text
u8g2_uint_t line2_offset; // current offset for the scrolling text
u8g2_uint_t item_offset; // current offset for the scrolling text

u8g2_uint_t line1_width;
u8g2_uint_t line2_width;
u8g2_uint_t item_width;

/*#################################################################*\
|* Menu
\*#################################################################*/

bool WaitForSourceUpdate = false;
bool WaitForLibraryUpdate = false;
bool WaitForQueueUpdate = false;