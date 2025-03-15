#include <OneWire.h>
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
#define PIN_ONEWIRE             8


void on_footswitch_event(uint8_t id, bool state);


// -------------------------------------------------------------
// GLOBALS
// -------------------------------------------------------------
OneWire         _onewire(PIN_ONEWIRE);
Footswitch      _channel_button(PIN_BUTTON_CHANNEL, PIN_LED_CHANNEL, on_footswitch_event);
Footswitch      _boost_button(PIN_BUTTON_BOOST, PIN_LED_BOOST, on_footswitch_event);
Footswitch      _fxloop_button(PIN_BUTTON_FXLOOP, PIN_LED_FXLOOP, on_footswitch_event);



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

void on_footswitch_event(uint8_t id, bool state) {
    if (id == PIN_BUTTON_CHANNEL) {
        update_relay(COMM_RELAY_CHANNEL, state);
    }
    else if (id == PIN_BUTTON_BOOST) {
        update_relay(COMM_RELAY_BOOST, state);
    }
    else if (id == PIN_BUTTON_FXLOOP) {
        update_relay(COMM_RELAY_FXLOOP, state);
    }
}


void setup() {
    dprintinit(9600);
    dprintln("start");
}

void loop() {
    _channel_button.scan();
    _boost_button.scan();
    _fxloop_button.scan();
}
