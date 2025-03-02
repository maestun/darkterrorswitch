/*
 * User event => callback event
 * ----------------------------
 * Press button => EButtonDown
 * Release button (before longpress time) => EButtonUp then EButtonClick
 * Keep button pressed for lonpress time => EButtonLongpress
 * Release button (after longpress time) => EButtonUnlongpress
 *
 */
#include <stdint.h>
#include <Arduino.h>

typedef enum EButtonScanResult {
    EButtonUp = 0,      // off, idle state
    EButtonDown,        // button is pressed
    EButtonClick,       // down then up events happened < longpress time
    EButtonLongpress,   // button help down for > longpress time
    EButtonHold,        // button is still held after longpress
    EButtonUnlongpress  // button up from longpress
} EButtonScanResult;


class ButtonListener {
public:
    virtual void onButtonEvent(uint8_t aID, EButtonScanResult aResult) = 0;
};

typedef void (*event_cb_t)(uint8_t id, EButtonScanResult result);

class Button {
protected:
    uint8_t             _id;
    uint8_t             _prevState;
    bool                _longpressed;
    uint32_t            _longpressTS;
    uint32_t            _debounceTS;
    uint16_t            _longpressMS;
    ButtonListener *    _listener;
    event_cb_t          _fptr;
    void                onButtonReleased();
    void                onButtonPressed();
    void                scanLogic(int8_t aState);
public:
    Button(uint8_t aPin, uint16_t aLongpressDelayMS, ButtonListener * aListener);
    void scan();
};


class AnalogButton : Button {
private:
    uint16_t            _analogPin;
    uint16_t            _analogValue;
    uint8_t             _deltaValue;
public:
    AnalogButton::AnalogButton(uint8_t aAnalogPin, uint8_t aAnalogID, uint16_t aAnalogValue, uint8_t aDeltaValue, uint16_t aLongpressDelayMS, ButtonListener * aListener) :
        Button(aAnalogID, aLongpressDelayMS, aListener) {
            _analogValue = aAnalogValue;
            _deltaValue= aDeltaValue;
    }

    AnalogButton::AnalogButton(uint8_t aAnalogPin, 
                               uint16_t aAnalogValue,
                               uint8_t aAnalogID,
                               uint8_t aDeltaValue, 
                               uint16_t aLongpressDelayMS, 
                               event_cb_t aCallback) :
        Button(aAnalogID, aLongpressDelayMS, NULL) {
            _analogPin = aAnalogPin;
            _analogValue = aAnalogValue;
            _deltaValue = aDeltaValue;
            _fptr = aCallback;
    }
    
    void scan();
};