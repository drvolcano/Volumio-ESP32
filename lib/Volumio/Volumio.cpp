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

#include "LibDebug.h"
#if (DEBUGLEVEL_VOLUMIO >= 1)
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINT(x) Serial.print(x)
#else
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT(x)
#endif

void Volumio::connect(String hostname, int port)
{
  lastTime = millis();

  if (!socketIoClient.connect(hostname, port))
    DEBUG_PRINTLN("Volumio: connect(): connection failed");

  if (socketIoClient.getConnected())
    DEBUG_PRINTLN("Volumio: connect(): connected");
}

bool Volumio::getConnected()
{
  return socketIoClient.getConnected();
}

bool Volumio::readPushToastMessage()
{
  if (pushType != pushToastMessage)
  {
    DEBUG_PRINTLN("Volumio: readPushToastMessage(): ERROR, no data avaliable");
    return false;
  }

  CurrentToastItem.message = "";
  CurrentToastItem.title = "";
  CurrentToastItem.type = "";

  while (jsonParser.next())
  {
    DEBUG_PRINT("Volumio: readPushToastMessage(): ");
    DEBUG_PRINT(jsonParser.getPath());
    DEBUG_PRINT(" = ");
    DEBUG_PRINTLN(jsonParser.getValue());

    if (jsonParser.getNode() == "message")
      CurrentToastItem.message = jsonParser.getValue();
    else if (jsonParser.getNode() == "title")
      CurrentToastItem.title = jsonParser.getValue();
    else if (jsonParser.getNode() == "type")
      CurrentToastItem.type = jsonParser.getValue();
    else
    {
      DEBUG_PRINTLN("UNKNOWN ITEM!");
    }

    if (jsonParser.getBlockEnd())
      return true;
  }

  return false;
}

bool Volumio::readNextSourceItem()
{
  if (pushType != pushBrowseSources)
  {
    DEBUG_PRINTLN("Volumio: readNextSourceItem(): ERROR, no data avaliable");
    return false;
  }

  CurrentSourceItem.albumart = "";
  CurrentSourceItem.name = "";
  CurrentSourceItem.uri = "";
  CurrentSourceItem.plugin_type = "";
  CurrentSourceItem.plugin_name = "";
  CurrentSourceItem.icon = "";

  while (jsonParser.next())
  {
    DEBUG_PRINT("Volumio: readNextSourceItem(): ");
    DEBUG_PRINT(jsonParser.getPath());
    DEBUG_PRINT(" = ");
    DEBUG_PRINTLN(jsonParser.getValue());

    if (jsonParser.getNode() == "albumart")
      CurrentSourceItem.albumart = jsonParser.getValue();
    else if (jsonParser.getNode() == "name")
      CurrentSourceItem.name = jsonParser.getValue();
    else if (jsonParser.getNode() == "uri")
      CurrentSourceItem.uri = jsonParser.getValue();
    else if (jsonParser.getNode() == "plugin_type")
      CurrentSourceItem.plugin_type = jsonParser.getValue();
    else if (jsonParser.getNode() == "plugin_name")
      CurrentSourceItem.plugin_name = jsonParser.getValue();
    else if (jsonParser.getNode() == "icon")
      CurrentSourceItem.icon = jsonParser.getValue();
    else
    {
      DEBUG_PRINTLN("UNKNOWN ITEM!");
    }

    if (jsonParser.getBlockEnd())
      return true;
  }

  return false;
}

