#include <StreamDebugger.h>
#include <A76XX.h>

// dump all communication with the module to the standard serial port
#define DEBUG_AT true

// Use the correct `Serial` object to connect to the simcom module
#if DEBUG_AT
    StreamDebugger SerialAT(Serial1, Serial);
#else
    #define SerialAT Serial1
#endif

// MQTT detailed
const char* clientID      = "test_client";
const char* server        = "test.mosquitto.org";
const int   port          = 1883;
const int   keepalive     = 60;
const bool  clean_session = true;
const bool  use_ssl       = false;

// replace with your apn
const char* apn           = "simbase";

A76XX modem(SerialAT);
A76XXMQTTClient mqtt_client(modem, clientID, use_ssl);

// configuration for serial port to simcom module (check your board!)
#define PIN_TX   26
#define PIN_RX   27

void setup() {
    // begin serial port
    Serial.begin(115200);

    // must begin UART communicating with the SIMCOM module
    Serial1.begin(115200, SERIAL_8N1, PIN_RX, PIN_TX);

    // wait a little so we can see the output
    delay(3000);

    Serial.print("Waiting for modem ... ");
    if (modem.init() == false) {
        Serial.println("error");
        while (true) {}
    }
    Serial.println("OK");

    Serial.print("Waiting for modem to register on network ... ");
    if (modem.waitForRegistration() == false) {
        Serial.println("registration timed out");
        while (true) {}
    }
    Serial.println("done");

    Serial.print("Connecting  ... ");
    if (modem.GPRSConnect(apn) == false){
        Serial.println("cannot connect");
        while (true) {}
    }
    Serial.println("connected");

    Serial.print("Starting client  ... ");
    if (mqtt_client.begin() == false) {
        Serial.println("error");
        while (true) {}
    }
    Serial.println("done");

    Serial.print("Connecting to mosquitto test server  ... ");
    if (mqtt_client.connect(server, port, clean_session, keepalive) == false) {
        Serial.println("error");
        while (true) {}
    }
    Serial.println("done");

    Serial.print("Subscribe to topic  ... ");
    if (mqtt_client.subscribe("_this_is_a_test_topic_") == false) {
        Serial.println("error");
        while (true) {}
    }
    Serial.println("done");
}

void loop() {
    // main loop – we parse messages as fast as we can from the serial interface
    // with the SIMCOM module, and put them in a queue for the processing task
    // running on another core. If messages are received more quickly than the
    // processing task can process them, some messages might get dropped. Creating
    // a larger queue that can store more messages might alleviate such issues.
    
    A76XXURC_t urc = modem.listen(1000);
    Serial.print(".");

    switch (urc) {
        case A76XXURC_t::MQTT_MESSAGE_RX : {
            MQTTMessage_t msg = mqtt_client.getMessage();
            Serial.println("Received message ...");
            Serial.print("  topic: ");   Serial.println(msg.topic);
            Serial.print("  payload: "); Serial.println(msg.payload);

            // shut down the module remotely
            if (strcmp(msg.payload, "power-off") == 0) {
                modem.powerOff();
            }
            break;
        }

        case A76XXURC_t::MQTT_CONNECTION_LOST : {
            // esp_restart();
        }

        case A76XXURC_t::MQTT_NO_NET : {
            // esp_restart();
        }

        case A76XXURC_t::CGEV_PDP_DETACH : {
            // esp_restart();
        }

        default : {
            // esp_restart();
        }
    }
}
