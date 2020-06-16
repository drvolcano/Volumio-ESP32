//Original code from https://github.com/washo4evr/Socket.io-v1.x-Library
//Most was rewritten, due lack of advanced features (compression, >256 Bytes,...)

#include "SocketIO.h"

//#define DEBUG_SOCKETIO

#ifdef DEBUG_SOCKETIO
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINT(x) Serial.print(x)
#else
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT(x)
#endif

bool SocketIO::connect(String hostname, int portnr)
{
  //Connect to WiFi
  if (!client.connect(hostname.c_str(), portnr))
    return false;

  mem_hostname = hostname;
  mem_portnr = portnr;

  writeLine("GET /socket.io/?transport=polling HTTP/1.1");
  writeLine("Host: " + hostname);
  writeLine("");
  DEBUG_PRINTLN(F(""));

  //Wait for response
  if (!waitForData(3000))
    return false;

  readLine();

  //"HTTP/1.1 200"
  if (atoi(&databuffer[9]) != 200)
  {
    while (client.available())
      readLine();

    client.stop();
    return false;
  }

  while (client.available())
  { // consume the header
    readLine();
    if (strlen(databuffer) == 0)
      break;
  }

  readLine();

  DEBUG_PRINTLN(F(""));

  String tmp = databuffer;

  Parser.Load(tmp);
  while (Parser.Read())
  {
    DEBUG_PRINT("SocketIO: JSON: ");
    DEBUG_PRINT(Parser.Nodes[1]);
    DEBUG_PRINT(" = ");
    DEBUG_PRINTLN(Parser.Value);

    if (Parser.Nodes[1] == "sid")
      sid = Parser.Value;

    if (Parser.Nodes[1] == "upgrades")
      ;

    if (Parser.Nodes[1] == "pingInterval")
      keepAliveInterval = Parser.Value.toInt();

    if (Parser.Nodes[1] == "pingTimeout")
      ;
  }

  while (client.available())
    readLine();

  DEBUG_PRINTLN(F(""));

  //Generate 16-Byte random key
  byte key[16];

  esp_fill_random(key, 16);

  for (int i = 0; i < 16; i++)
  {
    DEBUG_PRINTLN(key[i]);
  }

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

  writeLine("GET /socket.io/?transport=websocket&sid=" + sid + " HTTP/1.1");
  writeLine("Host: " + hostname);
  writeLine("Connection: Upgrade");
  writeLine("Upgrade: websocket");
  writeLine("Sec-WebSocket-Version: 13");
  writeLine("Sec-WebSocket-Extensions: permessage-deflate");
  writeLine("Sec-WebSocket-Key: " + keytext); //16 byte in Base64
  writeLine("");

  DEBUG_PRINTLN(F(""));

  if (!waitForData(3000))
    return false;

  readLine();

  if (atoi(&databuffer[9]) != 101)
  { // check for "HTTP/1.1 101 response, means Updrage to Websocket OK
    while (client.available())
      readLine();
    client.stop();
    return false;
  }

  while (client.available())
    readLine();

  DEBUG_PRINTLN("");

  sendMessage("52"); //(String)engineIo_upgrade + (String)socketIo_event
  receive();         // treat the response as input
  return true;
}

bool SocketIO::connected()
{
  return client.connected();
}

void SocketIO::disconnect()
{
  client.stop();
}

void SocketIO::finalize()
{
}

char SocketIO::read()
{
  return ' ';
}

