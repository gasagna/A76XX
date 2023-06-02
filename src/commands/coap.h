#ifndef A76XX_COAP_CMDS_H_
#define A76XX_COAP_CMDS_H_

/*
    @brief Commands in section 14 of the AT command manual version 1.09

    Command       | Implemented | Method     | Function(s)
    ------------- | ----------- | ---------- | -----------
    COAPSTART     |     y       |            | start
    COAPSTOP      |     y       |            | stop
    COAPOPEN      |     y       |            | connect
    COAPCLOSE     |     y       |            | disconnect
    COAPHEAD      |     y       |            | head_cfg
    COAPOPTION    |     y       |            | option_cfg
    COAPSEND      |     y       |            | send
    COAPSENDTX    |             |            |
*/

enum COAPMessageMethod_t {
    GET    = 0,
    POST   = 1,
    PUT    = 2,
    DELETE = 3,
    FETCH  = 4,
    PATCH  = 5,
    IPATCH = 6
};

enum COAPMessageType_t {
    CON = 0,
    NON = 1,
    ACK = 2,
    RST = 3
};

static const char* A76XX_COAP_MESSAGE_METHODS[7] = {"get",
                                                    "post",
                                                    "put",
                                                    "delete",
                                                    "fetch",
                                                    "patch",
                                                    "ipatch"};

static const char* A76XX_COAP_MESSAGE_TYPES[4] = {"con", "non", "ack", "rst"};
 
class COAPCommands {
  public:
    ModemSerial& _serial;

    COAPCommands(ModemSerial& serial)
        : _serial(serial) {}

    int8_t start() {
        _serial.sendCMD("AT+COAPSTART");
        A76XX_RESPONSE_PROCESS(_serial.waitResponse(9000));
    }

    int8_t stop() {
        _serial.sendCMD("AT+COAPSTOP");
        A76XX_RESPONSE_PROCESS(_serial.waitResponse(9000));
    }

    int8_t connect(const char* server, uint port, uint8_t& session_id) {
        _serial.sendCMD("AT+COAPOPEN=", server, ",", port);
        switch (_serial.waitResponse("+COAPOPEN:", 9000, false, true)) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                session_id = _serial.parseIntClear();
                return A76XX_OPERATION_SUCCEEDED;
            }
            case Response_t::A76XX_RESPONSE_TIMEOUT : {
                return A76XX_OPERATION_TIMEDOUT;
            }
            default : {
                return A76XX_GENERIC_ERROR;
            }
        }
    }

    int8_t disconnect(uint8_t session_id) {
        _serial.sendCMD("AT+COAPCLOSE=", session_id);
        A76XX_RESPONSE_PROCESS(_serial.waitResponse(9000));
    }

    int8_t head_cfg(uint8_t session_id, uint16_t msg_id, const char* token) {
        _serial.sendCMD("AT+COAPHEAD=", session_id, ",", msg_id, ",", strlen(token), ",", token);
        A76XX_RESPONSE_PROCESS(_serial.waitResponse(9000));
    }

    int8_t option_cfg(uint8_t session_id, uint8_t opt_count, uint8_t opt_num, const char* opt_value) {
        _serial.sendCMD("AT+COAPOPTION=", session_id, ",", opt_count, ",", opt_num, ",", opt_value);
        A76XX_RESPONSE_PROCESS(_serial.waitResponse(9000));
    }

    int8_t send(uint8_t session_id, COAPMessageType_t type, COAPMessageMethod_t method, const char* data) {
        _serial.sendCMD("AT+COAPSEND=", session_id,  ",",
                        A76XX_COAP_MESSAGE_TYPES[type],  ",",
                        A76XX_COAP_MESSAGE_METHODS[method], ",", strlen(data), ",", data);
        A76XX_RESPONSE_PROCESS(_serial.waitResponse(9000));
    }
};

#endif A76XX_COAP_CMDS_H_