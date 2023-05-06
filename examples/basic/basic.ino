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

    // begin
    Serial.begin(115200);
    Serial1.begin(115200, SERIAL_8N1, PIN_RX, PIN_TX);
    delay(3000);

    //
    Serial.print("Initialising modem ... ");
    if (modem.init() == false) {
        Serial.println("error");
    } else {
        Serial.println("done");
    }

    //
    Serial.print("Waiting for modem to register on network ... ");
    if (!modem.waitForRegistration()) {
        Serial.println("registration timed out");
        while (true) {}
    }
    Serial.println("done");

    Serial.print("Registration status: ");
    Serial.println(modem.getRegistrationStatus());

    Serial.print("Model identification: ");
    Serial.println(modem.modelIdentification());

    Serial.print("Revision identification: ");
    Serial.println(modem.revisionIdentification());

    Serial.print("Connecting ... ");
    if (modem.connect(apn) == false ){
        Serial.println("cannot connect");
    } else {
        Serial.println("done");
    }

    Serial.print("Checking connection ... ");
    if (modem.isConnected()) {
        Serial.println("modem connected");
    } else {
        Serial.println("modem not connected");
    }

    Serial.print("Network system mode: ");
    Serial.println(modem.getNetworkSystemMode());

    // turn off radio
    Serial.print("Turning radio off ... ");
    if (modem.radioOFF() == false) {
        Serial.println("error");
    } else {
        Serial.println("done");
    }

    Serial.print("Going to sleep ... ");
    if (modem.sleep() == false) {
        Serial.println("error");
    } else {
        Serial.println("done");
    }

    Serial.print("Waking up ... ");
    if (modem.wakeUp() == false) {
        Serial.println("error");
    } else {
        Serial.println("done");
    }

    Serial.print("Setting radio on ... ");
    if (modem.radioON() == false) {
        Serial.println("error");
    } else {
        Serial.println("done");
    }

    Serial.print("Restarting ... ");
    if (modem.restart() == false) {
        Serial.println("error");
    } else {
        Serial.println("done");
    }

    Serial.print("Waiting for modem to register on network ... ");
    if (!modem.waitForRegistration()) {
        Serial.println("registration timed out");
        while (true) {}
    }
    Serial.println("done");

    Serial.print("Registration status: ");
    Serial.println(modem.getRegistrationStatus());

    // turn off radio
    Serial.print("Turning radio off ... ");
    if (modem.radioOFF() == false) {
        Serial.println("error");
    } else {
        Serial.println("done");
    }

    Serial.print("Powering off ... ");
    if (modem.powerOff() == false) {
        Serial.println("error");
    } else {
        Serial.println("done");
    }
}

void loop() {
    Serial.print(".");
    modem.sendCMD("AT");
    modem.waitResponse();
    delay(1000);
}