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


#include "Arduino.h"
#include "CharStream.h"

class JSON
{

public:
  void initialize(String Message);
  void initialize(CharStream *stream);
  bool next();
  String getValue() { return actualValue; }
  String getPath();
  String getNode(int i) { return nodes[i]; }
  int getLevel() { return nodeCount; }
  int getBlockEnd() { return popped; }

private:

#define Type_None 0
#define Type_Class 1
#define Type_Array 2
  bool popped;
  int nodeCount;
  int nodeType[10];
  String actualValue;
  bool fromStream = false;
  CharStream *charStream;
  String nodes[10];
  String stack[10];
  int stackType[10];
  int stackIndex = 0;
  bool lastwaspop = false;
  void stackPush();
  void stackPop();
  void generateOutput();
  bool text = false;
  bool value = false;
  String textValue = "";
  String buffer;
  int index = 0;
  int aryindex = 0;
};