bool Volumio::readNextQueueItem()
{
  if (pushType != pushQueue)
  {
    DEBUG_PRINTLN("Volumio: readNextQueueItem(): ERROR, no data avaliable");
    return false;
  }

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

  while (jsonParser.next())
  {
    DEBUG_PRINT("Volumio: readNextQueueItem(): ");
    DEBUG_PRINT(jsonParser.getPath());
    DEBUG_PRINT(" = ");
    DEBUG_PRINTLN(jsonParser.getValue());

    if (jsonParser.getNode() == "uri")
      CurrentQueueItem.uri = jsonParser.getValue();
    else if (jsonParser.getNode() == "service")
      CurrentQueueItem.service = jsonParser.getValue();
    else if (jsonParser.getNode() == "name")
      CurrentQueueItem.name = jsonParser.getValue();
    else if (jsonParser.getNode() == "artist")
      CurrentQueueItem.artist = jsonParser.getValue();
    else if (jsonParser.getNode() == "album")
      CurrentQueueItem.album = jsonParser.getValue();
    else if (jsonParser.getNode() == "type")
      CurrentQueueItem.type = jsonParser.getValue();
    else if (jsonParser.getNode() == "tracknumber")
      CurrentQueueItem.tracknumber = jsonParser.getValue();
    else if (jsonParser.getNode() == "albumart")
      CurrentQueueItem.albumart = jsonParser.getValue();
    else if (jsonParser.getNode() == "duration")
      CurrentQueueItem.duration = jsonParser.getValue();
    else if (jsonParser.getNode() == "samplerate")
      CurrentQueueItem.samplerate = jsonParser.getValue();
    else if (jsonParser.getNode() == "bitdepth")
      CurrentQueueItem.bitdepth = jsonParser.getValue();
    else if (jsonParser.getNode() == "trackType")
      CurrentQueueItem.trackType = jsonParser.getValue();
    else if (jsonParser.getNode() == "channels")
      CurrentQueueItem.channels = jsonParser.getValue();
    else
    {
      DEBUG_PRINTLN("UNKNOWN ITEM!");
    }

    if (jsonParser.getBlockEnd())
      return true;
  }

  return false;
}

bool Volumio::readNextLibraryItem()
{
  if (pushType != pushBrowseLibrary)
  {
    DEBUG_PRINTLN("Volumio: readNextLibraryItem(): ERROR, no data avaliable");
    return false;
  }

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

  while (jsonParser.next())
  {
    DEBUG_PRINT("Volumio: readNextLibraryItem():");
    DEBUG_PRINT(jsonParser.getPath());
    DEBUG_PRINT(" = ");
    DEBUG_PRINTLN(jsonParser.getValue());

    if (jsonParser.getLevel() == 7)
    {
      if (jsonParser.getNode() == "service")
        CurrentLibraryItem.service = jsonParser.getValue();
      else if (jsonParser.getNode() == "type")
        CurrentLibraryItem.type = jsonParser.getValue();
      else if (jsonParser.getNode() == "title")
        CurrentLibraryItem.title = jsonParser.getValue();
      else if (jsonParser.getNode() == "artist")
        CurrentLibraryItem.artist = jsonParser.getValue();
      else if (jsonParser.getNode() == "album")
        CurrentLibraryItem.album = jsonParser.getValue();
      else if (jsonParser.getNode() == "uri")
        CurrentLibraryItem.uri = jsonParser.getValue();
      else if (jsonParser.getNode() == "albumart")
        CurrentLibraryItem.albumart = jsonParser.getValue();
      else if (jsonParser.getNode() == "tracknumber")
        CurrentLibraryItem.tracknumber = jsonParser.getValue();
      else if (jsonParser.getNode() == "duration")
        CurrentLibraryItem.duration = jsonParser.getValue();
      else if (jsonParser.getNode() == "trackType")
        CurrentLibraryItem.trackType = jsonParser.getValue();
      else if (jsonParser.getNode() == "icon")
        CurrentLibraryItem.icon = jsonParser.getValue();
      else
      {
        DEBUG_PRINTLN("UNKNOWN ITEM!");
      }

      if (jsonParser.getBlockEnd())
        return true;
    }

    if (jsonParser.getLevel() == 4)
    {
      if (jsonParser.getParent() == "prev")
      {
        if (jsonParser.getNode() == "uri")
          LibraryPrev.uri = jsonParser.getValue();
        else
        {
          DEBUG_PRINTLN("UNKNOWN ITEM!");
        }
      }

      if (jsonParser.getParent() == "info")
      {
        if (jsonParser.getNode() == "uri")
          LibraryInfo.uri = jsonParser.getValue();
        else if (jsonParser.getNode() == "title")
          LibraryInfo.title = jsonParser.getValue();
        else if (jsonParser.getNode() == "service")
          LibraryInfo.service = jsonParser.getValue();
        else if (jsonParser.getNode() == "type")
          LibraryInfo.type = jsonParser.getValue();
        else if (jsonParser.getNode() == "albumart")
          LibraryInfo.albumart = jsonParser.getValue();
        else
        {
          DEBUG_PRINTLN("UNKNOWN ITEM!");
        }
      }
    }
  }

  return false;
}

