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

JSON parser;

#if Display == Display_SSD1351

void drawProgressBar(int x, int y, int widh, int lineThickness, int barThickness, int frameThickness, int frameSpacing, int knobWidth, int knobHeigth, float value)
{
  int thickest = (barThickness > knobHeigth) ? barThickness : knobHeigth;

  int thickness = frameThickness * 2 + thickest + frameSpacing * 2;
  float barlen = value * (widh - 2 * frameThickness - 2 * frameSpacing);
  float knobpos = value * (widh - 2 * frameThickness - 2 * frameSpacing - knobWidth);

  display.setColor(63, 164, 101);
  display.drawBox(x, y, widh, thickness);

  display.setColor(255, 255, 255);
  display.drawBox(x + frameThickness, y + frameThickness, widh - 2 * frameThickness, thickness - 2 * frameThickness);

  display.setColor(63, 164, 101);

  if (lineThickness > 0)
    display.drawBox(x + frameThickness + frameSpacing, y + frameThickness + frameSpacing + (thickest - lineThickness) / 2, widh - 2 * frameThickness - 2 * frameSpacing, lineThickness);

  if (knobWidth > 0 && knobHeigth > 0)
  {
    display.drawBox(x + int(knobpos), y + frameThickness + frameSpacing, knobWidth, knobHeigth);

    if (int(barlen) > 0 && barThickness > 0)
      display.drawBox(x + frameThickness + frameSpacing, y + frameThickness + frameSpacing + (thickest - barThickness) / 2, int(knobpos), barThickness);
  }
  else if (int(barlen) > 0 && barThickness > 0)
    display.drawBox(x + frameThickness + frameSpacing, y + frameThickness + frameSpacing + (thickest - barThickness) / 2, int(barlen), barThickness);
}

void drawProgressBar(int x, int y, int widh, int style, float value)
{
  switch (style)
  {
  case 0:
    drawProgressBar(x, y, widh, 1, 3, 0, 0, 5, 5, value);
    break;
  case 1:
    drawProgressBar(x, y, widh, 0, 4, 1, 1, 0, 0, value);
  case 2:
    drawProgressBar(x, y, widh, 0, 1, 0, 0, 0, 0, value);
    break;
  }
}

#endif

void DisplayMessage(String Message)
{

#if Display == Display_SSD1351

  display.clearScreen();
  display.drawBitmap65k(9, 9, 110, 15, LogoVolumio65k);

  // display.drawBitmap24bit(0, 0, 128, 128, Stars);
  // display.setColor(0, 0, 0);
  // display.drawBoxAlpha(0, 0, 128, 128);

  display.setColor(255, 255, 255);
  int w = display.getUTF8Width(Message.c_str());
  display.drawUTF8((DisplayWidth - w) / 2, 3 * MenuItemHeight + MenuItemHeight - (MenuItemHeight - MenuTextHeight) / 2, Message.c_str());

  display.flush();
#else
  display.firstPage();
  display.setFont(MenuTextFont);

  display.setFontDirection(0);

  do
  {

    //Logo and single line (true) or 4 lines scrolling (false)
    if (true)
    {

      display.drawXBM((DisplayWidth - logo_volumio_big_width) / 2, (DisplayHeight / 2 - logo_volumio_big_height) / 2, logo_volumio_big_width, logo_volumio_big_height, logo_volumio_big_bits);

      //Draw message text
      int w = display.getUTF8Width(Message.c_str());
      display.drawUTF8((DisplayWidth - w) / 2, 3 * MenuItemHeight + MenuItemHeight - (MenuItemHeight - MenuTextHeight) / 2, Message.c_str());
    }
    else
    {
      int msgcnt = 4;

      messagebuffer[messageindex] = Message;

      //Draw message texts
      for (int i = 0; i < msgcnt; i++)
        display.drawUTF8(0, i * MenuItemHeight + MenuItemHeight - (MenuItemHeight - MenuTextHeight) / 2, messagebuffer[(messageindex - i + msgcnt) % msgcnt].c_str());

      messageindex = (messageindex + 1) % msgcnt;
    }
  } while (display.nextPage());

#endif
}

String WiFiStatusString()
{
  String text = "";

  switch (WiFi.status())
  {
  case WL_CONNECTED:
    text = "WL_CONNECTED";
    break;
  case WL_NO_SHIELD:
    text = "WL_CONNECTED";
    break;
  case WL_IDLE_STATUS:
    text = "WL_IDLE_STATUS";
    break;
  case WL_NO_SSID_AVAIL:
    text = "WL_NO_SSID_AVAIL";
    break;
  case WL_SCAN_COMPLETED:
    text = "WL_SCAN_COMPLETED";
    break;
  case WL_CONNECT_FAILED:
    text = "WL_CONNECT_FAILED";
    break;
  case WL_CONNECTION_LOST:
    text = "WL_CONNECTION_LOST";
    break;
  case WL_DISCONNECTED:
    text = "WL_DISCONNECTED";
    break;
  default:
    text = "???";
  }

  return text;
}

//Interrupt: left encoder Pin A changed
void IRAM_ATTR ISR_LeftEncoder_A()
{
  leftEncoder.A_CHANGED();
}

//Interrupt: left encoder Pin B changed
void IRAM_ATTR ISR_LeftEncoder_B()
{
  leftEncoder.B_CHANGED();
}

//Interrupt: right encoder Pin A changed
void IRAM_ATTR ISR_RightEncoder_A()
{
  rightEncoder.A_CHANGED();
}

