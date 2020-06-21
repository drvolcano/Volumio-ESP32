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

#include "Inflate.h"

#include "LibDebug.h"
#if (DEBUGLEVEL_INFLATE >= 1)
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINT(x) Serial.print(x)
#else
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT(x)
#endif

#if (DEBUGLEVEL_INFLATE >= 2)
#define DEBUG_PRINTLN_LVL2(x) Serial.println(x)
#define DEBUG_PRINT_LVL2(x) Serial.print(x)
#else
#define DEBUG_PRINTLN_LVL2(x)
#define DEBUG_PRINT_LVL2(x)
#endif

#define BTYPE_NONE 0
#define BTYPE_FIXED 1
#define BTYPE_DYNAMIC 2
#define BTYPE_ERROR 3

/* //https://tools.ietf.org/html/rfc1951
  Extra               Extra               Extra
Code Bits Length(s) Code Bits Lengths   Code Bits Length(s)
---- ---- ------     ---- ---- -------   ---- ---- -------
  257   0     3       267   1   15,16     277   4   67-82
  258   0     4       268   1   17,18     278   4   83-98
  259   0     5       269   2   19-22     279   4   99-114
  260   0     6       270   2   23-26     280   4  115-130
  261   0     7       271   2   27-30     281   5  131-162
  262   0     8       272   2   31-34     282   5  163-194
  263   0     9       273   3   35-42     283   5  195-226
  264   0    10       274   3   43-50     284   5  227-257
  265   1  11,12      275   3   51-58     285   0    258
  266   1  13,14      276   3   59-66
*/

const int LengthBits[] =
    {
        0, 0, 0, 0,
        0, 0, 0, 0,
        1, 1, 1, 1,
        2, 2, 2, 2,
        3, 3, 3, 3,
        4, 4, 4, 4,
        5, 5, 5, 5,
        0};

const int LengthLengths[] =
    {
        3, 4, 5, 6,
        7, 8, 9, 10,
        11, 13, 15, 17,
        19, 23, 27, 31,
        35, 43, 51, 59,
        67, 83, 99, 115,
        131, 163, 195, 227,
        258};

/* //https://tools.ietf.org/html/rfc1951
  Extra           Extra               Extra
Code Bits Dist  Code Bits   Dist     Code Bits Distance
---- ---- ----  ---- ----  ------    ---- ---- --------
0   0    1     10   4     33-48    20    9   1025-1536
1   0    2     11   4     49-64    21    9   1537-2048
2   0    3     12   5     65-96    22   10   2049-3072
3   0    4     13   5     97-128   23   10   3073-4096
4   1   5,6    14   6    129-192   24   11   4097-6144
5   1   7,8    15   6    193-256   25   11   6145-8192
6   2   9-12   16   7    257-384   26   12  8193-12288
7   2  13-16   17   7    385-512   27   12 12289-16384
8   3  17-24   18   8    513-768   28   13 16385-24576
9   3  25-32   19   8   769-1024   29   13 24577-32768
*/

const int ExtraBits[] =
    {
        0, 0, 0, 0,
        1, 1, 2, 2,
        3, 3, 4, 4,
        5, 5, 6, 6,
        7, 7, 8, 8,
        9, 9, 10, 10,
        11, 11, 12, 12,
        13, 13};

const int ExtraDist[] =
    {
        1, 2, 3, 4,
        5, 7, 9, 13,
        17, 25, 33, 49,
        65, 97, 129, 193,
        257, 385, 513, 769,
        1025, 1537, 2049, 3073,
        4097, 6145, 8193, 12289,
        16385, 24577};

/* //https://tools.ietf.org/html/rfc1951
code lengths for the code length
alphabet given just above, in the order: 16, 17, 18,
0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
*/

const byte LengthOrder[] =
    {
        16, 17, 18, 0,
        8, 7, 9, 6,
        10, 5, 11, 4,
        12, 3, 13, 2,
        14, 1, 15};

int Inflate::readBits(int cnt)
{
  int b = 0;

  for (int i = 0; i < cnt; i++)
  {
    if (bcnt == 0)
    {
      TEMP = (*stream).read();
      abtualByte++;
      bcnt = 8;
    }

    b += (TEMP & 1) << i;
    TEMP = TEMP >> 1;
    bcnt--;
  }

  return b;
}

