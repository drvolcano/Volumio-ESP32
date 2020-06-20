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

#include "LibDebug.h"
#ifdef DEBUG_JSON
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINT(x) Serial.print(x)
#else
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT(x)
#endif

void JSON::stackPush()
{
  DEBUG_PRINTLN(" --> Push()");
  DEBUG_PRINT("JSON: Read: ");
  stackIndex++;
}

void JSON::stackPop()
{
  DEBUG_PRINTLN(" --> Pop()");
  DEBUG_PRINT("JSON: Read: ");
  stack[stackIndex] = "";
  stackType[stackIndex] = 0;
  stackIndex--;
  popped = true;
}

String JSON::getPath()
{
  String result = "";

  for (int j = 0; j <= nodeCount; j++)
  {
    if (nodeType[j] == Type_Class)
      result += ".";

    if (nodeType[j] == Type_Array)
      result += "[";

    result += nodes[j];

    if (nodeType[j] == Type_Array)
      result += "]";
  }

  return result;
}

void JSON::generateOutput()
{
  nodeCount = stackIndex;

  for (int j = 0; j < 10; j++)
  {
    nodes[j] = stack[j];
    nodeType[j] = stackType[j];

    if (j > stackIndex)
      nodes[j] = "";
  }

  actualValue = textValue;
}

void JSON::initialize(CharStream *stream)
{
  charStream = stream;
  fromStream = true;
  buffer = "";
  stackIndex = 0;
  index = 0;
  actualValue = "";
  text = false;
  value = false;
  lastwaspop = false;

  DEBUG_PRINTLN("JSON: initialize(CharStream *Stream)");
}

void JSON::initialize(String Message)
{
  fromStream = false;
  buffer = Message;
  stackIndex = 0;
  index = 0;
  actualValue = "";
  text = false;
  value = false;
  lastwaspop = false;

  DEBUG_PRINT("JSON: initialize(String Message): ");
  DEBUG_PRINTLN(Message);
}

bool JSON::next()
{
  DEBUG_PRINTLN("JSON: next()");
  DEBUG_PRINT("JSON: Read: ");

  popped = false;

  actualValue = "";

  while (index < buffer.length() || fromStream)
  {
    bool doreturn;

    char c;

    if (fromStream)
    {
      c = charStream->readChar();
      if (c == 0 || c == -1)
      {  
      //  DEBUG_PRINT("<END>");
        break;
      }

      DEBUG_PRINT(c);
    }
    else
    {
      c = buffer[index];
      DEBUG_PRINT(c);
    }

    index++;

    if (text)
      if (c == '"')
        text = false;
      else if (value)
        textValue += c;
      else
        stack[stackIndex] += c;
    else
      switch (c)
      {
      case '{':

        stackPush();
        value = false;
        stackType[stackIndex] = Type_Class;
        break;

      case '[':

        stackPush();
        value = true;
        textValue = "";
        stackType[stackIndex] = Type_Array;
        aryindex = 0;
        stack[stackIndex] = String(aryindex);
        break;

      case '}':

        doreturn = value;

        if (doreturn)
          generateOutput();

        value = false;
        stackPop();
        lastwaspop = true;

        if (doreturn)
        {
          DEBUG_PRINTLN("");
          DEBUG_PRINT("JSON: Result: ");
          DEBUG_PRINT(getPath());
          DEBUG_PRINT(" = ");
          DEBUG_PRINTLN(getValue());

          return true;
        }
        break;

      case ']':
        doreturn = value;

        if (doreturn)
          generateOutput();

        value = false;
        textValue = "";
        stackPop();
        lastwaspop = true;

        if (doreturn)
        {
          DEBUG_PRINTLN("");
          DEBUG_PRINT("JSON: Result: ");
          DEBUG_PRINT(getPath());
          DEBUG_PRINT(" = ");
          DEBUG_PRINTLN(getValue());

          return true;
        }
        break;

      case ':':
        value = true;
        textValue = "";
        break;

      case ',':
        value = (stackType[stackIndex] == Type_Array);
        doreturn = !lastwaspop;

        if (doreturn)
          generateOutput();

        if (stackType[stackIndex] == Type_Array)
          stack[stackIndex] = String(++aryindex);
        else
          stack[stackIndex] = "";

        if (doreturn)
        {
          DEBUG_PRINTLN("");
          DEBUG_PRINT("JSON: Result: ");
          DEBUG_PRINT(getPath());
          DEBUG_PRINT(" = ");
          DEBUG_PRINTLN(getValue());

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
  DEBUG_PRINTLN("JSON: END");

  return false;
}