bool Volumio::readLibraryPrev()
{
  if (pushType != pushBrowseLibrary)
    return false;

  if (jsonParser.next())
  {
    DEBUG_PRINT("Volumio: readLibraryPrev():");
    DEBUG_PRINT(jsonParser.getPath());
    DEBUG_PRINT(" = ");
    DEBUG_PRINTLN(jsonParser.getValue());

    if (jsonParser.getParent() == "prev")
    {
      if (jsonParser.getNode() == "uri")
        LibraryPrev.uri = jsonParser.getValue();
      else
      {
        DEBUG_PRINTLN("UNKNOWN ITEM!");
      }
    }

    if (jsonParser.getParent() == "info")
    {
      if (jsonParser.getNode() == "uri")
        LibraryInfo.uri = jsonParser.getValue();
      else if (jsonParser.getNode() == "title")
        LibraryInfo.title = jsonParser.getValue();
      else if (jsonParser.getNode() == "service")
        LibraryInfo.service = jsonParser.getValue();
      else if (jsonParser.getNode() == "type")
        LibraryInfo.type = jsonParser.getValue();
      else if (jsonParser.getNode() == "albumart")
        LibraryInfo.albumart = jsonParser.getValue();
      else
      {
        DEBUG_PRINTLN("UNKNOWN ITEM!");
      }
    }
    return true;
  }

  return false;
}

bool Volumio::readState()
{
  if (pushType != pushState)
  {
    DEBUG_PRINTLN("Volumio: readState(): ERROR, no data avaliable");
    return false;
  }

  while (jsonParser.next())
  {
    DEBUG_PRINT("Volumio: readState(): ");
    DEBUG_PRINT(jsonParser.getPath());
    DEBUG_PRINT(" = ");
    DEBUG_PRINTLN(jsonParser.getValue());

    if (jsonParser.getNode() == "status")
      State.status = jsonParser.getValue();
    else if (jsonParser.getNode() == "position")
      State.position = jsonParser.getValue();
    else if (jsonParser.getNode() == "title")
      State.title = jsonParser.getValue();
    else if (jsonParser.getNode() == "artist")
      State.artist = jsonParser.getValue();
    else if (jsonParser.getNode() == "album")
      State.album = jsonParser.getValue();
    else if (jsonParser.getNode() == "albumart")
      State.albumart = jsonParser.getValue();
    else if (jsonParser.getNode() == "uri")
      State.uri = jsonParser.getValue();
    else if (jsonParser.getNode() == "trackType")
      State.trackType = jsonParser.getValue();
    else if (jsonParser.getNode() == "seek")
      State.seek = atol(jsonParser.getValue().c_str());
    else if (jsonParser.getNode() == "duration")
      State.duration = atol(jsonParser.getValue().c_str());
    else if (jsonParser.getNode() == "samplerate")
      State.samplerate = jsonParser.getValue();
    else if (jsonParser.getNode() == "bitdepth")
      State.bitdepth = jsonParser.getValue();
    else if (jsonParser.getNode() == "channels")
      State.channels = jsonParser.getValue();
    else if (jsonParser.getNode() == "bitrate")
      State.bitrate = jsonParser.getValue();
    else if (jsonParser.getNode() == "random")
      State.random = jsonParser.getValue();
    else if (jsonParser.getNode() == "repeat")
      State.repeat = jsonParser.getValue();
    else if (jsonParser.getNode() == "repeatSingle")
      State.repeatSingle = jsonParser.getValue();
    else if (jsonParser.getNode() == "consume")
      State.consume = jsonParser.getValue();
    else if (jsonParser.getNode() == "volume")
      State.volume = atoi(jsonParser.getValue().c_str());
    else if (jsonParser.getNode() == "mute")
      State.mute = jsonParser.getValue();
    else if (jsonParser.getNode() == "disableVolumeControl")
      State.disableVolumeControl = jsonParser.getValue();
    else if (jsonParser.getNode() == "stream")
      State.stream = jsonParser.getValue();
    else if (jsonParser.getNode() == "updatedb")
      State.updatedb = jsonParser.getValue();
    else if (jsonParser.getNode() == "volatile")
      State._volatile = jsonParser.getValue();
    else if (jsonParser.getNode() == "service")
      State.service = jsonParser.getValue();

    else
    {
      DEBUG_PRINTLN("UNKNOWN ITEM!");
    }
  }

  DEBUG_PRINTLN("");
}

