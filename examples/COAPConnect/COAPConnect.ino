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

// COAP test server
const char* server        = "test.mosquitto.org";
const int   port          = 1883;

// replace with your apn
const char* apn           = "simbase";

A76XX modem(SerialAT);
A76XXCOAPClient coap_client(modem);

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

    delay(3000);

    // COAP client usage
    Serial.print("Starting COAP client ... ");
    if (coap_client.begin()){
        Serial.println("cannot start");
        while (true) {}
    }
    Serial.println("done");

    Serial.print("Stopping COAP client ... ");
    if (coap_client.end()){
        Serial.println("cannot stop");
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