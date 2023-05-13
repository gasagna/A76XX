#ifndef A76XX_HTTP_CMDS_H_
#define A76XX_HTTP_CMDS_H_

/*
    @brief Commands in section 16 of the AT command manual version 1.09

    Command     | Implemented | Method | Function(s)
    ----------- | ----------- | ------ |-----------------
    HTTPINIT    |      y      | EXEC   | init
    HTTPTERM    |      y      | EXEC   | term
    HTTPPARA    |      y      | WRITE  | config_http_*
    HTTPACTION  |      y      | WRITE  | action
    HTTPHEAD    |      y      | EXEC   | read_header
    HTTPREAD    |      y      | R/W    | get_content_length, read_response_body
    HTTPDATA    |      y      | WRITE  | input_data
    HTTPPOSTFILE|             |        |
    HTTPREADFILE|             |        |
*/
template <typename MODEM>
class A76XX_HTTP_Commands {
  public:
    MODEM* _modem = NULL;

    // HTTPINIT
    int8_t init() {
        _modem->sendCMD("AT+HTTPINIT");
        A76XX_RESPONSE_PROCESS(_modem->waitResponse(120000))
    }

    // HTTPTERM
    int8_t term() {
        _modem->sendCMD("AT+HTTPTERM");
        A76XX_RESPONSE_PROCESS(_modem->waitResponse(120000))
    }

    // HTTPPARA URL
    int8_t config_http_url(const char* server, uint16_t port, const char* path, bool use_ssl) {
        // add the protocol if not present
        if (strstr(server, "https://") == NULL || strstr(server, "http://") == NULL) {
            if (use_ssl == true) {
                _modem->sendCMD("AT+HTTPPARA=\"URL\",", "\"https://", server, ":", port, "/", path, "\"");
            } else {
                _modem->sendCMD("AT+HTTPPARA=\"URL\",", "\"http://", server, ":", port, "/", path, "\"");
            }
        } else {
            _modem->sendCMD("AT+HTTPPARA=\"URL\",", "\"", server, ":", port, "/", path, "\"");
        }
        A76XX_RESPONSE_PROCESS(_modem->waitResponse(120000))
    }

    // HTTPPARA CONNECTTO
    int8_t config_http_conn_timeout(int conn_timeout) {
        _modem->sendCMD("AT+HTTPPARA=\"CONNECTTO\",", conn_timeout);
        A76XX_RESPONSE_PROCESS(_modem->waitResponse(120000))
    }

    // HTTPPARA RECVTO
    int8_t config_http_recv_timeout(int recv_timeout) {
        _modem->sendCMD("AT+HTTPPARA=\"RECVTO\",", recv_timeout);
        A76XX_RESPONSE_PROCESS(_modem->waitResponse(120000))
    }

    // HTTPPARA CONTENT
    int8_t config_http_content_type(const char* content_type) {
        _modem->sendCMD("AT+HTTPPARA=\"CONTENT\",\"", content_type, "\"");
        A76XX_RESPONSE_PROCESS(_modem->waitResponse(120000))
    }

    // HTTPPARA ACCEPT
    int8_t config_http_accept(const char* accept) {
        _modem->sendCMD("AT+HTTPPARA=\"ACCEPT\",\"", accept, "\"");
        A76XX_RESPONSE_PROCESS(_modem->waitResponse(120000))
    }

    // HTTPPARA SSLCFG
    int8_t config_http_sslcfg_id(uint8_t sslcfg_id) {
        _modem->sendCMD("AT+HTTPPARA=\"SSLCFG\",", sslcfg_id);
        A76XX_RESPONSE_PROCESS(_modem->waitResponse(120000))
    }

    // HTTPPARA USERDATA
    int8_t config_http_user_data(const char* header, const char* value) {
        _modem->sendCMD("AT+HTTPPARA=\"USERDATA\",\"", header, ":", value, "\"");
        A76XX_RESPONSE_PROCESS(_modem->waitResponse(120000))
    }

    // HTTPPARA READMODE
    int8_t config_http_read_mode(uint8_t readmode) {
        _modem->sendCMD("AT+HTTPPARA=\"READMODE\",", readmode);
        A76XX_RESPONSE_PROCESS(_modem->waitResponse(120000))
    }