void Inflate::finalize()
{
  DEBUG_PRINT("Inflate: finalize(): bytes to terminate = ");
  DEBUG_PRINTLN(totalBytes - abtualByte);

  while (abtualByte < totalBytes)
  {
    (*stream).read();
    abtualByte++;
  }
}

char Inflate::readChar()
{
  if (length <= RESPTR)
  {
    int temp2 = 0;
    int i2 = 0;
    bool ok2 = false;
    int res2 = 0;

    while (true)
    {
      bool bit = readBits(1) != 0;
      temp2 <<= 1;
      if (bit)
        temp2 += 1;

      i2++;

      for (int j = 0; j < HLIT; j++)
      {
        if (lengths[j] == i2)

          if (treecode_lit[j] == temp2)
          {
            res2 = j;
            ok2 = true;
            break;
          }
      }

      if (ok2)
        break;
    }

    //End
    if (res2 == 256)
    {

      DEBUG_PRINTLN_LVL2("");
      done = true;
      return -1;
    }
    //Normal char
    else if (res2 < 256)
    {
      distbuffer[length % LEN_DISTBUFFER] = (char)res2;
      length++;
    }
    //Distance
    else if (res2 > 256)
    {
      int val = res2 - 257;

      int extra = LengthBits[val];
      int extra2 = 0;

      if (extra > 0)
      {
        extra2 = readBits(extra);
      }

      int res3 = 0;
      int temp3 = 0;
      int i3 = 0;
      bool ok3 = false;

      while (true)
      {
        bool bit = readBits(1) != 0;
        temp3 <<= 1;
        if (bit)
          temp3 += 1;

        i3++;

        for (int j = 0; j < HDIST; j++)
        {
          if (lengths[j + HLIT] == i3)

            if (treecode_dist[j] == temp3)
            {
              res3 = j;
              ok3 = true;
              break;
            }
        }

        if (ok3)
          break;
      }

      int extra4 = ExtraBits[res3];
      int extra5 = readBits(extra4);

      int addlen = LengthLengths[val] + extra2;
      int adddist = ExtraDist[res3] + extra5;

      int addpos = length - adddist;

      for (int i = 0; i < addlen; i++)
      {
        distbuffer[length % LEN_DISTBUFFER] = distbuffer[(addpos + i + LEN_DISTBUFFER) % LEN_DISTBUFFER];
        length++;
      }
    }
  }
  DEBUG_PRINT_LVL2(distbuffer[(RESPTR) % LEN_DISTBUFFER]);
  return distbuffer[(RESPTR++) % LEN_DISTBUFFER];
}