//Interrupt: right encoder Pin B changed
void IRAM_ATTR ISR_RightEncoder_B()
{
  rightEncoder.B_CHANGED();
}

void menuAction(MenuItemType type, String data)
{
  DEBUG_PRINT("Main: menuAction(): ");

  waitForSourceUpdate = false;
  waitForQueueUpdate = false;
  waitForLibraryUpdate = false;
  restoreMenuPosition = false;
  waitForMenuItemsUpdate = false;
  waitForUiConfigSection = false;

  switch (type)
  {
  case MENU_HOME:
    DEBUG_PRINT("MENU_HOME");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    restoreMenuPosition = true;
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
    restoreMenuPosition = true;
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
    genMenuItem(ICON_HOME, locale.COMMON.HOME, MENU_HOME);
    waitForSourceUpdate = true;
    volumio.getBrowseSources();
    break;
  case MENU_BROWSE_SOURCE:
    DEBUG_PRINT("MENU_BROWSE_SOURCE");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    menuPush();
    genMenuStart();
    genMenuItem(ICON_HOME, locale.COMMON.HOME, MENU_HOME);
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
    genMenuItem(ICON_HOME, locale.COMMON.HOME, MENU_HOME);
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
    genMenuItem(ICON_HOME, locale.COMMON.HOME, MENU_HOME);
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
    genMenuItem(ICON_HOME, locale.COMMON.HOME, MENU_HOME);
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
    genMenuItem(ICON_HOME, locale.COMMON.HOME, MENU_HOME);
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
    volumio.addToFavourites("mpd", menuStack[menuStackIndex - 1].Text, menuStack[menuStackIndex - 1].Data);
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
    volumio.addToQueue("webradio", menuStack[menuStackIndex - 1].Text, menuStack[menuStackIndex - 1].Data);
    break;
  case MENU_BROWSE_WEBRADIO_CLEARANDPLAY:
    DEBUG_PRINT("MENU_BROWSE_WEBRADIO_CLEARANDPLAY");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    volumio.replaceAndPlay("webradio", menuStack[menuStackIndex - 1].Text, menuStack[menuStackIndex - 1].Data);
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
    volumio.addToFavourites("webradio", menuStack[menuStackIndex - 1].Text, menuStack[menuStackIndex - 1].Data);
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
    genMenuItem(ICON_HOME, locale.COMMON.HOME, MENU_HOME);
    genMenuItem(ICON_CLEAR, locale.TRACK_INFO_BAR.CLEAR_QUEUE, MENU_QUEUE_CLEAR);
    waitForQueueUpdate = true;
    volumio.getQueue();
    break;
  case MENU_QUEUE_CLEAR:
    DEBUG_PRINT("MENU_QUEUE_CLEAR");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    volumio.clearQueue();
    genMenuStart();
    genMenuItem(ICON_HOME, locale.COMMON.HOME, MENU_HOME);
    genMenuItem(ICON_CLEAR, locale.TRACK_INFO_BAR.CLEAR_QUEUE, MENU_QUEUE_CLEAR);
    waitForQueueUpdate = true;
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
  case MENU_SETTINGS:
    DEBUG_PRINT("MENU_SETTINGS");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    menuPush();
    genMenuStart();
    genMenuItem(ICON_HOME, locale.COMMON.HOME, MENU_HOME);
    volumio.getMenuItems();
    waitForMenuItemsUpdate = true;
    break;

  case MENU_SETTINGS_PLUGIN:
    DEBUG_PRINT("MENU_SETTINGS_PLUGIN");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    menuPush();
    genMenuStart();
    genMenuItem(ICON_HOME, locale.COMMON.HOME, MENU_HOME);
    genMenuItem(ICON_BACK, menuStack[menuStackIndex - 2].Text, MENU_BACK);
    volumio.getUiConfig(data);
    waitForUiConfigSection = true;
    break;

  case MENU_SETTINGS_MODAL:
    DEBUG_PRINT("MENU_SETTINGS_MODAL");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    menuPush();
    genMenuStart();
    genMenuItem(ICON_HOME, locale.COMMON.HOME, MENU_HOME);
    genMenuItem(ICON_BACK, menuStack[menuStackIndex - 2].Text, MENU_BACK);
    volumio.getUiConfig(data);
    waitForUiConfigSection = true;
    break;

  case MENU_SETTINGS_URL:
    DEBUG_PRINT("MENU_SETTINGS_URL");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    menuPush();
    genMenuStart();
    genMenuItem(ICON_HOME, locale.COMMON.HOME, MENU_HOME);
    genMenuItem(ICON_BACK, menuStack[menuStackIndex - 2].Text, MENU_BACK);
    volumio.getUiConfig(data);
    waitForUiConfigSection = true;
    break;

  case MENU_DARK:
    DEBUG_PRINT("MENU_DARK");
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(data);
    noDisplay = true;
    break;
  }
}

