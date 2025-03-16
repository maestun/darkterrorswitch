#include <Arduino.h>
#include <SoftwareSerial.h>
#include "common.h"
#include "debug.h"

// -------------------------------------------------------------
// HARDWARE CONFIG
// add a 4k7 resistor between PIN_SOFTSERIAL and VCC
// -------------------------------------------------------------
#define PIN_RELAY_CHANNEL   2
#define PIN_RELAY_BOOST     4
#define PIN_RELAY_FXLOOP    6
#define PIN_SOFTSERIAL      8

SoftwareSerial _serial(PIN_SOFTSERIAL, PIN_SOFTSERIAL + 1/* ignore TX */);

void reset() {
    // set amp default state, all relays off:
    // - boost ON
    // - channel DIRT
    // - fxloop ON
    digitalWrite(PIN_RELAY_CHANNEL, LOW);
    digitalWrite(PIN_RELAY_BOOST, LOW);
    digitalWrite(PIN_RELAY_FXLOOP, LOW);
}

void setup() {
    dprintinit(9600);
    dprintln(F("start"));
    _serial.begin(9600);
    pinMode(PIN_RELAY_CHANNEL, OUTPUT);
    pinMode(PIN_RELAY_BOOST, OUTPUT);
    pinMode(PIN_RELAY_FXLOOP, OUTPUT);
    reset();
}

void loop() {

    if (_serial.available()) {
        uint8_t buf[2];
        _serial.readBytes(buf, 2);
        if (buf[0] == COMM_HEADER) {
            // last bit == state
            // in my amp's wiring, relays off => default amp behaviour !!
            uint8_t state = !(buf[1] & 0x01);
            // other bits == relay value
            uint8_t relay = buf[1] & 0xfe;
            dprint(F("relay: "));
            dprint(relay);
            dprint(F(" - state: "));
            dprintln(state);
            if (relay == COMM_RELAY_CHANNEL) {
                digitalWrite(PIN_RELAY_CHANNEL, state);
            }
            else if (relay == COMM_RELAY_BOOST) { 
                digitalWrite(PIN_RELAY_BOOST, state);
            }
            else if (relay == COMM_RELAY_FXLOOP) {
                digitalWrite(PIN_RELAY_FXLOOP, state);
            }
        } 
        else {
            dprintln(F("wrong header"));
        }
    }
    else {
        dprint(".");
    }
    delay(20);
}
