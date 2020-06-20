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

#include "main.h"

#include "AppDebug.h"
#ifdef DEBUG_MAIN
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINT(x) Serial.print(x)
#else
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT(x)
#endif

void DisplayMessage(String Message)
{
  display.firstPage();
  display.setFont(MenuTextFont);
  display.setFontDirection(0);

  //Logo and single line (true) or 4 lines scrolling (false)
  if (true)
  {
    do
    {
      //Draw logo in upper half of display centered
      display.drawXBM((DisplayWidth - logo_volumio_big_width) / 2, (DisplayHeight / 2 - logo_volumio_big_height) / 2, logo_volumio_big_width, logo_volumio_big_height, logo_volumio_big_bits);

      //Draw message text
      int w = display.getUTF8Width(Message.c_str());
      display.drawUTF8((DisplayWidth - w) / 2, 3 * MenuItemHeight + MenuItemHeight - (MenuItemHeight - MenuTextHeight) / 2, Message.c_str());

    } while (display.nextPage());
  }
  else
  {
    int msgcnt = 4;

    messagebuffer[messageindex] = Message;

    do
    {
      //Draw message texts
      for (int i = 0; i < msgcnt; i++)
        display.drawUTF8(0, i * MenuItemHeight + MenuItemHeight - (MenuItemHeight - MenuTextHeight) / 2, messagebuffer[(messageindex - i + msgcnt) % msgcnt].c_str());

    } while (display.nextPage());

    messageindex = (messageindex + 1) % msgcnt;
  }
}

String WiFiStatusString()
{
  String text = "";

  switch (WiFi.status())
  {
  case WL_CONNECTED:
    text = TEXT_WL_CONNECTED;
    break;
  case WL_NO_SHIELD:
    text = TEXT_WL_NO_SHIELD;
    break;
  case WL_IDLE_STATUS:
    text = TEXT_WL_IDLE_STATUS;
    break;
  case WL_NO_SSID_AVAIL:
    text = TEXT_WL_NO_SSID_AVAIL;
    break;
  case WL_SCAN_COMPLETED:
    text = TEXT_WL_SCAN_COMPLETED;
    break;
  case WL_CONNECT_FAILED:
    text = TEXT_WL_CONNECT_FAILED;
    break;
  case WL_CONNECTION_LOST:
    text = TEXT_WL_CONNECTION_LOST;
    break;
  case WL_DISCONNECTED:
    text = TEXT_WL_DISCONNECTED;
    break;
  default:
    text = "???";
  }

  return text;
}

//Interrupt: left encoder Pin A changed
void IRAM_ATTR ISR_LeftEncoder_A()
{
  LeftEncoder.A_CHANGED();
}

//Interrupt: left encoder Pin B changed
void IRAM_ATTR ISR_LeftEncoder_B()
{
  LeftEncoder.B_CHANGED();
}

//Interrupt: right encoder Pin A changed
void IRAM_ATTR ISR_RightEncoder_A()
{
  RightEncoder.A_CHANGED();
}

//Interrupt: right encoder Pin B changed
void IRAM_ATTR ISR_RightEncoder_B()
{
  RightEncoder.B_CHANGED();
}

