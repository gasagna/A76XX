#ifndef A76XX_MQTT_CMDS_H_
#define A76XX_MQTT_CMDS_H_

/*
    @brief Commands in section 18 of the AT command manual version 1.09

    Command        | Implemented | Method | Function(s)
    -------------- | ----------- | ------ |-----------------
    CMQTTSTART     |      y      |        | start
    CMQTTSTOP      |      y      |        | stop
    CMQTTACCQ      |      y      |        | acquireClient
    CMQTTREL       |      y      |        | releaseClient
    CMQTTSSLCFG    |      y      |        | setSSLContext
    CMQTTWILLTOPIC |      y      |        | setWillTopic
    CMQTTWILLMSG   |      y      |        | setWillMessage
    CMQTTCONNECT   |      y      |        | connect
    CMQTTDISC      |      y      |        | disconnect, isConnected
    CMQTTTOPIC     |      y      |        | setTopic
    CMQTTPAYLOAD   |      y      |        | setPayload
    CMQTTPUB       |      y      |        | publish
    CMQTTSUBTOPIC  |             |        |
    CMQTTSUB       |             |        |
    CMQTTUNSUBTOPIC|             |        |
    CMQTTUNSUB     |             |        |
    CMQTTCFG       |             |        |
*/
template <typename MODEM>
class MQTTCommands {
  public:
    MODEM* _modem = NULL;

    // CMQTTSTART
    int8_t start() {
        // start MQTT service by activating PDP context
        _modem->sendCMD("AT+CMQTTSTART");
        Response_t rsp = _modem->waitResponse("+CMQTTSTART: ", 12000, false, true);

        if (rsp == Response_t::A76XX_RESPONSE_ERROR)
            return A76XX_MQTT_ALREADY_STARTED;

        // read return code (could be 0) and clear stream
        return _modem->streamParseIntClear();
    }

    // CMQTTSTOP
    int8_t stop() {
        _modem->sendCMD("AT+CMQTTSTOP");
        Response_t rsp = _modem->waitResponse("+CMQTTSTOP: ", 12000, false, true);

        if (rsp == Response_t::A76XX_RESPONSE_ERROR)
            return A76XX_MQTT_ALREADY_STOPPED;

        // read return code (could be 0) and clear stream
        return _modem->streamParseIntClear();
    }

