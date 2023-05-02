#ifndef A76XX_MQTT_CMDS_H_
#define A76XX_MQTT_CMDS_H_

#include "modem.h"

#define A76XX_MQTT_FAILED                                             1
#define A76XX_MQTT_BAD_UTF8_STRING                                    2
#define A76XX_MQTT_SOCK_CONNECT_FAIL                                  3
#define A76XX_MQTT_SOCK_CREATE_FAIL                                   4
#define A76XX_MQTT_SOCK_CLOSE_FAIL                                    5
#define A76XX_MQTT_MESSAGE_RECEIVE_FAIL                               6
#define A76XX_MQTT_NETWORK_OPEN_FAIL                                  7
#define A76XX_MQTT_NETWORK_CLOSE_FAIL                                 8
#define A76XX_MQTT_NETWORK_NOT_OPENED                                 9
#define A76XX_MQTT_CLIENT_INDEX_ERROR                                10
#define A76XX_MQTT_NO_CONNECTION                                     11
#define A76XX_MQTT_INVALID_PARAMETER                                 12
#define A76XX_MQTT_NOT_SUPPORTED_OPERATION                           13
#define A76XX_MQTT_CLIENT_IS_BUSY                                    14
#define A76XX_MQTT_REQUIRE_CONNECTION_FAIL                           15
#define A76XX_MQTT_SOCK_SENDING_FAIL                                 16
#define A76XX_MQTT_TIMEOUT                                           17
#define A76XX_MQTT_TOPIC_IS_EMPTY                                    18
#define A76XX_MQTT_CLIENT_IS_USED                                    19
#define A76XX_MQTT_CLIENT_NOT_ACQUIRED                               20
#define A76XX_MQTT_CLIENT_NOT_RELEASED                               21
#define A76XX_MQTT_LENGTH_OUT_OF_RANGE                               22
#define A76XX_MQTT_NETWORK_IS_OPENED                                 23
#define A76XX_MQTT_PACKET_FAIL                                       24
#define A76XX_MQTT_DNS_ERROR                                         25
#define A76XX_MQTT_SOCKET_IS_CLOSED_BY_SERVER                        26
#define A76XX_MQTT_CONNECTION_REFUSED_UNACCEPTED_PROTOCOL_VERSION    27
#define A76XX_MQTT_CONNECTION_REFUSED_IDENTIFIER_REJECTED            28
#define A76XX_MQTT_CONNECTION_REFUSED_SERVER_UNAVAILABLE             29
#define A76XX_MQTT_CONNECTION_REFUSED_BAD_USER_NAME_OR_PASSWORD      30
#define A76XX_MQTT_CONNECTION_REFUSED_NOT_AUTHORIZED                 31
#define A76XX_MQTT_HANDSHAKE_FAIL                                    32
#define A76XX_MQTT_NOT_SET_CERTIFICATE                               33
#define A76XX_MQTT_OPEN_SESSION_FAILED                               34
#define A76XX_MQTT_DISCONNECT_FROM_SERVER_FAILED                     35

class A76XX_MQTT_Commands {
  private:
    A76XX& _modem;

  public:
    // Initialise from modem
    A76XX_MQTT_Commands(A76XX& modem)
        : _modem(modem) {}

    // CMQTTSTART
    int8_t start() {
        // start MQTT service by activating PDP context
        _modem.sendCMD("AT+CMQTTSTART");
        Response_t rsp = _modem.waitResponse("+CMQTTSTART: ", 12000, false, true);

        if (rsp == Response_t::A76XX_RESPONSE_ERROR)
            return A76XX_MQTT_ALREADY_STARTED;

        // read return code (could be 0) and clear stream
        int retcode = _modem._serial.parseInt(); _modem._serialClear();

        // return on error
        return retcode;
    }

    // CMQTTSTOP
    int8_t stop() {
        _modem.sendCMD("AT+CMQTTSTOP");
        Response_t rsp = _modem.waitResponse("+CMQTTSTOP: ", 12000, false, true);

        if (rsp == Response_t::A76XX_RESPONSE_ERROR)
            return A76XX_MQTT_ALREADY_STOPPED;

        // read return code (could be 0) and clear stream
        int retcode = _modem._serial.parseInt(); _modem._serialClear();

        // return on error
        return retcode;
    }

