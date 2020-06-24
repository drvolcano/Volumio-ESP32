#include "ScrollText.h"

void ScrollText::initialize(U8G2 *displayU8G2)
{
    display = displayU8G2;
};

void ScrollText::reset()
{
    offset = 0;
}

/*
void ScrollText::draw()
{

    //Text of first line centered or scrolling, depending on width
    line1_width = display->getUTF8Width(line1.c_str());

    if (line1_width < DisplayWidth)
        display->drawUTF8((DisplayWidth - line1_width) / 2, 16 * 3, line1.c_str());
    else
    //Scrolling text
    {
        int x = line1_offset;

        if (line1_width > 0)
            do
            {
                display->drawUTF8(x, 48, line1.c_str());

                //second text
                x += line1_width + scrollGapStatus;
            } while (x < DisplayWidth);
    }
};*/

void ScrollText::process()
{
    unsigned long now = millis();

    //Calculate milliseconds since last scroll
    if (lastscroll != 0)
        scrollbuffer += now - lastscroll;
    lastscroll = now;

    //Scroll only after x milliseconds (one pixel)
    if (scrollbuffer >= ScrollInterval)
    {
        scrollbuffer -= ScrollInterval;
        offset -= 1;
    }

    //Restart scrolling when at end
    if ((u8g2_uint_t)offset < (u8g2_uint_t)-width - scrollGapStatus)
        offset = 0;
};