    // CMQTTACCQ
    int8_t acquireClient(uint8_t client_index, const char clientID[], uint8_t server_type) {
        _modem->sendCMD("AT+CMQTTACCQ=", client_index, ",\"", clientID, "\",", server_type);
        Response_t rsp = _modem->waitResponse("+CMQTTACCQ: ", 9000, true, true);

        switch( rsp ) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                _modem->streamFind(',');
                return _modem->streamParseIntClear();
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
    int8_t releaseClient(uint8_t client_index) {
        _modem->sendCMD("AT+CMQTTREL=", client_index);
        Response_t rsp = _modem->waitResponse("+CMQTTREL: ", 9000, true, true);

        if (rsp == Response_t::A76XX_RESPONSE_OK)

        if (rsp == Response_t::A76XX_RESPONSE_ERROR)
            return A76XX_GENERIC_ERROR;

        // this is an error case
        if (rsp == Response_t::A76XX_RESPONSE_MATCH_1ST) {
            _modem->streamFind(',');
            return _modem->streamParseIntClear();
        }

        return A76XX_GENERIC_ERROR;
    }

    // CMQTTSSLCFG
    int8_t setSSLContext(uint8_t session_id, uint8_t ssl_ctx_index) {
        _modem->sendCMD("AT+CMQTTSSLCFG=", session_id, ",", ssl_ctx_index);
        return _modem->waitResponse();
    }

    // CMQTTWILLTOPIC
    int8_t setWillTopic(uint8_t client_index, const char* will_topic) {
        _modem->sendCMD("AT+CMQTTWILLTOPIC=", client_index, ",", strlen(will_topic));

        Response_t rsp = _modem->waitResponse(">", "+CMQTTWILLTOPIC: ", 9000);

        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                _modem->streamWrite(will_topic);
                _modem->streamFlush();
                if (_modem->waitResponse() == Response_t::A76XX_RESPONSE_OK) {
                    return A76XX_OPERATION_SUCCEEDED;
                } else {
                    return A76XX_GENERIC_ERROR;
                }
            }
            case Response_t::A76XX_RESPONSE_MATCH_2ND : {
                _modem->streamFind(',');
                return _modem->streamParseIntClear();
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
    int8_t setWillMessage(uint8_t client_index, const char* will_message, uint8_t will_qos) {
        _modem->sendCMD("AT+CMQTTWILLMSG=", client_index, ",", strlen(will_message), ",", will_qos);

        Response_t rsp = _modem->waitResponse(">", "+CMQTTWILLMSG: ", 9000);

        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                _modem->streamWrite(will_message);
                _modem->streamFlush();
                if (_modem->waitResponse() == Response_t::A76XX_RESPONSE_OK) {
                } else {
                    return A76XX_GENERIC_ERROR;
                }
            }
            case Response_t::A76XX_RESPONSE_MATCH_2ND : {
                _modem->streamFind(',');
                return _modem->streamParseIntClear();
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
            _modem->sendCMD("AT+CMQTTCONNECT=", client_index, ",\"tcp://", server, ":", port, "\",", keepalive, ",", clean_session, ",\"", username, "\",\"", password, "\"");
        } else {
            _modem->sendCMD("AT+CMQTTCONNECT=", client_index, ",\"tcp://", server, ":", port, "\",", keepalive, ",", clean_session);
        }

        // it might happen that the command only returns ERROR (case 5)
        // in this case we timeout
        Response_t rsp = _modem->waitResponse("+CMQTTCONNECT: ", 9000, false, false);

        // read int output
        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                _modem->streamFind(',');
                return _modem->streamParseIntClear();
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
    bool isConnected(uint8_t client_index) {
        _modem->sendCMD("AT+CMQTTDISC?");

        char match_str[15] = "+CMQTTDISC: x,";
        match_str[12] = client_index == 0 ? '0' : '1';

        Response_t rsp = _modem->waitResponse(match_str, 9000, false, true);

        if (rsp == Response_t::A76XX_RESPONSE_MATCH_1ST) {
            int retcode = _modem->streamParseIntClear();
            return retcode == 0 ? true : false;
        } else {
            return false;
        }
    }

    // CMQTTDISC
    int8_t disconnect(uint8_t client_index, uint8_t timeout) {
        _modem->sendCMD("AT+CMQTTDISC=", client_index, ",", timeout);
        Response_t rsp = _modem->waitResponse("+CMQTTDISC: ", timeout, false, true);

        // read int output
        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST: {
                _modem->streamFind(',');
                return _modem->streamParseIntClear();
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
    int8_t setTopic(uint8_t client_index, const char* topic) {
        _modem->sendCMD("AT+CMQTTTOPIC=", client_index, ",", strlen(topic));

        Response_t rsp = _modem->waitResponse(">", "+CMQTTTOPIC: ", 9000);

        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                _modem->streamWrite(topic);
                _modem->streamFlush();
                if (_modem->waitResponse() == Response_t::A76XX_RESPONSE_OK) {
                    return A76XX_OPERATION_SUCCEEDED;
                } else {
                    return A76XX_GENERIC_ERROR;
                }
            }
            case Response_t::A76XX_RESPONSE_MATCH_2ND : {
                _modem->streamFind(',');
                return _modem->streamParseIntClear();
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
    int8_t setPayload(uint8_t client_index, const uint8_t* payload, uint length) {
        _modem->sendCMD("AT+CMQTTPAYLOAD=0,", length);

        Response_t rsp = _modem->waitResponse(">", "+CMQTTTPAYLOAD: ", 9000);

        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                _modem->streamWrite(payload, length);
                _modem->streamFlush();
                if (_modem->waitResponse() == Response_t::A76XX_RESPONSE_OK) {
                    return A76XX_OPERATION_SUCCEEDED;
                } else {
                    return A76XX_GENERIC_ERROR;
                }
            }
            case Response_t::A76XX_RESPONSE_MATCH_2ND : {
                _modem->streamFind(',');
                return _modem->streamParseIntClear();
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
        _modem->sendCMD("AT+CMQTTPUB=", client_index, ",", qos, ",", pub_timeout, ",", _retained, ",", _dup);

        // we already have read OK
        Response_t rsp = _modem->waitResponse("+CMQTTPUB: ", 9000, false, true);

        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                _modem->streamFind(',');
                return _modem->streamParseIntClear();
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