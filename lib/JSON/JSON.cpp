#include "JSON.h"

//#define  DEBUG_JSON

#ifdef DEBUG_JSON
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINT(x) Serial.print(x)
#else
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT(x)
#endif

void JSON::Push()
{
  DEBUG_PRINTLN("JSON: Push ");
  stackpos++;
}

void JSON::Pop()
{
  DEBUG_PRINTLN("JSON: Pop ");
  stack[stackpos] = "";
  stacktype[stackpos] = 0;
  stackpos--;
  Popped = true;
}

String JSON::GetPath()
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

void JSON::ConnectToStream(CharStream* stream)
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

  DEBUG_PRINT("JSON: Connect to Stream ");
}

void JSON::Load(String Message)
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

bool JSON::Read()
{
  Popped = false;

  Value = "";

  while (Index < Buffer.length() || fromStream)
  {
    bool doreturn;

    char c;

    if (fromStream)
    {
      c = charStream->read();
      if (c == 0)
        break;
    }
    else
    {
      c = Buffer[Index];
    }

    DEBUG_PRINT("JSON: ");
    DEBUG_PRINTLN(c);

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
          return true;
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
          return true;
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
          return true;
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

  return false;
}

void JSON::Parse(String Message)
{

  for (int i = 0; i < Message.length(); i++)
  {
    char c = Message[i];

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
        if (value)
          Print();
        value = false;
        Pop();
        break;

      case ']':
        if (value)
          Print();
        value = false;
        textValue = "";
        Pop();
        break;

      case ':':
        value = true;
        textValue = "";
        break;

      case ',':
        Print();
        value = (stacktype[stackpos] == Type_Array);

        if (stacktype[stackpos] == Type_Array)
          stack[stackpos] = String(++aryindex);
        else
          stack[stackpos] = "";
        break;

      case ' ':
        break;

      case '"':
        text = true;
        break;

      default:
        textValue += c;
        break;
      }
  }
}
