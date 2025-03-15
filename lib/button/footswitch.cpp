#include "footswitch.h"
#include "debug.h"

void Footswitch::handle(EButtonScanResult event, int id, int led_pin) {
    switch (event) {
        case EButtonDown: {
            // button down: toggle fx on if it wasn't
            dprint(id);
            dprintln(F(" - DOWN"));
            if(_on == false) {
                _on = true;
                _disable = false;
                _fptr(id, true);
                digitalWrite(led_pin, true);
            }
            else {
                _disable = true;   
            }
        } break;

        case EButtonUp: {
            // button released from shortpress: ignore
            dprint(id);
            dprintln(F(" - UP"));
        } break;

        case EButtonClick: {
            // button clicked: turn fx off if it was on
            dprint(id);
            dprintln(F(" - CLICK"));
            if(_on == true && _disable) {
                _on = false;
                _fptr(id, false);
                digitalWrite(led_pin, false);
            }        
        } break;

        case EButtonLongpress: {
            // switch temp mode
            dprint(id);
            dprintln(" - LP");
            _led = true;
            _blinkTS = millis();
        } break;
        case EButtonHold: {
            if (millis() - _blinkTS > _blinkIntervalMS) {
                _led = !_led;
                digitalWrite(led_pin, _led);
                _blinkTS = millis();
            }    
        } break;
        case EButtonUnlongpress: {
            dprint(id);
            dprintln(" - ULP");
            _on = false;
            digitalWrite(led_pin, false);
            _fptr(id, false);
        } break;
    }
}