#include "Encoder.h"
#include "Arduino.h"

void Encoder::begin(int PinA, int PinB)
{
    PIN_A = PinA;
    PIN_B = PinB;
    Value = 0;
    OldValue = 0;
}

void Encoder::A_CHANGED()
{
    //debouncing method: memorize pin B when pin A changes
    ACT_B = digitalRead(PIN_B) == LOW;

    if (ACT_B ^ OLD_B)
    {
        if (ACT_A ^ OLD_B)
            cnt--;
        else
            cnt++;

        OLD_B = ACT_B;
    }
}

void Encoder::B_CHANGED()
{
    //debouncing method: memorize pin A when pin B changes
    ACT_A = digitalRead(PIN_A) == LOW;

    if (ACT_A ^ OLD_A)
    {
        if (ACT_B ^ OLD_A)
            cnt++;
        else
            cnt--;

        OLD_A = ACT_A;
    }
}

void Encoder::reset()
{
    set(0);
}

void Encoder::set(int value)
{
    //Internal value may have more digits than official value (e.g. factor 2)
    cnt = value << halvings;
    Value = cnt  >> halvings;
}

void Encoder::process()
{
    Value = (cnt + 2 )  >> halvings;
    //  Value = cnt;
    changed = Value != OldValue;
    OldValue = Value;
}
