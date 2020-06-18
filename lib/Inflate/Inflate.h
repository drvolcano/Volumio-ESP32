#include "Arduino.h"
#include "WiFi.h"
#include "CharStream.h"
class Inflate:public CharStream
{

public:
  void Run(WiFiClient *client, int count);
  char read();
  void finalize();
  int Length = 0;
  bool Done = false;

private:

  int maxread=0;
  int actualread=0;

  WiFiClient *source;
  int Bits(int cnt);
  int bcnt = 0;
  unsigned long RESPTR;
  byte TEMP;

  bool BFINAL = false;
  int BTYPE = 0;

  #define LEN_DISTBUFFER 32768
  char distbuffer[LEN_DISTBUFFER];

  int lensrt[19];   //19 per definition
  int lengths[320]; //Maximum = 288 + 32 (HLIT + HLEN)

  int HLIT = 288; //Maximum = 257 + 2^5 - 1 = 288
  int HDIST = 32; //Maximum = 1 + 2^5 - 1 = 32
  int HCLEN = 0;  //Maximum = 4 + 2^4 - 1 = 19

  int lencnt_clen[8];  //Maximum = 2^3 = 8 (range 0 - 7)
  int lenctn_lit[16];  //Maximum = 16 (range 0-15)
  int lencnt_dist[16]; //Maximum = 16 (range 0-15)

  int code_clen = 0;
  int code_lit = 0;
  int code_dist = 0;

  int nextcode_clen[8];  //see lencnt
  int nextcode_lit[16];  //see lencnt
  int nextcode_dist[16]; //see lencnt

  int treecode_clen[19]; //Maximum = 4 + 2^4 - 1 = 19
  int treecode_lit[288]; //Maximum = 257 + 2^5 - 1 = 288
  int treecode_dist[32]; //Maximum = 1 + 2^5 - 1 = 32

  int lenpos = 0;
};
