#include "DigitalPin.h"
//TEST

void DigitalPin::begin(int pin)
{
  pinnr = pin;
  statusPin = digitalRead(pinnr) == LOW;
  lastchange = millis();
}

void DigitalPin::process()
{
  unsigned long now = millis();

  //now should always be higher than lastchange.
  //but after overflow this may occur
  if (now < lastchange)
    lastchange = now;

  //Get actual state of pin (pressed = low)
  statusPin = digitalRead(pinnr) == LOW;

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
