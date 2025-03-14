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
OneWire         _oneWire(PIN_ONEWIRE);
Button          _btChannel(PIN_BUTTON_CHANNEL, LONGPRESS_INTERVAL_MS, on_button_event);
Button          _btBoost(PIN_BUTTON_BOOST, LONGPRESS_INTERVAL_MS, on_button_event);
Button          _btFXLoop(PIN_BUTTON_FXLOOP, LONGPRESS_INTERVAL_MS, on_button_event);



// -------------------------------------------------------------
// CODE
// -------------------------------------------------------------

// Fonction pour envoyer une commande OneWire
void update_relay(int command, bool state) {
    // Démarrer la communication 1-Wire
    _oneWire.reset();         // Réinitialisation du bus
    _oneWire.write(0xCC);     // Skip ROM (ignorer l'adresse, un seul dispositif)
    _oneWire.write(0x00);     // Commande générique (peut être personnalisée)
    
    // Envoi de la commande spécifique pour chaque relais
    _oneWire.write(command & state);
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


void handle(EButtonScanResult event, int led_pin, int relay_command, sState* state) {
    static bool fx_on = false;
    static bool fx_led = false;
    static bool fx_disable = false;
    static unsigned long fx_ts = 0;

    switch (event) {
        case EButtonDown: {
            // button down: toggle fx on if it wasn't
            dprintln(F("DOWN"));
            if(fx_on == false) {
                fx_on = true;
                fx_disable = false;
                update_relay(relay_command, true);
                digitalWrite(led_pin, true);
            }
            else {
                fx_disable = true;   
            }
        } break;

        case EButtonUp: {
            // button released from shortpress: ignore
            dprintln(F("UP"));

        } break;

        case EButtonClick: {
            // button clicked: turn fx off if it was on
            dprintln(F("CLICK"));
            if(fx_on == true && fx_disable) {
                fx_on = false;
                update_relay(relay_command, false);
                digitalWrite(led_pin, false);
            }        
        } break;

        case EButtonLongpress: {
            // switch temp mode
            dprintln(" - lp");
            fx_led = true;
            fx_ts = millis();
        } break;
        case EButtonHold: {
            if (millis() - fx_ts > TEMP_BLINK_INTERVAL_MS) {
                fx_led = !fx_led;
                digitalWrite(led_pin, fx_led);
                fx_ts = millis();
            }    
        } break;
        case EButtonUnlongpress: {
            dprintln(" - ulp");
            fx_on = false;
            digitalWrite(led_pin, false);
            update_relay(relay_command, false);
        } break;
    }
}


void on_button_event(uint8_t id, EButtonScanResult event) {

    // static bool channel_on = false;
    // static bool boost_on = false;
    // static bool fxloop_on = false;

    // static bool channel_led = false;
    // static bool boost_led = false;
    // static bool fxloop_led = false;

    // static unsigned long channel_ts = 0;
    // static unsigned long boost_ts = 0;
    // static unsigned long fxloop_ts = 0;

    if (id == PIN_BUTTON_CHANNEL) {
        handle(event, PIN_LED_CHANNEL, COMM_RELAY_CHANNEL, NULL);
    }

    // switch (event) {

    //     case EButtonDown: {
    //         if (id == PIN_BUTTON_CHANNEL) {
    //             // button down: toggle fx on if it wasn't
    //             dprintln(F("DOWN"));
    //             if(channel_on == false) {
    //                 channel_on = true;
    //                 // fx_disable = false;
    //                 update_relay(COMM_RELAY_CHANNEL, channel_on);
    //                 digitalWrite(PIN_LED_CHANNEL, channel_on);
    //             }
    //             else {
    //                 // fx_disable = true;   
    //             }
    //         }
    //     } break;

    //     case EButtonUp: {
    //         // button released from shortpress: ignore
    //         dprintln(F("UP"));

    //     } break;

    //     case EButtonClick: {
    //         // button clicked: turn fx off if it was on
    //         if (id == PIN_BUTTON_CHANNEL) {
    //             dprintln(F("CLICK"));
    //             if(channel_on == true/*&& fx_disable*/) {
    //                 channel_on = false;
    //                 update_relay(COMM_RELAY_CHANNEL, channel_on);
    //                 digitalWrite(PIN_LED_CHANNEL, channel_on);
    //             }        
    //         }
    //     } break;


    //     case EButtonLongpress: {
    //         // switch temp mode
    //         dprint(id);
    //         dprint(":");
    //         dprint(event);
    //         dprintln(" - lp");
    //         if (id == PIN_BUTTON_CHANNEL) {
    //             channel_led = true;
    //             channel_ts = millis();
    //         }
    //         else if (id == PIN_BUTTON_BOOST) {
    //         }
    //         if (id == PIN_BUTTON_FXLOOP) {
    //         }
    //     } break;
    //     case EButtonHold: {
    //         dprint(id);
    //         dprint(":");
    //         dprint(event);
    //         dprintln(" - h");
    //         if (id == PIN_BUTTON_CHANNEL) {
    //             dprintln(millis() - channel_ts);
    //             if (millis() - channel_ts > TEMP_BLINK_INTERVAL_MS) {
    //                 dprintln(F("toogle"));
    //                 channel_led = !channel_led;
    //                 digitalWrite(PIN_LED_CHANNEL, channel_led);
    //                 channel_ts = millis();
    //             }    
    //         }
    //         else if (id == PIN_BUTTON_BOOST) {
                
    //         }
    //         if (id == PIN_BUTTON_FXLOOP) {
                
    //         }
            
    //     } break;
    //     case EButtonUnlongpress: {
    //         dprint(id);
    //         dprint(":");
    //         dprint(event);
    //         dprintln(" - ulp");
    //         if (id == PIN_BUTTON_CHANNEL) {
    //             digitalWrite(PIN_LED_CHANNEL, false);
    //             update_relay(COMM_RELAY_CHANNEL, false);
    //         }
    //         else if (id == PIN_BUTTON_BOOST) {
                
    //         }
    //         if (id == PIN_BUTTON_FXLOOP) {
                
    //         }
    //     } break;
    //     // case EButtonClick: {
    //     //     // toggle
    //     //     dprint(id);
    //     //     dprint(":");
    //     //     dprint(event);
    //     //     dprintln(" - clic");
    //     //     if (id == PIN_BUTTON_CHANNEL) {
    //     //         channel_on = !channel_on;
    //     //         update_relay(COMM_RELAY_CHANNEL, channel_on);
    //     //         digitalWrite(PIN_LED_CHANNEL, channel_on);
    //     //     }
    //     //     else if (id == PIN_BUTTON_BOOST) {
    //     //         boost_on = !boost_on;
    //     //         update_relay(COMM_RELAY_BOOST, boost_on);
    //     //         digitalWrite(PIN_LED_BOOST, boost_on);
    //     //     }
    //     //     if (id == PIN_BUTTON_FXLOOP) {
    //     //         fxloop_on = !fxloop_on;
    //     //         update_relay(COMM_RELAY_FXLOOP, fxloop_on);
    //     //         digitalWrite(PIN_LED_FXLOOP, fxloop_on);
    //     //     }
    //     // } break;
    //     default: break;
    // }
}


void setup() {

    dprintinit(9600);

    dprintln("start");

    pinMode(PIN_LED_CHANNEL, OUTPUT);
    pinMode(PIN_LED_BOOST, OUTPUT);
    pinMode(PIN_LED_FXLOOP, OUTPUT);

    // _btChannel.setup(PIN_BUTTON_CHANNEL, INPUT_PULLUP, true);
    // _btBoost.setup(PIN_BUTTON_BOOST, INPUT_PULLUP, true);
    // _btFXLoop.setup(PIN_BUTTON_FXLOOP, INPUT_PULLUP, true);

    // _btChannel.setLongPressIntervalMs(LONGPRESS_INTERVAL_MS);
    // _btBoost.setLongPressIntervalMs(LONGPRESS_INTERVAL_MS);
    // _btFXLoop.setLongPressIntervalMs(LONGPRESS_INTERVAL_MS);

    // _btChannel.attachLongPressStart([] () {
    //     Serial.println(F("lpstart channel"));
    //     _isChannelTemp = true;
    //     digitalWrite(PIN_LED_CHANNEL, HIGH);
    //     update_relay(COMM_RELAY_CHANNEL, true);
    // });
    // _btChannel.attachLongPressStop([] () {
    //     Serial.println(F("lpstop channel"));
    //     _isChannelTemp = false;
    //     digitalWrite(PIN_LED_CHANNEL, LOW);
    //     update_relay(COMM_RELAY_CHANNEL, false);
    // });
    // _btChannel.attachDuringLongPress([] () {
    //     // Serial.println(F("lpidle chanel"));
    //     static bool led_state = HIGH;
    //     static unsigned long ts = 0;
    //     if (millis() - ts > TEMP_BLINK_INTERVAL_MS) {
    //         ts = millis();
    //         led_state = !led_state;
    //         digitalWrite(PIN_BUTTON_CHANNEL, led_state);
    //     }
    // });
    // _btChannel.attachClick([] () {
    //     Serial.println(F("click channel"));
    //     static bool relay_state = true;
    //     update_relay(COMM_RELAY_CHANNEL, relay_state);
    //     digitalWrite(PIN_BUTTON_CHANNEL, relay_state);
    //     relay_state = !relay_state;
    // });


    // _btBoost.attachLongPressStart([] () {
    //     Serial.println(F("lpstart boost"));
    //     // _isBoostTemp = true;
    //     digitalWrite(PIN_LED_BOOST, HIGH);
    //     update_relay(COMM_RELAY_BOOST, true);
    // });
    // _btBoost.attachLongPressStop([] () {
    //     Serial.println(F("lpstop boost"));
    //     // _isBoostTemp = false;
    //     digitalWrite(PIN_LED_BOOST, LOW);
    //     update_relay(COMM_RELAY_BOOST, false);
    // });
    // _btBoost.attachDuringLongPress([] () {
    //     // Serial.println(F("lpidle boost"));
    //     static bool led_state = HIGH;
    //     static unsigned long ts = 0;
    //     if (millis() - ts > TEMP_BLINK_INTERVAL_MS) {
    //         ts = millis();
    //         led_state = !led_state;
    //         digitalWrite(PIN_BUTTON_BOOST, led_state);
    //     }
    // });
    // _btBoost.attachClick([] () {
    //     Serial.println(F("click boost"));
    //     static bool relay_state = true;
    //     update_relay(COMM_RELAY_BOOST, relay_state);
    //     digitalWrite(PIN_BUTTON_BOOST, relay_state);
    //     relay_state = !relay_state;
    // });


    // _btFXLoop.attachLongPressStart([] () {
    //     Serial.println(F("lpstart fxloop"));
    //     _isFXLoopTemp = true;
    //     digitalWrite(PIN_LED_FXLOOP, HIGH);
    //     update_relay(COMM_RELAY_FXLOOP, true);
    // });
    // _btFXLoop.attachLongPressStop([] () {
    //     Serial.println(F("lpstop fxloop"));
    //     _isFXLoopTemp = false;
    //     digitalWrite(PIN_LED_FXLOOP, LOW);
    //     update_relay(COMM_RELAY_FXLOOP, false);
    // });
    // _btFXLoop.attachDuringLongPress([] () {
    //     // Serial.println(F("lpidle fxloop"));
    //     static bool led_state = HIGH;
    //     static unsigned long ts = 0;
    //     if (millis() - ts > TEMP_BLINK_INTERVAL_MS) {
    //         ts = millis();
    //         led_state = !led_state;
    //         digitalWrite(PIN_BUTTON_FXLOOP, led_state);
    //     }
    // });
    // _btFXLoop.attachClick([] () {
    //     Serial.println(F("click fxloop"));
    //     static bool relay_state = true;
    //     update_relay(COMM_RELAY_FXLOOP, relay_state);
    //     digitalWrite(PIN_BUTTON_FXLOOP, relay_state);
    //     relay_state = !relay_state;
    // });
}

void loop() {
    _btChannel.scan();
    _btBoost.scan();
    _btFXLoop.scan();
    // _btChannel.tick();
    // _btBoost.tick();
    // _btFXLoop.tick();
}
