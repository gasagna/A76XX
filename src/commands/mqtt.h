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
    CMQTTTOPIC     |      y      |        | setPublishTopic
    CMQTTPAYLOAD   |      y      |        | setPublishPayload
    CMQTTPUB       |      y      |        | publish
    CMQTTSUBTOPIC  |             |        |
    CMQTTSUB       |      y      |        | subscribe
    CMQTTUNSUBTOPIC|             |        |
    CMQTTUNSUB     |             |        |
    CMQTTCFG       |             |        |
*/

class MQTTCommands {
  public:
    ModemSerial& _serial;

    MQTTCommands(ModemSerial& serial)
        : _serial(serial) {}

    // CMQTTSTART
    int8_t start() {
        // start MQTT service by activating PDP context
        _serial.sendCMD("AT+CMQTTSTART");
        Response_t rsp = _serial.waitResponse("+CMQTTSTART: ", 12000, false, true);

        if (rsp == Response_t::A76XX_RESPONSE_ERROR)
            return A76XX_MQTT_ALREADY_STARTED;

        // read return code (could be 0) and clear stream
        return _serial.parseIntClear();
    }

    // CMQTTSTOP
    int8_t stop() {
        _serial.sendCMD("AT+CMQTTSTOP");
        Response_t rsp = _serial.waitResponse("+CMQTTSTOP: ", 12000, false, true);

        if (rsp == Response_t::A76XX_RESPONSE_ERROR)
            return A76XX_MQTT_ALREADY_STOPPED;

        // read return code (could be 0) and clear stream
        return _serial.parseIntClear();
    }

    // CMQTTACCQ
    int8_t acquireClient(uint8_t client_index, const char clientID[], uint8_t server_type) {
        _serial.sendCMD("AT+CMQTTACCQ=", client_index, ",\"", clientID, "\",", server_type);
        Response_t rsp = _serial.waitResponse("+CMQTTACCQ: ", 9000, true, true);

        switch( rsp ) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                _serial.find(',');
                return _serial.parseIntClear();
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
        _serial.sendCMD("AT+CMQTTREL=", client_index);
        Response_t rsp = _serial.waitResponse("+CMQTTREL: ", 9000, true, true);

        if (rsp == Response_t::A76XX_RESPONSE_OK)

        if (rsp == Response_t::A76XX_RESPONSE_ERROR)
            return A76XX_GENERIC_ERROR;

        // this is an error case
        if (rsp == Response_t::A76XX_RESPONSE_MATCH_1ST) {
            _serial.find(',');
            return _serial.parseIntClear();
        }

        return A76XX_GENERIC_ERROR;
    }

    // CMQTTSSLCFG
    int8_t setSSLContext(uint8_t session_id, uint8_t ssl_ctx_index) {
        _serial.sendCMD("AT+CMQTTSSLCFG=", session_id, ",", ssl_ctx_index);
        return _serial.waitResponse();
    }

    // CMQTTWILLTOPIC
    int8_t setWillTopic(uint8_t client_index, const char* will_topic) {
        _serial.sendCMD("AT+CMQTTWILLTOPIC=", client_index, ",", strlen(will_topic));

        Response_t rsp = _serial.waitResponse(">", "+CMQTTWILLTOPIC: ", 9000);

        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                _serial.write(will_topic);
                _serial.flush();
                if (_serial.waitResponse() == Response_t::A76XX_RESPONSE_OK) {
                    return A76XX_OPERATION_SUCCEEDED;
                } else {
                    return A76XX_GENERIC_ERROR;
                }
            }
            case Response_t::A76XX_RESPONSE_MATCH_2ND : {
                _serial.find(',');
                return _serial.parseIntClear();
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
        _serial.sendCMD("AT+CMQTTWILLMSG=", client_index, ",", strlen(will_message), ",", will_qos);

        Response_t rsp = _serial.waitResponse(">", "+CMQTTWILLMSG: ", 9000);

        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                _serial.write(will_message);
                _serial.flush();
                if (_serial.waitResponse() == Response_t::A76XX_RESPONSE_OK) {
                } else {
                    return A76XX_GENERIC_ERROR;
                }
            }
            case Response_t::A76XX_RESPONSE_MATCH_2ND : {
                _serial.find(',');
                return _serial.parseIntClear();
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
            _serial.sendCMD("AT+CMQTTCONNECT=", client_index, ",\"tcp://", server, ":", port, "\",", keepalive, ",", clean_session, ",\"", username, "\",\"", password, "\"");
        } else {
            _serial.sendCMD("AT+CMQTTCONNECT=", client_index, ",\"tcp://", server, ":", port, "\",", keepalive, ",", clean_session);
        }

