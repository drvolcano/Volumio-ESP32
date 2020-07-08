#include "Arduino.h"

class TextSplitter
{

public:
    void initialize(String text, int max);
    bool next();
    String line;

private:
    bool process();
    String sum;
    int maxchars;
    int i;
    String block;
    String buffer;

   bool openbracket1 = false;
   bool openbracket2 = false;


};