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

  while (Parser.Read())
  {
    DEBUG_PRINT("Volumio: Receive: pushToastMessage: ");
    DEBUG_PRINT(Parser.GetPath());
    DEBUG_PRINT(" = ");
    DEBUG_PRINTLN(Parser.Value);

    if (Parser.Nodes[2] == "message")
      CurrentToastItem.message = Parser.Value;
    if (Parser.Nodes[2] == "title")
      CurrentToastItem.title = Parser.Value;
    if (Parser.Nodes[2] == "type")
      CurrentToastItem.type = Parser.Value;

    if (Parser.Popped)
    {
      while (Parser.Read())
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

  while (Parser.Read())
  {
    DEBUG_PRINT("Volumio: Receive: pushBrowseSources: ");
    DEBUG_PRINT(Parser.GetPath());
    DEBUG_PRINT(" = ");
    DEBUG_PRINTLN(Parser.Value);

    if (Parser.Nodes[3] == "albumart")
      CurrentSourceItem.albumart = Parser.Value;
    if (Parser.Nodes[3] == "name")
      CurrentSourceItem.name = Parser.Value;
    if (Parser.Nodes[3] == "uri")
      CurrentSourceItem.uri = Parser.Value;
    if (Parser.Nodes[3] == "plugin_type")
      CurrentSourceItem.plugin_type = Parser.Value;
    if (Parser.Nodes[3] == "plugin_name")
      CurrentSourceItem.plugin_name = Parser.Value;
    if (Parser.Nodes[3] == "icon")
      CurrentSourceItem.icon = Parser.Value;

    if (Parser.Popped)
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

  while (Parser.Read())
  {
    DEBUG_PRINT("Volumio: Receive: pushQueue: ");
    DEBUG_PRINT(Parser.GetPath());
    DEBUG_PRINT(" = ");
    DEBUG_PRINTLN(Parser.Value);

    if (Parser.Nodes[3] == "uri")
      CurrentQueueItem.uri = Parser.Value;
    else if (Parser.Nodes[3] == "service")
      CurrentQueueItem.service = Parser.Value;
    else if (Parser.Nodes[3] == "name")
      CurrentQueueItem.name = Parser.Value;
    else if (Parser.Nodes[3] == "artist")
      CurrentQueueItem.artist = Parser.Value;
    else if (Parser.Nodes[3] == "album")
      CurrentQueueItem.album = Parser.Value;
    else if (Parser.Nodes[3] == "type")
      CurrentQueueItem.type = Parser.Value;
    else if (Parser.Nodes[3] == "tracknumber")
      CurrentQueueItem.tracknumber = Parser.Value;
    else if (Parser.Nodes[3] == "albumart")
      CurrentQueueItem.albumart = Parser.Value;
    else if (Parser.Nodes[3] == "duration")
      CurrentQueueItem.duration = Parser.Value;
    else if (Parser.Nodes[3] == "samplerate")
      CurrentQueueItem.samplerate = Parser.Value;
    else if (Parser.Nodes[3] == "bitdepth")
      CurrentQueueItem.bitdepth = Parser.Value;
    else if (Parser.Nodes[3] == "trackType")
      CurrentQueueItem.trackType = Parser.Value;
    else if (Parser.Nodes[3] == "channels")
      CurrentQueueItem.channels = Parser.Value;

    if (Parser.Popped)
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

  while (Parser.Read())
  {
    DEBUG_PRINT("Volumio: Receive: pushBrowseLibrary: ");
    DEBUG_PRINT(Parser.GetPath());
    DEBUG_PRINT(" = ");
    DEBUG_PRINTLN(Parser.Value);
    if (Parser.Count == 7)
    {

      if (Parser.Nodes[7] == "service")
        CurrentLibraryItem.service = Parser.Value;
      else if (Parser.Nodes[7] == "type")
        CurrentLibraryItem.type = Parser.Value;
      else if (Parser.Nodes[7] == "title")
        CurrentLibraryItem.title = Parser.Value;
      else if (Parser.Nodes[7] == "artist")
        CurrentLibraryItem.artist = Parser.Value;
      else if (Parser.Nodes[7] == "album")
        CurrentLibraryItem.album = Parser.Value;
      else if (Parser.Nodes[7] == "uri")
        CurrentLibraryItem.uri = Parser.Value;
      else if (Parser.Nodes[7] == "albumart")
        CurrentLibraryItem.albumart = Parser.Value;
      else if (Parser.Nodes[7] == "tracknumber")
        CurrentLibraryItem.tracknumber = Parser.Value;
      else if (Parser.Nodes[7] == "duration")
        CurrentLibraryItem.duration = Parser.Value;
      else if (Parser.Nodes[7] == "trackType")
        CurrentLibraryItem.trackType = Parser.Value;
      else if (Parser.Nodes[7] == "icon")
        CurrentLibraryItem.icon = Parser.Value;

      if (Parser.Popped)
        return true;
    }
    if (Parser.Count == 4)
    {
      if (Parser.Nodes[3] == "prev")
      {
        if (Parser.Nodes[4] == "uri")
          LibraryPrev.uri = Parser.Value;
      }

      if (Parser.Nodes[3] == "info")
      {
        if (Parser.Nodes[4] == "uri")
          LibraryInfo.uri = Parser.Value;
        if (Parser.Nodes[4] == "title")
          LibraryInfo.title = Parser.Value;
        if (Parser.Nodes[4] == "service")
          LibraryInfo.service = Parser.Value;
        if (Parser.Nodes[4] == "type")
          LibraryInfo.type = Parser.Value;
        if (Parser.Nodes[4] == "albumart")
          LibraryInfo.albumart = Parser.Value;
      }
    }
  }

  return false;
}

bool Volumio::ReadLibraryPrev()
{
  //if (!LibraryUpdate)
  //   return false;

  if (Parser.Read())
  //if (Parser.Count == 4)
  {
    DEBUG_PRINT("Volumio: Receive: pushBrowseLibrary: #### ");
    DEBUG_PRINT(Parser.GetPath());
    DEBUG_PRINT(" = ");
    DEBUG_PRINTLN(Parser.Value);

    if (Parser.Nodes[3] == "prev")
    {
      if (Parser.Nodes[4] == "uri")
        LibraryPrev.uri = Parser.Value;
    }

    if (Parser.Nodes[3] == "info")
    {
      if (Parser.Nodes[4] == "uri")
        LibraryInfo.uri = Parser.Value;
      if (Parser.Nodes[4] == "title")
        LibraryInfo.title = Parser.Value;
      if (Parser.Nodes[4] == "service")
        LibraryInfo.service = Parser.Value;
      if (Parser.Nodes[4] == "type")
        LibraryInfo.type = Parser.Value;
      if (Parser.Nodes[4] == "albumart")
        LibraryInfo.albumart = Parser.Value;
    }
    return true;
  }

  return false;
}

void Volumio::ReadState()
{
  while (Parser.Read())
  {
    if (Parser.Nodes[2] == "status")
      State.status = Parser.Value;
    if (Parser.Nodes[2] == "position")
      State.position = Parser.Value;
    if (Parser.Nodes[2] == "title")
      State.title = Parser.Value;
    if (Parser.Nodes[2] == "artist")
      State.artist = Parser.Value;
    if (Parser.Nodes[2] == "album")
      State.album = Parser.Value;
    if (Parser.Nodes[2] == "albumart")
      State.albumart = Parser.Value;
    if (Parser.Nodes[2] == "uri")
      State.uri = Parser.Value;
    if (Parser.Nodes[2] == "trackType")
      State.trackType = Parser.Value;
    if (Parser.Nodes[2] == "seek")
      State.seek = atol(Parser.Value.c_str());
    if (Parser.Nodes[2] == "duration")
      State.duration = atol(Parser.Value.c_str());
    if (Parser.Nodes[2] == "samplerate")
      State.samplerate = Parser.Value;
    if (Parser.Nodes[2] == "bitdepth")
      State.bitdepth = Parser.Value;
    if (Parser.Nodes[2] == "channels")
      State.channels = Parser.Value;
    if (Parser.Nodes[2] == "random")
      State.random = Parser.Value;
    if (Parser.Nodes[2] == "repeat")
      State.repeat = Parser.Value;
    if (Parser.Nodes[2] == "repeatSingle")
      State.repeatSingle = Parser.Value;
    if (Parser.Nodes[2] == "consume")
      State.consume = Parser.Value;
    if (Parser.Nodes[2] == "volume")
      State.volume = atoi(Parser.Value.c_str());
    if (Parser.Nodes[2] == "mute")
      State.mute = Parser.Value;
    if (Parser.Nodes[2] == "disableVolumeControl")
      State.disableVolumeControl = Parser.Value;
    if (Parser.Nodes[2] == "stream")
      State.stream = Parser.Value;
    if (Parser.Nodes[2] == "updatedb")
      State.updatedb = Parser.Value;
    if (Parser.Nodes[2] == "service")
      State.service = Parser.Value;
    if (Parser.Nodes[2] == "Streaming")
      State.Streaming = Parser.Value;

    DEBUG_PRINT("Volumio: Receive: pushState: ");
    DEBUG_PRINT(Parser.Nodes[2]);
    DEBUG_PRINT(" = ");
    DEBUG_PRINTLN(Parser.Value);
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
    Parser.ConnectToStream(&socketIoClient);

    //Process data as long as data generates lines
    if (Parser.Read())
    {
      DEBUG_PRINT("Volumio: Receive: ");
      DEBUG_PRINTLN(Parser.Value);

      if (Parser.Value == "pushToastMessage")
      {
        PushToastUpdate = true;
      }
      else if (Parser.Value == "pushBrowseLibrary")
      {
        LibraryUpdate = true;

        LibraryInfo.uri = "";
        LibraryInfo.title = "";
        LibraryInfo.service = "";
        LibraryInfo.type = "";
        LibraryInfo.albumart = "";
      }
      else if (Parser.Value == "pushBrowseSources")
        SourceUpdate = true;
      else if (Parser.Value == "pushQueue")
        QueueUpdate = true;
      else if (Parser.Value == "pushState")
        StatusUpdate = true;
      else
      {
        while (Parser.Read())
        {
          DEBUG_PRINT(Parser.GetPath());
          DEBUG_PRINT(" = ");
          DEBUG_PRINTLN(Parser.Value);
        }
      }

      if (StatusUpdate)
      {
        ReadState();
      }
    }
  }
}