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
#include "Arduino.h"
#include "WiFi.h"
#include "CharStream.h"

class Inflate : public CharStream
{

public:
  void initialize(Stream *dataStream, int count);
  char readChar();
  void finalize();
  bool getDone() { return done; }

private:
  int readBits(int cnt);
  Stream *stream;
  bool done = false;
  int totalBytes = 0;
  int abtualByte = 0;
  int length = 0;
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