bool Volumio::readMultiRoomDevice()
{
  if (pushType != pushMultiRoomDevices)
  {
    DEBUG_PRINTLN("Volumio: readMultiRoomDevice(): ERROR, no data avaliable");
    return false;
  }

  CurrentMultiRoomDevice.id = "";
  CurrentMultiRoomDevice.host = "";
  CurrentMultiRoomDevice.name = "";
  CurrentMultiRoomDevice.isSelf = "";
  CurrentMultiRoomDevice.type = "";
  CurrentMultiRoomDevice.volumeAvailable = "";
  CurrentMultiRoomDevice.state.status = "";
  CurrentMultiRoomDevice.state.volume = "";
  CurrentMultiRoomDevice.state.mute = "";
  CurrentMultiRoomDevice.state.artist = "";
  CurrentMultiRoomDevice.state.track = "";
  CurrentMultiRoomDevice.state.albumart = "";

  while (jsonParser.next())
  {
    DEBUG_PRINT("Volumio: readMultiRoomDevice(): ");
    DEBUG_PRINT(jsonParser.getPath());
    DEBUG_PRINT(" =");
    DEBUG_PRINTLN(jsonParser.getValue());

    if (jsonParser.getNode() == "id")
      CurrentMultiRoomDevice.id = jsonParser.getValue();
    else if (jsonParser.getNode() == "host")
      CurrentMultiRoomDevice.host = jsonParser.getValue();
    else if (jsonParser.getNode() == "name")
      CurrentMultiRoomDevice.name = jsonParser.getValue();
    else if (jsonParser.getNode() == "isSelf")
      CurrentMultiRoomDevice.isSelf = jsonParser.getValue();
    else if (jsonParser.getNode() == "type")
      CurrentMultiRoomDevice.type = jsonParser.getValue();
    else if (jsonParser.getNode() == "volumeAvailable")
      CurrentMultiRoomDevice.volumeAvailable = jsonParser.getValue();
    else if (jsonParser.getNode() == "status")
      CurrentMultiRoomDevice.state.status = jsonParser.getValue();
    else if (jsonParser.getNode() == "volume")
      CurrentMultiRoomDevice.state.volume = jsonParser.getValue();
    else if (jsonParser.getNode() == "mute")
      CurrentMultiRoomDevice.state.mute = jsonParser.getValue();
    else if (jsonParser.getNode() == "artist")
      CurrentMultiRoomDevice.state.artist = jsonParser.getValue();
    else if (jsonParser.getNode() == "track")
      CurrentMultiRoomDevice.state.track = jsonParser.getValue();
    else if (jsonParser.getNode() == "albumart")
      CurrentMultiRoomDevice.state.albumart = jsonParser.getValue();
    else
    {
      DEBUG_PRINTLN("UNKNOWN ITEM!");
    }

    if (jsonParser.getBlockEnd())
      return true;
  }

  return false;
}

void Volumio::process()
{
  //cleanup, just in case application didnt read all data
  if (pushType != pushNone)
    while (jsonParser.next())
      ;

  unsigned long now = millis();

  //Calculate seek state if in play
  if (State.status == "play")
    State.seek += (now - lastTime);

  lastTime = now;

  pushType = pushNone;

  if (socketIoClient.process())
  {
    //link messagstream to JSON-parser
    jsonParser.initialize(&socketIoClient);

    //Process data as long as data generates lines
    if (jsonParser.next())
    {
      DEBUG_PRINT("Volumio: process(): Receive:");
      DEBUG_PRINTLN(jsonParser.getValue());

      if (jsonParser.getValue() == "pushToastMessage")
      {
        pushType = pushToastMessage;
      }
      else if (jsonParser.getValue() == "pushBrowseLibrary")
      {
        pushType = pushBrowseLibrary;

        LibraryInfo.uri = "";
        LibraryInfo.title = "";
        LibraryInfo.service = "";
        LibraryInfo.type = "";
        LibraryInfo.albumart = "";
      }
      else if (jsonParser.getValue() == "pushBrowseSources")
        pushType = pushBrowseSources;
      else if (jsonParser.getValue() == "pushQueue")
        pushType = pushQueue;
      else if (jsonParser.getValue() == "pushState")
        pushType = pushState;
      else if (jsonParser.getValue() == "pushMultiRoomDevices")
        pushType = pushMultiRoomDevices;
      else
      {
        while (jsonParser.next())
        {
          DEBUG_PRINT(jsonParser.getPath());
          DEBUG_PRINT(" =");
          DEBUG_PRINTLN(jsonParser.getValue());
        }
      }
    }
  }
}