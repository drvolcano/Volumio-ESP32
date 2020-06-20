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
#include "define_text.h"

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
int MenuOffset = 0;

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

bool restore = false;

MenuStruct Menu[200];
int MenuPosition = 0;

StackStruct Stack[10];
int stack_pos = 0;

void GenMenuItem(String Icon, String Text, MenuItemType Type, String Data = "")
{
  Menu[MenuLength].Icon = Icon;
  Menu[MenuLength].Text = Text;
  Menu[MenuLength].Type = Type;
  Menu[MenuLength].Data = Data;
  MenuLength++;
}

void GenMenuStart()
{
  MenuLength = 0;
}

void GenMenuEnd()
{
  // LeftEncoder.reset();
  MenuOffset = 0;
}

void MenuMain()
{
  DEBUG_PRINTLN("Main: MenuMain()");

  stack_pos = 0;

  GenMenuStart();

  GenMenuItem(ICON_PLAY, TEXT_MENU_STATUS, MENU_STATUS);
  GenMenuItem(ICON_CLOCK, TEXT_MENU_PLAYBACK, MENU_PLAYBACK);
  GenMenuItem(ICON_MUSIC, TEXT_MENU_BROWSE, MENU_BROWSE);
  GenMenuItem(ICON_LIST, TEXT_MENU_QUEUE, MENU_QUEUE);
  GenMenuItem(ICON_STOP, TEXT_MENU_DARK, MENU_DARK);

  GenMenuEnd();
}

void MenuPlayback()
{
  DEBUG_PRINTLN("Main: MenuPlayback()");

  GenMenuStart();
  GenMenuItem(ICON_HOME, TEXT_HOME, MENU_HOME);
  GenMenuItem(ICON_PREV, TEXT_PLAYBACK_PREV, MENU_PLAYBACK_PREV);
  GenMenuItem(ICON_NEXT, TEXT_PLAYBACK_NEXT, MENU_PLAYBACK_NEXT);
  GenMenuItem(ICON_PLAY, TEXT_PLAYBACK_PLAY, MENU_PLAYBACK_PLAY);
  GenMenuItem(ICON_PAUSE, TEXT_PLAYBACK_PAUSE, MENU_PLAYBACK_PAUSE);
  // GenMenuItem(ICON_PAUSE, TEXT_PLAYBACK_TOGGLE, MENU_PLAYBACK_TOGGLE);
  GenMenuItem(ICON_STOP, TEXT_PLAYBACK_STOP, MENU_PLAYBACK_STOP);
  GenMenuItem(ICON_RANDOM, TEXT_PLAYBACK_RANDOM, MENU_PLAYBACK_RANDOM_SET_RESET);
  GenMenuItem(ICON_REPEAT, TEXT_PLAYBACK_REPEAT, MENU_PLAYBACK_REPEAT_SET_RESET);
  GenMenuEnd();
}

void ContextMenuSong(String data)
{
  DEBUG_PRINTLN("Main: ContextMenuSong()");

  GenMenuStart();
  GenMenuItem(ICON_HOME, TEXT_HOME, MENU_HOME);
  GenMenuItem(ICON_BACK, Stack[stack_pos - 2].Text, MENU_BACK);
  GenMenuItem(ICON_PLAY, TEXT_BROWSE_SONG_PLAY, MENU_BROWSE_SONG_PLAY, data);
  GenMenuItem(ICON_LIST, TEXT_BROWSE_SONG_ADDTOQUEUE, MENU_BROWSE_SONG_ADDTOQUEUE, data);
  GenMenuItem(ICON_LIST, TEXT_BROWSE_SONG_CLEARANDPLAY, MENU_BROWSE_SONG_CLEARANDPLAY, data);
  // GenMenuItem(ICON_PLUS, TEXT_BROWSE_SONG_ADDTOPLAYLIST, MENU_BROWSE_SONG_ADDTOPLAYLIST, data);
  // GenMenuItem(ICON_HEART, TEXT_BROWSE_SONG_ADDTOFAVORITES, MENU_BROWSE_SONG_ADDTOFAVORITES, data);
  GenMenuEnd();
}

void ContextMenuWebradio(String data)
{
  DEBUG_PRINTLN("Main: ContextMenuWebradio()");

  GenMenuStart();
  GenMenuItem(ICON_HOME, TEXT_HOME, MENU_HOME);
  GenMenuItem(ICON_BACK, Stack[stack_pos - 2].Text, MENU_BACK);
  GenMenuItem(ICON_PLAY, TEXT_BROWSE_WEBRADIO_PLAY, MENU_BROWSE_WEBRADIO_PLAY, data);
  GenMenuItem(ICON_LIST, TEXT_BROWSE_WEBRADIO_ADDTOQUEUE, MENU_BROWSE_WEBRADIO_ADDTOQUEUE, data);
  GenMenuItem(ICON_LIST, TEXT_BROWSE_WEBRADIO_CLEARANDPLAY, MENU_BROWSE_WEBRADIO_CLEARANDPLAY, data);
  // GenMenuItem(ICON_PLUS, TEXT_BROWSE_WEBRADIO_ADDTOPLAYLIST, MENU_BROWSE_WEBRADIO_ADDTOPLAYLIST, data);
  // GenMenuItem(ICON_HEART, TEXT_BROWSE_WEBRADIO_ADDTOFAVORITES, MENU_BROWSE_WEBRADIO_ADDTOFAVORITES, data);
  GenMenuEnd();
}

void ContextMenuQueue(int index)
{
  DEBUG_PRINTLN("Main: ContextMenuQueue()");

  GenMenuStart();
  GenMenuItem(ICON_HOME, TEXT_HOME, MENU_HOME);
  GenMenuItem(ICON_BACK, Stack[stack_pos - 2].Text, MENU_BACK);
  GenMenuItem(ICON_PLAY, TEXT_QUEUE_TRACK_PLAY, MENU_QUEUE_TRACK_PLAY, String(index));
  GenMenuItem(ICON_CLEAR, TEXT_QUEUE_TRACK_DELETE, MENU_QUEUE_TRACK_DELETE, String(index));

  GenMenuEnd();
}

void MenuSystem()
{
  DEBUG_PRINTLN("Main: MenuSystem()");
}

void MenuPush()
{
  DEBUG_PRINTLN("Main: MenuPush()");

  Stack[stack_pos].Text = Menu[MenuPosition].Text;
  Stack[stack_pos].Type = Menu[MenuPosition].Type;
  Stack[stack_pos].Data = Menu[MenuPosition].Data;
  Stack[stack_pos].Position = MenuPosition;
  Stack[stack_pos].Offset = MenuOffset;

  stack_pos++;
}

void MenuPop()
{
  DEBUG_PRINTLN("Main: MenuPop()");

  if (stack_pos > 0)
    stack_pos--;

  // Menu[menu_pos] = Stack[stack_pos];
}
