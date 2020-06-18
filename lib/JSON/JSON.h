#include "Arduino.h"
#include "CharStream.h"

class JSON
{
#define Type_None 0
#define Type_Class 1
#define Type_Array 2

public:
  void Parse(String Message);
  void Load(String Message);
  void ConnectToStream(CharStream* stream);
  bool Read();

  String Value;
  String Nodes[10];
  int NodeType[10];
  bool Popped;
  int Count;
  String GetPath();

private:

  bool fromStream= false;
  CharStream* charStream;

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
