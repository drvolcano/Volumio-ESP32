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

#pragma once
#include <WiFi.h>
#include "Inflate.h"
#include "JSON.h"
#include "CharStream.h"

#define message_len_16bit 126
#define message_len_64bit 127
#define message_masked 128
#define message_header 129
#define message_header2 193

#define step_header 0
#define step_length 1
#define step_length2 2
#define step_engineIo 3
#define step_socketIo 4

//https://github.com/socketio/engine.io-protocol
#define engineIo_open '0'
#define engineIo_close '1'
#define engineIo_ping '2'
#define engineIo_pong '3'
#define engineIo_message '4'
#define engineIo_upgrade '5'
#define engineIo_noop '6'

//https://github.com/socketio/socket.io-protocol
#define socketIo_connect '0'
#define socketIo_disconnect '1'
#define socketIo_event '2'
#define socketIo_ack '3'
#define socketIo_error '4'
#define socketIo_binary_event '5'
#define socketIo_binary_ack '6'

class SocketIO : public CharStream
{
public:
  bool connect(String hostname, int portnr);
  bool getConnected();
  void disconnect();

  void sendMessage(String message);
  void sendJSON(String RID, String JSON = "");

  bool process();
  void finalize();
  char readChar();

private:
  bool waitForData(unsigned long delay);
  String readLine();
  void writeLine(String line);
  String genKey();
  bool Process2();
  String mem_hostname;
  int mem_portnr;

  unsigned long lastPing = 0;
  long pingInterval = 5000; //ms
  long pingTimeout = 10000; //ms
  

  WiFiClient client;
  Inflate inflater;

  char *dataptr;

  String sid;
  JSON parser;

  //Length of data
  unsigned long len = 0;
  bool masked = false;
  bool compressed = false;
  bool inflate = false;
  bool inflaterInitialized = false;
  bool direct = false;
  char c;
};
