#include "../lib/LedVerteActuator.h"

LedVerteActuator::LedVerteActuator(int pin) : pin(pin) {
    pinMode(pin, OUTPUT);
}

void LedVerteActuator::allumer() {
    digitalWrite(pin, HIGH);
}

void LedVerteActuator::eteindre() {
    digitalWrite(pin, LOW);
}
