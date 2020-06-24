#pragma once

#include "Arduino.h"

//Texts that cant be found in Volumio data
struct STRUCT_ESP32
{
    String DisplayOff;
    String Control;
    String ConnectWiFi;
    String ConnectVolumio;

};

//FOllowing structs from derived from JSON:
//https://github.com/volumio/Volumio2-UI/tree/development/src/app/i18n

struct STRUCT_COMMON
{
    String SAVE;
    String OK;
    String YES;
    String NO;
    String GOT_IT;
    String CANCEL;
    String RESTART_SYSTEM;
    String TRY_AGAIN;
    String DELETE;
    String ADD;
    String POWER_OFF;
    String REBOOT_SYSTEM;
    String ACTION;
    String ACTIVE;
    String INACTIVE;
    String SHOW_SETTINGS;
    String SETTINGS;
    String TAB_BROWSE;
    String TAB_HOME;
    String TAB_SEARCH;
    String TAB_SETTINGS;
    String TAB_PLAYBACK;
    String TAB_QUEUE;
    String TAB_OUTPUTS;
    String TAB_VOLUME;
    String ARTIST;
    String ARTISTS;
    String ALBUM;
    String ALBUMS;
    String SONG;
    String SONGS;
    String PLAYTIME;
    String TIME;
    String RESCAN;
    String SYSTEM_VERSION;
    String RELEASED;
    String CLOSE;
    String MY_MUSIC;
    String SEARCH;
    String NEW;
    String FAVOURITES;
    String NAME;
    String URL;
    String BACK;
    String OPTIONS;
    String CHOOSE_PLAYLIST;
    String STOP_MUSIC;
    String HOME;
    String VOLUME_MUTE;
    String VOLUME_MUTED;
    String VOLUME_UP;
    String VOLUME_DOWN;
    String HIDDEN;
    String LIST;
    String PLAY_QUEUE;
    String ENTER_PASSWORD;
    String INVALID_PASSWORD;
    String RIP;
    String SKIP;
    String NEXT;
    String PREVIOUS;
    String DONE;
    String EMAIL;
    String SUBSCRIBE_ME;
    String SHARE;
    String EJECT;
    String FUNCTIONALIES_CONTROL;
    String UPDATE_ALBUMART;
    String YEAR;
    String GENRE;
    String EDIT;
    String GRID;
};

struct STRUCT_BROWSER
{
    String PLAY;
    String ADD_TO_QUEUE;
    String ADD_TO_PLAYLIST;
    String REMOVE_FROM_PLAYLIST;
    String ADD_TO_FAVOURITES;
    String REMOVE_FROM_FAVOURITES;
    String ADD_WEBRADIO;
    String EDIT_WEBRADIO;
    String DELETE_WEBRADIO;
    String ADD_TO_RADIO_FAVOURITES;
    String REMOVE_FROM_RADIO_FAVOURITES;
    String ADD_WEBRADIO_TITLE;
    String URL_DOC;
    String DELETE_PLAYLIST;
    String GO_TO_SHARE;
    String NEXT_TRACK;
    String PREVIOUS_TRACK;
    String PAUSE;
    String CLEAR_AND_PLAY;
    String SAFE_REMOVE;
    String UPDATE_FOLDER;
    String DELETE_FOLDER;
    String SKIP_BACKWARDS;
    String SKIP_FORWARD;
    String CREDITS;
    String READ_MORE;
    String NO_ITEMS;
    String NOT_FOUND;
    String CREDITS_NOT_FOUND;
};

  struct STRUCT_TRACK_INFO_BAR {
    String RANDOM ;
    String REPEAT;
    String REPEAT_ALL;
    String REPEAT_SINGLE;
    String REPEAT_OFF;
    String SAVE_QUEUE_AS_PLAYLIST;
    String CONSUME_PLAYLIST;
    String CLEAR_QUEUE;
  };


struct STRUCT_LOCALE
{
    STRUCT_ESP32 ESP;
    STRUCT_COMMON COMMON;
    STRUCT_BROWSER BROWSER;
    STRUCT_TRACK_INFO_BAR TRACK_INFO_BAR;
};