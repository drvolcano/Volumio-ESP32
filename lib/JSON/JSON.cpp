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

void JSON::stackPush()
{
  DEBUG_PRINTLN_LVL2("");
  DEBUG_PRINT_LVL2("JSON: stackPush(): ");
  DEBUG_PRINT_LVL2(stackIndex);

  stackIndex++;
  popped = false;

  DEBUG_PRINT_LVL2(" --> ");
  DEBUG_PRINTLN_LVL2(stackIndex);
  DEBUG_PRINT_LVL2("JSON: next(): read: ");
}

void JSON::stackPop()
{
  DEBUG_PRINTLN_LVL2("");
  DEBUG_PRINT_LVL2("JSON: stackPop(): ");
  DEBUG_PRINT_LVL2(stackIndex);

  stack[stackIndex].Name = "";
  stack[stackIndex].Type = unknown;
  stack[stackIndex].ArrayIndex = 0;

  stackIndex--;
  popped = true;

  DEBUG_PRINT_LVL2(" --> ");
  DEBUG_PRINTLN_LVL2(stackIndex);

  DEBUG_PRINT_LVL2("JSON: stackPop(): Name = ");
  DEBUG_PRINTLN_LVL2(stack[stackIndex].Name);
  DEBUG_PRINT_LVL2("JSON: stackPop(): Type = ");
  DEBUG_PRINTLN_LVL2(printType(stack[stackIndex].Type));
  DEBUG_PRINT_LVL2("JSON: stackPop(): ArrayIndex = ");
  DEBUG_PRINTLN_LVL2(stack[stackIndex].ArrayIndex);

  DEBUG_PRINT_LVL2("JSON: next(): read: ");
}

String JSON::getPath()
{
  String result = "";

  for (int j = 0; j <= nodeCount; j++)
  {
    if (nodes[j].Type == Class)
      result += ".";

    if (nodes[j].Type == Array)
      result += "[";

    result += nodes[j].Name;

    if (nodes[j].Type == Array)
      result += "]";
  }

  return result;
}

void JSON::generateOutput()
{
  nodeCount = stackIndex;

  for (int j = 0; j < 10; j++)
  {

    if (j > stackIndex)
    {
      nodes[j].Name = "";
      nodes[j].Type = unknown;
      nodes[j].ArrayIndex = -1;
    }
    else
      nodes[j] = stack[j];
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
  DEBUG_PRINT_LVL2("JSON: next(): read: ");

  popped = false;

  actualValue = "";
  textValue = "";

  bool doreturn;
  char c;

  //As long as there is data
  while (index < buffer.length() || fromStream)
  {
    //Read one char
    if (fromStream)
    {
      c = charStream->readChar();
      if (c == 0 || c == -1)
        break;
    }
    else
    {
      c = buffer[index++];
    }

    DEBUG_PRINT_LVL2(c);

    if (text)
      if (controlchar)
      {
        textValue += c;
        controlchar = false;
      }
      else if (c == '"')
        text = false;
      else if (c == '\\')
        controlchar = true;
      else if (value)
        textValue += c;
      else
        stack[stackIndex].Name += c;
    else
      switch (c)
      {
      case '{':

        stackPush();
        value = false;
        stack[stackIndex].Type = Class;
        break;

      case '[':

        stackPush();
        value = true;
        textValue = "";
        stack[stackIndex].Type = Array;
        stack[stackIndex].ArrayIndex = 0;
        stack[stackIndex].Name = String(stack[stackIndex].ArrayIndex);
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
          DEBUG_PRINTLN_LVL2("");
          DEBUG_PRINT("JSON: next(): line: ");
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
          DEBUG_PRINTLN_LVL2("");
          DEBUG_PRINT("JSON: next(): line: ");
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
        value = (stack[stackIndex].Type == Array);
        doreturn = !lastwaspop;

        if (doreturn)
          generateOutput();

        DEBUG_PRINTLN_LVL2("");
        DEBUG_PRINT_LVL2("JSON: next(): Type = ");
        DEBUG_PRINTLN_LVL2(printType(stack[stackIndex].Type));

        if (stack[stackIndex].Type == Array)
        {
          DEBUG_PRINT_LVL2("JSON: next(): Arrayindex : ");
          DEBUG_PRINT_LVL2(stack[stackIndex].ArrayIndex);

          stack[stackIndex].Name = String(++stack[stackIndex].ArrayIndex);

          DEBUG_PRINT_LVL2(" --> ");
          DEBUG_PRINTLN_LVL2(stack[stackIndex].ArrayIndex);
        }
        else
          stack[stackIndex].Name = "";

        if (doreturn)
        {

          DEBUG_PRINT("JSON: next(): line: ");
          DEBUG_PRINT(getPath());
          DEBUG_PRINT(" = ");
          DEBUG_PRINTLN(getValue());

          return true;
        }
        else
          DEBUG_PRINT_LVL2("JSON: next(): read: ");
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

  DEBUG_PRINTLN("JSON: next(): END");

  return false;
}

String JSON::printType(StackType type)
{
  switch (type)
  {
  case unknown:
    return "unknown";
    break;
  case None:
    return "None";
    break;
  case Class:
    return "Class";
    break;
  case Array:
    return "Array";
    break;
  }
}