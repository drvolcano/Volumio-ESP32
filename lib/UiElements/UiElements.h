#include "U8g2lib.h"

class UiElements
{

public:
    void drawProgressBar(int x,
                         int y,
                         int widh,
                         int lineThickness ,
                         int barThickness ,
                         int frameThickness,
                         int frameSpacing,
                         int knobWidth,
                         int knobHeigth,
                         float value

    );

    void drawProgressBar(int x,
                         int y,
                         int widh,
                         int style,
                         float value

    );

    void initialize(U8G2 *displayU8G2);

private:
    U8G2 *display;
};