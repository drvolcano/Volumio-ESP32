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

#include "Volumio.h"

//Uncomment line below to enable debugging messages on COM-port
//#define DEBUG_VOLUMIO

#ifdef DEBUG_VOLUMIO
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINT(x) Serial.print(x)
#else
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT(x)
#endif

void Volumio::connect(char *hostname, int port)
{
  if (!socketIoClient.connect(hostname, port))
  {
    DEBUG_PRINTLN("Volumio: connection failed");
  }
  if (socketIoClient.connected())
  {
    DEBUG_PRINTLN("Volumio: connected");
  }
}

bool Volumio::connected()
{
  return socketIoClient.connected();
}

bool Volumio::ReadPushToastMessage()
{
  //42["pushToastMessage",{"type":"success","title":"Entfernen","message":"Entferne Hell To The Heavens aus Warteschlange"}]

  if (!PushToastUpdate)
    return false;

  CurrentToastItem.message = "";
  CurrentToastItem.title = "";
  CurrentToastItem.type = "";

  while (Parser.next())
  {
    DEBUG_PRINT("Volumio: Receive: pushToastMessage: ");
    DEBUG_PRINT(Parser.getPath());
    DEBUG_PRINT(" = ");
    DEBUG_PRINTLN(Parser.getValue());

    if (Parser.getNode(2) == "message")
      CurrentToastItem.message = Parser.getValue();
    if (Parser.getNode(2) == "title")
      CurrentToastItem.title = Parser.getValue();
    if (Parser.getNode(2) == "type")
      CurrentToastItem.type = Parser.getValue();

    if (Parser.getBlockEnd())
    {
      while (Parser.next())
        ;

      return true;
    }
  }

  return false;
}

bool Volumio::ReadNextSourceItem()
{
  if (!SourceUpdate)
    return false;

  CurrentSourceItem.albumart = "";
  CurrentSourceItem.name = "";
  CurrentSourceItem.uri = "";
  CurrentSourceItem.plugin_type = "";
  CurrentSourceItem.plugin_name = "";
  CurrentSourceItem.icon = "";

  while (Parser.next())
  {
    DEBUG_PRINT("Volumio: Receive: pushBrowseSources: ");
    DEBUG_PRINT(Parser.getPath());
    DEBUG_PRINT(" = ");
    DEBUG_PRINTLN(Parser.getValue());

    if (Parser.getNode(3) == "albumart")
      CurrentSourceItem.albumart = Parser.getValue();
    if (Parser.getNode(3) == "name")
      CurrentSourceItem.name = Parser.getValue();
    if (Parser.getNode(3) == "uri")
      CurrentSourceItem.uri = Parser.getValue();
    if (Parser.getNode(3) == "plugin_type")
      CurrentSourceItem.plugin_type = Parser.getValue();
    if (Parser.getNode(3) == "plugin_name")
      CurrentSourceItem.plugin_name = Parser.getValue();
    if (Parser.getNode(3) == "icon")
      CurrentSourceItem.icon = Parser.getValue();

    if (Parser.getBlockEnd())
      return true;
  }

  return false;
}

bool Volumio::ReadNextQueueItem()
{
  if (!QueueUpdate)
    return false;

  CurrentQueueItem.uri = "";
  CurrentQueueItem.service = "";
  CurrentQueueItem.name = "";
  CurrentQueueItem.artist = "";
  CurrentQueueItem.album = "";
  CurrentQueueItem.type = "";
  CurrentQueueItem.tracknumber = "";
  CurrentQueueItem.albumart = "";
  CurrentQueueItem.duration = "";
  CurrentQueueItem.samplerate = "";
  CurrentQueueItem.bitdepth = "";
  CurrentQueueItem.trackType = "";
  CurrentQueueItem.channels = "";

  while (Parser.next())
  {
    DEBUG_PRINT("Volumio: Receive: pushQueue: ");
    DEBUG_PRINT(Parser.getPath());
    DEBUG_PRINT(" = ");
    DEBUG_PRINTLN(Parser.getValue());

    if (Parser.getNode(3) == "uri")
      CurrentQueueItem.uri = Parser.getValue();
    else if (Parser.getNode(3) == "service")
      CurrentQueueItem.service = Parser.getValue();
    else if (Parser.getNode(3) == "name")
      CurrentQueueItem.name = Parser.getValue();
    else if (Parser.getNode(3) == "artist")
      CurrentQueueItem.artist = Parser.getValue();
    else if (Parser.getNode(3) == "album")
      CurrentQueueItem.album = Parser.getValue();
    else if (Parser.getNode(3) == "type")
      CurrentQueueItem.type = Parser.getValue();
    else if (Parser.getNode(3) == "tracknumber")
      CurrentQueueItem.tracknumber = Parser.getValue();
    else if (Parser.getNode(3) == "albumart")
      CurrentQueueItem.albumart = Parser.getValue();
    else if (Parser.getNode(3) == "duration")
      CurrentQueueItem.duration = Parser.getValue();
    else if (Parser.getNode(3) == "samplerate")
      CurrentQueueItem.samplerate = Parser.getValue();
    else if (Parser.getNode(3) == "bitdepth")
      CurrentQueueItem.bitdepth = Parser.getValue();
    else if (Parser.getNode(3) == "trackType")
      CurrentQueueItem.trackType = Parser.getValue();
    else if (Parser.getNode(3) == "channels")
      CurrentQueueItem.channels = Parser.getValue();

    if (Parser.getBlockEnd())
      return true;
  }

  return false;
}

