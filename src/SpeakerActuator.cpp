#include "../lib/SpeakerActuator.h"

SpeakerActuator::SpeakerActuator(int pin) : pin(pin) {
    pinMode(pin, OUTPUT);
}

void SpeakerActuator::jouerSon(int frequence) {
    tone(pin, frequence);
}

void SpeakerActuator::arreterSon() {
    noTone(pin);
}