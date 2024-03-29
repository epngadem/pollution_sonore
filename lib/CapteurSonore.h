#ifndef CAPTEUR_SONORE_H
#define CAPTEUR_SONORE_H

#include <Arduino.h>

class CapteurSonore
{
private:
    int pin;

public:
    CapteurSonore(); // Constructeur par défaut
    CapteurSonore(int pin);
    int lireValeur();
};

#endif