        // it might happen that the command only returns ERROR (case 5)
        // in this case we timeout
        Response_t rsp = _serial.waitResponse("+CMQTTCONNECT: ", 9000, false, false);

        // read int output
        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                _serial.find(',');
                return _serial.parseIntClear();
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
        _serial.sendCMD("AT+CMQTTDISC?");

        char match_str[15] = "+CMQTTDISC: x,";
        match_str[12] = client_index == 0 ? '0' : '1';

        Response_t rsp = _serial.waitResponse(match_str, 9000, false, true);

        if (rsp == Response_t::A76XX_RESPONSE_MATCH_1ST) {
            int retcode = _serial.parseIntClear();
            return retcode == 0 ? true : false;
        } else {
            return false;
        }
    }

    // CMQTTDISC
    int8_t disconnect(uint8_t client_index, uint8_t timeout) {
        _serial.sendCMD("AT+CMQTTDISC=", client_index, ",", timeout);
        Response_t rsp = _serial.waitResponse("+CMQTTDISC: ", timeout, false, true);

        // read int output
        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST: {
                _serial.find(',');
                return _serial.parseIntClear();
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
        _serial.sendCMD("AT+CMQTTTOPIC=", client_index, ",", strlen(topic));

        Response_t rsp = _serial.waitResponse(">", "+CMQTTTOPIC: ", 9000);

        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                _serial.write(topic);
                _serial.flush();
                if (_serial.waitResponse() == Response_t::A76XX_RESPONSE_OK) {
                    return A76XX_OPERATION_SUCCEEDED;
                } else {
                    return A76XX_GENERIC_ERROR;
                }
            }
            case Response_t::A76XX_RESPONSE_MATCH_2ND : {
                _serial.find(',');
                return _serial.parseIntClear();
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
        _serial.sendCMD("AT+CMQTTPAYLOAD=0,", length);

        Response_t rsp = _serial.waitResponse(">", "+CMQTTTPAYLOAD: ", 9000);

        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                _serial.write(payload, length);
                _serial.flush();
                if (_serial.waitResponse() == Response_t::A76XX_RESPONSE_OK) {
                    return A76XX_OPERATION_SUCCEEDED;
                } else {
                    return A76XX_GENERIC_ERROR;
                }
            }
            case Response_t::A76XX_RESPONSE_MATCH_2ND : {
                _serial.find(',');
                return _serial.parseIntClear();
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
        _serial.sendCMD("AT+CMQTTPUB=", client_index, ",", qos, ",", pub_timeout, ",", _retained, ",", _dup);

        // we already have read OK
        Response_t rsp = _serial.waitResponse("+CMQTTPUB: ", 9000, false, true);

        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                _serial.find(',');
                return _serial.parseIntClear();
            }
            case Response_t::A76XX_RESPONSE_TIMEOUT : {
                return A76XX_OPERATION_TIMEDOUT;
            }
            default : {
                return A76XX_GENERIC_ERROR;
            }
        }
    }

    // CMQTTSUB
    int8_t subscribe(uint8_t client_index, const char* topic, uint8_t qos) {
        _serial.sendCMD("AT+CMQTTSUB=", client_index, ",", strlen(topic), ",", qos);
        Response_t rsp = _serial.waitResponse(">", "+CMQTTSUB: ", 9000, false, true);
        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                _serial.write(topic);
                _serial.flush();
                if (_serial.waitResponse("+CMQTTSUB: ", 9000, false, true) == Response_t::A76XX_RESPONSE_MATCH_1ST) {
                    _serial.find(',');
                    return _serial.parseIntClear();
                } else {
                    return A76XX_GENERIC_ERROR;
                }
            }
            case Response_t::A76XX_RESPONSE_MATCH_2ND : {
                _serial.find(',');
                return _serial.parseIntClear();
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