void menuAction(MenuItemType type, String data)
{
  DEBUG_PRINT("Main: menuAction(): ");

  waitForSourceUpdate = false;
  WaitForQueueUpdate = false;
  waitForLibraryUpdate = false;
  restore = false;

  switch (type)
  {
  case MENU_HOME:
    DEBUG_PRINT("MENU_HOME");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    restore = true;
    menuMain();
    break;
  case MENU_BACK:
    DEBUG_PRINT("MENU_BACK");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    //Jump 2 menus up and then one down
    //Same effect as jumping one up, but minimizes stack size because only action, not full menu can be stored
    menuPop();
    menuPop();
    restore = true;
    menuAction(menuStack[menuStackIndex].Type, menuStack[menuStackIndex].Data);
    break;
  case MENU_STATUS:
    DEBUG_PRINT("MENU_STATUS");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    statusDisplay = true;
    break;
  case MENU_BROWSE:
    DEBUG_PRINT("MENU_BROWSE");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    menuPush();
    genMenuStart();
    genMenuItem(ICON_HOME, TEXT_HOME, MENU_HOME);
    waitForSourceUpdate = true;
    volumio.getBrowseSources();
    break;
  case MENU_BROWSE_SOURCE:
    DEBUG_PRINT("MENU_BROWSE_SOURCE");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    menuPush();
    genMenuStart();
    genMenuItem(ICON_HOME, TEXT_HOME, MENU_HOME);
    genMenuItem(ICON_BACK, menuStack[menuStackIndex - 2].Text, MENU_BACK);
    waitForLibraryUpdate = true;
    volumio.browseLibrary(data);
    break;
  case MENU_BROWSE_PLAYLIST:
    DEBUG_PRINT("MENU_BROWSE_PLAYLIST");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    menuPush();
    genMenuStart();
    genMenuItem(ICON_HOME, TEXT_HOME, MENU_HOME);
    genMenuItem(ICON_BACK, menuStack[menuStackIndex - 2].Text, MENU_BACK);
    waitForLibraryUpdate = true;
    volumio.browseLibrary(data);
    break;
  case MENU_BROWSE_FOLDER:
    DEBUG_PRINT("MENU_BROWSE_FOLDER");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    menuPush();
    genMenuStart();
    genMenuItem(ICON_HOME, TEXT_HOME, MENU_HOME);
    genMenuItem(ICON_BACK, menuStack[menuStackIndex - 2].Text, MENU_BACK);
    waitForLibraryUpdate = true;
    volumio.browseLibrary(data);
    break;
  case MENU_BROWSE_SONG:
    DEBUG_PRINT("MENU_BROWSE_SONG");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    menuPush();
    contextMenuSong(data);
    break;
  case MENU_BROWSE_RADIOCATEGORY:
    DEBUG_PRINT("MENU_BROWSE_RADIOCATEGORY");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    menuPush();
    genMenuStart();
    genMenuItem(ICON_HOME, TEXT_HOME, MENU_HOME);
    genMenuItem(ICON_BACK, menuStack[menuStackIndex - 2].Text, MENU_BACK);
    waitForLibraryUpdate = true;
    volumio.browseLibrary(data);
    break;
  case MENU_BROWSE_WEBRADIO:
    DEBUG_PRINT("MENU_BROWSE_WEBRADIO");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    menuPush();
    contextMenuWebradio(data);
    break;
  case MENU_BROWSE_STREAMINGCATEGORY:
    DEBUG_PRINT("MENU_BROWSE_STREAMINGCATEGORY");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    menuPush();
    genMenuStart();
    genMenuItem(ICON_HOME, TEXT_HOME, MENU_HOME);
    genMenuItem(ICON_BACK, menuStack[menuStackIndex - 2].Text, MENU_BACK);
    waitForLibraryUpdate = true;
    volumio.browseLibrary(data);
    break;
  case MENU_BROWSE_FOLDER_PLAY:
    DEBUG_PRINT("MENU_BROWSE_FOLDER_PLAY");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    volumio.addPlay(data);
    break;
  case MENU_BROWSE_FOLDER_ADDTOQUEUE:
    DEBUG_PRINT("MENU_BROWSE_FOLDER_ADDTOQUEUE");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    volumio.addToQueue(data);
    break;
  case MENU_BROWSE_FOLDER_CLEARANDPLAY:
    DEBUG_PRINT("MENU_BROWSE_FOLDER_CLEARANDPLAY");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    volumio.replaceAndPlay(data);
    break;
  case MENU_BROWSE_FOLDER_ADDTOPLAYLIST:
    DEBUG_PRINT("MENU_BROWSE_FOLDER_ADDTOPLAYLIST");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    break;
  case MENU_BROWSE_FOLDER_ADDTOFAVORITES:
    DEBUG_PRINT("MENU_BROWSE_FOLDER_ADDTOFAVORITES");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    break;
  case MENU_BROWSE_SONG_PLAY:
    DEBUG_PRINT("MENU_BROWSE_SONG_PLAY");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    volumio.addPlay(data);
    break;
  case MENU_BROWSE_SONG_ADDTOQUEUE:
    DEBUG_PRINT("MENU_BROWSE_SONG_ADDTOQUEUE");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    volumio.addToQueue(data);
    break;
  case MENU_BROWSE_SONG_CLEARANDPLAY:
    DEBUG_PRINT("MENU_BROWSE_SONG_CLEARANDPLAY");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    volumio.replaceAndPlay(data);
    break;
  case MENU_BROWSE_SONG_ADDTOPLAYLIST:
    DEBUG_PRINT("MENU_BROWSE_SONG_ADDTOPLAYLIST");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    break;
  case MENU_BROWSE_SONG_ADDTOFAVORITES:
    DEBUG_PRINT("MENU_BROWSE_SONG_ADDTOFAVORITES");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    break;
  case MENU_BROWSE_WEBRADIO_PLAY:
    DEBUG_PRINT("MENU_BROWSE_WEBRADIO_PLAY");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    volumio.addPlay("webradio", menuStack[menuStackIndex - 1].Text, menuStack[menuStackIndex - 1].Data);
    break;
  case MENU_BROWSE_WEBRADIO_ADDTOQUEUE:
    DEBUG_PRINT("MENU_BROWSE_WEBRADIO_ADDTOQUEUE");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    volumio.addToQueue(data);
    break;
  case MENU_BROWSE_WEBRADIO_CLEARANDPLAY:
    DEBUG_PRINT("MENU_BROWSE_WEBRADIO_CLEARANDPLAY");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    volumio.replaceAndPlay(data);
    break;
  case MENU_BROWSE_WEBRADIO_ADDTOPLAYLIST:
    DEBUG_PRINT("MENU_BROWSE_WEBRADIO_ADDTOPLAYLIST");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    break;
  case MENU_BROWSE_WEBRADIO_ADDTOFAVORITES:
    DEBUG_PRINT("MENU_BROWSE_WEBRADIO_ADDTOFAVORITES");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    break;
  case MENU_PLAYBACK:
    DEBUG_PRINT("MENU_PLAYBACK");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    menuPush();
    menuPlayback();
    break;
  case MENU_PLAYBACK_PREV:
    DEBUG_PRINT("MENU_PLAYBACK_PREV");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    volumio.prev();
    break;
  case MENU_PLAYBACK_NEXT:
    DEBUG_PRINT("MENU_PLAYBACK_NEXT");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    volumio.next();
    break;
  case MENU_PLAYBACK_PLAY:
    DEBUG_PRINT("MENU_PLAYBACK_PLAY");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    volumio.play();
    break;
  case MENU_PLAYBACK_PAUSE:
    DEBUG_PRINT("MENU_PLAYBACK_PAUSE");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    volumio.pause();
    break;
  case MENU_PLAYBACK_TOGGLE:
    DEBUG_PRINT("MENU_PLAYBACK_TOGGLE");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    volumio.toggle();
    break;
  case MENU_PLAYBACK_STOP:
    DEBUG_PRINT("MENU_PLAYBACK_STOP");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    volumio.stop();
    break;
  case MENU_PLAYBACK_RANDOM_SET:
    DEBUG_PRINT("MENU_PLAYBACK_RANDOM_SET");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    volumio.setRandom(true);
    break;
  case MENU_PLAYBACK_RANDOM_RESET:
    DEBUG_PRINT("MENU_PLAYBACK_RANDOM_RESET");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    volumio.setRandom(false);
    break;
  case MENU_PLAYBACK_RANDOM_SET_RESET:
    DEBUG_PRINT("MENU_PLAYBACK_RANDOM_SET_RESET");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    volumio.setRandom(volumio.State.random == "false");
    break;
  case MENU_PLAYBACK_REPEAT_SET:
    DEBUG_PRINT("MENU_PLAYBACK_REPEAT_SET");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    volumio.setRepeat(true);
    break;
  case MENU_PLAYBACK_REPEAT_RESET:
    DEBUG_PRINT("MENU_PLAYBACK_REPEAT_RESET");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    volumio.setRepeat(false);
    break;
  case MENU_PLAYBACK_REPEAT_SET_RESET:
    DEBUG_PRINT("MENU_PLAYBACK_REPEAT_SET_RESET");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    volumio.setRepeat(volumio.State.repeat == "false");
    break;
  case MENU_PLAYBACK_FAVORITE_SET:
    DEBUG_PRINT("MENU_PLAYBACK_FAVORITE_SET");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    break;
  case MENU_PLAYBACK_FAVORITE_RESET:
    DEBUG_PRINT("MENU_PLAYBACK_FAVORITE_RESET");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    break;
  case MENU_PLAYBACK_FAVORITE_SET_RESET:
    DEBUG_PRINT("MENU_PLAYBACK_FAVORITE_SET_RESET");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    break;
  case MENU_PLAYBACK_PLAYLIST_ADD:
    DEBUG_PRINT("MENU_PLAYBACK_PLAYLIST_ADD");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    break;
  case MENU_PLAYBACK_MUTE_SET:
    DEBUG_PRINT("MENU_PLAYBACK_MUTE_SET");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    volumio.mute();
    break;
  case MENU_PLAYBACK_MUTE_RESET:
    DEBUG_PRINT("MENU_PLAYBACK_MUTE_RESET");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    volumio.unmute();
    break;
  case MENU_PLAYBACK_MUTE_SET_RESET:
    DEBUG_PRINT("MENU_PLAYBACK_MUTE_SET_RESET");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    break;
  case MENU_QUEUE:
    DEBUG_PRINT("MENU_QUEUE");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    menuPush();
    genMenuStart();
    genMenuItem(ICON_HOME, TEXT_HOME, MENU_HOME);
    genMenuItem(ICON_CLEAR, TEXT_CLEAR, MENU_QUEUE_CLEAR);
    WaitForQueueUpdate = true;
    volumio.getQueue();
    break;
  case MENU_QUEUE_CLEAR:
    DEBUG_PRINT("MENU_QUEUE_CLEAR");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    volumio.clearQueue();
    genMenuStart();
    genMenuItem(ICON_HOME, TEXT_HOME, MENU_HOME);
    genMenuItem(ICON_CLEAR, TEXT_CLEAR, MENU_QUEUE_CLEAR);
    WaitForQueueUpdate = true;
    volumio.getQueue();
    break;
  case MENU_QUEUE_RANDOM:
    DEBUG_PRINT("MENU_QUEUE_RANDOM");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    break;
  case MENU_QUEUE_REPEAT:
    DEBUG_PRINT("MENU_QUEUE_REPEAT");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    break;
  case MENU_QUEUE_TRACK:
    DEBUG_PRINT("MENU_QUEUE_TRACK");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    menuPush();
    contextMenuQueue(atoi(data.c_str()));
    break;
  case MENU_QUEUE_TRACK_DELETE:
    DEBUG_PRINT("MENU_QUEUE_TRACK_DELETE");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    volumio.removeQueueItem(atoi(data.c_str()));
    menuPop();
    menuPop();
    menuAction(menuStack[menuStackIndex].Type, menuStack[menuStackIndex].Data);
    break;
  case MENU_QUEUE_TRACK_PLAY:
    DEBUG_PRINT("MENU_QUEUE_TRACK_PLAY");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    volumio.play(atoi(data.c_str()));
    menuPop();
    menuPop();
    menuAction(menuStack[menuStackIndex].Type, menuStack[menuStackIndex].Data);
    break;
  case MENU_SYSTEM:
    DEBUG_PRINT("MENU_SYSTEM");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    menuSystem();
    break;
  case MENU_DARK:
    DEBUG_PRINT("MENU_DARK");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    NoDisplay = true;
    break;
  }
}

