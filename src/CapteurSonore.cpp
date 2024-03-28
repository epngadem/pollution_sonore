#include "../lib/CapteurSonore.h"

CapteurSonore::CapteurSonore(int pin) : pin(pin)
{
    pinMode(pin, INPUT);
}

int CapteurSonore::lireValeur()
{
    return analogRead(pin);
}