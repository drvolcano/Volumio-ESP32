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


#include "SocketIO.h"

class Volumio
{

  struct StateStruct
  {
    String status;
    String position;
    String title;
    String artist;
    String album;
    String albumart;
    String uri;
    String trackType;
    unsigned long seek;
    unsigned long duration;
    String samplerate;
    String bitdepth;
    String channels;
    String random;
    String repeat;
    String repeatSingle;
    String consume;
    unsigned int volume;
    String mute;
    String disableVolumeControl;
    String stream;
    String updatedb;
    //     String volatile ;
    String service;
    String Streaming;
  };

  struct LibraryPrevStruct
  {

    String uri;
  };

  struct LibraryInfoStruct
  {

    String uri;
    String title;
    String service;
    String type;
    String albumart;
  };

  struct LibraryItemStruct
  {
    String service;
    String type;
    String title;
    String artist;
    String album;
    String uri;
    String albumart;
    String tracknumber;
    String duration;
    String trackType;
    String icon;
  };

  struct QueueItemStruct
  {
    String uri;
    String service;
    String name;
    String artist;
    String album;
    String type;
    String tracknumber;
    String albumart;
    String duration;
    String samplerate;
    String bitdepth;
    String trackType;
    String channels;
  };

  struct SourceStruct
  {
    String albumart;
    String name;
    String uri;
    String plugin_type;
    String plugin_name;
    String icon;
  };

  struct ToastStruct
  {
    String type;
    String title;
    String message;
  };

public:
  enum PushType
  {
    pushNone,
    pushState,
    pushBrowseSources,
    pushQueue,
    pushBrowseLibrary,
    pushToastMessage,
    pushMultiRoomDevices
  };

  //State of system
  StateStruct State;

  SourceStruct CurrentSourceItem;
  LibraryItemStruct CurrentLibraryItem;
  LibraryInfoStruct LibraryInfo;
  LibraryPrevStruct LibraryPrev;
  QueueItemStruct CurrentQueueItem;
  ToastStruct CurrentToastItem;

  //New State received
  PushType getPushType() { return pushType; };

  //Call oce in Setup
  void connect(String hostname, int portnr);
  bool getConnected();

  //Call cyclic in loop()
  void process();

  bool readNextSourceItem();
  bool readNextLibraryItem();
  bool readNextQueueItem();
  bool readLibraryPrev();
  bool readPushToastMessage();
  bool readState();

  //Commands
  void getState();
  void getQueue();
  void removeQueueItem(int index);
  void addToQueue(String uri);
  void replaceAndPlay(String uri);
  void addPlay(String uri);
  void addPlay(String service, String name, String uri);
  void seek(int position);
  void getBrowseSources();
  void browseLibrary(String uri);
  void play();
  void play(int index);
  void volatilePlay();
  void pause();
  void toggle();
  void stop();
  void clearQueue();
  void prev();
  void next();
  void setRandom(bool flag);
  void setRepeat(bool flag);
  void volume(int volume);
  void mute();
  void unmute();

  void getDeviceInfo();
  void getSystemVersion();
  void getAudioOutputs();
  void getUiSettings();
  void getAvailableLanguages();
  void getPlaylistIndex();

private:
  PushType pushType = pushNone;
  SocketIO socketIoClient;
  JSON jsonParser;
  unsigned long lastTime = 0;
};
