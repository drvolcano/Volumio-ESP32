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

void Volumio::getState()
{
  DEBUG_PRINTLN("Volumio: getState()");
  socketIoClient.sendJSON("getState");
}

void Volumio::getQueue()
{
  DEBUG_PRINTLN("Volumio: getQueue()");
  socketIoClient.sendJSON("getQueue");
}

void Volumio::removeQueueItem(int index)
{
  DEBUG_PRINTLN("Volumio: removeQueueItem(" + String(index) + ")");
  socketIoClient.sendJSON("removeQueueItem", "{ \"value\" : " + String(index) + "}");
}

void Volumio::addToQueue(String uri)
{
  DEBUG_PRINTLN("Volumio: addToQueue(" + uri + ")");
  socketIoClient.sendJSON("addToQueue", "{ \"uri\" : \"" + uri + "\"}");
}

void Volumio::replaceAndPlay(String uri)
{
  DEBUG_PRINTLN("Volumio: replaceAndPlay(" + uri + ")");
  socketIoClient.sendJSON("replaceAndPlay", "{ \"uri\" : \"" + uri + "\"}");
}

void Volumio::addPlay(String uri)
{
  DEBUG_PRINTLN("Volumio: addPlay(" + uri + ")");
  socketIoClient.sendJSON("addPlay", "{ \"uri\" : \"" + uri + "\"}");
}

void Volumio::addPlay(String service, String name, String uri)
{
  DEBUG_PRINTLN("Volumio: addPlay(" + service + "," + name + "," + uri + ")");
  socketIoClient.sendJSON("addPlay", " { \"service\" : \"" + service + "\" , \"name\" : \"" + name + "\" , \"uri\" : \"" + uri + "\"}");
  DEBUG_PRINTLN(" { \"service\" : \"" + service + "\" , \"name\" : \"" + name + "\" , \"uri\" : \"" + uri + "\"}");
}

void Volumio::seek(int position)
{
  DEBUG_PRINTLN("Volumio: seek(" + String(position) + ")");
  socketIoClient.sendJSON("seek", String(position));
}

void Volumio::getBrowseSources()
{
  DEBUG_PRINTLN("Volumio: getBrowseSources()");
  socketIoClient.sendJSON("getBrowseSources");
}

void Volumio::browseLibrary(String uri)
{
  DEBUG_PRINTLN("Volumio: browseLibrary(" + uri + ")");
  socketIoClient.sendJSON("browseLibrary", "{ \"uri\" : \"" + uri + "\"}");
}

void Volumio::play()
{
  DEBUG_PRINTLN("Volumio: play()");
  socketIoClient.sendJSON("play");
}

void Volumio::play(int index)
{
  DEBUG_PRINTLN("Volumio: play(" + String(index) + ")");
  socketIoClient.sendJSON("play", "{ \"value\" : " + String(index) + "}");
}

void Volumio::volatilePlay()
{
  DEBUG_PRINTLN("Volumio: volatilePlay()");
  socketIoClient.sendJSON("volatilePlay");
}

void Volumio::pause()
{
  DEBUG_PRINTLN("Volumio: pause()");
  socketIoClient.sendJSON("pause");
}

void Volumio::toggle()
{
  DEBUG_PRINTLN("Volumio: toggle()");
  socketIoClient.sendJSON("toggle");
}

void Volumio::stop()
{
  DEBUG_PRINTLN("Volumio: stop()");
  socketIoClient.sendJSON("stop");
}

void Volumio::clearQueue()
{
  DEBUG_PRINTLN("Volumio: clearQueue()");
  socketIoClient.sendJSON("clearQueue");
}

void Volumio::prev()
{
  DEBUG_PRINTLN("Volumio: prev()");
  socketIoClient.sendJSON("prev");
}

void Volumio::next()
{
  DEBUG_PRINTLN("Volumio: next()");
  socketIoClient.sendJSON("next");
}

void Volumio::setRandom(bool flag)
{
  DEBUG_PRINTLN("Volumio: setRandom(" + String(flag ? "true" : "false") + ")");
  socketIoClient.sendJSON("setRandom", "{ \"value\" : \"" + String(flag ? "true" : "false") + "\"}");
}

void Volumio::setRepeat(bool flag)
{
  DEBUG_PRINTLN("Volumio: setRepeat(" + String(flag ? "true" : "false") + ")");
  socketIoClient.sendJSON("setRepeat", "{ \"value\" : \"" + String(flag ? "true" : "false") + "\"}");
}

void Volumio::volume(int volume)
{
  DEBUG_PRINTLN("Volumio: volume(" + String(volume) + ")");
  socketIoClient.sendJSON("volume", String(volume));
}

void Volumio::mute()
{
  DEBUG_PRINTLN("Volumio: mute()");
  socketIoClient.sendJSON("mute");
}

void Volumio::unmute()
{
  DEBUG_PRINTLN("Volumio: unmute()");
  socketIoClient.sendJSON("unmute");
}

void Volumio::getDeviceInfo()
{
  DEBUG_PRINTLN("Volumio: getDeviceInfo()");
  socketIoClient.sendJSON("getDeviceInfo");
}

void Volumio::getSystemVersion()
{
  DEBUG_PRINTLN("Volumio: getSystemVersion()");
  socketIoClient.sendJSON("getSystemVersion");
}

void Volumio::getAudioOutputs()
{
  DEBUG_PRINTLN("Volumio: getAudioOutputs()");
  socketIoClient.sendJSON("getAudioOutputs");
}

void Volumio::getUiSettings()
{
  DEBUG_PRINTLN("Volumio: getUiSettings()");
  socketIoClient.sendJSON("getUiSettings");
}

void Volumio::getAvailableLanguages()
{
  DEBUG_PRINTLN("Volumio: getAvailableLanguages()");
  socketIoClient.sendJSON("getAvailableLanguages");
}

void Volumio::getPlaylistIndex()
{
  DEBUG_PRINTLN("Volumio: getPlaylistIndex()");
  socketIoClient.sendJSON("getPlaylistIndex");
}
