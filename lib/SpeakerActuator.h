#ifndef SPEAKER_ACTUATOR_H
#define SPEAKER_ACTUATOR_H

#include <Arduino.h>

class SpeakerActuator {
private:
    int pin;

public:
    SpeakerActuator(int pin);
    void jouerSon(int frequence);
    void arreterSon();
};

#endif
