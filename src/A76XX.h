#ifndef A76XX_H_
#define A76XX_H_

#include "Arduino.h"
#include "time.h"

enum Response_t {
    A76XX_RESPONSE_OK        = 0,
    A76XX_RESPONSE_MATCH_1ST = 1,
    A76XX_RESPONSE_MATCH_2ND = 2,
    A76XX_RESPONSE_ERROR     = 3,
    A76XX_RESPONSE_TIMEOUT   = 4
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

#include "internet_service_commands.h"
#include "serial_interface_commands.h"
#include "status_control_commands.h"
#include "packet_domain_commands.h"
#include "network_commands.h"
#include "v25ter_commands.h"
#include "http_commands.h"
#include "mqtt_commands.h"
#include "ssl_commands.h"
#include "sim_commands.h"

#include "modem.h"

#include "base_client.h"
#include "secure_client.h"
#include "mqtt_client.h"
#include "http_client.h"

#endif A76XX_H_