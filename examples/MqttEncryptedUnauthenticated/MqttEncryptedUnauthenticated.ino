#include <StreamDebugger.h>
#include "A76XX.h"

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
const int   port          = 8883;
const int   keepalive     = 60;
const bool  clean_session = true;
const bool  use_ssl       = true;

// mosquitto CA cert
static const char* mosquittoCert PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIEAzCCAuugAwIBAgIUBY1hlCGvdj4NhBXkZ/uLUZNILAwwDQYJKoZIhvcNAQEL
BQAwgZAxCzAJBgNVBAYTAkdCMRcwFQYDVQQIDA5Vbml0ZWQgS2luZ2RvbTEOMAwG
A1UEBwwFRGVyYnkxEjAQBgNVBAoMCU1vc3F1aXR0bzELMAkGA1UECwwCQ0ExFjAU
BgNVBAMMDW1vc3F1aXR0by5vcmcxHzAdBgkqhkiG9w0BCQEWEHJvZ2VyQGF0Y2hv
by5vcmcwHhcNMjAwNjA5MTEwNjM5WhcNMzAwNjA3MTEwNjM5WjCBkDELMAkGA1UE
BhMCR0IxFzAVBgNVBAgMDlVuaXRlZCBLaW5nZG9tMQ4wDAYDVQQHDAVEZXJieTES
MBAGA1UECgwJTW9zcXVpdHRvMQswCQYDVQQLDAJDQTEWMBQGA1UEAwwNbW9zcXVp
dHRvLm9yZzEfMB0GCSqGSIb3DQEJARYQcm9nZXJAYXRjaG9vLm9yZzCCASIwDQYJ
KoZIhvcNAQEBBQADggEPADCCAQoCggEBAME0HKmIzfTOwkKLT3THHe+ObdizamPg
UZmD64Tf3zJdNeYGYn4CEXbyP6fy3tWc8S2boW6dzrH8SdFf9uo320GJA9B7U1FW
Te3xda/Lm3JFfaHjkWw7jBwcauQZjpGINHapHRlpiCZsquAthOgxW9SgDgYlGzEA
s06pkEFiMw+qDfLo/sxFKB6vQlFekMeCymjLCbNwPJyqyhFmPWwio/PDMruBTzPH
3cioBnrJWKXc3OjXdLGFJOfj7pP0j/dr2LH72eSvv3PQQFl90CZPFhrCUcRHSSxo
E6yjGOdnz7f6PveLIB574kQORwt8ePn0yidrTC1ictikED3nHYhMUOUCAwEAAaNT
MFEwHQYDVR0OBBYEFPVV6xBUFPiGKDyo5V3+Hbh4N9YSMB8GA1UdIwQYMBaAFPVV
6xBUFPiGKDyo5V3+Hbh4N9YSMA8GA1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQEL
BQADggEBAGa9kS21N70ThM6/Hj9D7mbVxKLBjVWe2TPsGfbl3rEDfZ+OKRZ2j6AC
6r7jb4TZO3dzF2p6dgbrlU71Y/4K0TdzIjRj3cQ3KSm41JvUQ0hZ/c04iGDg/xWf
+pp58nfPAYwuerruPNWmlStWAXf0UTqRtg4hQDWBuUFDJTuWuuBvEXudz74eh/wK
sMwfu1HFvjy5Z0iMDU8PUDepjVolOCue9ashlS4EB5IECdSR2TItnAIiIwimx839
LdUdRudafMu5T5Xma182OC0/u/xRlEm+tvKGGmfFcN0piqVl8OrSPBgIlb+1IKJE
m/XriWr/Cq4h/JfB7NTsezVslgkBaoU=
-----END CERTIFICATE-----
)EOF";

// replace with your apn
const char* apn           = "simbase";

A76XX modem(SerialAT);
A76XXMQTTClient mqtt_client(&modem, clientID, use_ssl);

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

    Serial.print("Downloading certificate to the module ... ");
    if (mqtt_client.setCaCert(mosquittoCert) == false) {
        Serial.println("error");
        while (true) {}
    }
    Serial.println("done");

    Serial.print("Starting client  ... ");
    if (mqtt_client.begin() == false) {
        Serial.println("error");
        while (true) {}
    }
    Serial.println("done");

    Serial.print("Connecting to mosquitto test server ... ");
    if (mqtt_client.connect(server, port, clean_session, keepalive, 
            NULL, NULL, will_topic, will_message, will_qos) == false) {
        Serial.println("error");
        while (true) {}
    }
    Serial.println("done");

    Serial.print("Publishing a message ... ");
    if (mqtt_client.publish("test_topic", "test_message", 0, 60) == false) {
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
