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

// MQTT detailed
const char* server        = "http://vsh.pp.ua";
const char* resource      = "Adieu!";
const int   will_qos      = 0;
const int   port          = 8883;
const int   keepalive     = 60;
const bool  clean_session = true;
const bool  use_ssl       = true;

// mosquitto CA cert