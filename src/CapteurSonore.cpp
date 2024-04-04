#include "../lib/CapteurSonore.h"

CapteurSonore::CapteurSonore()// Constructeur par défaut 
{
    
    pin = 32;
}

CapteurSonore::CapteurSonore(int pin) : pin(pin)// Constructeur personnalisé
{
    pinMode(pin, INPUT);
}

int CapteurSonore::lireValeur()//Méthode lireValeur
{
    return analogRead(pin);
}