#include "main.h"

//#define DEBUG_MAIN

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

void MenuAction(MenuItemType type, String data)
{
  DEBUG_PRINT("ACTION: ");
  DEBUG_PRINT(target.Type);
  DEBUG_PRINTLN("#");

  WaitForSourceUpdate = false;
  WaitForQueueUpdate = false;
  WaitForLibraryUpdate = false;
  restore = false;

  switch (type)
  {
  case MENU_HOME:
    restore = true;
    MenuMain();
    break;
  case MENU_BACK:
    //Jump 2 menus up and then one down
    //Same effect as jumping one up, but minimizes stack size because only action, not full menu can be stored
    MenuPop();
    MenuPop();
    restore = true;
    MenuAction(Stack[stack_pos].Type, Stack[stack_pos].Data);
    break;
  case MENU_STATUS:
    StatusDisplay = true;
    break;
  case MENU_BROWSE:
    MenuPush();
    GenMenuStart();
    GenMenuItem(ICON_HOME, TEXT_HOME, MENU_HOME);
    WaitForSourceUpdate = true;
    volumio.getBrowseSources();
    break;
  case MENU_BROWSE_SOURCE:
    MenuPush();
    GenMenuStart();
    GenMenuItem(ICON_HOME, TEXT_HOME, MENU_HOME);
    GenMenuItem(ICON_BACK, Stack[stack_pos - 2].Text, MENU_BACK);
    WaitForLibraryUpdate = true;
    volumio.browseLibrary(data);
    break;
  case MENU_BROWSE_PLAYLIST:
    MenuPush();
    GenMenuStart();
    GenMenuItem(ICON_HOME, TEXT_HOME, MENU_HOME);
    GenMenuItem(ICON_BACK, Stack[stack_pos - 2].Text, MENU_BACK);
    WaitForLibraryUpdate = true;
    volumio.browseLibrary(data);
    break;
  case MENU_BROWSE_FOLDER:
    MenuPush();
    GenMenuStart();
    GenMenuItem(ICON_HOME, TEXT_HOME, MENU_HOME);
    GenMenuItem(ICON_BACK, Stack[stack_pos - 2].Text, MENU_BACK);
    WaitForLibraryUpdate = true;
    volumio.browseLibrary(data);
    break;
  case MENU_BROWSE_SONG:
    MenuPush();
    ContextMenuSong(data);
    break;
  case MENU_BROWSE_RADIOCATEGORY:
    MenuPush();
    GenMenuStart();
    GenMenuItem(ICON_HOME, TEXT_HOME, MENU_HOME);
    GenMenuItem(ICON_BACK, Stack[stack_pos - 2].Text, MENU_BACK);
    WaitForLibraryUpdate = true;
    volumio.browseLibrary(data);
    break;
  case MENU_BROWSE_WEBRADIO:
    MenuPush();
    ContextMenuWebradio(data);
    break;
  case MENU_BROWSE_STREAMINGCATEGORY:
    MenuPush();
    GenMenuStart();
    GenMenuItem(ICON_HOME, TEXT_HOME, MENU_HOME);
    GenMenuItem(ICON_BACK, Stack[stack_pos - 2].Text, MENU_BACK);
    WaitForLibraryUpdate = true;
    volumio.browseLibrary(data);
    break;
  case MENU_BROWSE_FOLDER_PLAY:
    volumio.addPlay(data);
    break;
  case MENU_BROWSE_FOLDER_ADDTOQUEUE:
    volumio.addToQueue(data);
    break;
  case MENU_BROWSE_FOLDER_CLEARANDPLAY:
    volumio.replaceAndPlay(data);
    break;
  case MENU_BROWSE_FOLDER_ADDTOPLAYLIST:
    break;
  case MENU_BROWSE_FOLDER_ADDTOFAVORITES:
    break;
  case MENU_BROWSE_SONG_PLAY:
    volumio.addPlay(data);
    break;
  case MENU_BROWSE_SONG_ADDTOQUEUE:
    volumio.addToQueue(data);
    break;
  case MENU_BROWSE_SONG_CLEARANDPLAY:
    volumio.replaceAndPlay(data);
    break;
  case MENU_BROWSE_SONG_ADDTOPLAYLIST:
    break;
  case MENU_BROWSE_SONG_ADDTOFAVORITES:
    break;
  case MENU_BROWSE_WEBRADIO_PLAY:
    volumio.addPlay("webradio", Stack[stack_pos - 1].Text, Stack[stack_pos - 1].Data);
    break;
  case MENU_BROWSE_WEBRADIO_ADDTOQUEUE:
    volumio.addToQueue(data);
    break;
  case MENU_BROWSE_WEBRADIO_CLEARANDPLAY:
    volumio.replaceAndPlay(data);
    break;
  case MENU_BROWSE_WEBRADIO_ADDTOPLAYLIST:
    break;
  case MENU_BROWSE_WEBRADIO_ADDTOFAVORITES:
    break;
  case MENU_PLAYBACK:
    MenuPush();
    MenuPlayback();
    break;
  case MENU_PLAYBACK_PREV:
    volumio.prev();
    break;
  case MENU_PLAYBACK_NEXT:
    volumio.next();
    break;
  case MENU_PLAYBACK_PLAY:
    volumio.play();
    break;
  case MENU_PLAYBACK_PAUSE:
    volumio.pause();
    break;
  case MENU_PLAYBACK_TOGGLE:
    volumio.toggle();
    break;
  case MENU_PLAYBACK_STOP:
    volumio.stop();
    break;
  case MENU_PLAYBACK_RANDOM_SET:
    volumio.setRandom(true);
    break;
  case MENU_PLAYBACK_RANDOM_RESET:
    volumio.setRandom(false);
    break;
  case MENU_PLAYBACK_RANDOM_SET_RESET:
    volumio.setRandom(volumio.State.random == "false");
    break;
  case MENU_PLAYBACK_REPEAT_SET:
    volumio.setRepeat(true);
    break;
  case MENU_PLAYBACK_REPEAT_RESET:
    volumio.setRepeat(false);
    break;
  case MENU_PLAYBACK_REPEAT_SET_RESET:
    volumio.setRepeat(volumio.State.repeat == "false");
    break;
  case MENU_PLAYBACK_FAVORITE_SET:
    break;
  case MENU_PLAYBACK_FAVORITE_RESET:
    break;
  case MENU_PLAYBACK_FAVORITE_SET_RESET:
    break;
  case MENU_PLAYBACK_PLAYLIST_ADD:
    break;
  case MENU_PLAYBACK_MUTE_SET:
    volumio.mute();
    break;
  case MENU_PLAYBACK_MUTE_RESET:
    volumio.unmute();
    break;
  case MENU_PLAYBACK_MUTE_SET_RESET:
    break;
  case MENU_QUEUE:
    MenuPush();
    GenMenuStart();
    GenMenuItem(ICON_HOME, TEXT_HOME, MENU_HOME);
    GenMenuItem(ICON_CLEAR, TEXT_CLEAR, MENU_QUEUE_CLEAR);
    WaitForQueueUpdate = true;
    volumio.getQueue();
    break;
  case MENU_QUEUE_CLEAR:
    volumio.clearQueue();
    GenMenuStart();
    GenMenuItem(ICON_HOME, TEXT_HOME, MENU_HOME);
    GenMenuItem(ICON_CLEAR, TEXT_CLEAR, MENU_QUEUE_CLEAR);
    WaitForQueueUpdate = true;
    volumio.getQueue();
    break;
  case MENU_QUEUE_RANDOM:
    break;
  case MENU_QUEUE_REPEAT:
    break;
  case MENU_QUEUE_TRACK:
    MenuPush();
    ContextMenuQueue(atoi(data.c_str()));
    break;
  case MENU_QUEUE_TRACK_DELETE:
    volumio.removeQueueItem(atoi(data.c_str()));
    MenuPop();
    MenuPop();
    MenuAction(Stack[stack_pos].Type, Stack[stack_pos].Data);
    break;
  case MENU_QUEUE_TRACK_PLAY:
    volumio.play(atoi(data.c_str()));
    MenuPop();
    MenuPop();
    MenuAction(Stack[stack_pos].Type, Stack[stack_pos].Data);
    break;
  case MENU_SYSTEM:
    MenuSystem();
    break;
  case MENU_DARK:
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
  pinMode(PIN_LeftEncoder_CLK, INPUT_PULLUP);
  pinMode(PIN_LeftEncoder_DT, INPUT_PULLUP);
  LeftEncoder.begin(PIN_LeftEncoder_CLK, PIN_LeftEncoder_DT);
  attachInterrupt(PIN_LeftEncoder_CLK, ISR_LeftEncoder_A, CHANGE);
  attachInterrupt(PIN_LeftEncoder_DT, ISR_LeftEncoder_B, CHANGE);

  //Initialize right encoder
  pinMode(PIN_RightEncoder_CLK, INPUT_PULLUP);
  pinMode(PIN_RightEncoder_DT, INPUT_PULLUP);
  RightEncoder.begin(PIN_RightEncoder_CLK, PIN_RightEncoder_DT);
  attachInterrupt(PIN_RightEncoder_CLK, ISR_RightEncoder_A, CHANGE);
  attachInterrupt(PIN_RightEncoder_DT, ISR_RightEncoder_B, CHANGE);

  //Initialize push button of left encoder
  pinMode(PIN_LeftEncoder_SW, INPUT_PULLUP);
  LeftSwitch.begin(PIN_LeftEncoder_SW);

  //Initialize push button of right encoder
  pinMode(PIN_RightEncoder_SW, INPUT_PULLUP);
  RightSwitch.begin(PIN_RightEncoder_SW);

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

  //######################################################################
  // Connection to WiFi and Voulumio
  //######################################################################

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

      MenuMain();
    }
  }

  //######################################################################
  // Process data from Volumio (Updates)
  //######################################################################

  volumio.process();

  //Volumio pushes list of sources
  if (volumio.SourceUpdate)
  {
    DEBUG_PRINTLN("MAIN: Sourceupdate");

    //Only if push was requested
    if (WaitForSourceUpdate)
    {
      while (volumio.ReadNextSourceItem())
        GenMenuItem(ICON_ARROW, volumio.CurrentSourceItem.name, MENU_BROWSE_SOURCE, volumio.CurrentSourceItem.uri);

      GenMenuEnd();

      if (restore && false)
      {
        restore = false;
        LeftEncoder.set(Stack[stack_pos].Position);
        MenuOffset = Stack[stack_pos].Offset;
      }
    }
  }

  //Volumio pushes library content
  if (volumio.LibraryUpdate)
  {
    DEBUG_PRINTLN("MAIN: LibraryUpdate");

    //Only if push was requested
    if (WaitForLibraryUpdate)
    {
      volumio.LibraryPrev.uri = "";

      while (volumio.ReadNextLibraryItem())
      {
        DEBUG_PRINTLN(volumio.CurrentLibraryItem.title);

        if (volumio.CurrentLibraryItem.type == "song")
          GenMenuItem(ICON_SONG, volumio.CurrentLibraryItem.title, MENU_BROWSE_SONG, volumio.CurrentLibraryItem.uri);
        else if (volumio.CurrentLibraryItem.type == "folder")
          GenMenuItem(ICON_ARROW, volumio.CurrentLibraryItem.title, MENU_BROWSE_FOLDER, volumio.CurrentLibraryItem.uri);
        else if (volumio.CurrentLibraryItem.type == "radio-category")
          GenMenuItem(ICON_ARROW, volumio.CurrentLibraryItem.title, MENU_BROWSE_RADIOCATEGORY, volumio.CurrentLibraryItem.uri);
        else if (volumio.CurrentLibraryItem.type == "mywebradio-category")
          GenMenuItem(ICON_ARROW, volumio.CurrentLibraryItem.title, MENU_BROWSE_RADIOCATEGORY, volumio.CurrentLibraryItem.uri);
        else if (volumio.CurrentLibraryItem.type == "radio-favourites")
          GenMenuItem(ICON_ARROW, volumio.CurrentLibraryItem.title, MENU_BROWSE_RADIOCATEGORY, volumio.CurrentLibraryItem.uri);
        else if (volumio.CurrentLibraryItem.type == "streaming-category")
          GenMenuItem(ICON_ARROW, volumio.CurrentLibraryItem.title, MENU_BROWSE_STREAMINGCATEGORY, volumio.CurrentLibraryItem.uri);
        else if (volumio.CurrentLibraryItem.type == "webradio")
          GenMenuItem(ICON_ARROW, volumio.CurrentLibraryItem.title, MENU_BROWSE_WEBRADIO, volumio.CurrentLibraryItem.uri);
        else if (volumio.CurrentLibraryItem.type == "playlist")
          GenMenuItem(ICON_ARROW, volumio.CurrentLibraryItem.title, MENU_BROWSE_PLAYLIST, volumio.CurrentLibraryItem.uri);
        else
          GenMenuItem(ICON_ARROW, volumio.CurrentLibraryItem.type, MENU_HOME);
      }

      while (volumio.ReadLibraryPrev())
        ;

      DEBUG_PRINTLN(volumio.LibraryPrev.uri);

      //If there is an uri for full library, add play options
      if (volumio.LibraryInfo.uri != "")
      {
        GenMenuItem(ICON_PLAY, TEXT_BROWSE_FOLDER_PLAY, MENU_BROWSE_FOLDER_PLAY, volumio.LibraryInfo.uri);
        GenMenuItem(ICON_LIST, TEXT_BROWSE_FOLDER_ADDTOQUEUE, MENU_BROWSE_FOLDER_ADDTOQUEUE, volumio.LibraryInfo.uri);
        GenMenuItem(ICON_LIST, TEXT_BROWSE_FOLDER_CLEARANDPLAY, MENU_BROWSE_FOLDER_CLEARANDPLAY, volumio.LibraryInfo.uri);
      }

      GenMenuEnd();

      if (restore && false)
      {
        restore = false;
        LeftEncoder.set(Stack[stack_pos].Position);
        MenuOffset = Stack[stack_pos].Offset;
      }
    }
  }

  //Volumio pushes queue content
  if (volumio.QueueUpdate)
  {
    DEBUG_PRINTLN("MAIN: QueueUpdate");

    //Only if push was requested
    if (WaitForQueueUpdate)
    {
      //Queue is referenced by index, not uri
      int index = 0;

      while (volumio.ReadNextQueueItem())
        if (volumio.CurrentQueueItem.name != "")
          GenMenuItem(ICON_SONG, volumio.CurrentQueueItem.name, MENU_QUEUE_TRACK, String(index++));

      GenMenuEnd();

      if (restore && false)
      {
        restore = false;
        LeftEncoder.set(Stack[stack_pos].Position);
        MenuOffset = Stack[stack_pos].Offset;
      }
    }
  }

  //Volumio pushes toast message
  if (volumio.PushToastUpdate)
  {
    DEBUG_PRINTLN("MAIN: PushToastUpdate");

    if (volumio.ReadPushToastMessage())
    {
      ToastDisplay = true;
      ToastStart = now;
    }
  }

  if (ToastDisplay)
  {
    if (now > ToastStart + ToastDuration)
      ToastDisplay = false;
  }

  //######################################################################
  // Touch pins
  //######################################################################

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

  //######################################################################
  // Left switch - navigate through menu
  //######################################################################

  LeftSwitch.process();

  if (LeftSwitch.pressed)
  {
    //Memorize last input timesamp for screensaver
    lastinput = now;
    lastmenuchange= now;

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
      else if (StatusDisplay)
      {
        StatusDisplay = false;
      }
      //Activate menu-item if in menu
      else
      {
        MenuAction(Menu[MenuPosition].Type, Menu[MenuPosition].Data);
      }
    }
  }

  //######################################################################
  // Right switch - toggle play/pause (play/stop on radio)
  //######################################################################

  RightSwitch.process();

  if (RightSwitch.pressed)
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

  //######################################################################
  // Left rotary encoder - scroll through menu
  //######################################################################

  LeftEncoder.process();

  if (LeftEncoder.changed)
  {
    //Memorize last input timesamp for screensaver
    lastinput = now;
    lastmenuchange= now;
    item_offset = 0;
  }

  //Do nothing if statusdisplay active
  if (StatusDisplay)
  {
  }
  else
  {

    if (restore && !WaitForLibraryUpdate && !WaitForQueueUpdate && !WaitForSourceUpdate)
    {
      restore = false;
      LeftEncoder.set(Stack[stack_pos].Position);
      MenuOffset = Stack[stack_pos].Offset;
    }

    //Limit encoder value to menu length
    if (LeftEncoder.Value > (MenuLength - 1))
      LeftEncoder.set(MenuLength - 1);
    if (LeftEncoder.Value < 0)
      LeftEncoder.set(0);

    //Position of menu = encoder-value
    MenuPosition = LeftEncoder.Value;

    //Scroll menu when encoder reaches lower end of screen
    if (MenuPosition >= MenuVisibleItems + MenuOffset - 1)
      MenuOffset = MenuPosition - MenuVisibleItems + 1;

    //Scroll menu when encoder reaches upper end of screen
    if (MenuPosition < MenuOffset)
      MenuOffset = MenuPosition;
  }

  //######################################################################
  // Right rotary encoder - volume up/down
  //######################################################################

  RightEncoder.process();

  if (RightEncoder.changed)
  {
    //Memorize last input timesamp for screensaver
    lastinput = now;

    //Show volume progress bar
    volumeprogress = true;
    volumedisplay = true;
    lastsetvolume = now;
  }

  if (volumeprogress)
  {
    newvolume = startvolume - ((RightEncoder.Value - startcnt));

    if (newvolume < VolumeMinimum)
      newvolume = VolumeMinimum;

    if (newvolume > VolumeMaximum)
      newvolume = VolumeMaximum;

    volumio.State.volume = newvolume;

    if ((now - lastsetvolume) > VolumeSetDelay)
    {
      volumio.volume(newvolume);
      volumeprogress = false;
    }
  }
  //Memorize start values when volume update not in progress
  else
  {
    startcnt = RightEncoder.Value;
    startvolume = volumio.State.volume;
  }

  //Close progressbar after given time
  if (volumedisplay)
  {
    if ((now - lastsetvolume) > VolumeDuration)
      volumedisplay = false;
  }

  //######################################################################
  // Timer
  //######################################################################

  //After some time without action automatically go back to status display
  if (delayBackStatus > 0)
    if (now - lastinput > delayBackStatus && volumio.State.status != "")
      StatusDisplay = true;

  //After some time without action automatically switch off display
  if (delayDisplayOff > 0)
    if (now - lastinput > delayDisplayOff && volumio.State.status != "")
      NoDisplay = true;

  //After some time without action automatically switch off display, when in stop
  if (delayDisplayOffWhenNotPlay > 0)
    if (now - lastinput > delayDisplayOffWhenNotPlay && volumio.State.status != "" && volumio.State.status != "play")
      NoDisplay = true;

  //######################################################################
  // Display
  //######################################################################

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
      else if (StatusDisplay)
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

        float SeekPercent = (float)volumio.State.seek / (float)volumio.State.duration / 1000.0;
      }
      else
      {
        bool style2 = false;

        if (style2)
          display.drawRBox(0, MenuItemHeight * (MenuPosition - MenuOffset), MenuPixelWidth - 3, MenuItemHeight, 0);

        //Write menu texts
        for (int i = 0; i < MenuVisibleItems; i++)
          if (i + MenuOffset < MenuLength)
          {

            if (i == MenuPosition - MenuOffset)
            {
              if (style2)
                display.setColorIndex(0);
              else
                display.setColorIndex(1);

              display.setFont(MenuTextFont);
              item_width = display.getUTF8Width(Menu[i + MenuOffset].Text.c_str()) + MenuItemHeight;

              if (item_width <= MenuPixelWidth - 3 || now < lastmenuchange + delayScrollMenu )
              {  
                 item_offset = 0;
                display.setFont(MenuTextFont);
                display.drawUTF8(MenuItemHeight, i * MenuItemHeight + MenuItemHeight - (MenuItemHeight - MenuTextHeight) / 2, Menu[i + MenuOffset].Text.c_str());
                display.setFont(MenuIconFont);
                display.drawUTF8((MenuItemHeight - MenuTextHeight) / 2, i * MenuItemHeight + MenuItemHeight - (MenuItemHeight - MenuTextHeight) / 2, Menu[i + MenuOffset].Icon.c_str());
              }
              else
              //Scrolling text
              {
                x = item_offset;

                do
                {
                  display.setFont(MenuTextFont);
                  display.drawUTF8(MenuItemHeight + x, i * MenuItemHeight + MenuItemHeight - (MenuItemHeight - MenuTextHeight) / 2, Menu[i + MenuOffset].Text.c_str());
                  display.setFont(MenuIconFont);
                  display.drawUTF8((MenuItemHeight - MenuTextHeight) / 2 + x, i * MenuItemHeight + MenuItemHeight - (MenuItemHeight - MenuTextHeight) / 2, Menu[i + MenuOffset].Icon.c_str());

                  //second text
                  x += item_width + scrollGapMenu;
                } while (x < DisplayWidth);
              }
            }
            else
            {
              display.setFont(MenuTextFont);
              display.drawUTF8(MenuItemHeight, i * MenuItemHeight + MenuItemHeight - (MenuItemHeight - MenuTextHeight) / 2, Menu[i + MenuOffset].Text.c_str());
              display.setFont(MenuIconFont);
              display.drawUTF8((MenuItemHeight - MenuTextHeight) / 2, i * MenuItemHeight + MenuItemHeight - (MenuItemHeight - MenuTextHeight) / 2, Menu[i + MenuOffset].Icon.c_str());
            }
          }

        //Highlight actual menu item
        if (MenuLength > MenuVisibleItems)
          display.drawRFrame(0, MenuItemHeight * (MenuPosition - MenuOffset), MenuPixelWidth - 3, MenuItemHeight, 0);
        else
          display.drawRFrame(0, MenuItemHeight * (MenuPosition - MenuOffset), MenuPixelWidth, MenuItemHeight, 0);

        //Show scrollbar if menu is longer than screen
        if (MenuLength > MenuVisibleItems)
        {
          display.setColorIndex(0);
          display.drawRBox(MenuPixelWidth - 3, 0, 3, MenuPixelHeight, 0);
          display.setColorIndex(1);

          int barlen = (int)((float)MenuVisibleItems / (float)MenuLength * MenuPixelHeight);
          int barpos = (int)((float)MenuOffset / (float)MenuLength * MenuPixelHeight);
          display.drawLine(MenuPixelWidth - 1, barpos, MenuPixelWidth - 1, barpos + barlen);
        }
      }

      //Display volume
      if (volumedisplay)
      {
        float VolumePercent = float(volumio.State.volume) / (float)VolumeMaximum;

        float barlen = VolumePercent * (DisplayWidth - 4);
        float barBoxHeight = 8;
        float barHeight = 4;

        int posy = MenuItemHeight * 5 + (MenuItemHeight - barBoxHeight) / 2;

        display.setColorIndex(0);
        display.drawRBox(0, posy, DisplayWidth, barBoxHeight, 0);
        display.setColorIndex(1);

        display.drawRFrame(0, posy, DisplayWidth, barBoxHeight, 0);
        display.drawRBox(2, posy + (barBoxHeight - barHeight) / 2, int(barlen), barHeight, 0);
      }
    }
  } while (display.nextPage());

  //######################################################################
  // Scrolling text
  //######################################################################

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

  //######################################################################
  // Send screenshot to PC
  //######################################################################

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