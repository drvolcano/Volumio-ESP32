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

//inspired by https://github.com/washo4evr/Socket.io-v1.x-Library

#include "SocketIO.h"

#include "LibDebug.h"
#if (DEBUGLEVEL_SOCKETIO >= 1)
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINT(x) Serial.print(x)
#else
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT(x)
#endif

bool SocketIO::connect(String hostname, int portnr)
{
  DEBUG_PRINT("SocketIO: connect(): hostname = ");
  DEBUG_PRINTLN(hostname);
  DEBUG_PRINT("SocketIO: connect(): portnr = ");
  DEBUG_PRINTLN(portnr);

  //Connect to WiFi
  if (!client.connect(hostname.c_str(), portnr))
  {
    DEBUG_PRINTLN("SocketIO: connect(): ERROR, could not connect to host");
    return false;
  }

  mem_hostname = hostname;
  mem_portnr = portnr;

  writeLine("GET /socket.io/?EIO=3&transport=polling HTTP/1.1");
  writeLine("Host: " + hostname);
  //writeLine("Connection: keep-alive");
  //writeLine("Accept: */*");
  //writeLine("Origin: ");
  //writeLine("Referer: ");

  writeLine("");
  DEBUG_PRINTLN(F(""));

  //Wait for response
  if (!waitForData(3000))
  {
    DEBUG_PRINTLN("SocketIO: connect(): ERROR, no response");
    return false;
  }

  if (!readLine().startsWith("HTTP/1.1 200"))
  {
    DEBUG_PRINTLN("SocketIO: connect(): ERROR, HTTP/1.1 200 from host");

    while (client.available())
      readLine();

    client.stop();
    return false;
  }

  //Read until end of message (empty line)
  while (client.available())
    if (readLine().length() == 0)
      break;

  DEBUG_PRINTLN("");

  //read 00 09 07 FF
  for (int i = 0; i < 4; i++)
    DEBUG_PRINTLN("C:" + String(client.read()));

  //Load string into JSON-parser
  parser.initialize(readLine());

  while (parser.next())
  {
    DEBUG_PRINT("SocketIO: connect(): JSON: ");
    DEBUG_PRINT(parser.getNode(1));
    DEBUG_PRINT(" = ");
    DEBUG_PRINTLN(parser.getValue());

    if (parser.getNode(1) == "sid")
      sid = parser.getValue();

    if (parser.getNode(1) == "upgrades")
      ;

    if (parser.getNode(1) == "pingInterval")
      pingInterval = parser.getValue().toInt();

    if (parser.getNode(1) == "pingTimeout")
      pingTimeout = parser.getValue().toInt();
  }

  //Read rest of message
  while (client.available())
    readLine();

  DEBUG_PRINTLN(F(""));

  String keytext = genKey();

  writeLine("GET /socket.io/?EIO=3&transport=websocket&sid=" + sid + " HTTP/1.1");
  writeLine("Host: " + hostname);
  writeLine("Connection: Upgrade");
  writeLine("Pragma: no-cache");
  writeLine("Cache-Control: no-cache");
  writeLine("Upgrade: websocket");
  writeLine("Sec-WebSocket-Version: 13");
  writeLine("Accept-Encoding: deflate");
  writeLine("Cookie: io=" + sid);
  writeLine("Sec-WebSocket-Key: " + keytext); //16 byte in Base64
  writeLine("Sec-WebSocket-Extensions: permessage-deflate");
  writeLine("");

  DEBUG_PRINTLN(F(""));

  if (!waitForData(3000))
  {
    DEBUG_PRINTLN("SocketIO: connect(): ERROR, no response");
    return false;
  }

  if (!readLine().startsWith("HTTP/1.1 101"))
  {
    DEBUG_PRINTLN("SocketIO: connect(): ERROR, HTTP/1.1 101 from host");

    while (client.available())
      readLine();
    client.stop();
    return false;
  }

  while (client.available())
    readLine();

  DEBUG_PRINTLN("");

  sendMessage(String(engineIo_upgrade));

  return true;
}

bool SocketIO::getConnected()
{
  return client.connected();
}

void SocketIO::disconnect()
{
  DEBUG_PRINTLN("SocketIO: disconnect()");
  client.stop();
}

void SocketIO::finalize()
{
  DEBUG_PRINTLN("SocketIO: finalize()");
}

char SocketIO::readChar()
{
  //Compressed data
  if (inflate)
  {
    if (!inflaterInitialized)
    {
      DEBUG_PRINTLN("SocketIO: readChar(): Inflate");
      DEBUG_PRINTLN("");

      inflater.initialize(&client, len);
      len = 0;
      inflaterInitialized = true;
    }
    //Read one char from inflater
    c = inflater.readChar();

    if (inflater.getDone())
    {
      inflater.finalize();

      DEBUG_PRINTLN("");
      DEBUG_PRINTLN("SocketIO: readChar(): END");
      DEBUG_PRINTLN("");
      return 0;
    }
    else
      return c;
  }
  //Uncompressed data
  else if (direct)
  {
    if (len == 0)
    {
      DEBUG_PRINTLN("");
      DEBUG_PRINTLN("SocketIO: readChar(): END");
      DEBUG_PRINTLN("");
      return 0;
    }

    len--;
    return client.read();
  }
  //Before data type known
  else
  {
    //Read one char from client
    return client.read();
  }
}

