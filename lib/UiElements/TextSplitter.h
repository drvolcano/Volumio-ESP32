#include "Arduino.h"

class TextSplitter
{

public:
    void initialize(String text, int max, char breakChar);
    bool next();
    String line;

private:

    bool process();
    String sum;
    int maxchars;
    int i;
    String block;
    String buffer;
    char breakingChar= 0;

   bool openbracket1 = false;
   bool openbracket2 = false;


};