bool Volumio::ReadNextLibraryItem()
{
  if (!LibraryUpdate)
    return false;

  CurrentLibraryItem.service = "";
  CurrentLibraryItem.type = "";
  CurrentLibraryItem.title = "";
  CurrentLibraryItem.artist = "";
  CurrentLibraryItem.album = "";
  CurrentLibraryItem.uri = "";
  CurrentLibraryItem.albumart = "";
  CurrentLibraryItem.tracknumber = "";
  CurrentLibraryItem.duration = "";
  CurrentLibraryItem.trackType = "";
  CurrentLibraryItem.icon = "";

  while (Parser.next())
  {
    DEBUG_PRINT("Volumio: Receive: pushBrowseLibrary: ");
    DEBUG_PRINT(Parser.getPath());
    DEBUG_PRINT(" = ");
    DEBUG_PRINTLN(Parser.getValue());
    if (Parser.getLevel() == 7)
    {

      if (Parser.getNode(7) == "service")
        CurrentLibraryItem.service = Parser.getValue();
      else if (Parser.getNode(7) == "type")
        CurrentLibraryItem.type = Parser.getValue();
      else if (Parser.getNode(7) == "title")
        CurrentLibraryItem.title = Parser.getValue();
      else if (Parser.getNode(7) == "artist")
        CurrentLibraryItem.artist = Parser.getValue();
      else if (Parser.getNode(7) == "album")
        CurrentLibraryItem.album = Parser.getValue();
      else if (Parser.getNode(7) == "uri")
        CurrentLibraryItem.uri = Parser.getValue();
      else if (Parser.getNode(7) == "albumart")
        CurrentLibraryItem.albumart = Parser.getValue();
      else if (Parser.getNode(7) == "tracknumber")
        CurrentLibraryItem.tracknumber = Parser.getValue();
      else if (Parser.getNode(7) == "duration")
        CurrentLibraryItem.duration = Parser.getValue();
      else if (Parser.getNode(7) == "trackType")
        CurrentLibraryItem.trackType = Parser.getValue();
      else if (Parser.getNode(7) == "icon")
        CurrentLibraryItem.icon = Parser.getValue();

      if (Parser.getBlockEnd())
        return true;
    }
    if (Parser.getLevel() == 4)
    {
      if (Parser.getNode(3) == "prev")
      {
        if (Parser.getNode(4) == "uri")
          LibraryPrev.uri = Parser.getValue();
      }

      if (Parser.getNode(3) == "info")
      {
        if (Parser.getNode(4) == "uri")
          LibraryInfo.uri = Parser.getValue();
        if (Parser.getNode(4) == "title")
          LibraryInfo.title = Parser.getValue();
        if (Parser.getNode(4) == "service")
          LibraryInfo.service = Parser.getValue();
        if (Parser.getNode(4) == "type")
          LibraryInfo.type = Parser.getValue();
        if (Parser.getNode(4) == "albumart")
          LibraryInfo.albumart = Parser.getValue();
      }
    }
  }

  return false;
}

bool Volumio::ReadLibraryPrev()
{
  //if (!LibraryUpdate)
  //   return false;

  if (Parser.next())
  //if (Parser.Count == 4)
  {
    DEBUG_PRINT("Volumio: Receive: pushBrowseLibrary: #### ");
    DEBUG_PRINT(Parser.getPath());
    DEBUG_PRINT(" = ");
    DEBUG_PRINTLN(Parser.getValue());

    if (Parser.getNode(3) == "prev")
    {
      if (Parser.getNode(4) == "uri")
        LibraryPrev.uri = Parser.getValue();
    }

    if (Parser.getNode(3) == "info")
    {
      if (Parser.getNode(4) == "uri")
        LibraryInfo.uri = Parser.getValue();
      if (Parser.getNode(4) == "title")
        LibraryInfo.title = Parser.getValue();
      if (Parser.getNode(4) == "service")
        LibraryInfo.service = Parser.getValue();
      if (Parser.getNode(4) == "type")
        LibraryInfo.type = Parser.getValue();
      if (Parser.getNode(4) == "albumart")
        LibraryInfo.albumart = Parser.getValue();
    }
    return true;
  }

  return false;
}