bool SocketIO::receive()
{
  unsigned long now = millis();

  if (now - lastKeepAlive > keepAliveInterval)
  {
    lastKeepAlive = now;

    DEBUG_PRINTLN("SocketIO: Send: Ping (keep alive)");
    sendMessage("2");
  }

  if (!client.connected())
  {
    if (!client.connect(mem_hostname.c_str(), mem_portnr))
      return 0;
  }

  if (!client.available())
  {
    return 0;
  }

  //Step of header readout
  int step = step_header;

  Message = "";

  //Count of bytes needed for length
  int lencnt = 0;

  //Length of data
  unsigned long len = 0;

  bool masked = false;
  bool compressed = false;

  char c;
  bool inflate = false;
  bool inflated = false;
  bool direct = false;

  while (true)
  {
    //Compressed data
    if (inflate)
    {
      if (!inflated)
      {
        DEBUG_PRINTLN("SocketIO: Receive: Inflate");
        DEBUG_PRINTLN("");

        inflater.Run(&client, len);
        len = 0;
        inflated = true;
      }
      //Read one char from inflater
      c = inflater.read();

      if (inflater.Done)
      {
        inflater.finalize();

        DEBUG_PRINTLN("");
        DEBUG_PRINTLN("SocketIO: Receive: END");
        DEBUG_PRINTLN("");
        return 1;
      }
    }
    //Uncompressed data
    else if (direct)
    {

      if (len == 0)

      {
        DEBUG_PRINTLN("");
        DEBUG_PRINTLN("SocketIO: Receive: END");
        DEBUG_PRINTLN("");
        return 1;
      }

      c = client.read();
      len--;
    }
    //Before data type known
    else
    {
      //Read one char from client
      c = client.read();
    }

    switch (step)
    {
    //Wait for hader character (0x81)
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

        DEBUG_PRINT("SocketIO: Receive: Header: ");
        DEBUG_PRINTLN((uint8_t)c);
        DEBUG_PRINT("SocketIO: Receive: FIN: ");
        DEBUG_PRINTLN(FIN);
        DEBUG_PRINT("SocketIO: Receive: RSV1: ");
        DEBUG_PRINT(RSV1);

        if (compressed)
          DEBUG_PRINTLN(" (compressed)");
        else
          DEBUG_PRINTLN(" (uncompressed)");

        DEBUG_PRINT("SocketIO: Receive: RSV2: ");
        DEBUG_PRINTLN(RSV2);
        DEBUG_PRINT("SocketIO: Receive: RSV3: ");
        DEBUG_PRINTLN(RSV3);
        DEBUG_PRINT("SocketIO: Receive: OPCODE: ");
        DEBUG_PRINTLN(OPCODE);
      }

      break;

    case step_length:

      masked = (c & message_masked);

      if (masked)
      {
        DEBUG_PRINTLN("SocketIO: Receive: Masked");
        c -= message_masked;
      }
      else
        DEBUG_PRINTLN("SocketIO: Receive: Unmasked");

      switch (c)
      {
      case message_len_16bit:
        lencnt = 2;
        len = 0;
        DEBUG_PRINT("SocketIO: Receive: Paylod data len (16 bit) = ");
        step = step_length2;
        break;

      case message_len_64bit:
        lencnt = 8;
        len = 0;
        DEBUG_PRINT("SocketIO: Receive: Paylod data len (64 bit) = ");
        step = step_length2;
        break;

      default:
        lencnt = 0;
        DEBUG_PRINT("SocketIO: Receive: Payload data len (8 bit) = ");
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

      DEBUG_PRINT("SocketIO: Receive: engineIo: ");
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

      DEBUG_PRINT("SocketIO: Receive: socketIo: ");
      DEBUG_PRINT(c);
      DEBUG_PRINT(" (");

      switch (c)
      {
      case socketIo_connect:
        DEBUG_PRINT("connect");
        step = step_data;
        break;
      case socketIo_disconnect:
        DEBUG_PRINT("disconnect");
        step = step_data;
        break;
      case socketIo_event:
        DEBUG_PRINT("event");
        step = step_data;
        break;
      case socketIo_ack:
        DEBUG_PRINT("ack");
        step = step_data;
        break;
      case socketIo_error:
        DEBUG_PRINT("error");
        step = step_data;
        break;
      case socketIo_binary_event:
        DEBUG_PRINT("binary event");
        step = step_data;
        break;
      case socketIo_binary_ack:
        DEBUG_PRINT("binary ack");
        step = step_data;
        break;
      default:
        DEBUG_PRINT("???");
        break;
      }

      DEBUG_PRINTLN(")");

      //DEBUG_PRINT("SocketIO: Receive: JSON: ");

      break;

    case step_data:

      DEBUG_PRINT(c);
      Message += c;

      break;
    }
  }

  return 0;
}

bool SocketIO::waitForData(unsigned long delay)
{
  unsigned long start = millis();
  while (!client.available() && (millis() < (start + delay)))
    ;

  return client.available();
}

void SocketIO::writeLine(String line)
{
  client.println(line);
  DEBUG_PRINT("SocketIO: Send: ");
  DEBUG_PRINTLN(line);
}

void SocketIO::readLine()
{
  for (int i = 0; i < DATA_BUFFER_LEN; i++)
    databuffer[i] = ' ';

  dataptr = databuffer;

  while (client.available() && (dataptr < &databuffer[DATA_BUFFER_LEN - 2]))
  {
    char c = client.read();
    if (c == 0)
      ;
    else if (c == 255)
      ;
    else if (c == '\r')
      ;
    else if (c == '\n')
      break;
    else
      *dataptr++ = c;
  }
  *dataptr = 0;

  DEBUG_PRINT("SocketIO: Receive: ");
  DEBUG_PRINTLN(databuffer);
}

void SocketIO::sendMessage(String message)
{
  int msglength = message.length();
  randomSeed(analogRead(0));
  String mask = "";
  String masked = message;

  for (int i = 0; i < 4; i++)
  {
    char a = random(48, 57);
    mask += a;
  }

  for (int i = 0; i < msglength; i++)
    masked[i] = message[i] ^ mask[i % 4];

  client.print((char)(message_header)); //has to be sent for proper communication

  //Depending on the size of the message
  if (msglength <= 125)
  {
    client.print((char)(msglength + message_masked)); //size of the message + 128 because message has to be masked
    DEBUG_PRINT("SocketIO: Send: Payload data len (8 bit) = ");
  }
  else if (msglength >= 126 && msglength <= 65535)
  {
    client.print((char)(message_len_16bit + message_masked));
    DEBUG_PRINT("SocketIO: Send: Paylod data len (16 bit) = ");
    for (int i = 0; i < 2; i++)
    {
      char temp = (char)((msglength >> (8 * (2 - 1 - i))) & 255);
      client.print(temp);
    }
  }
  else
  {
    client.print((char)(message_len_64bit + message_masked));
    DEBUG_PRINT("SocketIO: Send: Paylod data len (64 bit) = ");
    for (int i = 0; i < 8; i++)
    {
      char temp = (char)((msglength >> (8 * (8 - 1 - i))) & 255);
      client.print(temp);
    }
  }

  DEBUG_PRINTLN(msglength);
  DEBUG_PRINT("SocketIO: Send: Message = ");
  DEBUG_PRINTLN(message);
  DEBUG_PRINTLN("");

  client.print(mask);
  client.print(masked);
}

void SocketIO::sendJSON(String command, String data)
{
  String message = "42[\"" + command + "\"";

  if (data != "")
    message += "," + data;

  message += "]";

  sendMessage(message);
}