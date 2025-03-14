#include <OneWire.h>
#include "button.h"
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
#define PIN_ONEWIRE             8

#define LONGPRESS_INTERVAL_MS   1000
#define TEMP_BLINK_INTERVAL_MS  50



void on_button_event(uint8_t id, EButtonScanResult event);


// -------------------------------------------------------------
// GLOBALS
// -------------------------------------------------------------
OneWire         _onewire(PIN_ONEWIRE);
Button          _channel_button(PIN_BUTTON_CHANNEL, LONGPRESS_INTERVAL_MS, on_button_event);
Button          _boost_button(PIN_BUTTON_BOOST, LONGPRESS_INTERVAL_MS, on_button_event);
Button          _fxloop_button(PIN_BUTTON_FXLOOP, LONGPRESS_INTERVAL_MS, on_button_event);



// -------------------------------------------------------------
// CODE
// -------------------------------------------------------------

// Fonction pour envoyer une commande OneWire
void update_relay(int command, bool state) {
    // Démarrer la communication 1-Wire
    _onewire.reset();         // Réinitialisation du bus
    _onewire.write(0xCC);     // Skip ROM (ignorer l'adresse, un seul dispositif)
    _onewire.write(0x00);     // Commande générique (peut être personnalisée)
    
    // Envoi de la commande spécifique pour chaque relais
    _onewire.write(command & state);
}

typedef struct {
    bool on;
    bool led;
    bool disable;
    unsigned long ts;
} sState;

sState _channel_state = { .on = false, .led = false, .disable = false, .ts = 0};
sState _boost_state = { .on = false, .led = false, .disable = false, .ts = 0};
sState _fxloop_state = { .on = false, .led = false, .disable = false, .ts = 0};


void handle(EButtonScanResult event, int id, int led_pin, int relay_command, sState* state) {
    switch (event) {
        case EButtonDown: {
            // button down: toggle fx on if it wasn't
            dprint(id);
            dprintln(F(" - DOWN"));
            if(state->on == false) {
                state->on = true;
                state->disable = false;
                update_relay(relay_command, true);
                digitalWrite(led_pin, true);
            }
            else {
                state->disable = true;   
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
            if(state->on == true && state->disable) {
                state->on = false;
                update_relay(relay_command, false);
                digitalWrite(led_pin, false);
            }        
        } break;

        case EButtonLongpress: {
            // switch temp mode
            dprint(id);
            dprintln(" - LP");
            state->led = true;
            state->ts = millis();
        } break;
        case EButtonHold: {
            if (millis() - state->ts > TEMP_BLINK_INTERVAL_MS) {
                state->led = !state->led;
                digitalWrite(led_pin, state->led);
                state->ts = millis();
            }    
        } break;
        case EButtonUnlongpress: {
            dprint(id);
            dprintln(" - ULP");
            state->on = false;
            digitalWrite(led_pin, false);
            update_relay(relay_command, false);
        } break;
    }
}


void on_button_event(uint8_t id, EButtonScanResult event) {

    if (id == PIN_BUTTON_CHANNEL) {
        handle(event, id, PIN_LED_CHANNEL, COMM_RELAY_CHANNEL, &_channel_state);
    }
    else if (id == PIN_BUTTON_BOOST) {
        handle(event, id, PIN_LED_BOOST, COMM_RELAY_BOOST, &_boost_state);
    }
    else if (id == PIN_BUTTON_FXLOOP) {
        handle(event, id, PIN_LED_FXLOOP, COMM_RELAY_FXLOOP, &_fxloop_state);
    }
}


void setup() {
    dprintinit(9600);
    dprintln("start");
    pinMode(PIN_LED_CHANNEL, OUTPUT);
    pinMode(PIN_LED_BOOST, OUTPUT);
    pinMode(PIN_LED_FXLOOP, OUTPUT);
}

void loop() {
    _channel_button.scan();
    _boost_button.scan();
    _fxloop_button.scan();
}