void Volumio::ReadState()
{
  while (Parser.next())
  {
    if (Parser.getNode(2) == "status")
      State.status = Parser.getValue();
    if (Parser.getNode(2) == "position")
      State.position = Parser.getValue();
    if (Parser.getNode(2) == "title")
      State.title = Parser.getValue();
    if (Parser.getNode(2) == "artist")
      State.artist = Parser.getValue();
    if (Parser.getNode(2) == "album")
      State.album = Parser.getValue();
    if (Parser.getNode(2) == "albumart")
      State.albumart = Parser.getValue();
    if (Parser.getNode(2) == "uri")
      State.uri = Parser.getValue();
    if (Parser.getNode(2) == "trackType")
      State.trackType = Parser.getValue();
    if (Parser.getNode(2) == "seek")
      State.seek = atol(Parser.getValue().c_str());
    if (Parser.getNode(2) == "duration")
      State.duration = atol(Parser.getValue().c_str());
    if (Parser.getNode(2) == "samplerate")
      State.samplerate = Parser.getValue();
    if (Parser.getNode(2) == "bitdepth")
      State.bitdepth = Parser.getValue();
    if (Parser.getNode(2) == "channels")
      State.channels = Parser.getValue();
    if (Parser.getNode(2) == "random")
      State.random = Parser.getValue();
    if (Parser.getNode(2) == "repeat")
      State.repeat = Parser.getValue();
    if (Parser.getNode(2) == "repeatSingle")
      State.repeatSingle = Parser.getValue();
    if (Parser.getNode(2) == "consume")
      State.consume = Parser.getValue();
    if (Parser.getNode(2) == "volume")
      State.volume = atoi(Parser.getValue().c_str());
    if (Parser.getNode(2) == "mute")
      State.mute = Parser.getValue();
    if (Parser.getNode(2) == "disableVolumeControl")
      State.disableVolumeControl = Parser.getValue();
    if (Parser.getNode(2) == "stream")
      State.stream = Parser.getValue();
    if (Parser.getNode(2) == "updatedb")
      State.updatedb = Parser.getValue();
    if (Parser.getNode(2) == "service")
      State.service = Parser.getValue();
    if (Parser.getNode(2) == "Streaming")
      State.Streaming = Parser.getValue();

    DEBUG_PRINT("Volumio: Receive: pushState: ");
    DEBUG_PRINT(Parser.getNode(2));
    DEBUG_PRINT(" = ");
    DEBUG_PRINTLN(Parser.getValue());
  }

  DEBUG_PRINTLN("");
}

void Volumio::process()
{
  unsigned long currentMillis = millis();

  if (State.status == "play")
    State.seek += (currentMillis - previousMillis);

  previousMillis = currentMillis;

  StatusUpdate = false;
  SourceUpdate = false;
  LibraryUpdate = false;
  QueueUpdate = false;
  PushToastUpdate = false;

  if (socketIoClient.receive())
  {
    //link messagstream to JSON-parser
    Parser.initialize(&socketIoClient);

    //Process data as long as data generates lines
    if (Parser.next())
    {
      DEBUG_PRINT("Volumio: Receive: ");
      DEBUG_PRINTLN(Parser.getValue());

      if (Parser.getValue() == "pushToastMessage")
      {
        PushToastUpdate = true;
      }
      else if (Parser.getValue() == "pushBrowseLibrary")
      {
        LibraryUpdate = true;

        LibraryInfo.uri = "";
        LibraryInfo.title = "";
        LibraryInfo.service = "";
        LibraryInfo.type = "";
        LibraryInfo.albumart = "";
      }
      else if (Parser.getValue() == "pushBrowseSources")
        SourceUpdate = true;
      else if (Parser.getValue() == "pushQueue")
        QueueUpdate = true;
      else if (Parser.getValue() == "pushState")
        StatusUpdate = true;
      else
      {
        while (Parser.next())
        {
          DEBUG_PRINT(Parser.getPath());
          DEBUG_PRINT(" = ");
          DEBUG_PRINTLN(Parser.getValue());
        }
      }

      if (StatusUpdate)
      {
        ReadState();
      }
    }
  }
}