void setup()
{
  //Initialize serial port for debugging
  Serial.begin(921600);
  delay(10);

  locale = Locale_en();

//Initialize display
#if Display == Display_SSD1351

  display.initialize();
  display.clearScreen();
  display.flush();

#else
  display.begin();
  display.enableUTF8Print();
  ui.initialize(&display);
#endif

  scroll1.initialize();
  scroll2.initialize();
  scroll3.initialize();

  DEBUG_PRINTLN();

  //Initialize WiFi
  WiFi.enableSTA(true);
  WiFi.persistent(false);
  WiFi.begin(ssid.c_str(), password.c_str());
  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);
  WiFi.mode(WIFI_STA);

  //Initialize left encoder
  leftEncoder.begin(PIN_LeftEncoder_CLK, PIN_LeftEncoder_DT, INPUT_PULLUP);
  attachInterrupt(PIN_LeftEncoder_CLK, ISR_LeftEncoder_A, CHANGE);
  attachInterrupt(PIN_LeftEncoder_DT, ISR_LeftEncoder_B, CHANGE);

  //Initialize right encoder
  rightEncoder.begin(PIN_RightEncoder_CLK, PIN_RightEncoder_DT, INPUT_PULLUP);
  attachInterrupt(PIN_RightEncoder_CLK, ISR_RightEncoder_A, CHANGE);
  attachInterrupt(PIN_RightEncoder_DT, ISR_RightEncoder_B, CHANGE);

  //Initialize push button of left encoder
  leftSwitch.begin(PIN_LeftEncoder_SW, INPUT_PULLUP);

  //Initialize push button of right encoder
  rightSwitch.begin(PIN_RightEncoder_SW, INPUT_PULLUP);

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

int py = 0;
bool dir = false;
WiFiClient client;