void setup()
{
  //Initialize display
  display.begin();
  display.enableUTF8Print();

  //Initialize serial port for debugging
  Serial.begin(115200);
  delay(10);

  DEBUG_PRINTLN();

  //Initialize WiFi
  WiFi.enableSTA(true);
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);

  //Initialize left encoder
  LeftEncoder.begin(PIN_LeftEncoder_CLK, PIN_LeftEncoder_DT, INPUT_PULLUP);
  attachInterrupt(PIN_LeftEncoder_CLK, ISR_LeftEncoder_A, CHANGE);
  attachInterrupt(PIN_LeftEncoder_DT, ISR_LeftEncoder_B, CHANGE);

  //Initialize right encoder
  RightEncoder.begin(PIN_RightEncoder_CLK, PIN_RightEncoder_DT, INPUT_PULLUP);
  attachInterrupt(PIN_RightEncoder_CLK, ISR_RightEncoder_A, CHANGE);
  attachInterrupt(PIN_RightEncoder_DT, ISR_RightEncoder_B, CHANGE);

  //Initialize push button of left encoder
  LeftSwitch.begin(PIN_LeftEncoder_SW, INPUT_PULLUP);

  //Initialize push button of right encoder
  RightSwitch.begin(PIN_RightEncoder_SW, INPUT_PULLUP);

  //Capacitive touch pins
  //actually not used
  /*
  TouchPin0.begin(PIN_TOUCH0);
  TouchPin1.begin(PIN_TOUCH1);
  TouchPin2.begin(PIN_TOUCH2);
  TouchPin3.begin(PIN_TOUCH3);
  TouchPin4.begin(PIN_TOUCH4);
  TouchPin5.begin(PIN_TOUCH5);
  TouchPin6.begin(PIN_TOUCH6);
  */

  //Capacitive touch pins
  //not useable, pins already used by encoders
  /*
  TouchPin7.begin(PIN_TOUCH7);
  TouchPin8.begin(PIN_TOUCH8);
  TouchPin9.begin(PIN_TOUCH9);
  */
}

