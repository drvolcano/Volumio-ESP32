//#include "Arduino.h"
#include <WiFi.h>
#include "Inflate.h"

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
#define step_data 5

#define socketIo_connect '0'
#define socketIo_disconnect '1'
#define socketIo_event '2'
#define socketIo_ack '3'
#define socketIo_error '4'
#define socketIo_binary_event '5'
#define socketIo_binary_ack '6'

#define engineIo_open '0'
#define engineIo_close '1'
#define engineIo_ping '2'
#define engineIo_pong '3'
#define engineIo_message '4'
#define engineIo_upgrade '5'
#define engineIo_noop '6'


class SocketIO
{
public:
  bool connect(char *hostname, int port = 80);
  bool connected();
  void disconnect();
 
  void sendMessage(String message);
  void sendJSON(String RID, String JSON = "");

  bool receive();
  void finalize();
  char read();

  String Message;

private:
  bool waitForInput(void);
  void readLine();
  void writeLine(String line);

  unsigned long previousMillis = 0;
  long interval = 5000;

  WiFiClient client;
  Inflate inflater;

  char *dataptr;

  String sid;
  String hostname;
  int port;
};
