#ifndef LED_ROUGE_ACTUATOR_H
#define LED_ROUGE_ACTUATOR_H

#include <Arduino.h>

class LedRougeActuator {
private:
    int pin;

public:
    LedRougeActuator(int pin);
    void allumer();
    void eteindre();
};

#endif
