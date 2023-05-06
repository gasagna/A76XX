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

const char* server_name   = "vsh.pp.ua";
const int   server_port   = 80;

const char* path          = "TinyGSM/logo.txt";
const bool  use_ssl       = false;
const char* user_agent    = "Arduino!!";

// replace with your apn
const char* apn           = "simbase";

A76XX modem(SerialAT);
A76XXHTTPClient http_client(modem, server_name, server_port, use_ssl, user_agent);

// configuration for serial port to simcom module (check your board!)
#define PIN_TX   26
#define PIN_RX   27

void setup() {
    // start ports
    Serial.begin(115200);
    Serial1.begin(115200, SERIAL_8N1, PIN_RX, PIN_TX);
    delay(3000);

    Serial.print("Waiting for modem ... ");
    if (modem.init() == false) {
        Serial.println("error");
    } else {
        Serial.println("OK");
    }

    Serial.print("Waiting for modem to register on network ... ");
    if (!modem.waitForRegistration()) {
        Serial.println("registration timed out");
        while (true) {}
    }
    Serial.println("done");

    Serial.print("Connecting  ... ");
    if (modem.connect(apn) == false){
        Serial.println("cannot connect");
        while (true) {}
    } else {
        Serial.println("connected");
    }

    Serial.print("Starting client  ... ");
    if (http_client.begin() == false) {
        Serial.print("error... code: ");
        Serial.println(http_client.getLastError());
        while (true) {}
    } else {
        Serial.println("done");
    }

    Serial.print("Get resource  ... ");
    if (http_client.get(path) == false) {
        Serial.print("error... code: ");
        Serial.println(http_client.getLastError());
        while (true) {}
    } else {
        Serial.println("Done");
        
        String header;
        if (http_client.getResponseHeader(header) == false) {
            Serial.print("Failed to read header. Code: ");
            Serial.println(http_client.getLastError());
        }

        String body;
        if (http_client.getResponseBody(body) == false) {
            Serial.print("Failed to read body. Code: ");
            Serial.println(http_client.getLastError());
        }

        Serial.print("Received: "); Serial.println(http_client.getResponseStatusCode());
        
        Serial.println("\nHeader\n----------"); 
        Serial.println(header); Serial.println(); 

        Serial.println("\nBody\n----------"); 
        Serial.println(body); Serial.println(); 
    }

    Serial.print("Stopping HTTP service ... ");
    if (http_client.end() == false) {
        Serial.print("error... code: ");
        Serial.println(http_client.getLastError());
        while (true) {}
    } else {
        Serial.println("done");
    }

    Serial.print("Powering off ... ");
    if (modem.powerOff() == false) {
        Serial.println("error");
        while (true) {}
    } else {
        Serial.println("done");
    }
}


void loop() {}