#include "TouchPin.h"

void TouchPin::begin(int pin)
{
  pinnr = pin;
  value = touchRead(pinnr);
  lastchange = millis();
}

void TouchPin::process()
{
  unsigned long now = millis();

  //now should always be higher than lastchange.
  //but after overflow this may occur
  if (now < lastchange)
    lastchange = now;

  value = touchRead(pinnr);

  if (value > valuehigh)
    statusPin = false;

  if (value < valuelow)
    statusPin = true;

 //Only recognize a pin change after a certain time
  if (statusPin != statusDeb)
    if (now > lastchange + debounce)
    {
      statusDeb = statusPin;
      lastchange = now;
    }

 //shift states ( not --> old, debounced --> now)
  statusOld = statusNow;
  statusNow = statusDeb;

  //pressed = rising edge (is now but was not before)
  pressed = statusNow && !statusOld;

  //released = falling edge (is not now but was before)
  released = !statusNow && statusOld;
}
