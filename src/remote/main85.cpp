#include <EEPROM.h>
#include <OneWire.h>
#include "common.h"
#include "button.h"

// -------------------------------------------------------------
// HARDWARE CONFIG
// -------------------------------------------------------------
#define PIN_BUTTONS     A0
#define PIN_LED1        2
#define PIN_LED2        3
#define PIN_LED3        4
#define PIN_SOFTSERIAL     5

// -------------------------------------------------------------
// GLOBALS
// -------------------------------------------------------------
OneWire oneWire(PIN_SOFTSERIAL);
bool led1_on = false;
bool led2_on = false;
bool led3_on = false;

bool bt1_on = false;
bool bt2_on = false;
bool bt3_on = false;

#define ADDR_LED1        0
#define ADDR_LED2        1
#define ADDR_LED3        2

// -------------------------------------------------------------
// CODE
// -------------------------------------------------------------

// Fonction pour envoyer une commande OneWire
void update_relay(int command, bool state) {
  // Démarrer la communication 1-Wire
  oneWire.reset();         // Réinitialisation du bus
  oneWire.write(0xCC);     // Skip ROM (ignorer l'adresse, un seul dispositif)
  oneWire.write(0x00);     // Commande générique (peut être personnalisée)
  
  // Envoi de la commande spécifique pour chaque relais
  oneWire.write(command & state);
}



/*
    -- buttons --
    VCC (5v) --> 10k --> A0 pin
    A0 pin --> bt1 + bt2 + bt3
    bt1 --> 10k --> GND
    bt2 --> 22k --> GND
    bt2 --> 47k --> GND

    -- leds --
    Dxx --> 10k --> transistor Qxx base 
    VCC --> transistor Qxx collector
    GND -> LEDxx cathode
    LEDxx anode --> 470k --> Qxx emitter
*/

void setup() {
    // led1_on = led2_on = led3_on = false;
    pinMode(PIN_BUTTONS, INPUT);
    pinMode(PIN_LED1, OUTPUT);
    pinMode(PIN_LED2, OUTPUT);
    pinMode(PIN_LED3, OUTPUT);

    oneWire.begin(PIN_SOFTSERIAL);
    Serial.begin(9600);

    EEPROM.begin();
    led1_on = EEPROM.read(ADDR_LED1);
    led2_on = EEPROM.read(ADDR_LED2);
    led3_on = EEPROM.read(ADDR_LED3);

    update_relay(COMM_RELAY_CHANNEL, led1_on);
    update_relay(COMM_RELAY_BOOST, led2_on);
    update_relay(COMM_RELAY_FXLOOP, led3_on);

    digitalWrite(PIN_LED1, led1_on);
    digitalWrite(PIN_LED2, led2_on);
    digitalWrite(PIN_LED3, led3_on);
}

#define ANALOG_DELTA          (10)

#define BT1_ID                (1)
#define BT1_BT2_ID            (12)
#define BT1_BT3_ID            (13)
#define BT2_ID                (2)
#define BT2_BT3_ID            (23)
#define BT3_ID                (3)
#define BT1_BT2_BT3_ID        (123)


#define BT1_ANALOG            (505)
#define BT2_ANALOG            (704)
#define BT3_ANALOG            (845)

#define BT1_BT2_ANALOG        (413)
#define BT1_BT3_ANALOG        (457)
#define BT2_BT3_ANALOG        (614)
#define BT1_BT2_BT3_ANALOG    (379)

#define IS_NEAR(x, val)       ((x - ANALOG_DELTA < val) && (x + ANALOG_DELTA > val))



void bt1_handler(uint8_t id, EButtonScanResult result) {
    if (result == EButtonClick) {
        Serial.println("B1 clic");
        led1_on = !led1_on;
        digitalWrite(PIN_LED1, led1_on);
        update_relay(COMM_RELAY_CHANNEL, led1_on);
        EEPROM.update(ADDR_LED1, led1_on);
    }
}

void bt2_handler(uint8_t id, EButtonScanResult result) {
    if (result == EButtonClick) {
        Serial.println("B2 clic");
        led2_on = !led2_on;
        digitalWrite(PIN_LED2, led2_on);
        update_relay(COMM_RELAY_BOOST, led2_on);
        EEPROM.write(ADDR_LED2, led2_on);
    }
}

