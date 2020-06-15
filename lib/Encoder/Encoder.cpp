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
    Update();
}

void Encoder::B_CHANGED()
{
    //debouncing method: memorize pin A when pin B changes
    ACT_A = digitalRead(PIN_A) == LOW;
    Update();
}

void Encoder::reset()
{
    set(0);
}

void Encoder::set(int value)
{
    //Internal value may have more digits than official value (e.g. factor 2)
    cnt = value << halvings;
    Value = cnt >> halvings;

}

void Encoder::Update()
{
    if (ACT_A && OLD_B && (ACT_B ^ OLD_A))
        cnt++;

    if (ACT_B && OLD_A && (ACT_A ^ OLD_B))
        cnt--;

    OLD_A = ACT_A;
    OLD_B = ACT_B;

    Value = cnt >> halvings;
}

void Encoder::process()
{
    changed = Value != OldValue;
    OldValue = Value;
}
