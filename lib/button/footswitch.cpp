#include "footswitch.h"
#include "debug.h"

void Footswitch::onButtonEvent(uint8_t aID, EButtonScanResult aResult) {
    switch (aResult) {
        case EButtonDown: {
            // button down: toggle fx on if it wasn't
            dprint(aID);
            dprintln(F(" - DOWN"));
            if(_on == false) {
                _on = true;
                _disable = false;
                _fptr(aID, true);
                digitalWrite(_ledPin, true);
            }
            else {
                _disable = true;   
            }
        } break;

        case EButtonUp: {
            // button released from shortpress: ignore
            dprint(aID);
            dprintln(F(" - UP"));
        } break;

        case EButtonClick: {
            // button clicked: turn fx off if it was on
            dprint(aID);
            dprintln(F(" - CLICK"));
            if(_on == true && _disable) {
                _on = false;
                _fptr(aID, false);
                digitalWrite(_ledPin, false);
            }        
        } break;

        case EButtonLongpress: {
            // switch temp mode
            dprint(aID);
            dprintln(" - LP");
            _led = true;
            _blinkTS = millis();
        } break;
        case EButtonHold: {
            if (millis() - _blinkTS > _blinkIntervalMS) {
                _led = !_led;
                digitalWrite(_ledPin, _led);
                _blinkTS = millis();
            }    
        } break;
        case EButtonUnlongpress: {
            dprint(aID);
            dprintln(" - ULP");
            _on = false;
            digitalWrite(_ledPin, false);
            _fptr(aID, false);
        } break;
    }
}