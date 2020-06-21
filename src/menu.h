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
#include "define_menu.h"
#include "define_icons.h"
#include "Locale/Locale.h"

#include "AppDebug.h"
#ifdef DEBUG_MENU
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINT(x) Serial.print(x)
#else
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT(x)
#endif

int messageindex = 0;
String messagebuffer[4];

int MenuLength = 0;
int menuOffset = 0;

struct MenuStruct
{
  String Icon;
  String Text;
  String Data;
  MenuItemType Type;
};

struct StackStruct
{
  String Text;
  String Data;
  MenuItemType Type;
  int Position;
  int Offset;
};

bool restoreMenuPosition = false;

MenuStruct Menu[200];
int menuPosition = 0;

StackStruct menuStack[10];
int menuStackIndex = 0;

void genMenuItem(String Icon, String Text, MenuItemType Type, String Data = "")
{
  Menu[MenuLength].Icon = Icon;
  Menu[MenuLength].Text = Text;
  Menu[MenuLength].Type = Type;
  Menu[MenuLength].Data = Data;
  MenuLength++;
}

void genMenuStart()
{
  MenuLength = 0;
}

void GenMenuEnd()
{
  // LeftEncoder.reset();
  menuOffset = 0;
}

void menuMain()
{
  DEBUG_PRINTLN("Main: MenuMain()");

  menuStackIndex = 0;

  genMenuStart();

  genMenuItem(ICON_PLAY, locale.COMMON.TAB_PLAYBACK, MENU_STATUS);
  genMenuItem(ICON_CLOCK, locale.ESP.Control, MENU_PLAYBACK);
  genMenuItem(ICON_MUSIC, locale.COMMON.TAB_BROWSE, MENU_BROWSE);
  genMenuItem(ICON_LIST, locale.COMMON.TAB_QUEUE, MENU_QUEUE);
  genMenuItem(ICON_STOP, locale.ESP.DisplayOff, MENU_DARK);

  GenMenuEnd();
}

void menuPlayback()
{
  DEBUG_PRINTLN("Main: MenuPlayback()");

  genMenuStart();
  genMenuItem(ICON_HOME, locale.COMMON.HOME, MENU_HOME);
  genMenuItem(ICON_PREV, locale.BROWSER.PREVIOUS_TRACK, MENU_PLAYBACK_PREV);
  genMenuItem(ICON_NEXT, locale.BROWSER.NEXT_TRACK, MENU_PLAYBACK_NEXT);
  genMenuItem(ICON_PLAY, locale.BROWSER.PLAY, MENU_PLAYBACK_PLAY);
  genMenuItem(ICON_PAUSE, locale.BROWSER.PAUSE, MENU_PLAYBACK_PAUSE);
  genMenuItem(ICON_STOP, locale.COMMON.STOP_MUSIC, MENU_PLAYBACK_STOP);
  genMenuItem(ICON_RANDOM, locale.TRACK_INFO_BAR.RANDOM, MENU_PLAYBACK_RANDOM_SET_RESET);
  genMenuItem(ICON_REPEAT, locale.TRACK_INFO_BAR.REPEAT, MENU_PLAYBACK_REPEAT_SET_RESET);
  GenMenuEnd();
}

void contextMenuSong(String data)
{
  DEBUG_PRINTLN("Main: ContextMenuSong()");

  genMenuStart();
  genMenuItem(ICON_HOME, locale.COMMON.HOME, MENU_HOME);
  genMenuItem(ICON_BACK, menuStack[menuStackIndex - 2].Text, MENU_BACK);
  genMenuItem(ICON_PLAY, locale.BROWSER.PLAY, MENU_BROWSE_SONG_PLAY, data);
  genMenuItem(ICON_LIST, locale.BROWSER.ADD_TO_QUEUE, MENU_BROWSE_SONG_ADDTOQUEUE, data);
  genMenuItem(ICON_LIST, locale.BROWSER.CLEAR_AND_PLAY, MENU_BROWSE_SONG_CLEARANDPLAY, data);
  // GenMenuItem(ICON_PLUS, TEXT_BROWSE_SONG_ADDTOPLAYLIST, MENU_BROWSE_SONG_ADDTOPLAYLIST, data);
  // GenMenuItem(ICON_HEART, TEXT_BROWSE_SONG_ADDTOFAVORITES, MENU_BROWSE_SONG_ADDTOFAVORITES, data);
  GenMenuEnd();
}