bool SocketIO::process()
{
  unsigned long now = millis();

  //Keep connection open
  if (now > lastPing + pingInterval)
  {
    lastPing = now;

    DEBUG_PRINTLN("SocketIO: process(): Ping (keep alive)");
    sendMessage(String(engineIo_ping));
  }

  //Reconnect if necessary
  if (!client.connected())
  {
    DEBUG_PRINTLN("SocketIO: process(): client not connected, trying to reconnect");

    if (!client.connect(mem_hostname.c_str(), mem_portnr))
    {
      DEBUG_PRINTLN("SocketIO: process(): ERROR, could not connected");
      return 0;
    }
  }

  //CHeck if client has data
  if (!client.available())
    return 0;

  return Process2();
}

bool SocketIO::Process2()
{
  //Step of header readout
  int step = step_header;

  //Count of bytes needed for length
  int lencnt = 0;

  //Length of data
  len = 0;
  masked = false;
  compressed = false;
  inflate = false;
  inflaterInitialized = false;
  direct = false;

  while (true)
  {
    c = readChar();

    if (c == 0 && step != step_length2)
    {
      DEBUG_PRINTLN("SocketIO: process(): receiving: char = 0 ");
      return 0;
    }

    switch (step)
    {
    //Wait for hader character
    case step_header:
      if (true)
      {
        bool FIN = c & (1 << 7);
        bool RSV1 = c & (1 << 6); //Compressed?
        bool RSV2 = c & (1 << 5);
        bool RSV3 = c & (1 << 4);
        byte OPCODE = c & 0xf;

        compressed = RSV1;

        step = step_length;

        DEBUG_PRINT("SocketIO: process(): receiving: Header = ");
        DEBUG_PRINTLN((uint8_t)c);
        DEBUG_PRINT("SocketIO: process(): receiving: FIN = ");
        DEBUG_PRINTLN(FIN);
        DEBUG_PRINT("SocketIO: process(): receiving: RSV1 = ");
        DEBUG_PRINT(RSV1);

        if (compressed)
          DEBUG_PRINTLN(" (compressed)");
        else
          DEBUG_PRINTLN(" (uncompressed)");

        DEBUG_PRINT("SocketIO: process(): receiving: RSV2 = ");
        DEBUG_PRINTLN(RSV2);
        DEBUG_PRINT("SocketIO: process(): receiving: RSV3 = ");
        DEBUG_PRINTLN(RSV3);
        DEBUG_PRINT("SocketIO: process(): receiving: OPCODE = ");
        DEBUG_PRINTLN(OPCODE);
      }

      break;

    case step_length:

      masked = (c & message_masked);

      if (masked)
      {
        DEBUG_PRINTLN("SocketIO: process(): receiving: Masked");
        c -= message_masked;
      }
      else
        DEBUG_PRINTLN("SocketIO: process(): receiving: Unmasked");

      switch (c)
      {
      case message_len_16bit:
        lencnt = 2;
        len = 0;
        DEBUG_PRINT("SocketIO: process(): receiving: Paylod data len (16 bit) = ");
        step = step_length2;
        break;

      case message_len_64bit:
        lencnt = 8;
        len = 0;
        DEBUG_PRINT("SocketIO: process(): receiving: Paylod data len (64 bit) = ");
        step = step_length2;
        break;

      default:
        lencnt = 0;
        DEBUG_PRINT("SocketIO: process(): receiving: Payload data len (8 bit) = ");
        len = c;
        DEBUG_PRINTLN(len);

        inflate = compressed;
        direct = !compressed;
        step = step_engineIo;

        if (len == 0)
          return 0;

        break;
      }

      break;

    //Determine length of payload if > 125
    case step_length2:

      len = len << 8 | c;
      lencnt--;

      if (lencnt == 0)
      {
        inflate = compressed;
        direct = !compressed;

        step = step_engineIo;

        DEBUG_PRINTLN(len);
      }

      break;

    case step_engineIo:

      DEBUG_PRINT("SocketIO: process(): receiving: engineIo = ");
      DEBUG_PRINT(c);
      DEBUG_PRINT(" (");

      switch (c)
      {
      case engineIo_open:
        DEBUG_PRINT("open");
        step = step_header;
        break;
      case engineIo_close:
        DEBUG_PRINT("close");
        step = step_header;
        break;
      case engineIo_pong:
        DEBUG_PRINT("pong");
        step = step_header;
        break;
      case engineIo_ping:
        DEBUG_PRINT("ping");
        step = step_header;
        break;
      case engineIo_message:
        DEBUG_PRINT("message");
        step = step_socketIo;
        break;
      case engineIo_upgrade:
        DEBUG_PRINT("upgrade");
        step = step_socketIo;
        break;
      case engineIo_noop:
        DEBUG_PRINT("noop");
        step = step_header;
        break;
      default:
        DEBUG_PRINT("???");
        break;
      }

      DEBUG_PRINTLN(")");

      break;

    case step_socketIo:

      DEBUG_PRINT("SocketIO: process(): receiving: socketIo = ");
      DEBUG_PRINT(c);
      DEBUG_PRINT(" (");

      switch (c)
      {
      case socketIo_connect:
        DEBUG_PRINT("connect");
        break;
      case socketIo_disconnect:
        DEBUG_PRINT("disconnect");
        break;
      case socketIo_event:
        DEBUG_PRINT("event");
        break;
      case socketIo_ack:
        DEBUG_PRINT("ack");
        break;
      case socketIo_error:
        DEBUG_PRINT("error");
        break;
      case socketIo_binary_event:
        DEBUG_PRINT("binary event");
        break;
      case socketIo_binary_ack:
        DEBUG_PRINT("binary ack");
        break;
      default:
        DEBUG_PRINT("???");
        break;
      }

      DEBUG_PRINTLN(")");

      return 1;

      break;
    }
  }
  return 0;
}

