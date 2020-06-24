#include "ScrollText.h"

void ScrollText::initialize()
{
    offset = 0;
}

void ScrollText::reset()
{
    offset = 0;
}

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