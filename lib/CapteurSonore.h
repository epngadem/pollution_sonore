#ifndef CAPTEUR_SONORE_H
#define CAPTEUR_SONORE_H

#include <Arduino.h>

class CapteurSonore
{
private:
    int pin;

public:
    CapteurSonore(int pin);
    int lireValeur();
};

#endif