#include <WiFi.h>
#include "Inflate.h"
#include "JSON.h"

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

class SocketIO
{
public:
  bool connect(String hostname, int portnr);
  bool connected();
  void disconnect();
 
  void sendMessage(String message);
  void sendJSON(String RID, String JSON = "");

  bool receive();
  void finalize();
  char read();

  String Message;

private:
  bool waitForData(unsigned long delay);
  String readLine();
  void writeLine(String line);
  String genKey();

  String mem_hostname;
  int mem_portnr;

  unsigned long lastKeepAlive = 0;
  long keepAliveInterval = 5000;//ms

  WiFiClient client;
  Inflate inflater;

  char *dataptr;


  String sid;
  JSON Parser;

  #define DATA_BUFFER_LEN 1000
  char databuffer[DATA_BUFFER_LEN];


 //Length of data
  unsigned long len = 0;
  bool masked = false;
  bool compressed = false;
  bool inflate = false;
  bool inflated = false;
  bool direct = false;
  char c;

};
