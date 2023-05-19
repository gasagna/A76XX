#include <StreamDebugger.h>
#include <A76XX.h>

// dump all communication with the module to the standard serial port
#define DEBUG_AT false

// Use the correct `Serial` object to connect to the simcom module
#if DEBUG_AT
    StreamDebugger SerialAT(Serial1, Serial);
#else
    #define SerialAT Serial1
#endif

// MQTT detailed
const char* clientID      = "test_client";
const char* server        = "test.mosquitto.org";
const char* will_topic    = "dying-nodes";
const char* will_message  = "Adieu!";
const int   will_qos      = 0;
const int   port          = 1883;
const int   keepalive     = 60;
const bool  clean_session = true;
const bool  use_ssl       = false;

// replace with your apn
const char* apn           = "simbase";

A76XX modem(SerialAT);
A76XXMQTTClient mqtt(modem, clientID, use_ssl);

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
    if (mqtt.begin() == false) {
        Serial.println("error");
        while (true) {}
    }
    Serial.println("done");

    Serial.print("Connecting to mosquitto test server  ... ");
    if (mqtt.connect(server, port, clean_session, keepalive, NULL, NULL, will_topic, will_message, will_qos) == false) {
        Serial.println("error");
        while (true) {}
    }
    Serial.println("done");

    Serial.print("Publishing a message  ... ");
    if (mqtt.publish("test_topic", "test_message", 0, 60) == false) {
        Serial.println("error");
        while (true) {}
    }
    Serial.println("done");

    Serial.print("Powering off ... ");
    if (modem.powerOff() == false) {
        Serial.println("error");
        while (true) {}
    }
    Serial.println("done");
}

void loop() {}
