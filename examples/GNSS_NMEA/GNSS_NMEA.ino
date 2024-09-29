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

// modem and GNSS client objects
A76XX modem(SerialAT);
A76XXGNSSClient gnss(modem);

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

    // wait for modem
    Serial.print("Waiting for modem ... ");
    modem.init(NULL, 10000);
    Serial.println("OK");

    // enable gps
    Serial.print("Enabling GNSS ...");
    if (gnss.enableGNSS(GPSStart_t::HOT) == false) {
        Serial.println("error");
        while (true) {}
    }
    Serial.println("done");

    // enable nmea stream
    Serial.print("Enabling NMEA stream ...");
    if (gnss.enableNMEAStream() == false) {
        Serial.println("error");
        while (true) {}
    }
    Serial.println("done");
}

void loop() {
    // listen to SIMCOM serial port
    modem.listen();

    // print all messages received to the debug serial port
    while (gnss.nmeaAvailable() > 0) {
        NMEAMessage_t msg = gnss.getNMEAMessage();
        Serial.print("New message: ");
        Serial.println(msg.payload);
    }
}