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
const char* apn           = "simbase";

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

    Serial.print("Initialising modem ... ");
    if (modem.init() == false) {
        Serial.println("error");
        while (true) {}
    }
    Serial.println("OK");

    // update system time with the default ntp server before connecting to the 
    // network will produce an error which is retrieved using getLastError.
    // Let's provide the timezone too, i.e. for London (GMT+0). This is passed
    // in units of 15 minutes. We do not have a mechanism to understand Daylight
    // Saving Time (DST), so use the time zone information when there is no DST.
    Serial.print("Syncing time with NTP server ... ");
    if (modem.syncTime(0) == false) {
        Serial.print("error, code: "); Serial.println(modem.getLastError());
    } else {
        Serial.println("all good, but this should not be printed!");
    }

    Serial.print("Waiting for modem to register on network ... ");
    if (!modem.waitForRegistration()) {
        Serial.println("registration timed out");
        while (true) {}
    }
    Serial.println("done");

    // this should now work
    Serial.print("Syncing time with NTP server ... ");
    if (modem.syncTime(0) == false) {
        Serial.print("NTP sync error, code: "); Serial.println(modem.getLastError());
        while (true) {}
    }
    Serial.println("all good!");

    // get date and time as a string in the format "yy/MM/dd,hh:mm:ss±zz"
    Serial.print("Time: "); Serial.println(modem.getDateTime());

    // get unix time stamp, with respect to UTC
    Serial.print("Timestamp UTC: "); Serial.println(modem.getUnixTime(true));

    Serial.print("Powering off ... ");
    if (modem.powerOff() == false) {
        Serial.println("error");
        while (true) {}
    }
    Serial.println("done");
}


void loop() {}