    // CMQTTACCQ
    int8_t acquire_client(uint8_t client_index, const char clientID[], uint8_t server_type) {
        _modem.sendCMD("AT+CMQTTACCQ=", client_index, ",\"", clientID, "\",", server_type);
        Response_t rsp = _modem.waitResponse("+CMQTTACCQ: ", 9000, true, true);

        switch( rsp ) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                _modem._serial.find(',');
                return _modem._serialParseIntClear();
            }
            case Response_t::A76XX_RESPONSE_OK : {
                return A76XX_OPERATION_SUCCEEDED;
            }
            case Response_t::A76XX_RESPONSE_TIMEOUT : {
                return A76XX_OPERATION_TIMEDOUT;
            }
            case Response_t::A76XX_RESPONSE_ERROR : {
                return A76XX_GENERIC_ERROR;
            }
            default : {
                return A76XX_GENERIC_ERROR;
            }
        }
    }

    // CMQTTREL
    int8_t release_client(uint8_t client_index) {
        _modem.sendCMD("AT+CMQTTREL=", client_index);
        Response_t rsp = _modem.waitResponse("+CMQTTREL: ", 9000, true, true);

        if (rsp == Response_t::A76XX_RESPONSE_OK)

        if (rsp == Response_t::A76XX_RESPONSE_ERROR)
            return A76XX_GENERIC_ERROR;

        // this is an error case
        if (rsp == Response_t::A76XX_RESPONSE_MATCH_1ST) {
            _modem._serial.find(',');
            return _modem._serialParseIntClear();
        }

        return A76XX_GENERIC_ERROR;
    }

    // CMQTTSSLCFG
    int8_t set_SSL_context(uint8_t session_id, uint8_t ssl_ctx_index) {
        _modem.sendCMD("AT+CMQTTSSLCFG=", session_id, ",", ssl_ctx_index);
        return _modem.waitResponse();
    }

    // CMQTTWILLTOPIC
    int8_t set_will_topic(uint8_t client_index, const char* will_topic) {
        _modem.sendCMD("AT+CMQTTWILLTOPIC=", client_index, ",", strlen(will_topic));

        Response_t rsp = _modem.waitResponse(">", "+CMQTTWILLTOPIC: ", 9000);

        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                _modem._serial.write(will_topic);
                _modem._serial.flush();
                if (_modem.waitResponse() == Response_t::A76XX_RESPONSE_OK) {
                } else {
                    return A76XX_GENERIC_ERROR;
                }
            }
            case Response_t::A76XX_RESPONSE_MATCH_2ND : {
                _modem._serial.find(',');
                return _modem._serialParseIntClear();
            }
            case Response_t::A76XX_RESPONSE_TIMEOUT : {
                return A76XX_OPERATION_TIMEDOUT;
            }
            default : {
                return A76XX_GENERIC_ERROR;
            }
        }
    }

    // CMQTTWILLMSG
    int8_t set_will_message(uint8_t client_index, const char* will_message, uint8_t will_qos) {
        _modem.sendCMD("AT+CMQTTWILLMSG=", client_index, ",", strlen(will_message), ",", will_qos);

        Response_t rsp = _modem.waitResponse(">", "+CMQTTWILLMSG: ", 9000);

        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                _modem._serial.write(will_message);
                _modem._serial.flush();
                if (_modem.waitResponse() == Response_t::A76XX_RESPONSE_OK) {
                } else {
                    return A76XX_GENERIC_ERROR;
                }
            }
            case Response_t::A76XX_RESPONSE_MATCH_2ND : {
                _modem._serial.find(',');
                return _modem._serialParseIntClear();
            }
            case Response_t::A76XX_RESPONSE_TIMEOUT : {
                return A76XX_OPERATION_TIMEDOUT;
            }
            default : {
                return A76XX_GENERIC_ERROR;
            }
        }
    }

    // CMQTTCONNECT
    int8_t connect(uint8_t client_index, const char* server, int port,
                   bool clean_session, int keepalive = 60,
                   const char* username = NULL, const char* password = NULL) {

        if (username && password) {
            _modem.sendCMD("AT+CMQTTCONNECT=", client_index, ",\"tcp://", server, ":", port, "\",", keepalive, ",", clean_session, ",\"", username, "\",\"", password, "\"");
        } else {
            _modem.sendCMD("AT+CMQTTCONNECT=", client_index, ",\"tcp://", server, ":", port, "\",", keepalive, ",", clean_session);
        }

        // it might happen that the command only returns ERROR (case 5)
        // in this case we timeout
        Response_t rsp = _modem.waitResponse("+CMQTTCONNECT: ", 9000, false, false);

        // read int output
        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                _modem._serial.find(',');
                return _modem._serialParseIntClear();
            }
            case Response_t::A76XX_RESPONSE_TIMEOUT : {
                return A76XX_OPERATION_TIMEDOUT;
            }
            default : {
                return A76XX_GENERIC_ERROR;
            }
        }
    }

    // CMQTTDISC?
    bool is_connected(uint8_t client_index) {
        _modem.sendCMD("AT+CMQTTDISC?");

        char match_str[15] = "+CMQTTDISC: x,";
        match_str[12] = client_index == 0 ? '0' : '1';

        Response_t rsp = _modem.waitResponse(match_str, 9000, false, true);

        if (rsp == Response_t::A76XX_RESPONSE_MATCH_1ST) {
            int retcode = _modem._serialParseIntClear();
            return retcode == 0 ? true : false;
        } else {
            return false;
        }
    }

    // CMQTTDISC
    int8_t disconnect(uint8_t client_index, uint8_t timeout) {
        _modem.sendCMD("AT+CMQTTDISC=", client_index, ",", timeout);
        Response_t rsp = _modem.waitResponse("+CMQTTDISC: ", timeout, false, true);

        // read int output
        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST: {
                _modem._serial.find(',');
                return _modem._serialParseIntClear();
            }
            case Response_t::A76XX_RESPONSE_TIMEOUT: {
                return A76XX_OPERATION_TIMEDOUT;
            }
            default : {
                return A76XX_GENERIC_ERROR;
            }
        }
    }

    // CMQTTTOPIC
    int8_t set_topic(uint8_t client_index, const char* topic) {
        _modem.sendCMD("AT+CMQTTTOPIC=", client_index, ",", strlen(topic));

        Response_t rsp = _modem.waitResponse(">", "+CMQTTTOPIC: ", 9000);

        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                _modem._serial.write(topic);
                _modem._serial.flush();
                if (_modem.waitResponse() == Response_t::A76XX_RESPONSE_OK) {
                } else {
                    return A76XX_GENERIC_ERROR;
                }
            }
            case Response_t::A76XX_RESPONSE_MATCH_2ND : {
                _modem._serial.find(',');
                return _modem._serialParseIntClear();
            }
            case Response_t::A76XX_RESPONSE_TIMEOUT : {
                return A76XX_OPERATION_TIMEDOUT;
            }
            default : {
                return A76XX_GENERIC_ERROR;
            }
        }
    }

    // CMQTTPAYLOAD
    int8_t set_payload(uint8_t client_index, const uint8_t* payload, uint length) {
        _modem.sendCMD("AT+CMQTTPAYLOAD=0,", length);

        Response_t rsp = _modem.waitResponse(">", "+CMQTTTPAYLOAD: ", 9000);

        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                _modem._serial.write(payload, length);
                _modem._serial.flush();
                if (_modem.waitResponse() == Response_t::A76XX_RESPONSE_OK) {
                } else {
                    return A76XX_GENERIC_ERROR;
                }
            }
            case Response_t::A76XX_RESPONSE_MATCH_2ND : {
                _modem._serial.find(',');
                return _modem._serialParseIntClear();
            }
            case Response_t::A76XX_RESPONSE_TIMEOUT : {
                return A76XX_OPERATION_TIMEDOUT;
            }
            default : {
                return A76XX_GENERIC_ERROR;
            }
        }
    }

    // CMQTTPUB
    int8_t publish(uint8_t client_index, uint8_t qos, uint8_t pub_timeout, bool retained = false, bool dup = false) {
        uint8_t _retained = retained ? 1 : 0;
        uint8_t _dup      = dup      ? 1 : 0;
        _modem.sendCMD("AT+CMQTTPUB=", client_index, ",", qos, ",", pub_timeout, ",", _retained, ",", _dup);

        // we already have read OK
        Response_t rsp = _modem.waitResponse("+CMQTTPUB: ", 9000, false, true);

        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                _modem._serial.find(',');
                return _modem._serialParseIntClear();
            }
            case Response_t::A76XX_RESPONSE_TIMEOUT : {
                return A76XX_OPERATION_TIMEDOUT;
            }
            default : {
                return A76XX_GENERIC_ERROR;
            }
        }
    }

};

#endif A76XX_MQTT_CMDS_H_