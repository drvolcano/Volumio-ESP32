#include "UiElements.h"

void UiElements::initialize(U8G2 *displayU8G2)
{

    display = displayU8G2;
}

void UiElements::drawProgressBar(int x, int y, int widh, int style, float value)
{
    switch (style)
    {
    case 0:
        UiElements::drawProgressBar(x, y, widh, 1, 3, 0, 0, 5, 5, value);
        break;
    case 1:
        UiElements::drawProgressBar(x, y, widh, 0, 4, 1, 1, 0, 0, value);
   case 2:
        UiElements::drawProgressBar(x, y, widh, 0, 1, 0, 0, 0, 0, value);
        break;
    }
}

void UiElements::drawProgressBar(int x, int y, int widh, int lineThickness, int barThickness, int frameThickness, int frameSpacing, int knobWidth, int knobHeigth, float value)
{
    int thickest = (barThickness > knobHeigth) ? barThickness : knobHeigth;

    int thickness = frameThickness * 2 + thickest + frameSpacing * 2;
    float barlen = value * (widh - 2 * frameThickness - 2 * frameSpacing);
    float knobpos = value * (widh - 2 * frameThickness - 2 * frameSpacing - knobWidth);

    display->setColorIndex(1);
    display->drawRBox(x, y, widh, thickness, 0);

    display->setColorIndex(0);
    display->drawRBox(x + frameThickness, y + frameThickness, widh - 2 * frameThickness, thickness - 2 * frameThickness, 0);

    display->setColorIndex(1);

    if (lineThickness > 0)
        display->drawRBox(x + frameThickness + frameSpacing, y + frameThickness + frameSpacing + (thickest - lineThickness) / 2, widh - 2 * frameThickness - 2 * frameSpacing, lineThickness, 0);

    if (knobWidth > 0 && knobHeigth > 0)
    {
        display->drawRBox(x + int(knobpos), y + frameThickness + frameSpacing, knobWidth, knobHeigth, 0);

        if (int(barlen) > 0 && barThickness > 0)
            display->drawRBox(x + frameThickness + frameSpacing, y + frameThickness + frameSpacing + (thickest - barThickness) / 2, int(knobpos), barThickness, 0);
    }
    else if (int(barlen) > 0 && barThickness > 0)
        display->drawRBox(x + frameThickness + frameSpacing, y + frameThickness + frameSpacing + (thickest - barThickness) / 2, int(barlen), barThickness, 0);
}
