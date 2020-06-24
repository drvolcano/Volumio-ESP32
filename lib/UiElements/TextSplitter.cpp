#include "TextSplitter.h"

void TextSplitter::initialize(String text, int max)
{
    sum = text;
    maxchars = max;
    i = 0;
    block = "";
    buffer = "";
    line = "";
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
        block += c;

        if (c == ' ')
            if (!process())
                break;
    }

    process();
    line = buffer;
    buffer = block;
    block = "";

    return i != sum.length() || block != "" || buffer != "" || line != "";
};
