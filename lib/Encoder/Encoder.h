#include "Arduino.h"

class Encoder
{

public:
    void begin(int PinA, int PinB); //Initialize with numbers for both pins. Pin mode must be set outside
    void A_CHANGED();   //Call this in ISR for Pin A
    void B_CHANGED();   //Call this in ISR for Pin B
    void reset();       //Reset value to 0
    void set(int value);//Set value to a given number
    int Value;          //Actual value of encoder, readonly
    bool changed;
    void process();          //cyclic call of internal functions

private:
    int PIN_A;  //number of input pin A
    int PIN_B;  //number of input pin B
    bool ACT_A; //state of pin A when pin B changed
    bool ACT_B; //state of pin B when pin A changed
    bool OLD_A; //old state of pin A
    bool OLD_B; //old state of pin B
    int oldState; //state of both pins
    int cnt = 0; //internal counter value
    int halvings = 1; //how many times "cnt" is divided by 2 to get "Value"
    void Update(); //internal function to process pin changes
    int OldValue = 0;

};
