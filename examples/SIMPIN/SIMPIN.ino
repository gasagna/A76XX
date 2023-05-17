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

// replace with your PIN code
const char* PINCODE = "0000";

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

    // we call modem.init without pin code to try unlock the SIM. This will error
    Serial.print("Attempting to initialise modem without PIN ... ");
    if (modem.init() == false) {
        Serial.print("error: "); Serial.println(modem.getLastError());
    } else {
        Serial.println("done");
    }

    // let's do it correctly now
    Serial.print("Attempting to initialise modem with PIN ... ");
    if (modem.init(PINCODE) == false) {
        // will likely print A76XX_SIM_PIN_REQUIRED i.e. -6
        Serial.print("error: "); Serial.println(modem.getLastError());
        while (true) {}
    }
    Serial.println("done");

    Serial.print("Powering off ... ");
    if (modem.powerOff() == false) {
        Serial.print("error: "); Serial.println(modem.getLastError());
        while (true) {}
    }
    Serial.println("done");
}

void loop() {}