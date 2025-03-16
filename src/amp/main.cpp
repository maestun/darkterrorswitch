#include <OneWire.h>  // Bibliothèque pour 1-Wire
#include <util/delay.h> // Pour les délais

#include "common.h"

// Définition des pins
#define PIN_RELAY1   PB0
#define PIN_RELAY2   PB1
#define PIN_RELAY3   PB2
#define PIN_COMM     PB3

OneWire oneWire(PIN_SOFTSERIAL);

void reset() {
}

void setup() {
    pinMode(PIN_RELAY_CHANNEL, OUTPUT);
    pinMode(PIN_RELAY_BOOST, OUTPUT);
    pinMode(PIN_RELAY_FXLOOP, OUTPUT);
    oneWire.begin(PIN_SOFTSERIAL);

    digitalWrite(PIN_RELAY_CHANNEL, LOW);
    digitalWrite(PIN_RELAY_BOOST, LOW);
    digitalWrite(PIN_RELAY_FXLOOP, LOW);
}

void loop() {
    byte buffer[1];
    byte value = 0;
    byte state = 0;
    byte relay = 0;

    oneWire.read_bytes(buffer, sizeof(buffer));

    value = buffer[0];
    state = value & 0x01; // last bit == state
    relay = value & 0xfe; // other bits == relay value

    if (relay == COMM_RELAY1) {
        digitalWrite(PIN_RELAY_CHANNEL, state);
    }

    if (relay == COMM_RELAY2) { 
        digitalWrite(PIN_RELAY_BOOST, state);
    }

    if (relay == COMM_RELAY3) {
        digitalWrite(PIN_RELAY_FXLOOP, state);
    }

    delay(50);
}
