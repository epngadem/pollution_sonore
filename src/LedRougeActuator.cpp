#include "../lib/LedRougeActuator.h"

LedRougeActuator::LedRougeActuator(int pin) : pin(pin) {
    pinMode(pin, OUTPUT);
}

void LedRougeActuator::allumer() {
    digitalWrite(pin, HIGH);
}

void LedRougeActuator::eteindre() {
    digitalWrite(pin, LOW);
}
