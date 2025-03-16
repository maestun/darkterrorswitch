#include <Arduino.h>
#include <SoftwareSerial.h>
#include "common.h"
#include "debug.h"

// -------------------------------------------------------------
// HARDWARE CONFIG
// add a 4k7 resistor between PIN_SOFTSERIAL and VCC
// no transistor needed to drive dual latch relays (TQ2-L2-5V)
// -------------------------------------------------------------
#define LATCH_PULSE_MS              20
#define PIN_RELAY_CHANNEL_SET       2
#define PIN_RELAY_CHANNEL_RESET     3
#define PIN_RELAY_BOOST_SET         4
#define PIN_RELAY_BOOST_RESET       5
#define PIN_RELAY_FXLOOP_SET        6
#define PIN_RELAY_FXLOOP_RESET      7
#define PIN_SOFTSERIAL              8

SoftwareSerial _serial(PIN_SOFTSERIAL, PIN_SOFTSERIAL/* ignore TX */);

typedef enum {
    RELAY_CHANNEL,
    RELAY_BOOST,
    RELAY_FXLOOP,
} eRelay;


void setup() {
    dprintinit(9600);
    dprintln(F("start"));
    _serial.begin(9600);
    pinMode(PIN_RELAY_CHANNEL_SET, OUTPUT);
    pinMode(PIN_RELAY_CHANNEL_RESET, OUTPUT);
    pinMode(PIN_RELAY_BOOST_SET, OUTPUT);
    pinMode(PIN_RELAY_BOOST_RESET, OUTPUT);
    pinMode(PIN_RELAY_FXLOOP_SET, OUTPUT);
    pinMode(PIN_RELAY_FXLOOP_RESET, OUTPUT);

    // reset to default amp settings
    digitalWrite(PIN_RELAY_CHANNEL_RESET, HIGH);
    delay(LATCH_PULSE_MS);
    digitalWrite(PIN_RELAY_CHANNEL_RESET, LOW);

    digitalWrite(PIN_RELAY_BOOST_RESET, HIGH);
    delay(LATCH_PULSE_MS);
    digitalWrite(PIN_RELAY_BOOST_RESET, LOW);

    digitalWrite(PIN_RELAY_FXLOOP_RESET, HIGH);
    delay(LATCH_PULSE_MS);
    digitalWrite(PIN_RELAY_FXLOOP_RESET, LOW);

    // wait for reset instructions from remote
    delay(1000);
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
                dprint(F("CHANNEL "));
                dprintln(state ? F("CLEAN") : F("DIRTY"));
                digitalWrite(state ? PIN_RELAY_CHANNEL_SET : PIN_RELAY_CHANNEL_RESET, HIGH);
                delay(LATCH_PULSE_MS);
                digitalWrite(state ? PIN_RELAY_CHANNEL_SET : PIN_RELAY_CHANNEL_RESET, LOW);
            }
            else if (relay == COMM_RELAY_BOOST) { 
                dprint(F("BOOST "));
                dprintln(state ? F("OFF") : F("ON"));
                digitalWrite(state ? PIN_RELAY_BOOST_SET : PIN_RELAY_BOOST_RESET, HIGH);
                delay(LATCH_PULSE_MS);
                digitalWrite(state ? PIN_RELAY_BOOST_SET : PIN_RELAY_BOOST_RESET, LOW);
            }
            else if (relay == COMM_RELAY_FXLOOP) {
                dprint(F("FXLOOP "));
                dprintln(state ? F("OF") : F("ON"));
                digitalWrite(state ? PIN_RELAY_FXLOOP_SET : PIN_RELAY_FXLOOP_RESET, HIGH);
                delay(LATCH_PULSE_MS);
                digitalWrite(state ? PIN_RELAY_FXLOOP_SET : PIN_RELAY_FXLOOP_RESET, LOW);
            }
        } 
        else {
            dprintln(F("wrong header"));
        }
    }
    delay(20);
}
