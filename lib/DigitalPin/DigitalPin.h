#include "Arduino.h"

class DigitalPin
{
public:
 //---
  bool pressed = false;    //pressed = rising edge (is now but was not before), readonly
  bool released = false;   //released = falling edge (is not now but was before), readonly
  void begin(int pin);     //initialize with pin number. Pin mode must be set outside
  void process();          //cyclic call of internal functions

private:
  int pinnr;              //number of input pin
  int debounce = 100;     //debounce time in milliseconds
  bool statusPin = false; //actual state of pin (read from input)
  bool statusDeb = false; //debounced state of pin (used debounce-delay)
  bool statusNow = false; //actual state after debouncing
  bool statusOld = false; //state of last processing
  unsigned long lastchange = 0; //last time the debounced state changed
};
