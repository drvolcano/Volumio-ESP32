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