void contextMenuWebradio(String data)
{
  DEBUG_PRINTLN("Main: ContextMenuWebradio()");

  genMenuStart();
  genMenuItem(ICON_HOME, locale.COMMON.HOME, MENU_HOME);
  genMenuItem(ICON_BACK, menuStack[menuStackIndex - 2].Text, MENU_BACK);
  genMenuItem(ICON_PLAY, locale.BROWSER.PLAY, MENU_BROWSE_WEBRADIO_PLAY, data);
  genMenuItem(ICON_LIST, locale.BROWSER.ADD_TO_QUEUE, MENU_BROWSE_WEBRADIO_ADDTOQUEUE, data);
  genMenuItem(ICON_LIST, locale.BROWSER.CLEAR_AND_PLAY, MENU_BROWSE_WEBRADIO_CLEARANDPLAY, data);
  // GenMenuItem(ICON_PLUS, TEXT_BROWSE_WEBRADIO_ADDTOPLAYLIST, MENU_BROWSE_WEBRADIO_ADDTOPLAYLIST, data);
  // GenMenuItem(ICON_HEART, TEXT_BROWSE_WEBRADIO_ADDTOFAVORITES, MENU_BROWSE_WEBRADIO_ADDTOFAVORITES, data);
  GenMenuEnd();
}

void contextMenuQueue(int index)
{
  DEBUG_PRINTLN("Main: ContextMenuQueue()");

  genMenuStart();
  genMenuItem(ICON_HOME, locale.COMMON.HOME, MENU_HOME);
  genMenuItem(ICON_BACK, menuStack[menuStackIndex - 2].Text, MENU_BACK);
  genMenuItem(ICON_PLAY, locale.BROWSER.PLAY, MENU_QUEUE_TRACK_PLAY, String(index));
  genMenuItem(ICON_CLEAR, locale.COMMON.DELETE, MENU_QUEUE_TRACK_DELETE, String(index));

  GenMenuEnd();
}

void menuSystem()
{
  DEBUG_PRINTLN("Main: MenuSystem()");
}

void menuPush()
{

  menuStack[menuStackIndex].Text = Menu[menuPosition].Text;
  menuStack[menuStackIndex].Type = Menu[menuPosition].Type;
  menuStack[menuStackIndex].Data = Menu[menuPosition].Data;
  menuStack[menuStackIndex].Position = menuPosition;
  menuStack[menuStackIndex].Offset = menuOffset;

  DEBUG_PRINT("Main: MenuPush(): Text: ");
  DEBUG_PRINTLN(menuStack[menuStackIndex].Text);
  DEBUG_PRINT("Main: MenuPush(): Type: ");
  DEBUG_PRINTLN(menuStack[menuStackIndex].Type);
  DEBUG_PRINT("Main: MenuPush(): Data: ");
  DEBUG_PRINTLN(menuStack[menuStackIndex].Data);
  DEBUG_PRINT("Main: MenuPush(): Position: ");
  DEBUG_PRINTLN(menuStack[menuStackIndex].Position);
  DEBUG_PRINT("Main: MenuPush(): Offset:   ");
  DEBUG_PRINTLN(menuStack[menuStackIndex].Offset);

  DEBUG_PRINT("Main: MenuPush(): ");
  DEBUG_PRINT(menuStackIndex);
  menuStackIndex++;
  DEBUG_PRINT(" --> ");
  DEBUG_PRINT(menuStackIndex);
  DEBUG_PRINTLN(" ");
}

void menuPop()
{
  DEBUG_PRINT("Main: MenuPop(): ");
  DEBUG_PRINT(menuStackIndex);

  if (menuStackIndex > 0)
    menuStackIndex--;

  menuPosition = menuStack[menuStackIndex].Position;
  menuOffset = menuStack[menuStackIndex].Offset;
  Menu[menuPosition].Text = menuStack[menuStackIndex].Text;
  Menu[menuPosition].Type = menuStack[menuStackIndex].Type;
  Menu[menuPosition].Data = menuStack[menuStackIndex].Data;

  DEBUG_PRINT(" --> ");
  DEBUG_PRINTLN(menuStackIndex);

  DEBUG_PRINT("Main: MenuPop(): Text: ");
  DEBUG_PRINTLN(menuStack[menuStackIndex].Text);
  DEBUG_PRINT("Main: MenuPop(): Type: ");
  DEBUG_PRINTLN(menuStack[menuStackIndex].Type);
  DEBUG_PRINT("Main: MenuPop(): Data: ");
  DEBUG_PRINTLN(menuStack[menuStackIndex].Data);
  DEBUG_PRINT("Main: MenuPop(): Position: ");
  DEBUG_PRINTLN(menuStack[menuStackIndex].Position);
  DEBUG_PRINT("Main: MenuPop(): Offset:   ");
  DEBUG_PRINTLN(menuStack[menuStackIndex].Offset);

  // Menu[menu_pos] = Stack[stack_pos];
}
