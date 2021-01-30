#include "TextSplitter.h"

void TextSplitter::initialize(String text, int max, char breakChar)
{
    breakingChar= breakChar;
    sum = text;
    maxchars = max;
    i = 0;
    block = "";
    buffer = "";
    line = "";

    openbracket1 = false;
    openbracket2 = false;
};

bool TextSplitter::process()
{
    if (buffer.length() + block.length() <= maxchars)
    {
        buffer += block;
        block = "";
        return true;
    }
    return false;
}

bool TextSplitter::next()
{

    while (i < sum.length())
    {
        char c = sum[i++];

        //Split text with special char
        if(c==breakingChar)
          break;
           
        block += c;

        if (c == '(')
            openbracket1 = true;

        if (c == ')')
            openbracket1 = false;

        if (c == '[')
            openbracket2 = true;

        if (c == ']')
            openbracket2 = false;

        if (c == ' ' && !openbracket1 && !openbracket2)
            if (!process())
                break;
        
    }

    process();
    line = buffer;
    buffer = block;
    block = "";

    line.trim();

    return i != sum.length() || block != "" || buffer != "" || line != "";
};