void loop()
{
  long now = millis();

  /*#################################################################*\
  |* Connection to WiFi and Voulumio
  \*#################################################################*/

  //Check if WiFi is connected. If not --> reconnect
  while (WiFi.status() != WL_CONNECTED)
  {
    DEBUG_PRINT("WiFi: Status: ");
    DEBUG_PRINTLN(WiFiStatusString());

    DisplayMessage(TEXT_CONNECT_WLAN);

    int i = 0;

    while (WiFi.status() != WL_CONNECTED)
    {
      WiFi.disconnect();

      i++;
      DisplayMessage("connect WiFi " + String(i));

      DEBUG_PRINT("WiFi: Connecting to ");
      DEBUG_PRINTLN(ssid);

      int laswtifistate = WiFi.status();

      WiFi.disconnect();
      WiFi.begin(ssid, password);

      for (int i = 0; i < 10; i++)
      {
        DEBUG_PRINT("WiFi: Status: ");
        DEBUG_PRINTLN(WiFiStatusString());
        DisplayMessage(WiFiStatusString());

        if (WiFi.status() == WL_CONNECTED)
          break;

        if (WiFi.status() != laswtifistate)
          break;

        delay(100);
      }
    }

    DEBUG_PRINT("WiFi: IP address: ");
    DEBUG_PRINTLN(WiFi.localIP());
    DisplayMessage("IP:" + WiFi.localIP().toString());
    DEBUG_PRINTLN();
  }

  //Check if Volumi (SocketIO) is connected. If not --> reconnect
  while (!volumio.connected())
  {
    DisplayMessage(TEXT_CONNECT_VOLUMIO);

    volumio.connect(host, port);

    if (!volumio.connected())
      delay(100);
    else
    {
      volumio.getState();

      menuMain();
    }
  }

  /*#################################################################*\
  |* Process data from Volumio (Updates)
  \*#################################################################*/

  volumio.process();

  switch (volumio.getPushType())
  {
  case Volumio::pushBrowseSources: //Volumio pushes list of sources
    DEBUG_PRINTLN("Main: Volumio: pushBrowseSources");

    //Only if push was requested
    if (waitForSourceUpdate)
    {
      while (volumio.readNextSourceItem())
      {
        DEBUG_PRINT("Main: Volumio: SourceItem: ");
        DEBUG_PRINT(volumio.CurrentSourceItem.name);
        DEBUG_PRINT(": ");
        DEBUG_PRINTLN(volumio.CurrentSourceItem.uri);

        genMenuItem(ICON_ARROW, volumio.CurrentSourceItem.name, MENU_BROWSE_SOURCE, volumio.CurrentSourceItem.uri);
      }

      GenMenuEnd();

      if (restore && false)
      {
        restore = false;
        LeftEncoder.setValue(menuStack[menuStackIndex].Position);
        menuOffset = menuStack[menuStackIndex].Offset;
      }
    }

    break;

  case Volumio::pushBrowseLibrary: //Volumio pushes library content
    DEBUG_PRINTLN("Main: Volumio: pushBrowseLibrary");

    //Only if push was requested
    if (waitForLibraryUpdate)
    {
      volumio.LibraryPrev.uri = "";

      while (volumio.readNextLibraryItem())
      {
        DEBUG_PRINT("Main: Volumio: LibraryItem: ");
        DEBUG_PRINT(volumio.CurrentLibraryItem.type);
        DEBUG_PRINT(": ");
        DEBUG_PRINT(volumio.CurrentLibraryItem.title);
        DEBUG_PRINT(": ");
        DEBUG_PRINTLN(volumio.CurrentLibraryItem.uri);

        if (volumio.CurrentLibraryItem.type == "song")
          genMenuItem(ICON_SONG, volumio.CurrentLibraryItem.title, MENU_BROWSE_SONG, volumio.CurrentLibraryItem.uri);
        else if (volumio.CurrentLibraryItem.type == "folder")
          genMenuItem(ICON_ARROW, volumio.CurrentLibraryItem.title, MENU_BROWSE_FOLDER, volumio.CurrentLibraryItem.uri);
        else if (volumio.CurrentLibraryItem.type == "radio-category")
          genMenuItem(ICON_ARROW, volumio.CurrentLibraryItem.title, MENU_BROWSE_RADIOCATEGORY, volumio.CurrentLibraryItem.uri);
        else if (volumio.CurrentLibraryItem.type == "mywebradio-category")
          genMenuItem(ICON_ARROW, volumio.CurrentLibraryItem.title, MENU_BROWSE_RADIOCATEGORY, volumio.CurrentLibraryItem.uri);
        else if (volumio.CurrentLibraryItem.type == "radio-favourites")
          genMenuItem(ICON_ARROW, volumio.CurrentLibraryItem.title, MENU_BROWSE_RADIOCATEGORY, volumio.CurrentLibraryItem.uri);
        else if (volumio.CurrentLibraryItem.type == "streaming-category")
          genMenuItem(ICON_ARROW, volumio.CurrentLibraryItem.title, MENU_BROWSE_STREAMINGCATEGORY, volumio.CurrentLibraryItem.uri);
        else if (volumio.CurrentLibraryItem.type == "webradio")
          genMenuItem(ICON_ARROW, volumio.CurrentLibraryItem.title, MENU_BROWSE_WEBRADIO, volumio.CurrentLibraryItem.uri);
        else if (volumio.CurrentLibraryItem.type == "playlist")
          genMenuItem(ICON_ARROW, volumio.CurrentLibraryItem.title, MENU_BROWSE_PLAYLIST, volumio.CurrentLibraryItem.uri);
        else
          genMenuItem(ICON_ARROW, volumio.CurrentLibraryItem.type, MENU_HOME);
      }

      while (volumio.readLibraryPrev())
        ;
      DEBUG_PRINT("Main: Volumio: LibraryPrev: ");
      DEBUG_PRINTLN(volumio.LibraryPrev.uri);

      //If there is an uri for full library, add play options
      if (volumio.LibraryInfo.uri != "")
      {
        DEBUG_PRINT("Main: Volumio: LibraryInfo: ");
        DEBUG_PRINT(volumio.LibraryInfo.title);
        DEBUG_PRINT(": ");
        DEBUG_PRINTLN(volumio.LibraryInfo.uri);

        genMenuItem(ICON_PLAY, TEXT_BROWSE_FOLDER_PLAY, MENU_BROWSE_FOLDER_PLAY, volumio.LibraryInfo.uri);
        genMenuItem(ICON_LIST, TEXT_BROWSE_FOLDER_ADDTOQUEUE, MENU_BROWSE_FOLDER_ADDTOQUEUE, volumio.LibraryInfo.uri);
        genMenuItem(ICON_LIST, TEXT_BROWSE_FOLDER_CLEARANDPLAY, MENU_BROWSE_FOLDER_CLEARANDPLAY, volumio.LibraryInfo.uri);
      }

      GenMenuEnd();

      if (restore && false)
      {
        restore = false;
        LeftEncoder.setValue(menuStack[menuStackIndex].Position);
        menuOffset = menuStack[menuStackIndex].Offset;
      }
    }

    break;

  case Volumio::pushQueue: //Volumio pushes queue content
    DEBUG_PRINTLN("Main: Volumio: pushQueue");

    //Only if push was requested
    if (WaitForQueueUpdate)
    {
      //Queue is referenced by index, not uri
      int index = 0;

      while (volumio.readNextQueueItem())
        if (volumio.CurrentQueueItem.name != "")
        {
          DEBUG_PRINT("Main: Volumio: QueueItem: ");
          DEBUG_PRINT(volumio.CurrentQueueItem.name);
          DEBUG_PRINT(": ");
          DEBUG_PRINTLN(index);

          genMenuItem(ICON_SONG, volumio.CurrentQueueItem.name, MENU_QUEUE_TRACK, String(index++));
        }

      GenMenuEnd();

      if (restore && false)
      {
        restore = false;
        LeftEncoder.setValue(menuStack[menuStackIndex].Position);
        menuOffset = menuStack[menuStackIndex].Offset;
      }
    }
    else
    {
      while (volumio.readNextQueueItem())
        ;
    }
    break;

  case Volumio::pushToastMessage: //Volumio pushes toast message
    DEBUG_PRINTLN("Main: Volumio: pushToastMessage");

    if (volumio.readPushToastMessage())
    {
      ToastDisplay = true;
      ToastStart = now;
    }

    break;

  case Volumio::pushState: //Volumio pushes status update
    DEBUG_PRINTLN("Main: Volumio: pushState");

    volumio.readState();
    waitvolumechange = false;

    break;
  }

  /*#################################################################*\
  |* Touch pins
  \*#################################################################*/

  /*
  TouchPin0.process();
  TouchPin1.process();
  TouchPin2.process();
  TouchPin3.process();
  TouchPin4.process();
  TouchPin5.process();
  TouchPin6.process();
  TouchPin7.process();
  TouchPin8.process();
  TouchPin9.process();
  */

  /*#################################################################*\
  |* Left switch - navigate through menu
  \*#################################################################*/

  LeftSwitch.process();

  if (LeftSwitch.getPressed())
  {
    //Memorize last input timesamp for screensaver
    lastinput = now;
    lastmenuchange = now;

    //Activate display if not switched on
    if (NoDisplay)
      NoDisplay = false;
    else
    {
      //Cancel display of toast message if displayed
      if (ToastDisplay)
      {
        ToastDisplay = false;
      }
      //Switch to menu if status is displayed
      else if (statusDisplay)
      {
        statusDisplay = false;
      }
      //Activate menu-item if in menu
      else
      {
        menuAction(Menu[menuPosition].Type, Menu[menuPosition].Data);
      }
    }
  }

  /*#################################################################*\
  |* Right switch - toggle play/pause (play/stop on radio)
  \*#################################################################*/

  RightSwitch.process();

  if (RightSwitch.getReleased())
  {
    //Memorize last input timesamp for screensaver
    lastinput = now;

    //Activate display if not switched on
    if (NoDisplay)
      NoDisplay = false;
    //Toggler play/pause
    else
      volumio.toggle();
  }

  /*#################################################################*\
  |* Left rotary encoder - scroll through menu
  \*#################################################################*/

  LeftEncoder.process();

  if (LeftEncoder.getChanged())
  {
    //Memorize last input timesamp for screensaver
    lastinput = now;
    lastmenuchange = now;
    item_offset = 0;
  }

  //Do nothing if statusdisplay active
  if (statusDisplay)
  {
  }
  else
  {

    if (restore && !waitForLibraryUpdate && !WaitForQueueUpdate && !waitForSourceUpdate)
    {
      restore = false;
      LeftEncoder.setValue(menuStack[menuStackIndex].Position);
      menuOffset = menuStack[menuStackIndex].Offset;
    }

    //Limit encoder value to menu length
    if (LeftEncoder.getValue() > (MenuLength - 1))
      LeftEncoder.setValue(MenuLength - 1);
    if (LeftEncoder.getValue() < 0)
      LeftEncoder.setValue(0);

    //Position of menu = encoder-value
    menuPosition = LeftEncoder.getValue();

    //Scroll menu when encoder reaches lower end of screen
    if (menuPosition >= MenuVisibleItems + menuOffset - 1)
      menuOffset = menuPosition - MenuVisibleItems + 1;

    //Scroll menu when encoder reaches upper end of screen
    if (menuPosition < menuOffset)
      menuOffset = menuPosition;
  }

  /*#################################################################*\
  |* Right rotary encoder - volume up/down
  \*#################################################################*/

  RightEncoder.process();

  if (RightEncoder.getChanged())
  {
    //Memorize last input timesamp for screensaver
    lastinput = now;

    //Show volume progress bar
    volumedisplay = true;
    lastsetvolume = now;

    newvolume = startvolume - ((RightEncoder.getValue() - startcnt));

    if (newvolume < VolumeMinimum)
      newvolume = VolumeMinimum;

    if (newvolume > VolumeMaximum)
      newvolume = VolumeMaximum;
  }

  if (volumedisplay)
  {
    if (volumio.State.volume != newvolume)
      //  if (!waitvolumechange)
      if (now > lastsendvolume + VolumeSetDelay)
      {
        waitvolumechange = true;
        volumio.volume(newvolume);
        lastsendvolume = now;
      }
  }
  //Memorize start values when volume update not in progress
  else
  {
    startcnt = RightEncoder.getValue();
    startvolume = volumio.State.volume;
    lastsendvolume = 0;
    //    waitvolumechange = false;
  }

  /*#################################################################*\
  |* Timer
  \*#################################################################*/

  //After some time without action automatically go back to status display
  if (delayBackStatus > 0)
    if (now - lastinput > delayBackStatus && volumio.State.status != "")
      statusDisplay = true;

  //After some time without action automatically switch off display
  if (delayDisplayOff > 0)
    if (now - lastinput > delayDisplayOff && volumio.State.status != "")
      NoDisplay = true;

  //After some time without action automatically switch off display, when in stop
  if (delayDisplayOffWhenNotPlay > 0)
    if (now - lastinput > delayDisplayOffWhenNotPlay && volumio.State.status != "" && volumio.State.status != "play")
      NoDisplay = true;

  //Duration of toast display
  if (ToastDisplay)
    if (now > ToastStart + ToastDuration)
      ToastDisplay = false;

  //Close progressbar after given time
  if (volumedisplay)
    if ((now - lastsetvolume) > VolumeDuration)
      volumedisplay = false;

  /*#################################################################*\
  |* Display
  \*#################################################################*/

  display.setFontDirection(0);
  display.firstPage();
  u8g2_uint_t x;

  do
  {
    //Display off
    if (NoDisplay)
    {
    }
    //Display on
    else
    {
      //Display toast message from Volumio
      if (ToastDisplay)
      {
        display.setFont(ToastTextFont);
        int y = 0;
        int maxchars = 20;

        for (int p = 0; p < 2; p++)
        {
          String block = "";
          String line = "";

          String sum = volumio.CurrentToastItem.title;
          if (p == 1)
            sum = volumio.CurrentToastItem.message;

          //Split text into multiple lines
          for (int i = 0; i < sum.length(); i++)
          {
            char c = sum[i];
            block += c;

            if (c == ' ')
            {
              if (block.length() + line.length() <= maxchars)
              {
                line += block;
                block = "";
              }
              else
              {
                display.drawUTF8(0, 13 + MenuItemHeight * y++, line.c_str());

                line = block;
                block = "";
              }
            }
          }

          //Draw last part of text
          if (block.length() + line.length() <= maxchars)
          {
            line += block;
            display.drawUTF8(0, 13 + MenuItemHeight * y++, line.c_str());
          }
          else
          {
            display.drawUTF8(0, 13 + MenuItemHeight * y++, line.c_str());
            display.drawUTF8(0, 13 + MenuItemHeight * y++, block.c_str());
          }

          display.drawLine(0, 63, (int)(DisplayWidth * ((float)(ToastStart + ToastDuration - now)) / (float)ToastDuration), 63);
        }
      }
      //Display player status
      else if (statusDisplay)
      {
        //Draw small Volumio logo
        display.drawXBM((DisplayWidth - logo_volumio_small_width) / 2, (MenuItemHeight - logo_volumio_small_height) / 2, logo_volumio_small_width, logo_volumio_small_height, logo_volumio_small_bits);

        //Draw status-icons (play/pause/strop and random and repeat)
        int px = 8;
        int py = MenuItemHeight - (MenuItemHeight - MenuTextHeight) / 2;

        display.setFont(MenuIconFont);
        if (volumio.State.status == "pause")
          display.drawUTF8(px, py, ICON_PAUSE);
        if (volumio.State.status == "play")
          display.drawUTF8(px, py, ICON_PLAY);
        if (volumio.State.status == "stop")
          display.drawUTF8(px, py, ICON_STOP);
        if (volumio.State.random == "true")
          display.drawUTF8(80, py, ICON_RANDOM);
        if (volumio.State.repeat == "true")
          display.drawUTF8(104, py, ICON_REPEAT);

        //Draw texts
        display.setFont(StatusTextFont);

        //Artist
        int widthArtist = display.getUTF8Width(volumio.State.artist.c_str());
        display.drawUTF8((DisplayWidth - widthArtist) / 2, 16 * 2, volumio.State.artist.c_str());

        //Title
        String line1 = volumio.State.title;
        String line2 = "";
        String line3 = "";

        //Split longer texts if they contain a "-" (often on webradio)
        int splitIndex = volumio.State.title.indexOf(" - ");

        if (splitIndex > 0)
        {
          line1 = volumio.State.title.substring(0, splitIndex);
          line2 = volumio.State.title.substring(splitIndex + 3);
        }

        //Text of first line centered or scrolling, depending on width
        line1_width = display.getUTF8Width(line1.c_str());

        if (line1_width < DisplayWidth)
          display.drawUTF8((DisplayWidth - line1_width) / 2, 16 * 3, line1.c_str());
        else
        //Scrolling text
        {
          x = line1_offset;

          if (line1_width > 0)
            do
            {
              display.drawUTF8(x, 48, line1.c_str());

              //second text
              x += line1_width + scrollGap;
            } while (x < DisplayWidth);
        }

        //Text of second line centered or scrolling, depending on width
        line2_width = display.getUTF8Width(line2.c_str());

        if (line2_width < DisplayWidth)
          display.drawUTF8((DisplayWidth - line2_width) / 2, 16 * 4, line2.c_str());
        else
        //Scrolling text
        {
          x = line2_offset;

          if (line2_width > 0)
            do
            {
              display.drawUTF8(x, 48 + 16, line2.c_str());

              //second text
              x += line2_width + scrollGap;
            } while (x < DisplayWidth);
        }

        if (volumio.State.duration > 0)
        {

          float SeekPercent = (float)volumio.State.seek / (float)volumio.State.duration / 1000.0;

          float barlen = SeekPercent * (DisplayWidth - 4);
          float barBoxHeight = 8;
          float barHeight = 4;

          int posy = MenuItemHeight * 5 + (MenuItemHeight - barBoxHeight) / 2;

          display.setColorIndex(0);
          display.drawRBox(0, posy, DisplayWidth, barBoxHeight, 0);
          display.setColorIndex(1);

          display.drawRFrame(0, posy, DisplayWidth, barBoxHeight, 0);

          if (int(barlen) > 0)
            display.drawRBox(2, posy + (barBoxHeight - barHeight) / 2, int(barlen), barHeight, 0);

          display.setFont(StatusTextFont);

          int trackMin = volumio.State.duration / 60;
          int trackSec = volumio.State.duration % 60;
          String txt1 = String(trackMin) + ":" + ((trackSec < 10) ? "0" : "") + String(trackSec);
          display.drawUTF8(DisplayWidth - 32, 16 * 7, txt1.c_str());

          int seekMin = int(volumio.State.seek / 1000.0) / 60;
          int seekSec = int(volumio.State.seek / 1000.0) % 60;
          String txt2 = String(seekMin) + ":" + ((seekSec < 10) ? "0" : "") + String(seekSec);
          display.drawUTF8(0, 16 * 7, txt2.c_str());
        }
      }
      else
      {
        bool style2 = false;

        if (style2)
          display.drawRBox(0, MenuItemHeight * (menuPosition - menuOffset), MenuPixelWidth - 3, MenuItemHeight, 0);

        //Write menu texts
        for (int i = 0; i < MenuVisibleItems; i++)
          if (i + menuOffset < MenuLength)
          {

            if (i == menuPosition - menuOffset)
            {
              if (style2)
                display.setColorIndex(0);
              else
                display.setColorIndex(1);

              display.setFont(MenuTextFont);
              item_width = display.getUTF8Width(Menu[i + menuOffset].Text.c_str()) + MenuItemHeight;

              if (item_width <= MenuPixelWidth - 3 || now < lastmenuchange + delayScrollMenu)
              {
                item_offset = 0;
                display.setFont(MenuTextFont);
                display.drawUTF8(MenuItemHeight, i * MenuItemHeight + MenuItemHeight - (MenuItemHeight - MenuTextHeight) / 2, Menu[i + menuOffset].Text.c_str());
                display.setFont(MenuIconFont);
                display.drawUTF8((MenuItemHeight - MenuTextHeight) / 2, i * MenuItemHeight + MenuItemHeight - (MenuItemHeight - MenuTextHeight) / 2, Menu[i + menuOffset].Icon.c_str());
              }
              else
              //Scrolling text
              {
                x = item_offset;

                do
                {
                  display.setFont(MenuTextFont);
                  display.drawUTF8(MenuItemHeight + x, i * MenuItemHeight + MenuItemHeight - (MenuItemHeight - MenuTextHeight) / 2, Menu[i + menuOffset].Text.c_str());
                  display.setFont(MenuIconFont);
                  display.drawUTF8((MenuItemHeight - MenuTextHeight) / 2 + x, i * MenuItemHeight + MenuItemHeight - (MenuItemHeight - MenuTextHeight) / 2, Menu[i + menuOffset].Icon.c_str());

                  //second text
                  x += item_width + scrollGapMenu;
                } while (x < DisplayWidth);
              }
            }
            else
            {
              display.setFont(MenuTextFont);
              display.drawUTF8(MenuItemHeight, i * MenuItemHeight + MenuItemHeight - (MenuItemHeight - MenuTextHeight) / 2, Menu[i + menuOffset].Text.c_str());
              display.setFont(MenuIconFont);
              display.drawUTF8((MenuItemHeight - MenuTextHeight) / 2, i * MenuItemHeight + MenuItemHeight - (MenuItemHeight - MenuTextHeight) / 2, Menu[i + menuOffset].Icon.c_str());
            }
          }

        //Highlight actual menu item
        if (MenuLength > MenuVisibleItems)
          display.drawRFrame(0, MenuItemHeight * (menuPosition - menuOffset), MenuPixelWidth - 3, MenuItemHeight, 0);
        else
          display.drawRFrame(0, MenuItemHeight * (menuPosition - menuOffset), MenuPixelWidth, MenuItemHeight, 0);

        //Show scrollbar if menu is longer than screen
        if (MenuLength > MenuVisibleItems)
        {
          display.setColorIndex(0);
          display.drawRBox(MenuPixelWidth - 3, 0, 3, MenuPixelHeight, 0);
          display.setColorIndex(1);

          int barlen = (int)((float)MenuVisibleItems / (float)MenuLength * MenuPixelHeight);
          int barpos = (int)((float)menuOffset / (float)MenuLength * MenuPixelHeight);
          display.drawLine(MenuPixelWidth - 1, barpos, MenuPixelWidth - 1, barpos + barlen);
        }
      }

      //Display volume
      if (volumedisplay)
      {
        //   float VolumePercent = float(volumio.State.volume) / (float)VolumeMaximum;
        float VolumePercent = float(newvolume) / (float)VolumeMaximum;

        float barlen = VolumePercent * (DisplayWidth - 4);
        float barBoxHeight = 8;
        float barHeight = 4;

        int posy = MenuItemHeight * 5 + (MenuItemHeight - barBoxHeight) / 2;

        display.setColorIndex(0);
        display.drawRBox(0, posy, DisplayWidth, barBoxHeight, 0);
        display.setColorIndex(1);

        display.drawRFrame(0, posy, DisplayWidth, barBoxHeight, 0);

        if (int(barlen) > 0)
          display.drawRBox(2, posy + (barBoxHeight - barHeight) / 2, int(barlen), barHeight, 0);
      }
    }
  } while (display.nextPage());

  /*#################################################################*\
  |* Scrolling Text
  \*#################################################################*/

  //Calculate milliseconds since last scroll
  if (lastscroll != 0)
    scrollbuffer += now - lastscroll;
  lastscroll = now;

  //Scroll only after x milliseconds (one pixel)
  if (scrollbuffer >= ScrollInterval)
  {
    scrollbuffer -= ScrollInterval;
    line1_offset -= 1;
    line2_offset -= 1;
    item_offset -= 1;
  }

  //Restart scrolling when at end
  if ((u8g2_uint_t)line1_offset < (u8g2_uint_t)-line1_width - scrollGap)
    line1_offset = 0;

  if ((u8g2_uint_t)line2_offset < (u8g2_uint_t)-line2_width - scrollGap)
    line2_offset = 0;

  if ((u8g2_uint_t)item_offset < (u8g2_uint_t)-item_width - scrollGapMenu)
    item_offset = 0;

  /*#################################################################*\
  |* Send Screenshot to PC
  \*#################################################################*/

  //not working, bug in u8g2 with this display?
  //only receiving 8 lines with empty pixels
  while (Serial.available())
  {

    switch (Serial.read())
    {
    //Screenshot
    case 1:
      Serial.write(STX);

      display.writeBufferXBM(Serial);
      Serial.write(ETX);
      break;
    }
  }
}