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

// replace with your apn
const char* apn           = "simbase";

A76XX modem(SerialAT);

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
        Serial.println("ERROR");
        while (true) {}
    } else {
        Serial.println("OK");
    }

    Serial.print("Waiting for modem to register on network ... ");
    if (!modem.waitForRegistration()) {
        Serial.println("registration timed out");
        while (true) {}
    }
    Serial.println("done");

    // update system time with ntp server
    int retcode = modem.syncTime();
    Serial.print("NTP update return code: "); Serial.println(retcode);

    // get date and time as a string in the format "yy/MM/dd,hh:mm:ss±zz"
    String date_time;
    modem.getDateTime(date_time);
    Serial.println(date_time);

    // get unix time stamp, with respect to UTC
    uint32_t timestamp = modem.getUnixTime(true);
    Serial.println(timestamp);

    Serial.print("Powering off ... ");
    if (modem.powerOff() == false) {
        Serial.println("error");
        while (true) {}
    } else {
        Serial.println("done");
    }
}


void loop() {}