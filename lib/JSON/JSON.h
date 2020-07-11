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
#include "WString.h"
#include "CharStream.h"

#include "LibDebug.h"
#if (DEBUGLEVEL_JSON >= 1)
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINT(x) Serial.print(x)
#else
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT(x)
#endif

#if (DEBUGLEVEL_JSON >= 2)
#define DEBUG_PRINTLN_LVL2(x) Serial.println(x)
#define DEBUG_PRINT_LVL2(x) Serial.print(x)
#else
#define DEBUG_PRINTLN_LVL2(x)
#define DEBUG_PRINT_LVL2(x)
#endif



class JSON
{

public:
  
  void initialize(String Message);
  void initialize(CharStream *stream);
  bool next();
  String getValue() { return actualValue; }
  String getPath();
  String getType() { return actualType; }

  String getNode(int i) { return nodes[i].Name; }
  String getNode() { return nodes[nodeCount].Name; }
  String getParent() { return nodes[nodeCount - 1].Name; }

  int getLevel() { return nodeCount; }
  int getBlockEnd() { return popped; }

private:
  enum StackType
  {
    unknown,
    None,
    Class,
    Array
  };

  struct NodeStruct
  {
    int ArrayIndex;
    StackType Type;
    String Name;
  };

  bool popped;
  int nodeCount;

  String actualValue;
  String actualType;
  bool fromStream = false;
  CharStream *charStream;

  NodeStruct nodes[10];
  NodeStruct stack[10];
  int stackIndex = 0;
  bool lastwaspop = false;
  void stackPush();
  void stackPop();
  void generateOutput();
  String printType(StackType type);
  bool text = false;
  bool controlchar = false;
  bool value = false;
  String textValue = "";
  String buffer;
  int index = 0;
};