    // HTTPACTION
    // 0 =>    GET
    // 1 =>   POST
    // 2 =>   HEAD
    // 3 => DELETE
    // 4 =>    PUT
    int8_t action(uint8_t method, uint16_t* status_code, uint32_t* length) {
        _modem->sendCMD("AT+HTTPACTION=", method);
        Response_t rsp = _modem->waitResponse("+HTTPACTION: ", 120000, false, true);
        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                _modem->streamParseInt();
                _modem->streamFind(',');
                *status_code = _modem->streamParseInt();
                _modem->streamFind(',');
                *length = _modem->streamParseInt();
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

    // HTTPHEAD
    int8_t read_header(String& header) {
        _modem->sendCMD("AT+HTTPHEAD");
        Response_t rsp = _modem->waitResponse("+HTTPHEAD: ", 120000, false, true);
        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                // get length of header
                int header_length = _modem->streamParseInt();
                
                // reserve space for the string
                if (header.reserve(header_length) == 0) {
                    return A76XX_OUT_OF_MEMORY;
                }

                // advance till we start with the actual content
                _modem->streamFind("\n");

                // read as many bytes as we said
                for (uint32_t i = 0; i < header_length; i++) {
                    while (_modem->streamAvailable() == 0) {}
                    header += static_cast<char>(_modem->streamRead());
                }

                if (_modem->waitResponse() == Response_t::A76XX_RESPONSE_OK) {
                    return A76XX_OPERATION_SUCCEEDED;
                } else {
                    return A76XX_GENERIC_ERROR;
                }
            }
            case Response_t::A76XX_RESPONSE_TIMEOUT : {
                return A76XX_OPERATION_TIMEDOUT;
            }
            default : {
                return A76XX_GENERIC_ERROR;
            }
        }
    }

    int8_t get_content_length(uint32_t* len) {
        _modem->sendCMD("AT+HTTPREAD?");
        Response_t rsp = _modem->waitResponse("+HTTPREAD: LEN,", 120000, false, true);
        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                *len = _modem->streamParseIntClear();
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

    // HTTPREAD - read entire response
    int8_t read_response_body(String& body, uint32_t body_length) {
        _modem->sendCMD("AT+HTTPREAD=", 0, ",", body_length);
        Response_t rsp = _modem->waitResponse("+HTTPREAD: ", 120000, false, true);
        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                // this should match with body_length
                if (_modem->streamParseInt() != body_length) {
                    return A76XX_GENERIC_ERROR;
                }

                // check we can allocate the string
                if (body.reserve(body_length) == 0) {
                    return A76XX_OUT_OF_MEMORY;
                }

                // advance till we start with the actual content
                _modem->streamFind('\n');

                // read as many bytes as we said
                for (uint32_t i = 0; i < body_length; i++) {
                    while (_modem->streamAvailable() == 0) {}
                    body += static_cast<char>(_modem->streamRead());
                }

                // clear stream
                if (_modem->waitResponse("+HTTPREAD: 0") == Response_t::A76XX_RESPONSE_MATCH_1ST) {
                    return A76XX_OPERATION_SUCCEEDED;
                } else {
                    return A76XX_GENERIC_ERROR;
                }
            }
            case Response_t::A76XX_RESPONSE_TIMEOUT : {
                return A76XX_OPERATION_TIMEDOUT;
            }
            default : {
                return A76XX_GENERIC_ERROR;
            }
        }
    }

    // HTTPDATA
    int8_t input_data(const char* data, uint32_t length) {
        // use 30 seconds timeout
        _modem->sendCMD("AT+HTTPDATA=", length, ",", 30);

        // timeout after 10 seconds
        Response_t rsp = _modem->waitResponse("DOWNLOAD", 10, false, true);

        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                _modem->streamWrite(data, length);
                _modem->streamFlush();
                switch (_modem->waitResponse()) {
                    case Response_t::A76XX_RESPONSE_OK : {
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
            case Response_t::A76XX_RESPONSE_TIMEOUT : {
                return A76XX_OPERATION_TIMEDOUT;
            }
            default : {
                return A76XX_GENERIC_ERROR;
            }
        }
    }
};

#endif A76XX_HTTP_CMDS_H_