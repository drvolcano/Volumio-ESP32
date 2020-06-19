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
  String getValue() { return Value; }
  String getPath();
  String getNode(int i) { return Nodes[i]; }
  int getLevel() { return Count; }
  int getBlockEnd() { return Popped; }

private:

#define Type_None 0
#define Type_Class 1
#define Type_Array 2
  bool Popped;
  int Count;
  int NodeType[10];
  String Value;
  bool fromStream = false;
  CharStream *charStream;
  String Nodes[10];
  String stack[10];
  int stacktype[10];
  int stackpos = 0;
  bool lastwaspop = false;
  void Push();
  void Pop();
  void Print();
  bool text = false;
  bool value = false;
  String textValue = "";
  String Buffer;
  int Index = 0;
  int aryindex = 0;
};
