#include <SoftwareSerial.h>
#include "footswitch.h"
#include "common.h"
#include "debug.h"

// -------------------------------------------------------------
// HARDWARE CONFIG
// -------------------------------------------------------------
#define PIN_BUTTON_CHANNEL      2
#define PIN_LED_CHANNEL         3
#define PIN_BUTTON_BOOST        4
#define PIN_LED_BOOST           5
#define PIN_BUTTON_FXLOOP       6
#define PIN_LED_FXLOOP          7
#define PIN_SOFTSERIAL          8

void on_footswitch_event(uint8_t id, bool state);

// -------------------------------------------------------------
// GLOBALS
// -------------------------------------------------------------
SoftwareSerial  _serial(PIN_SOFTSERIAL + 1/* ignore RX */, PIN_SOFTSERIAL);
Footswitch      _channel_button(PIN_BUTTON_CHANNEL, PIN_LED_CHANNEL, on_footswitch_event);
Footswitch      _boost_button(PIN_BUTTON_BOOST, PIN_LED_BOOST, on_footswitch_event);
Footswitch      _fxloop_button(PIN_BUTTON_FXLOOP, PIN_LED_FXLOOP, on_footswitch_event);

// -------------------------------------------------------------
// CODE
// -------------------------------------------------------------
void update_relay(int command, bool state) {
    uint8_t data = (command & 0xfe) + (state & 0x1);
    _serial.write(COMM_HEADER);
    _serial.write(data);
    // https://arduino.stackexchange.com/a/14202
    delay(2);
}

void on_footswitch_event(uint8_t id, bool state) {
    if (id == PIN_BUTTON_CHANNEL) {
        dprint(F("CHANNEL "));
        dprintln(state ? F("ON") : F("OFF"));
        update_relay(COMM_RELAY_CHANNEL, state);
    }
    else if (id == PIN_BUTTON_BOOST) {
        dprint(F("BOOST "));
        dprintln(state ? F("ON") : F("OFF"));
        update_relay(COMM_RELAY_BOOST, state);
    }
    else if (id == PIN_BUTTON_FXLOOP) {
        dprint(F("FXLOOP "));
        dprintln(state ? F("ON") : F("OFF"));
        update_relay(COMM_RELAY_FXLOOP, state);
    }
}

void reset() {
    // set amp default state
    _channel_button.enable(false);
    _boost_button.enable(false);
    _fxloop_button.enable(true);
}

void setup() {
    dprintinit(9600);
    dprintln("start");
    _serial.begin(9600);
    reset();
}

void loop() {
    _channel_button.scan();
    _boost_button.scan();
    _fxloop_button.scan();
}
