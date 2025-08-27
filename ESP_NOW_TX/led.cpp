#include "led.h"

LED::LED(uint8_t pin) : _pin(pin), _state(false) {}

void LED::begin() {
    pinMode(_pin, OUTPUT);
    off(); // 默认关闭LED
}

void LED::on() {
    digitalWrite(_pin, HIGH);
    _state = true;
}

void LED::off() {
    digitalWrite(_pin, LOW);
    _state = false;
}

void LED::toggle() {
    if (_state) {
        off();
    } else {
        on();
    }
}

bool LED::isOn() {
    return _state;
}
