#pragma once

#include "button.h"

#define DEFAULT_LONGPRESS_INTERVAL_MS   1000
#define DEFAULT_TEMP_BLINK_INTERVAL_MS  50

/**
 * @brief Callback triggered when the footswitch's state changes
 * @param id footswitch identifier
 * @param enabled true if footswitch is on
 */
typedef void (*footswitch_cb_t)(uint8_t id, bool enabled);


/**
 * @brief Implements the behaviour of a footswitch, with its LED.
 *
 * Can simulate the behaviour of a mechanical footswitch:
 * - state becomes *enabled* as soon as the footswitch is *down*
 * 
 * - state will be *disabled* when footswitch is *clicked*
 * 
 * - temp mode: when longpressed, the footswitch triggers temp mode and will be 
 * enabled as long as it is *held down*
 * 
 * Can simulate the behaviour of a logic footswitch:
 * - state becomes enabled/disabled as soon as the footswitch is *down*
 * 
 * - temp mode: when longpressed, the footswitch triggers temp mode and will 
 * keep the new state (enabled/disabled) as long as it is *held down*
 * 
 * In temp mode, the LED will blink.
 * 
 * External pullup wiring: Vcc => button pin A
 *                                button pin B => input pin => 10k => GND
 * 
 * Internal pullup wiring: GND => button pin A
 *                                button pin B => input pin
 */
class Footswitch: public Button, ButtonListener {

private:
    bool            _mechanical;
    bool            _on;
    bool            _disable;
    bool            _led;
    unsigned long   _blinkTS;
    unsigned long   _blinkIntervalMS;
    uint8_t         _ledPin;
    footswitch_cb_t _fptr;

private:
    void simulate_mechanical(uint8_t aID, EButtonScanResult aResult);
    void simulate_immediate(uint8_t aID, EButtonScanResult aResult);

public:
    void onButtonEvent(uint8_t aID, EButtonScanResult aResult);
    
    /**
     * @brief Sets footswitch state by software
     * 
     * @param aEnable on/off
     */
    void enable(bool aEnable);
    
    /**
     * @brief Construct a new Footswitch object
     * 
     * @param aPin digital pin attached to the button (see wiring above)
     * @param aLEDPin digital pin where the LED is attached
     * @param aCallback catch footswitch state changes
     * @param aMechanical simulate mechanical footswitch
     * @param aMode digital read mode
     * @param aLongpressDelayMS enable temp mode after this time, when button is held down
     * @param aBlinkIntervalMS enable LED blinking in temp mode
     */
    Footswitch(uint8_t aPin, 
               uint8_t aLEDPin, 
               footswitch_cb_t aCallback,
               bool aMechanical = false,
               uint8_t aMode = INPUT_PULLUP,
               uint16_t aLongpressDelayMS = DEFAULT_LONGPRESS_INTERVAL_MS, 
               uint16_t aBlinkIntervalMS = DEFAULT_TEMP_BLINK_INTERVAL_MS) :
    Button(aPin, aLongpressDelayMS, this, aMode) {
        pinMode(_ledPin, OUTPUT);
        _on = _disable = _led = false;
        _mechanical = aMechanical;
        _blinkTS = 0;
        _blinkIntervalMS = aBlinkIntervalMS;
        _ledPin  = aLEDPin;
        _fptr = aCallback;
    }
};