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

#include "JSON.h"

//#define DEBUG_JSON

#ifdef DEBUG_JSON
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINT(x) Serial.print(x)
#else
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT(x)
#endif

void JSON::Push()
{
  DEBUG_PRINTLN("");
  DEBUG_PRINTLN("JSON: Push()");
  DEBUG_PRINT("JSON: ");
  stackpos++;
}

void JSON::Pop()
{
  DEBUG_PRINTLN("");
  DEBUG_PRINTLN("JSON: Pop()");
  DEBUG_PRINT("JSON: ");
  stack[stackpos] = "";
  stacktype[stackpos] = 0;
  stackpos--;
  Popped = true;
}

String JSON::getPath()
{

  String result = "";

  for (int j = 0; j <= Count; j++)
  {
    if (NodeType[j] == Type_Class)
      result += ".";

    if (NodeType[j] == Type_Array)
      result += "[";

    result += Nodes[j];

    if (NodeType[j] == Type_Array)
      result += "]";
  }

  return result;
}

void JSON::Print()
{

  Count = stackpos;

  for (int j = 0; j < 10; j++)
  {
    Nodes[j] = stack[j];
    NodeType[j] = stacktype[j];

    if (j > stackpos)
      Nodes[j] = "";
  }

  Value = textValue;
}

void JSON::initialize(CharStream *stream)
{
  charStream = stream;
  fromStream = true;
  Buffer = "";
  stackpos = 0;
  Index = 0;
  Value = "";
  text = false;
  value = false;
  lastwaspop = false;

  DEBUG_PRINTLN("JSON: Connect to Stream ");
}

void JSON::initialize(String Message)
{
  fromStream = false;
  Buffer = Message;
  stackpos = 0;
  Index = 0;
  Value = "";
  text = false;
  value = false;
  lastwaspop = false;

  DEBUG_PRINT("JSON: Load: ");
  DEBUG_PRINTLN(Message);
}

bool JSON::next()
{
  DEBUG_PRINTLN("JSON: next()");
  DEBUG_PRINT("JSON: ");
 
  Popped = false;

  Value = "";

  while (Index < Buffer.length() || fromStream)
  {
    bool doreturn;

    char c;

    if (fromStream)
    {
      c = charStream->readChar();

      DEBUG_PRINT(c);

      if (c == 0)
        break;
    }
    else
    {
      c = Buffer[Index];
      // DEBUG_PRINT("JSON: ");
      DEBUG_PRINT(c);
    }

    Index++;

    if (text)
      if (c == '"')
        text = false;
      else if (value)
        textValue += c;
      else
        stack[stackpos] += c;
    else
      switch (c)
      {
      case '{':

        Push();
        value = false;
        stacktype[stackpos] = Type_Class;
        break;

      case '[':

        Push();
        value = true;
        textValue = "";
        stacktype[stackpos] = Type_Array;
        aryindex = 0;
        stack[stackpos] = String(aryindex);
        break;

      case '}':

        doreturn = value;

        if (doreturn)
          Print();

        value = false;
        Pop();
        lastwaspop = true;

        if (doreturn)
        {
          DEBUG_PRINTLN("");
          return true;
        }
        break;

      case ']':
        doreturn = value;

        if (doreturn)
          Print();

        value = false;
        textValue = "";
        Pop();
        lastwaspop = true;

        if (doreturn)
        {
          DEBUG_PRINTLN("");
          return true;
        }
        break;

      case ':':
        value = true;
        textValue = "";
        break;

      case ',':
        value = (stacktype[stackpos] == Type_Array);
        doreturn = !lastwaspop;

        if (doreturn)
          Print();

        if (stacktype[stackpos] == Type_Array)
          stack[stackpos] = String(++aryindex);
        else
          stack[stackpos] = "";

        if (doreturn)
        {
          DEBUG_PRINTLN("");
          return true;
        }
        break;

      case ' ':
        break;

      case '"':
        lastwaspop = false;
        text = true;
        break;

      default:
        lastwaspop = false;
        textValue += c;
        break;
      }
  }
  DEBUG_PRINTLN("");
  return false;
}
