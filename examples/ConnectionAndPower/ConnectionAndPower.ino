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

// replace with your apn
const char* apn = "simbase";

A76XX modem(SerialAT);

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

    Serial.print("Initialising modem within 20 seconds ... ");
    if (modem.init(NULL, 20000) == false) {
        Serial.println("error");
        while (true) {}
    }
    Serial.println("done");

    Serial.print("Waiting 30 seconds for modem to register on GPRS network ... ");
    if (modem.waitForRegistration(1, 30000) == false) {
        Serial.println("registration timed out");
        while (true) {}
    }
    Serial.println("done");

    Serial.print("Registration status - GSM: ");
    Serial.println(modem.getRegistrationStatus(0));
    Serial.print("Registration status - GPRS: ");
    Serial.println(modem.getRegistrationStatus(1));
    Serial.print("Registration status - LTE: ");
    Serial.println(modem.getRegistrationStatus(2));

    Serial.print("Model identification: ");
    Serial.println(modem.modelIdentification());

    Serial.print("Revision identification: ");
    Serial.println(modem.revisionIdentification());

    Serial.print("Connecting ... ");
    if (modem.GPRSConnect(apn) == false ){
        Serial.println("error");
        while (true) {}
    }
    Serial.println("done");

    Serial.print("Checking connection ... ");
    if (modem.isGPRSConnected() == false) {
        Serial.println("modem not connected");
        while (true) {}
    }
    Serial.println("modem connected");

    Serial.print("Network system mode: ");
    Serial.println(modem.getNetworkSystemMode());

    Serial.print("Turning radio OFF ... ");
    if (modem.radioOFF() == false) {
        Serial.println("error");
        while (true) {}
    }
    Serial.println("done");

    Serial.print("Going to sleep ... ");
    if (modem.sleep() == false) {
        Serial.println("error");
        while (true) {}
    }
    Serial.println("done");

    Serial.print("Waking up ... ");
    if (modem.wakeUp() == false) {
        Serial.println("error");
        while (true) {}
    }
    Serial.println("done");

    Serial.print("Setting radio ON ... ");
    if (modem.radioON() == false) {
        Serial.println("error");
        while (true) {}
    }
    Serial.println("done");

    Serial.print("Restarting ... ");
    if (modem.restart() == false) {
        Serial.println("error");
        while (true) {}
    }
    Serial.println("done");

    Serial.print("Waiting for modem to register on network ... ");
    if (!modem.waitForRegistration()) {
        Serial.println("registration timed out");
        while (true) {}
    }
    Serial.println("done");

    Serial.print("Registration status: ");
    Serial.println(modem.getRegistrationStatus());

    Serial.print("Turning radio off ... ");
    if (modem.radioOFF() == false) {
        Serial.println("error");
        while (true) {}
    }
    Serial.println("done");

    Serial.print("Powering off within 10 seconds ... ");
    if (modem.powerOff(10000) == false) {
        Serial.println("error");
        while (true) {}
    }
    Serial.println("done");
}

void loop() {
    Serial.print(".");
    delay(1000);
}