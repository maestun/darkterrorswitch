#include <OneWire.h>  // Bibliothèque pour 1-Wire
#include <util/delay.h> // Pour les délais

#include "common.h"

// Définition des pins
#define PIN_RELAY1   PB0
#define PIN_RELAY2   PB1
#define PIN_RELAY3   PB2
#define PIN_COMM  PB3

OneWire oneWire(PIN_COMM);

void reset() {
    digitalWrite(PIN_RELAY1, LOW);
    digitalWrite(PIN_RELAY2, LOW);
    digitalWrite(PIN_RELAY3, LOW);
}

void setup() {
    pinMode(PIN_RELAY1, OUTPUT);
    pinMode(PIN_RELAY2, OUTPUT);
    pinMode(PIN_RELAY3, OUTPUT);
    oneWire.begin();

    reset();
}

void loop() {
    byte buffer[1];
    byte value = 0;
    byte state = 0;
    byte relay = 0;

    if (oneWire.available()) {
        oneWire.readBytes(buffer, sizeof(buffer));
    
        value = buffer[0];
        state = value & 1;
        relay = value >> 1;
    
        if (relay == COMM_RELAY1) {
            digitalWrite(PIN_RELAY1, state);
        }

        if (relay == COMM_RELAY2) { 
            digitalWrite(PIN_RELAY2, state);
        }

        if (relay == COMM_RELAY3) {
            digitalWrite(PIN_RELAY3, state);
        }
    }
  
    delay(50);
}