void loop()
{

  unsigned long now = millis();

  /*#################################################################*\
  |* Connection to WiFi and Voulumio
  \*#################################################################*/

  lastconnected = now;


  //Check if WiFi is connected. If not --> reconnect
  while (WiFi.status() != WL_CONNECTED)
  {
    while(client.available())
      client.read();

    DEBUG_PRINT("Main: WiFi: Status: ");
    DEBUG_PRINTLN(WiFiStatusString());
    DisplayMessage(locale.ESP.ConnectWiFi);
  
    leftSwitch.process();
    rightSwitch.process();
 
    if(leftSwitch.getPressed())
        ESP.restart();

    if(rightSwitch.getPressed())
        ESP.restart();

    //Auto reboot after a whilw (deactivate with delay 0)
    if(delayReboot != 0 && millis() > (lastconnected + delayReboot))
        ESP.restart();

  }
 



  /* while (WiFi.status() != WL_CONNECTED)
  {
    DEBUG_PRINT("Main: WiFi: Status: ");
    DEBUG_PRINTLN(WiFiStatusString());

    DisplayMessage(locale.ESP.ConnectWiFi);

    int retrys = 0;

    WiFi.reconnect();
    delay(1000);

    while (WiFi.status() != WL_CONNECTED)
    {
      WiFi.disconnect();

      DEBUG_PRINT("Main: WiFi: Connecting to ");
      DEBUG_PRINTLN(ssid);

      int laswtifistate = WiFi.status();

      WiFi.disconnect();
      WiFi.mode(WIFI_OFF);
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid.c_str(), password.c_str());

      for (int i = 0; i < 10; i++)
      {
        DEBUG_PRINT("Main: WiFi: Status: ");
        DEBUG_PRINTLN(WiFiStatusString());

        if (WiFi.status() == WL_CONNECTED)
          break;

        if (WiFi.status() != laswtifistate)
          break;

        delay(100);
      }

      if (retrys++ > 10 && WiFi.status() != WL_CONNECTED)
      {
        DEBUG_PRINT("Main: WiFi: Too many retrys, restarting ESP");
        ESP.restart();
      }
    }

    DEBUG_PRINT("Main: WiFi: IP address: ");
    DEBUG_PRINTLN(WiFi.localIP());
    DEBUG_PRINTLN();
  }
*/
  //Check if Volumio (SocketIO) is connected. If not --> reconnect
  while (!volumio.getConnected())
  {
    DisplayMessage(locale.ESP.ConnectVolumio);

    volumio.connect(host, port);

    if (!volumio.getConnected())
      delay(100);
    else
    {
      volumio.getState();
      volumio.getUiSettings();

      //  volumio.getDeviceInfo();
      //  volumio.getSystemVersion();
      //  volumio.getPushType();
      //  volumio.getPlaylistIndex();
      //  volumio.getAvailableLanguages();
      //  volumio.getAudioOutputs();
      //  volumio.getMenuItems();
      //  volumio.getUiConfig("miscellanea/my_music");
      //  volumio.getInstalledPlugins();
      //  volumio.getAvaliablePlugins();

      menuMain();
    }
  }

  /*#################################################################*\
  |* Process data from Volumio (pushed Updates)
  \*#################################################################*/

  volumio.process();

  switch (volumio.getPushType())
  {
  case Volumio::pushBrowseSources: //Volumio pushes list of sources
    DEBUG_PRINTLN("Main: Volumio: pushBrowseSources");

    //Only if push was requested
    if (waitForSourceUpdate)
    {
      waitForSourceUpdate = false;

      while (volumio.readNextSourceItem())
      {
        DEBUG_PRINT("Main: Volumio: SourceItem: ");
        DEBUG_PRINT(volumio.CurrentSourceItem.name);
        DEBUG_PRINT(": ");
        DEBUG_PRINTLN(volumio.CurrentSourceItem.uri);

        genMenuItem(ICON_ARROW, volumio.CurrentSourceItem.name, MENU_BROWSE_SOURCE, volumio.CurrentSourceItem.uri);
      }

      genMenuEnd();

      if (restoreMenuPosition && false)
      {
        restoreMenuPosition = false;
        leftEncoder.setValue(menuStack[menuStackIndex].Position);
        menuOffset = menuStack[menuStackIndex].Offset;
      }
    }

    break;

  case Volumio::pushBrowseLibrary: //Volumio pushes library content
    DEBUG_PRINTLN("Main: Volumio: pushBrowseLibrary");

    //Only if push was requested
    if (waitForLibraryUpdate)
    {
      waitForLibraryUpdate = false;

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

        genMenuItem(ICON_PLAY, locale.BROWSER.PLAY, MENU_BROWSE_FOLDER_PLAY, volumio.LibraryInfo.uri);
        genMenuItem(ICON_LIST, locale.BROWSER.ADD_TO_QUEUE, MENU_BROWSE_FOLDER_ADDTOQUEUE, volumio.LibraryInfo.uri);
        genMenuItem(ICON_LIST, locale.BROWSER.CLEAR_AND_PLAY, MENU_BROWSE_FOLDER_CLEARANDPLAY, volumio.LibraryInfo.uri);
      }

      genMenuEnd();

      if (restoreMenuPosition && false)
      {
        restoreMenuPosition = false;
        leftEncoder.setValue(menuStack[menuStackIndex].Position);
        menuOffset = menuStack[menuStackIndex].Offset;
      }
    }

    break;

  case Volumio::pushQueue: //Volumio pushes queue content
    DEBUG_PRINTLN("Main: Volumio: pushQueue");

    //Only if push was requested
    if (waitForQueueUpdate)
    {
      waitForQueueUpdate = false;

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

      genMenuEnd();

      if (restoreMenuPosition && false)
      {
        restoreMenuPosition = false;
        leftEncoder.setValue(menuStack[menuStackIndex].Position);
        menuOffset = menuStack[menuStackIndex].Offset;
      }
    }

    break;

  case Volumio::pushToastMessage: //Volumio pushes toast message
    DEBUG_PRINTLN("Main: Volumio: pushToastMessage");

    if (toastActivatesDisplay)
    {
      if (noDisplay)
        statusDisplay = true;

      lastinput = now;
      noDisplay = false;
    }

    if (volumio.readPushToastMessage())
    {
      if (durationShowToast > 0)
        toastDisplay = true;

      toastStart = now;
    }

    break;

  case Volumio::pushState: //Volumio pushes status update
    DEBUG_PRINTLN("Main: Volumio: pushState");

    volumio.readState();
    waitvolumechange = false;

    break;

  case Volumio::pushMultiRoomDevices:
    DEBUG_PRINTLN("Main: Volumio: pushMultiRoomDevices");

    while (volumio.readMultiRoomDevice())
      ;

    break;

  case Volumio::pushUiSettings:
    DEBUG_PRINTLN("Main: Volumio: pushUiSettings");

    volumio.readUiSettings();

    DEBUG_PRINT("Main: language = ");
    DEBUG_PRINTLN(volumio.UiSettings.language);

    if (volumio.UiSettings.language == "de")
      locale = Locale_de();
    else
      locale = Locale_en();

    menuMain();

    break;

  case Volumio::pushDeviceInfo:
    DEBUG_PRINTLN("Main: Volumio: pushDeviceInfo");

    volumio.readDeviceInfo();

  case Volumio::pushSystemVersion:
    DEBUG_PRINTLN("Main: Volumio: pushSystemVersion");

    volumio.readSystemVersion();

  case Volumio::pushMenuItems:
    DEBUG_PRINTLN("Main: Volumio: pushMenuItems");

    //Only if push was requested
    if (waitForMenuItemsUpdate)
    {
      waitForMenuItemsUpdate = false;

      //Queue is referenced by index, not uri
      int index = 0;

      while (volumio.readNextMenuItem())
        if (volumio.CurrentMenuItem.name != "")
        {
          DEBUG_PRINT("Main: Volumio: MenuItem: ");
          DEBUG_PRINT(volumio.CurrentMenuItem.name);
          DEBUG_PRINT(": ");
          DEBUG_PRINTLN(index);

          if (volumio.CurrentMenuItem.params.pluginName != "")
            genMenuItem(ICON_ARROW, volumio.CurrentMenuItem.name, MENU_SETTINGS_PLUGIN, volumio.CurrentMenuItem.params.pluginName);
          else if (volumio.CurrentMenuItem.params.modalName != "")
            genMenuItem(ICON_ARROW, volumio.CurrentMenuItem.name, MENU_SETTINGS_MODAL, volumio.CurrentMenuItem.params.modalName);
          else if (volumio.CurrentMenuItem.params.url != "")
            genMenuItem(ICON_ARROW, volumio.CurrentMenuItem.name, MENU_SETTINGS_URL, volumio.CurrentMenuItem.params.url);
        }

      genMenuEnd();

      if (restoreMenuPosition && false)
      {
        restoreMenuPosition = false;
        leftEncoder.setValue(menuStack[menuStackIndex].Position);
        menuOffset = menuStack[menuStackIndex].Offset;
      }
    }

  case Volumio::pushUiConfig:
    DEBUG_PRINTLN("Main: Volumio: pushUiConfig");

    if (waitForUiConfigSection)
    {
      waitForUiConfigSection = false;

      while (volumio.readNextUiConfigSection())
      {

        if (volumio.CurrentUiConfigSection.coreSection != "")
          genMenuItem(ICON_ARROW, volumio.CurrentUiConfigSection.coreSection, MENU_SETTINGS_PLUGIN, volumio.CurrentUiConfigSection.coreSection);

        if (volumio.CurrentUiConfigSection.label != "" && volumio.CurrentUiConfigSection.hidden != "true")
          genMenuItem(ICON_ARROW, volumio.CurrentUiConfigSection.label, MENU_SETTINGS_PLUGIN, volumio.CurrentUiConfigSection.id);
      }
    }

    break;

  case Volumio::pushInstalledPlugins:
    DEBUG_PRINTLN("Main: Volumio: pushInstalledPlugins");

    while (volumio.readNextInstalledPlugin())
    {
      DEBUG_PRINT("Main: Volumio: Plugin: ");
      DEBUG_PRINT(volumio.CurrentInstalledPlugin.name);
      DEBUG_PRINT(": ");
      DEBUG_PRINTLN(volumio.CurrentInstalledPlugin.prettyName);
    };

  case Volumio::pushNone:
    break;
  }

  /*#################################################################*\
  |* Touch pins
  \*#################################################################*/

  /*
  touchPin0.process();
  touchPin1.process();
  touchPin2.process();
  touchPin3.process();
  touchPin4.process();
  touchPin5.process();
  touchPin6.process();
  touchPin7.process();
  touchPin8.process();
  touchPin9.process();
  */

  /*#################################################################*\
  |* Left switch - navigate through menu
  \*#################################################################*/

  leftSwitch.process();

  if (leftSwitch.getPressed())
  {
    //Memorize last input timesamp for screensaver
    lastinput = now;
    lastmenuchange = now;

    //Activate display if not switched on
    if (noDisplay)
      noDisplay = false;
    else
    {
      //Cancel display of toast message if displayed
      if (toastDisplay)
      {
        toastDisplay = false;
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

  rightSwitch.process();

  if (rightSwitch.getPressed())
  {
    //Memorize last input timesamp for screensaver
    lastinput = now;

    if(volumio.State.status == "play")
    {
      volumio.toggle();
      noDisplay= true;
    }
    else
    {
      volumio.toggle();
      noDisplay= false;
      
    }
    


  }

  /*#################################################################*\
  |* Left rotary encoder - scroll through menu
  \*#################################################################*/

  leftEncoder.process();

  if (leftEncoder.getChanged())
  {
    //Memorize last input timesamp for screensaver
    lastinput = now;
    lastmenuchange = now;
    scroll3.reset();
  }

  //Do nothing if statusdisplay active
  if (!statusDisplay)
  {
    if (restoreMenuPosition && !waitForLibraryUpdate && !waitForQueueUpdate && !waitForSourceUpdate && !waitForMenuItemsUpdate && !waitForUiConfigSection)
    {
      restoreMenuPosition = false;
      leftEncoder.setValue(menuStack[menuStackIndex].Position);
      menuOffset = menuStack[menuStackIndex].Offset;
    }

    //Limit encoder value to menu length
    if (leftEncoder.getValue() > (MenuLength - 1))
      leftEncoder.setValue(MenuLength - 1);
    if (leftEncoder.getValue() < 0)
      leftEncoder.setValue(0);

    //Position of menu = encoder-value
    menuPosition = leftEncoder.getValue();

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

  rightEncoder.process();

  if (rightEncoder.getChanged())
  {
    noDisplay = false;

    //Memorize last input timesamp for screensaver
    lastinput = now;

    //Show volume progress bar
    volumeDisplay = true;
    lastsetvolume = now;

    newvolume = startvolume - ((rightEncoder.getValue() - startcnt));

    if (newvolume < volumeMinimum)
      newvolume = volumeMinimum;

    if (newvolume > volumeMaximum)
      newvolume = volumeMaximum;
  }

  if (volumeDisplay)
  {
    if (volumio.State.volume != newvolume)
      if (now > lastsendvolume + volumeSetInterval)
      {
        waitvolumechange = true;
        volumio.volume(newvolume);
        lastsendvolume = now;
      }
  }
  //Memorize start values when volume update not in progress
  else
  {
    startcnt = rightEncoder.getValue();
    startvolume = volumio.State.volume;
    lastsendvolume = 0;
  }

  /*#################################################################*\
  |* Timer
  \*#################################################################*/

  //After some time without action automatically go back to status display
  if (delayBackStatus > 0)
    if (now > lastinput + delayBackStatus && volumio.State.status != "")
      statusDisplay = true;

  //After some time without action automatically switch off display
  if (delayDisplayOff > 0)
    if (now > lastinput + delayDisplayOff && volumio.State.status != "")
      noDisplay = true;

  //After some time without action automatically switch off display, when in stop
  if (delayDisplayOffWhenNotPlay > 0)
    if (now > lastinput + delayDisplayOffWhenNotPlay && volumio.State.status != "" && volumio.State.status != "play")
      noDisplay = true;

  //Duration of toast display
  if (toastDisplay)
    if (now > toastStart + durationShowToast)
      toastDisplay = false;

  //Close progressbar after given time
  if (volumeDisplay)
    if (now > lastsetvolume + durationShowVolume)
      volumeDisplay = false;

  /*#################################################################*\
  |* Send Screenshot to PC
  \*#################################################################*/

  bool send = false;

  while (Serial.available())
  {
    switch (Serial.read())
    {
    //Screenshot
    case 1:
      send = true;
      break;
    }
  }

  /*#################################################################*\
  |* Display
  \*#################################################################*/

#if Display == Display_SSD1351

  int x;

  //Display on
  if (noDisplay)
    display.clearScreen(0, 0, 0);
  else
  {
    display.drawBitmap24bit(0, 0, 128, 128, Stars);
    display.setColor(0, 0, 0);
    display.drawBoxAlpha(0, 0, 128, 128);

    //Display toast message from Volumio
    if (toastDisplay)
    {
      //  display.setU8g2Font(ToastTextFont);
      int y = 0;
      int maxchars = 20;

      display.setColor(255, 255, 255);

      splitter.initialize(volumio.CurrentToastItem.title, maxchars);

      while (splitter.next())
        display.drawUTF8(0, 13 + MenuItemHeight * y++, splitter.line);

      splitter.initialize(volumio.CurrentToastItem.message, maxchars);

      while (splitter.next())
        display.drawUTF8(0, 13 + MenuItemHeight * y++, splitter.line);

      drawProgressBar(4, DisplayHeight - 8, DisplayWidth - 8, 2, ((float)(toastStart + durationShowToast - now)) / (float)durationShowToast);
    }
    //Display player status
    else if (statusDisplay)
    {
      //    display.setColor(0, 0, 0);
      //  display.drawBox(x, 96, 128, 32);

      display.setColor(255, 255, 255);

      //Draw status-icons (play/pause/strop and random and repeat)
      int px = 8;
      int py = MenuItemHeight - (MenuItemHeight - MenuTextHeight) / 2;

      /*
      display.setU8g2Font(MenuIconFont);
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
      display.setU8g2Font(StatusTextFont);
*/
      int textpos = 1;

      display.setColor(255, 255, 255);

      //  String line0 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
      //  display.drawUTF8(0, 16 * textpos++, line0);
      // display.drawUTF8(10, 16 * textpos++ * MenuItemHeight + MenuItemHeight - (MenuItemHeight - MenuTextHeight) / 2, "0123456789"); //ABCDEFGHIJKLMNOPQRSTUVWXYZ");

      int maxchars = 20;
      if (volumio.State.album != "null")
      {
        splitter.initialize(volumio.State.album, maxchars);
        display.setColor(63, 164, 101);

        while (splitter.next())
        {
          int width = display.getUTF8Width(splitter.line);
          display.drawUTF8((DisplayWidth - width) / 2, 16 * textpos++, splitter.line);
        }
      }

      splitter.initialize(volumio.State.title, maxchars);
      display.setColor(255, 255, 255);

      while (splitter.next())
      {
        int width = display.getUTF8Width(splitter.line);
        display.drawUTF8((DisplayWidth - width) / 2, 16 * textpos++, splitter.line);
      }

      /*
      //Split longer texts if they contain a "-" (often on webradio)
      int splitIndex = volumio.State.title.indexOf(" - ");
*/
      splitter.initialize(volumio.State.artist, maxchars);
      display.setColor(63, 164, 101);

      while (splitter.next())
      {
        int width = display.getUTF8Width(splitter.line);
        display.drawUTF8((DisplayWidth - width) / 2, 16 * textpos++, splitter.line);
      }

      if (volumio.State.duration > 0)
      {

        float SeekPercent = (float)volumio.State.seek / (float)volumio.State.duration / 1000.0;
        float barBoxHeight = 8;

        int posy = MenuItemHeight * 5 + (MenuItemHeight - barBoxHeight) / 2;

        display.setColor(255, 255, 255);
        display.drawBox(4, 120, 120, 4);

        int pos = (int)(116.0 * SeekPercent);

        display.setColor(63, 164, 101);
        display.drawBox(4, 120, pos, 4);
        display.drawBox(pos, 120 - 2, 4, 4 + 2);

        //   drawProgressBar(4, 128-16, DisplayWidth - 8, 0, SeekPercent);

        display.setColor(255, 255, 255);

        int trackMin = volumio.State.duration / 60;
        int trackSec = volumio.State.duration % 60;
        String txt1 = String(trackMin) + ":" + ((trackSec < 10) ? "0" : "") + String(trackSec);
        display.drawUTF8(DisplayWidth - 32, 16 * 7, txt1.c_str());

        int seekMin = int(volumio.State.seek / 1000.0) / 60;
        int seekSec = int(volumio.State.seek / 1000.0) % 60;
        String txt2 = String(seekMin) + ":" + ((seekSec < 10) ? "0" : "") + String(seekSec);
        display.drawUTF8(4, 16 * 7, txt2.c_str());
      }
    }
    else
    {

      display.setColor(63, 164, 101);

      //Highlight actual menu item
      if (MenuLength > MenuVisibleItems)
        display.drawRBoxAlpha(0, MenuItemHeight * (menuPosition - menuOffset), MenuPixelWidth - 3, MenuItemHeight, 8);
      else
        display.drawRBoxAlpha(0, MenuItemHeight * (menuPosition - menuOffset), MenuPixelWidth, MenuItemHeight, 8);

      display.setColor(255, 255, 255);

      //Write menu texts
      for (int i = 0; i < MenuVisibleItems; i++)
        if (i + menuOffset < MenuLength)
        {

          if (i == menuPosition - menuOffset)
          {
            /*         if (style2)
              display.setColorIndex(0);
            else
              display.setColorIndex(1);
*/
            //       display.setU8g2Font(MenuTextFont);
            scroll3.width = display.getUTF8Width(Menu[i + menuOffset].Text.c_str()) + MenuItemHeight;

            if (scroll3.width <= MenuPixelWidth - 3 || now < lastmenuchange + delayScrollMenu || delayScrollMenu == 0)
            {
              scroll3.reset();
              //   display.setU8g2Font(MenuTextFont);
              display.drawUTF8(MenuItemHeight, i * MenuItemHeight + MenuItemHeight - (MenuItemHeight - MenuTextHeight) / 2, Menu[i + menuOffset].Text.c_str());
              //     display.setU8g2Font(MenuIconFont);
              //      display.drawUTF8((MenuItemHeight - MenuTextHeight) / 2, i * MenuItemHeight + MenuItemHeight - (MenuItemHeight - MenuTextHeight) / 2, Menu[i + menuOffset].Icon.c_str());
            }
            else
            //Scrolling text
            {
              x = scroll3.offset;

              do
              {
                //      display.setU8g2Font(MenuTextFont);
                display.drawUTF8(MenuItemHeight + x, i * MenuItemHeight + MenuItemHeight - (MenuItemHeight - MenuTextHeight) / 2, Menu[i + menuOffset].Text.c_str());
                //      display.setU8g2Font(MenuIconFont);
                //      display.drawUTF8((MenuItemHeight - MenuTextHeight) / 2 + x, i * MenuItemHeight + MenuItemHeight - (MenuItemHeight - MenuTextHeight) / 2, Menu[i + menuOffset].Icon.c_str());

                //second text
                x += scroll3.width + scrollGapMenu;
              } while (x < DisplayWidth);
            }
          }
          else
          {
            //    display.setU8g2Font(MenuTextFont);
            display.drawUTF8(MenuItemHeight, i * MenuItemHeight + MenuItemHeight - (MenuItemHeight - MenuTextHeight) / 2, Menu[i + menuOffset].Text.c_str());
            //     display.setU8g2Font(MenuIconFont);
            //       display.drawUTF8((MenuItemHeight - MenuTextHeight) / 2, i * MenuItemHeight + MenuItemHeight - (MenuItemHeight - MenuTextHeight) / 2, Menu[i + menuOffset].Icon.c_str());
          }
        }

      display.setColor(63, 164, 101);

      //Highlight actual menu item
      if (MenuLength > MenuVisibleItems)
        display.drawFrame(0, MenuItemHeight * (menuPosition - menuOffset), MenuPixelWidth - 3, MenuItemHeight);
      else
        display.drawFrame(0, MenuItemHeight * (menuPosition - menuOffset), MenuPixelWidth, MenuItemHeight);

      //Show scrollbar if menu is longer than screen
      if (MenuLength > MenuVisibleItems)
      {

        display.setColor(0, 0, 0);
        display.drawBox(MenuPixelWidth - 3, 0, 3, MenuPixelHeight);
        //       display.setColorIndex(1);

        int barlen = (int)((float)MenuVisibleItems / (float)MenuLength * MenuPixelHeight);
        int barpos = (int)((float)menuOffset / (float)MenuLength * MenuPixelHeight);

        display.setColor(128, 128, 128);
        display.drawBox(MenuPixelWidth - 1, barpos, 1, barlen);
      }
    }

    //Display volume
    if (volumeDisplay)
    {
      //   float VolumePercent = float(volumio.State.volume) / (float)VolumeMaximum;
      float VolumePercent = float(newvolume) / (float)volumeMaximum;
      float barBoxHeight = 8;
      int posy = MenuItemHeight * 5 + (MenuItemHeight - barBoxHeight) / 2;

      drawProgressBar(0, posy, DisplayWidth, 1, VolumePercent);
    }
  }

  //display.clearScreen();

  //  display.drawBitmap24bit(0, 0, 128, 128, BitmapFonts);
  //display.drawBitmap24bit(14, 14, 100, 100, Fallen);
  /*  display.drawBitmap65k(9, 56, 110, 15, LogoVolumio65k);

  display.setColor(0, 255, 255);
  display.drawRect(0, py, 128, 16);
  display.drawString(0,0,"Playback");*/

  display.flush();

  if (send)
  {
    //Serial.write(STX);
    display.writeBuffer();
    // Serial.write(ETX);
  }

#else

  if (send)
    Serial.write(STX);

  display.setFontDirection(0);
  display.firstPage();

  u8g2_uint_t x;

  do
  {
    //Display on
    if (!noDisplay)
    {
      //Display toast message from Volumio
      if (toastDisplay)
      {
        display.setFont(ToastTextFont);
        int y = 0;
        int maxchars = 20;

        splitter.initialize(volumio.CurrentToastItem.title, maxchars,0);

        while (splitter.next())
          display.drawUTF8(0, 13 + MenuItemHeight * y++, splitter.line.c_str());

        splitter.initialize(volumio.CurrentToastItem.message, maxchars,0);

        while (splitter.next())
          display.drawUTF8(0, 13 + MenuItemHeight * y++, splitter.line.c_str());

        ui.drawProgressBar(4, DisplayHeight - 8, DisplayWidth - 8, 2, ((float)(toastStart + durationShowToast - now)) / (float)durationShowToast);
      }
      //Display player status
      else if (statusDisplay)
      {
        //Draw small Volumio logo
    //    display.drawXBM((DisplayWidth - logo_volumio_small_width) / 2, (MenuItemHeight - logo_volumio_small_height) / 2, logo_volumio_small_width, logo_volumio_small_height, logo_volumio_small_bits);

        //Draw status-icons (play/pause/strop and random and repeat)
     /*   int px = 8;
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
        scroll1.width = display.getUTF8Width(line1.c_str());

        if (scroll1.width < DisplayWidth)
          display.drawUTF8((DisplayWidth - scroll1.width) / 2, 16 * 3, line1.c_str());
        else
        //Scrolling text
        {
          x = scroll1.offset;

          if (scroll1.width > 0)
            do
            {
              display.drawUTF8(x, 48, line1.c_str());

              //second text
              x += scroll1.width + scrollGapStatus;
            } while (x < DisplayWidth);
        }

        //Text of second line centered or scrolling, depending on width
        scroll2.width = display.getUTF8Width(line2.c_str());

        if (scroll2.width < DisplayWidth)
          display.drawUTF8((DisplayWidth - scroll2.width) / 2, 16 * 4, line2.c_str());
        else
        //Scrolling text
        {
          x = scroll2.offset;

          if (scroll2.width > 0)
            do
            {
              display.drawUTF8(x, 48 + 16, line2.c_str());

              //second text
              x += scroll2.width + scrollGapStatus;
            } while (x < DisplayWidth);
        }*/
        
     display.setFont(StatusTextFont);
      int textpos = 0;

      int maxchars = 20;
      if (volumio.State.album != "null")
      {
        splitter.initialize(volumio.State.album, maxchars,0);
     
        while (splitter.next())
        {
          int width = display.getUTF8Width(splitter.line.c_str());
          display.drawUTF8((DisplayWidth - width) / 2,8+ 16 * textpos++, splitter.line.c_str());
        }
      }

      splitter.initialize(volumio.State.title, maxchars,'-');
    //  display.setColor(255, 255, 255);

      while (splitter.next())
      {
        int width = display.getUTF8Width(splitter.line.c_str());
        display.drawUTF8((DisplayWidth - width) / 2,16+ 16 * textpos++, splitter.line.c_str());
      }

      /*
      //Split longer texts if they contain a "-" (often on webradio)
      int splitIndex = volumio.State.title.indexOf(" - ");
*/
      splitter.initialize(volumio.State.artist, maxchars,0);
   //   display.setColor(63, 164, 101);

      while (splitter.next())
      {
        int width = display.getUTF8Width(splitter.line.c_str());
        display.drawUTF8((DisplayWidth - width) / 2,24+ 16 * textpos++, splitter.line.c_str());
      }



        if (volumio.State.duration > 0)
        {

          float SeekPercent = (float)volumio.State.seek / (float)volumio.State.duration / 1000.0;
          float barBoxHeight = 8;

          int posy = MenuItemHeight * 5 + (MenuItemHeight - barBoxHeight) / 2;

          ui.drawProgressBar(4, posy, DisplayWidth - 8, 0, SeekPercent);

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
              scroll3.width = display.getUTF8Width(Menu[i + menuOffset].Text.c_str()) + MenuItemHeight;

              if (scroll3.width <= MenuPixelWidth - 3 || now < lastmenuchange + delayScrollMenu || delayScrollMenu == 0)
              {
                scroll3.reset();
                display.setFont(MenuTextFont);
                display.drawUTF8(MenuItemHeight, i * MenuItemHeight + MenuItemHeight - (MenuItemHeight - MenuTextHeight) / 2, Menu[i + menuOffset].Text.c_str());
                display.setFont(MenuIconFont);
                display.drawUTF8((MenuItemHeight - MenuTextHeight) / 2, i * MenuItemHeight + MenuItemHeight - (MenuItemHeight - MenuTextHeight) / 2, Menu[i + menuOffset].Icon.c_str());
              }
              else
              //Scrolling text
              {
                x = scroll3.offset;

                do
                {
                  display.setFont(MenuTextFont);
                  display.drawUTF8(MenuItemHeight + x, i * MenuItemHeight + MenuItemHeight - (MenuItemHeight - MenuTextHeight) / 2, Menu[i + menuOffset].Text.c_str());
                  display.setFont(MenuIconFont);
                  display.drawUTF8((MenuItemHeight - MenuTextHeight) / 2 + x, i * MenuItemHeight + MenuItemHeight - (MenuItemHeight - MenuTextHeight) / 2, Menu[i + menuOffset].Icon.c_str());

                  //second text
                  x += scroll3.width + scrollGapMenu;
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
          display.drawFrame(0, MenuItemHeight * (menuPosition - menuOffset), MenuPixelWidth - 3, MenuItemHeight);
        else
          display.drawFrame(0, MenuItemHeight * (menuPosition - menuOffset), MenuPixelWidth, MenuItemHeight);

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
      if (volumeDisplay)
      {
        //   float VolumePercent = float(volumio.State.volume) / (float)VolumeMaximum;
        float VolumePercent = float(newvolume) / (float)volumeMaximum;
        float barBoxHeight = 8;
        int posy = MenuItemHeight * 5 + (MenuItemHeight - barBoxHeight) / 2;

        ui.drawProgressBar(0, posy, DisplayWidth, 1, VolumePercent);
      }
    }
    if (send)
      display.writeBufferXBM(Serial);
  } while (display.nextPage());

  if (send)
    Serial.write(ETX);

#endif

  /*#################################################################*\
  |* Scrolling Text
  \*#################################################################*/

  scroll1.scrollGapStatus = scrollGapStatus;
  scroll2.scrollGapStatus = scrollGapStatus;
  scroll3.scrollGapStatus = scrollGapMenu;

  scroll1.process();
  scroll2.process();
  scroll3.process();
}