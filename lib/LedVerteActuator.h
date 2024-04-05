#ifndef LED_VERTE_ACTUATOR_H
#define LED_VERTE_ACTUATOR_H

#include <Arduino.h>

class LedVerteActuator {
private:
    int pin;

public:
    LedVerteActuator(int pin);
    void allumer();
    void eteindre();
};

#endif
