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

//#define Color

#ifdef Color
#include <Ucglib.h>
#include "cover.h"
#include "SSD1351.h"

SSD1351 oled = SSD1351();

#define oled_cs   5
#define oled_rst  255
#define oled_dc   21
#define oled_sck  4
#define oled_din  23


#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#else
#include <U8g2lib.h> //enable #define U8G2_16BIT in "u8g2.h"
#endif

#include "TouchPin.h"
#include "DigitalPin.h"
#include "Volumio.h"
#include "Encoder.h"
#include "config.h"
#include "define_icons.h"
#include "define_logo.h"
#include "menu.h"
#include "UiElements.h"
#include "Locale/Locale.h"
#include "ScrollText.h"
#include "TextSplitter.h"


//#include "Language.h"

/*#################################################################*\
|* COM
\*#################################################################*/

#define STX (char)0x02
#define ETX (char)0x03

/*#################################################################*\
|* Hardware
\*#################################################################*/

/*
TouchPin touchPin0;
TouchPin touchPin1;
TouchPin touchPin2;
TouchPin touchPin3;
TouchPin touchPin4;
TouchPin touchPin5;
TouchPin touchPin6;
TouchPin touchPin7;
TouchPin touchPin8;
TouchPin touchPin9;
*/

Encoder leftEncoder;
Encoder rightEncoder;

DigitalPin leftSwitch;
DigitalPin rightSwitch;

Volumio volumio;
UiElements ui;
ScrollText scroll1;
ScrollText scroll2;
ScrollText scroll3;
TextSplitter splitter;

/*#################################################################*\
|* Volume
\*#################################################################*/

int startvolume = 0;
int startcnt = 0;
int newvolume = 0;
long laststop = 0;
long lastsetvolume = 0;
long lastsendvolume = 0;
long lastinput = 0;
bool waitvolumechange = false;

/*#################################################################*\
|* Display
\*#################################################################*/

bool volumeDisplay = false;
bool statusDisplay = true;
bool toastDisplay = false;
long toastStart = 0;
bool noDisplay = false;

long lastscreenshot = 0;
long lastmenuchange = 0;

/*#################################################################*\
|* Menu
\*#################################################################*/

bool waitForSourceUpdate = false;
bool waitForLibraryUpdate = false;
bool waitForQueueUpdate = false;
bool waitForMenuItemsUpdate = false;
bool waitForUiConfigSection = false;