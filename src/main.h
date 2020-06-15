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

#define STX (char)0x02
#define ETX (char)0x03

//###########################################################
// Hardware
//###########################################################

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

//###########################################################
// Volume
//###########################################################

bool volumeprogress = false;
bool volumedisplay = false;
int startvolume = 0;
int startcnt = 0;

int newvolume = 0;
long laststop = 0;
long lastsetvolume = 0;
long lastinput =0;

bool StatusDisplay = true;
bool ToastDisplay= false;
long ToastStart = 0;
bool NoDisplay= false;

int lastscroll = 0;
int scrollbuffer= 0;
int lastscreenshot =0;

u8g2_uint_t line1_offset; // current offset for the scrolling text
u8g2_uint_t line2_offset; // current offset for the scrolling text

u8g2_uint_t line1_width;
u8g2_uint_t line2_width;

bool WaitForSourceUpdate = false;
bool WaitForLibraryUpdate = false;
bool WaitForQueueUpdate = false;
