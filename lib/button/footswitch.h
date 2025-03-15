#pragma once

#include "button.h"

typedef void (*footswitch_cb_t)(uint8_t id, bool enabled);

class Footswitch: public Button, ButtonListener {

private:
    bool            _on;
    bool            _disable;
    bool            _led;
    unsigned long   _blinkTS;
    unsigned long   _blinkIntervalMS;
    uint8_t         _ledPin;
    footswitch_cb_t _fptr;
private:
    void handle(EButtonScanResult event, int id, int led_pin);

public:
    Footswitch(uint8_t aPin, 
               uint8_t aLEDPin, 
               uint16_t aLongpressDelayMS, 
               uint16_t aBlinkIntervalMS, 
               footswitch_cb_t aCallback) :
    Button(aPin, aLongpressDelayMS, this) {
        _on = _disable = _led = false;
        _blinkTS = 0;
        _blinkIntervalMS = aBlinkIntervalMS;
        _ledPin  = aLEDPin;
        _fptr = aCallback;
    }
};