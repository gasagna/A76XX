#ifndef A76XX_H_
#define A76XX_H_

#include "Arduino.h"
#include "time.h"

#ifndef MQTT_PAYLOAD_BUFF_LEN
    #define MQTT_PAYLOAD_BUFF_LEN 64
#endif

#ifndef MQTT_TOPIC_BUFF_LEN
    #define MQTT_TOPIC_BUFF_LEN 32
#endif

struct MQTTMessage_t {
    char topic[MQTT_TOPIC_BUFF_LEN];
    char payload[MQTT_PAYLOAD_BUFF_LEN];
};

enum Response_t {
    A76XX_RESPONSE_OK        = 0,
    A76XX_RESPONSE_MATCH_1ST = 1,
    A76XX_RESPONSE_MATCH_2ND = 2,
    A76XX_RESPONSE_MATCH_3RD = 3,
    A76XX_RESPONSE_ERROR     = 4,
    A76XX_RESPONSE_TIMEOUT   = 5,
    A76XX_HAS_MQTT_MESSAGE   = 6
};

#define RESPONSE_OK "OK\r\n"
#define RESPONSE_ERROR "ERROR\r\n"

// error codes
#define A76XX_OPERATION_SUCCEEDED             0
#define A76XX_OPERATION_TIMEDOUT             -1
#define A76XX_GENERIC_ERROR                  -2
#define A76XX_OUT_OF_MEMORY                  -3
#define A76XX_MQTT_ALREADY_STOPPED           -4
#define A76XX_MQTT_ALREADY_STARTED           -5
#define A76XX_SIM_PIN_REQUIRED               -6
#define A76XX_SIM_PIN_MODEM_ERROR            -7


// if retcode is an error, return it
#define A76XX_RETCODE_ASSERT_RETURN(retcode) {        \
        if( (retcode) != A76XX_OPERATION_SUCCEEDED) { \
            return retcode;                           \
        }                                             \
    }

// if retcode is an error, set last_error member and return false
#define A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode) {   \
        if( (retcode) != A76XX_OPERATION_SUCCEEDED ) {\
            _last_error_code = retcode;               \
            return false;                             \
        }                                             \
    }

// if response is not ok, return false
#define A76XX_RESPONSE_ASSERT_BOOL(rsp)               \
    if( (rsp) != Response_t::A76XX_RESPONSE_OK) {     \
        return false;                                 \
    }

// simple macro to process responses that are either OK, TIMEOUT, or ERROR
#define A76XX_RESPONSE_PROCESS(rsp)                   \
    switch( (rsp) ) {                                 \
        case Response_t::A76XX_RESPONSE_OK : {        \
            return A76XX_OPERATION_SUCCEEDED;         \
        }                                             \
        case Response_t::A76XX_RESPONSE_TIMEOUT : {   \
            return A76XX_OPERATION_TIMEDOUT;          \
        }                                             \
        case Response_t::A76XX_RESPONSE_ERROR :{      \
            return A76XX_GENERIC_ERROR;               \
        }                                             \
        default : {                                   \
            return A76XX_GENERIC_ERROR;               \
        }                                             \
    }

#include "utils/base64.h"
#include "modem_serial.h"

#include "commands/internet_service.h"
#include "commands/serial_interface.h"
#include "commands/status_control.h"
#include "commands/packet_domain.h"
#include "commands/network.h"
#include "commands/v25ter.h"
#include "commands/http.h"
#include "commands/mqtt.h"
#include "commands/ssl.h"
#include "commands/sim.h"

#include "modem.h"

#include "clients/base.h"
#include "clients/secure.h"
#include "clients/mqtt.h"
#include "clients/http.h"

#endif A76XX_H_