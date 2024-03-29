#include "../lib/CapteurSonore.h"

CapteurSonore::CapteurSonore()
{
    
    pin = 32;
}

CapteurSonore::CapteurSonore(int pin) : pin(pin)
{
    pinMode(pin, INPUT);
}

int CapteurSonore::lireValeur()
{
    return analogRead(pin);
}