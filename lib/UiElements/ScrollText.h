#include "U8g2lib.h"

class ScrollText
{

public:
    int scrollGapStatus = 16;
    void process();
    void reset();
    void initialize();
    String line1;
    long ScrollInterval = 10; //ms
    int offset;
    int width;
    long lastscroll = 0;
    int scrollbuffer = 0;

private:
  

#define DisplayWidth 128
};