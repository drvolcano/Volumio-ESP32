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
 


  //State of system
  StateStruct State;

  SourceStruct CurrentSourceItem;
  LibraryItemStruct CurrentLibraryItem;
  LibraryInfoStruct LibraryInfo;
  LibraryPrevStruct LibraryPrev;
  QueueItemStruct CurrentQueueItem;
  ToastStruct CurrentToastItem;

  //New State received
  bool StatusUpdate = false;
  bool SourceUpdate = false;
  bool QueueUpdate = false;
  bool LibraryUpdate = false;
  bool PushToastUpdate = false;

  //Call oce in Setup
  void connect(char hostname[], int port);
  bool connected();

  //Call cyclic in loop()
  void process();

  bool ReadNextSourceItem();
  bool ReadNextLibraryItem();
  bool ReadNextQueueItem();
  bool ReadLibraryPrev();
  bool ReadPushToastMessage();
  void ReadState();

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
  SocketIO socketIoClient;
  JSON Parser;
  unsigned long previousMillis = 0;
};
