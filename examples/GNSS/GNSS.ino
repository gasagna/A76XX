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
}

void loop() {
    // get GPS and GNSS info
    GPSInfo_t gps_info;
    GNSSInfo_t gnss_info;
    
    Serial.println("------------------------");
    Serial.print("Obtaining GPS data ... ");
    if (gnss.getGPSInfo(gps_info) == false) {
        Serial.print("error code: ");
        Serial.println(gnss.getLastError());
        while (true) {}
    }
    Serial.println("done");

    Serial.print("hasfix   : "); Serial.println(gps_info.hasfix);
    Serial.print("lat      : "); Serial.println(gps_info.lat);
    Serial.print("NS       : "); Serial.println(gps_info.NS);
    Serial.print("lon      : "); Serial.println(gps_info.lon);
    Serial.print("EW       : "); Serial.println(gps_info.EW);
    Serial.print("date     : "); Serial.println(gps_info.date);
    Serial.print("UTC_TIME : "); Serial.println(gps_info.UTC_TIME);
    Serial.print("alt      : "); Serial.println(gps_info.alt);
    Serial.print("speed    : "); Serial.println(gps_info.speed);
    Serial.print("course   : "); Serial.println(gps_info.course);
    Serial.println();

    Serial.println("------------------------");
    Serial.print("Obtaining GNSS data ... ");
    if (gnss.getGNSSInfo(gnss_info) == false) {
        Serial.print("error code: ");
        Serial.println(gnss.getLastError());
        while (true) {}
    }
    Serial.println("done");

    Serial.print("hasfix      : "); Serial.println(gnss_info.hasfix);
    Serial.print("mode        : "); Serial.println(gnss_info.mode);
    Serial.print("GPS_SVs     : "); Serial.println(gnss_info.GPS_SVs);
    Serial.print("GLONASS_SVs : "); Serial.println(gnss_info.GLONASS_SVs);
    Serial.print("BEIDOU_SVs  : "); Serial.println(gnss_info.BEIDOU_SVs);
    Serial.print("lat         : "); Serial.println(gnss_info.lat);
    Serial.print("NS          : "); Serial.println(gnss_info.NS);
    Serial.print("lon         : "); Serial.println(gnss_info.lon);
    Serial.print("EW          : "); Serial.println(gnss_info.EW);
    Serial.print("date        : "); Serial.println(gnss_info.date);
    Serial.print("UTC_TIME    : "); Serial.println(gnss_info.UTC_TIME);
    Serial.print("alt         : "); Serial.println(gnss_info.alt);
    Serial.print("speed       : "); Serial.println(gnss_info.speed);
    Serial.print("course      : "); Serial.println(gnss_info.course);
    Serial.print("PDOP        : "); Serial.println(gnss_info.PDOP);
    Serial.print("HDOP        : "); Serial.println(gnss_info.HDOP);
    Serial.print("VDOP        : "); Serial.println(gnss_info.VDOP);

    // every five seconds
    delay(5000);
}