bool SocketIO::waitForData(unsigned long delay)
{
  DEBUG_PRINT("SocketIO: waitForData(): ");
  DEBUG_PRINTLN(delay);

  unsigned long start = millis();
  while (!client.available() && (millis() < (start + delay)))
    ;

  return client.available();
}

void SocketIO::writeLine(String line)
{
  client.println(line);
  DEBUG_PRINT("SocketIO: writeLine(): ");
  DEBUG_PRINTLN(line);
}

String SocketIO::genKey()
{
  //Generate 16-Byte random key
  byte key[16];
  esp_fill_random(key, 16);

  //Convert into 24 digit base64 text
  String keytext = "";

  int j = 0;
  int cnt = 0;
  uint16_t buf = 0;

  for (int i = 0; i < 24; i++)
  {
    if (cnt <= 8)
      if (j < 16)
      {
        buf += key[j] << (8 - cnt);
        cnt += 8;
        j++;
      }

    byte val = buf >> 10;

    if (cnt < 0)
      keytext += (char)('=');
    else if (val <= 25)
      keytext += (char)('A' + val);
    else if (val <= 51)
      keytext += (char)('a' + val - 26);
    else if (val <= 61)
      keytext += (char)('0' + val - 52);
    else if (val == 62)
      keytext += (char)('+');
    else if (val == 63)
      keytext += (char)('/');

    cnt -= 6;
    buf = buf << 6;
  }

  return keytext;
}

String SocketIO::readLine()
{
  String line = "";

  while (client.available())
  {
    char c = client.read();
    if (c == '\n')
      break;

    if (c == 0 || c == 255 || c == '\r' || c == '\n')
      ;
    else
      line += c;
  }

  DEBUG_PRINT("SocketIO: readLine(): ");
  DEBUG_PRINTLN(line);

  return line;
}

void SocketIO::sendMessage(String message)
{
  int msglength = message.length();
  String mask = "";
  String masked = message;

  for (int i = 0; i < 4; i++)
  {
    char a = random((byte)'0', (byte)'9');
    mask += a;
  }

  for (int i = 0; i < msglength; i++)
    masked[i] = message[i] ^ mask[i % 4];

  client.print((char)(message_header));

  //Depending on the size of the message
  if (msglength <= 125)
  {
    client.print((char)(msglength + message_masked)); //size of the message + 128 because message has to be masked
    DEBUG_PRINT("SocketIO: sendMessage(): Payload data len (8 bit) = ");
  }
  else if (msglength >= 126 && msglength <= 65535)
  {
    client.print((char)(message_len_16bit + message_masked));
    DEBUG_PRINT("SocketIO: sendMessage(): Paylod data len (16 bit) = ");
    for (int i = 0; i < 2; i++)
    {
      char temp = (char)((msglength >> (8 * (2 - 1 - i))) & 255);
      client.print(temp);
    }
  }
  else
  {
    client.print((char)(message_len_64bit + message_masked));
    DEBUG_PRINT("SocketIO: sendMessage(): Paylod data len (64 bit) = ");
    for (int i = 0; i < 8; i++)
    {
      char temp = (char)((msglength >> (8 * (8 - 1 - i))) & 255);
      client.print(temp);
    }
  }

  DEBUG_PRINTLN(msglength);
  DEBUG_PRINT("SocketIO: sendMessage(): Message = ");
  DEBUG_PRINTLN(message);
  DEBUG_PRINTLN("");

  client.print(mask);
  client.print(masked);
}

void SocketIO::sendJSON(String command, String data)
{
  DEBUG_PRINT("SocketIO: sendJSON(): command = ");
  DEBUG_PRINTLN(command);
  DEBUG_PRINT("SocketIO: sendJSON(): data = ");
  DEBUG_PRINTLN(data);

  String message = String(engineIo_message) + String(socketIo_event) + "[\"" + command + "\"";

  if (data != "")
    message += "," + data;

  message += "]";

  DEBUG_PRINT("SocketIO: sendJSON(): message = ");
  DEBUG_PRINTLN(message);

  sendMessage(message);
}