void Inflate::initialize(Stream *dataStream, int byteCount)
{
  totalBytes = byteCount;
  abtualByte = 0;
  stream = dataStream;
  done = false;
  RESPTR = 0;
  bcnt = 0;
  TEMP = 0;

  DEBUG_PRINT("Inflate: initialize(): count = ");
  DEBUG_PRINTLN(totalBytes);

  BFINAL = readBits(1) != 0;
  BTYPE = readBits(2);

  DEBUG_PRINT("Inflate: initialize(): BFINAL = ");
  DEBUG_PRINTLN(BFINAL);
  DEBUG_PRINT("Inflate: initialize(): BTYPE = ");
  DEBUG_PRINT(BTYPE);

  switch (BTYPE)
  {
  case BTYPE_NONE:
    DEBUG_PRINTLN(" (no compression)");
    break;

  case BTYPE_FIXED:
    DEBUG_PRINTLN(" (compressed with fixed Huffman codes)");
    break;

  case BTYPE_DYNAMIC:
    DEBUG_PRINTLN(" (compressed with dynamic Huffman codes)");
    break;

  case BTYPE_ERROR:
    DEBUG_PRINTLN(" (reserved (error))");
    break;
  }

  //to be done
  if (BTYPE == BTYPE_ERROR)
  {
    DEBUG_PRINT("Inflate: initialize(): BTYPE_ERROR not programmed ");
    return;
  }

  //to be done
  if (BTYPE == BTYPE_NONE)
  {
    DEBUG_PRINT("Inflate: initialize(): BTYPE_NONE not programmed ");
    return;
  }

  //to be done
  if (BTYPE == BTYPE_FIXED)
  {
    DEBUG_PRINT("Inflate: BTYPE_FIXED not programmed ");
    return;

    //Fixed lengths
    HLIT = 288; //Maximum = 257 + 2^5 - 1 = 288
    HDIST = 32; //Maximum = 1 + 2^5 - 1 = 32
    HCLEN = 0;  //Maximum = 4 + 2^4 - 1 = 19

    for (int i = 0; i < 288; i++)
    {
      if (i <= 143)
        lengths[i] = 8;
      else if (i <= 255)
        lengths[i] = 9;
      else if (i <= 255)
        lengths[i] = 7;
      else if (i <= 287)
        lengths[i] = 8;
    }
  }

  //WORKING
  if (BTYPE == BTYPE_DYNAMIC)
  {
    // 5 Bits: HLIT, # of Literal/Length codes - 257 (257 - 286)
    // 5 Bits: HDIST, # of Distance codes - 1        (1 - 32)
    // 4 Bits: HCLEN, # of Code Length codes - 4     (4 - 19)

    HLIT = readBits(5) + 257;
    HDIST = readBits(5) + 1;
    HCLEN = readBits(4) + 4;

    DEBUG_PRINT("Inflate: initialize(): HLIT = ");
    DEBUG_PRINTLN(HLIT);
    DEBUG_PRINT("Inflate: initialize(): HDIST = ");
    DEBUG_PRINTLN(HDIST);
    DEBUG_PRINT("Inflate: initialize(): HCLEN = ");
    DEBUG_PRINTLN(HCLEN);

    lenpos = 0;
    code_clen = 0;
    code_lit = 0;
    code_dist = 0;
    length = 0;

    for (int i = 0; i < 8; i++)
    {
      lencnt_clen[i] = 0;
      nextcode_clen[i] = 0;
    }

    for (int i = 0; i < 16; i++)
    {
      lenctn_lit[i] = 0;
      lencnt_dist[i] = 0;
    }

    for (int i = 0; i < 19; i++)
    {
      lensrt[i] = 0;
      treecode_clen[i] = 0;
    }

    for (int i = 0; i < 32; i++)
    {
      treecode_dist[i] = 0;
    }

    for (int i = 0; i < 288; i++)
    {
      treecode_lit[i] = 0;
    }

    for (int i = 0; i < 320; i++)
    {
      lengths[i] = 0;
    }

    DEBUG_PRINT("Inflate: initialize(): Lengths = {");

    //Read lengths
    for (int i = 0; i < HCLEN; i++)
    {
      int clen = readBits(3);
      lensrt[LengthOrder[i]] = clen;
      DEBUG_PRINT(clen);

      if (i != HCLEN - 1)
        DEBUG_PRINT(",");
    }

    DEBUG_PRINTLN("}");

    DEBUG_PRINT("Inflate: initialize(): Len Counts = {");

    //Count, how often we have lengths
    for (int i = 0; i < 19; i++)
      if (lensrt[i] != 0)
        lencnt_clen[lensrt[i]]++;

    for (int i = 0; i < 8; i++)
    {
      DEBUG_PRINT(lencnt_clen[i]);

      if (i < 8 - 1)
        DEBUG_PRINT(",");
    }
    DEBUG_PRINTLN("}");

    DEBUG_PRINT("Inflate: initialize(): CLEN: Startcodes = {");

    //Calculate start codes for buildup
    for (int i = 1; i < 8; i++)
    {
      nextcode_clen[i] = (nextcode_clen[i - 1] + lencnt_clen[i - 1]) << 1;

      DEBUG_PRINT(nextcode_clen[i]);

      if (i < 8 - 1)
        DEBUG_PRINT(",");
    }

    DEBUG_PRINTLN("}");

    DEBUG_PRINT("Inflate: initialize(): CLEN: Codes = {");

    //Generate codes
    for (int n = 0; n < 19; n++)
    {
      int len = lensrt[n];

      if (len != 0)
        treecode_clen[n] = nextcode_clen[len]++;
      else
        treecode_clen[n] = 0;

      DEBUG_PRINT(treecode_clen[n]);

      if (n < 19 - 1)
        DEBUG_PRINT(",");
    }
    DEBUG_PRINTLN("}");

    DEBUG_PRINT("Inflate: initialize(): CLEN: Data = {");

    while (lenpos < HLIT + HDIST)
    {
      bool ok = false;
      int i = 0;
      byte temp = 0;
      int res = 0;

      while (true)
      {
        bool bit = readBits(1) != 0;
        temp <<= 1;
        if (bit)
          temp += 1;

        i++;

        for (int j = 0; j < 19; j++)
        {
          if (lensrt[j] == i)
            if (treecode_clen[j] == temp)
            {
              res = j;
              ok = true;
              break;
            }
        }

        if (ok)
          break;
      }

      if (res <= 15)
      {
        lengths[lenpos++] = res;
      }
      else if (res == 16)
      {
        int cnt = readBits(2) + 3;

        int prev = lengths[lenpos - 1];

        for (int n = 0; n < cnt; n++)
          lengths[lenpos++] = prev;
      }
      else if (res == 17)
      {
        int cnt = readBits(3) + 3;

        for (int n = 0; n < cnt; n++)
          lengths[lenpos++] = 0;
      }
      else if (res == 18)
      {
        int cnt = readBits(7) + 11;

        for (int n = 0; n < cnt; n++)
          lengths[lenpos++] = 0;
      }
      else
      {
      }

      DEBUG_PRINT(res);
      DEBUG_PRINT(",");
    }

    DEBUG_PRINTLN("}");

    //count lengths
    for (int i = 0; i < HLIT; i++)
      if (lengths[i] != 0)
        lenctn_lit[lengths[i]]++;

    nextcode_lit[0] = 0;

    DEBUG_PRINT("Inflate: initialize(): LIT: Startcodes = {");

    //calculate start codes
    for (int i = 1; i < 16; i++)
    {
      nextcode_lit[i] = (nextcode_lit[i - 1] + lenctn_lit[i - 1]) << 1;

      DEBUG_PRINT(nextcode_lit[i]);

      if (i < 16 - 1)
        DEBUG_PRINT(",");
    }

    DEBUG_PRINTLN("}");

    DEBUG_PRINT("Inflate: initialize(): LIT: Codes = {");

    for (int n = 0; n < HLIT; n++)
    {
      int len = lengths[n];
      if (len != 0)
      {
        treecode_lit[n] = nextcode_lit[len];
        nextcode_lit[len]++;
      }

      DEBUG_PRINT(treecode_lit[n]);

      if (n < HLIT - 1)
        DEBUG_PRINT(",");
    }
    DEBUG_PRINTLN("}");

    //count lengths
    for (int i = 0; i < HDIST; i++)
      if (lengths[i + HLIT] != 0)
        lencnt_dist[lengths[i + HLIT]]++;

    nextcode_dist[0] = 0;

    DEBUG_PRINT("Inflate: initialize(): DIST: Startcodes = {");

    //calculate start codes
    for (int i = 1; i < 16; i++)
    {
      nextcode_dist[i] = (nextcode_dist[i - 1] + lencnt_dist[i - 1]) << 1;

      DEBUG_PRINT(nextcode_dist[i]);

      if (i < 16 - 1)
        DEBUG_PRINT(",");
    }

    DEBUG_PRINTLN("}");

    DEBUG_PRINT("Inflate: initialize(): DIST: Codes = {");

    for (int n = 0; n < HDIST; n++)
    {
      int len = lengths[n + HLIT];
      if (len != 0)
      {
        treecode_dist[n] = nextcode_dist[len];
        nextcode_dist[len]++;
      }

      DEBUG_PRINT(treecode_dist[n]);

      if (n < HDIST - 1)
        DEBUG_PRINT(",");
    }
    DEBUG_PRINTLN("}");
    DEBUG_PRINTLN("");
  }

  return;
}