void bt3_handler(uint8_t id, EButtonScanResult result) {
    if (result == EButtonClick) {
      Serial.println("B3 clic");
        led3_on = !led3_on;
        digitalWrite(PIN_LED3, led3_on);
        update_relay(COMM_RELAY_FXLOOP, led3_on);
        EEPROM.write(ADDR_LED3, led3_on);   
    }
}

void bt12_handler(uint8_t id, EButtonScanResult result) {
    bt1_handler(id, result);
    bt2_handler(id, result);
}

void bt13_handler(uint8_t id, EButtonScanResult result) {
    bt1_handler(id, result);
    bt3_handler(id, result);
}

void bt23_handler(uint8_t id, EButtonScanResult result) {
    bt2_handler(id, result);
    bt3_handler(id, result);
}

void bt123_handler(uint8_t id, EButtonScanResult result) {
    bt1_handler(id, result);
    bt2_handler(id, result);
    bt3_handler(id, result);
}

AnalogButton * bt1 = new AnalogButton(PIN_BUTTONS, BT1_ANALOG, BT1_ID, ANALOG_DELTA, 1000, bt1_handler);
AnalogButton * bt2 = new AnalogButton(PIN_BUTTONS, BT2_ANALOG, BT2_ID, ANALOG_DELTA, 1000, bt2_handler);
AnalogButton * bt3 = new AnalogButton(PIN_BUTTONS, BT3_ANALOG, BT3_ID, ANALOG_DELTA, 1000, bt3_handler);

AnalogButton * bt12 = new AnalogButton(PIN_BUTTONS, BT1_BT2_ANALOG, BT1_BT2_ID, ANALOG_DELTA, 1000, bt12_handler);
AnalogButton * bt13 = new AnalogButton(PIN_BUTTONS, BT1_BT3_ANALOG, BT1_BT3_ID, ANALOG_DELTA, 1000, bt13_handler);
AnalogButton * bt23 = new AnalogButton(PIN_BUTTONS, BT2_BT3_ANALOG, BT2_BT3_ID, ANALOG_DELTA, 1000, bt23_handler);

AnalogButton * bt123 = new AnalogButton(PIN_BUTTONS, BT1_BT2_BT3_ANALOG, BT1_BT2_BT3_ID, ANALOG_DELTA, 1000, bt123_handler);

void loop() {
    // Serial.println(val);
    bt1->scan();
    bt2->scan();
    bt3->scan();

    bt12->scan();
    bt13->scan();
    bt23->scan();
    
    bt123->scan();
    // delay(100);


  //   int val = analogRead(PIN_BUTTONS);

  //   if (IS_NEAR(val, BT1_ANALOG) && bt1_on == false) {
  //     bt1_on = true;
  //       Serial.println("toggle B1");
  //       led1_on = !led1_on;
  //       digitalWrite(PIN_LED1, led1_on);
  //       update_relay(COMM_RELAY1, led1_on);
  //   }
  //   else {
      
  //       bt1_on = false;
  //   }

  // if (IS_NEAR(val, BT2_ANALOG)) {
  //   Serial.println("B2");
  //   led2_on = !led2_on;
  //   digitalWrite(PIN_LED2, led2_on);
  // }

  // if (IS_NEAR(val, BT3_ANALOG)) {
  //   Serial.println("B3");
  //   led3_on = !led3_on;
  //   digitalWrite(PIN_LED3, led3_on);
  // }

  // if (IS_NEAR(val, BT1_BT2_ANALOG)) {
  //   Serial.println("B1 + B2");
  // }

  // if (IS_NEAR(val, BT1_BT3_ANALOG)) {
  //   Serial.println("B1 + B3");
  // }

  // if (IS_NEAR(val, BT2_BT3_ANALOG)) {
  //   Serial.println("B2 + B3");
  // }

  // if (IS_NEAR(val, BT1_BT2_BT3_ANALOG)) {
  //   Serial.println("B1 + B2 + B3");
  // }

}
