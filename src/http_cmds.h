#ifndef A76XX_HTTP_CMDS_H_
#define A76XX_HTTP_CMDS_H_

#include "modem.h"

#define A76XX_HTTP_CONTINUE                                100
#define A76XX_HTTP_SWITCHING_PROTOCOLS                     101
#define A76XX_HTTP_OK                                      200
#define A76XX_HTTP_CREATED                                 201
#define A76XX_HTTP_ACCEPTED                                202
#define A76XX_HTTP_NON_AUTHORITATIVE_INFORMATION           203
#define A76XX_HTTP_NO_CONTENT                              204
#define A76XX_HTTP_RESET_CONTENT                           205
#define A76XX_HTTP_PARTIAL_CONTENT                         206
#define A76XX_HTTP_MULTIPLE_CHOICES                        300
#define A76XX_HTTP_MOVED_PERMANENTLY                       301
#define A76XX_HTTP_FOUND                                   302
#define A76XX_HTTP_SEE_OTHER                               303
#define A76XX_HTTP_NOT_MODIFIED                            304
#define A76XX_HTTP_USE_PROXY                               305
#define A76XX_HTTP_TEMPORARY_REDIRECT                      307
#define A76XX_HTTP_BAD_REQUEST                             400
#define A76XX_HTTP_UNAUTHORIZED                            401
#define A76XX_HTTP_PAYMENT_REQUIRED                        402
#define A76XX_HTTP_FORBIDDEN                               403
#define A76XX_HTTP_NOT_FOUND                               404
#define A76XX_HTTP_METHOD_NOT_ALLOWED                      405
#define A76XX_HTTP_NOT_ACCEPTABLE                          406
#define A76XX_HTTP_PROXY_AUTHENTICATION_REQUIRED           407
#define A76XX_HTTP_REQUEST_TIMEOUT                         408
#define A76XX_HTTP_CONFLICT                                409
#define A76XX_HTTP_GONE                                    410
#define A76XX_HTTP_LENGTH_REQUIRED                         411
#define A76XX_HTTP_PRECONDITION_FAILED                     412
#define A76XX_HTTP_REQUEST_ENTITY_TOO_LARGE                413
#define A76XX_HTTP_REQUEST_URI_TOO_LARGE                   414
#define A76XX_HTTP_UNSUPPORTED_MEDIA_TYPE                  415
#define A76XX_HTTP_REQUESTED_RANGE_NOT_SATISFIABLE         416
#define A76XX_HTTP_EXPECTATION_FAILED                      417
#define A76XX_HTTP_INTERNAL_SERVER_ERROR                   500
#define A76XX_HTTP_NOT_IMPLEMENTED                         501
#define A76XX_HTTP_BAD_GATEWAY                             502
#define A76XX_HTTP_SERVICE_UNAVAILABLE                     503
#define A76XX_HTTP_GATEWAY_TIMEOUT                         504
#define A76XX_HTTP_HTTP_VERSION_NOT_SUPPORTED              505
#define A76XX_HTTP_NOT_HTTP_PDU                            600
#define A76XX_HTTP_NETWORK_ERROR                           601
#define A76XX_HTTP_NO_MEMORY                               602
#define A76XX_HTTP_DNS_ERROR                               603
#define A76XX_HTTP_STACK_BUSY                              604

// additional error codes returned by the SIMCOM firmware
#define A76XX_HTTP_ALERT_STATE                             701
#define A76XX_HTTP_UNKNOWN_ERROR                           702
#define A76XX_HTTP_BUSY                                    703
#define A76XX_HTTP_CONNECTION_CLOSED_ERROR                 704
#define A76XX_HTTP_TIMEOUT                                 705
#define A76XX_HTTP_RECEIVE_SEND_SOCKET_DATA_FAILED         706
#define A76XX_HTTP_FILE_NOT_EXISTS_OR_OTHER_MEMORY_ERROR   707
#define A76XX_HTTP_INVALID_PARAMETER                       708
#define A76XX_HTTP_NETWORK_ERROR                           709
#define A76XX_HTTP_START_A_NEW_SSL_SESSION_FAILED          710
#define A76XX_HTTP_WRONG_STATE                             711
#define A76XX_HTTP_FAILED_TO_CREATE_SOCKET                 712
#define A76XX_HTTP_GET_DNS_FAILED                          713
#define A76XX_HTTP_CONNECT_SOCKET_FAILED                   714
#define A76XX_HTTP_HANDSHAKE_FAILED                        715
#define A76XX_HTTP_CLOSE_SOCKET_FAILED                     716
#define A76XX_HTTP_NO_NETWORK_ERROR                        717
#define A76XX_HTTP_SEND_DATA_TIMEOUT                       718
#define A76XX_HTTP_CA_MISSED                               719

class A76XX_HTTP_Commands {
  private:
    A76XX& _modem;

  public:
    // Initialise from modem
    A76XX_HTTP_Commands(A76XX& modem)
        : _modem(modem) {}

    // HTTPINIT
    int8_t init() {
        _modem.sendCMD("AT+HTTPINIT");
        A76XX_RESPONSE_PROCESS(_modem.waitResponse(120000))
    }

    // HTTPTERM
    int8_t term() {
        _modem.sendCMD("AT+HTTPTERM");
        A76XX_RESPONSE_PROCESS(_modem.waitResponse(120000))
    }

    // HTTPPARA URL
    template <typename ARG>
    int8_t config_http_url(ARG server, uint16_t server, const char* path) {
        _modem.sendCMD("AT+HTTPPARA=\"URL\",", server, ":", server, "/", path);
        A76XX_RESPONSE_PROCESS(_modem.waitResponse(120000))
    }

    // HTTPPARA CONNECTTO
    int8_t config_http_conn_timeout(int conn_timeout) {
        _modem.sendCMD("AT+HTTPPARA=\"CONNECTTO\",", conn_timeout);
        A76XX_RESPONSE_PROCESS(_modem.waitResponse(120000))
    }

    // HTTPPARA RECVTO
    int8_t config_http_recv_timeout(int recv_timeout) {
        _modem.sendCMD("AT+HTTPPARA=\"RECVTO\",", recv_timeout);
        A76XX_RESPONSE_PROCESS(_modem.waitResponse(120000))
    }

    // HTTPPARA CONTENT
    int8_t config_http_content_type(const char* content_type) {
        _modem.sendCMD("AT+HTTPPARA=\"CONTENT\",", content_type);
        A76XX_RESPONSE_PROCESS(_modem.waitResponse(120000))
    }

    // HTTPPARA ACCEPT
    int8_t config_http_accept(const char* accept) {
        _modem.sendCMD("AT+HTTPPARA=\"ACCEPT\",", accept);
        A76XX_RESPONSE_PROCESS(_modem.waitResponse(120000))
    }

    // HTTPPARA SSLCFG
    int8_t config_http_sslcfg_id(uint8_t sslcfg_id) {
        _modem.sendCMD("AT+HTTPPARA=\"SSLCFG\",", sslcfg_id);
        A76XX_RESPONSE_PROCESS(_modem.waitResponse(120000))
    }

    // HTTPPARA USERDATA
    int8_t config_http_user_data(const char* user_data) {
        _modem.sendCMD("AT+HTTPPARA=\"USERDATA\",", user_data);
        A76XX_RESPONSE_PROCESS(_modem.waitResponse(120000))
    }

    // HTTPPARA READMODE
    int8_t config_http_user_data(uint8_t readmode) {
        _modem.sendCMD("AT+HTTPPARA=\"READMODE\",", readmode);
        A76XX_RESPONSE_PROCESS(_modem.waitResponse(120000))
    }

    // HTTPACTION
    int8_t action(int method, uint16_t* status_code, uint32_t* length) {
        _modem.sendCMD("AT+HTTPACTION=", method);
        Response_t rsp = _modem.waitResponse("+HTTPACTION: ", 120000, false, true);
        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH1 : {
                _modem._serial.parseInt();
                _modem._serial.find(',');
                *status_code = _modem._serial.parseInt();
                _modem._serial.find(',');
                *length = _modem._serial.parseInt();
                return A76XX_HTTP_OPERATION_SUCCEEDED;
            }
            case Response_t::A76XX_RESPONSE_TIMEOUT : {
                return A76XX_OPERATION_TIMEDOUT;
            }
            default : {
                return A76XX_GENERIC_ERROR;
            }
        }
    }

    // HTTPACTION - helper methods
    int8_t    GET(int* status_code, int* length) { return action(0, status_code, length); }
    int8_t   POST(int* status_code, int* length) { return action(1, status_code, length); }
    int8_t   HEAD(int* status_code, int* length) { return action(2, status_code, length); }
    int8_t DELETE(int* status_code, int* length) { return action(3, status_code, length); }
    int8_t    PUT(int* status_code, int* length) { return action(4, status_code, length); }


    // HTTPHEAD
    int8_t read_header(String& header) {
        _modem.sendCMD("AT+HTTPHEAD");
        Response_t rsp = _modem.waitResponse("+HTTPHEAD: ", 120000, false, true);
        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH1 : {
                int data_len = _modem._serial.parseInt();
                header.reserve(data_len);
                size_t nbytes = _modem._serial.readBytes(header, data_len);
                if (nbytes == data_len) {
                    return A76XX_HTTP_OPERATION_SUCCEEDED;
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
        _modem.sendCMD("AT+HTTPREAD?");
        Response_t rsp = _modem.waitResponse("+HTTPREAD: LEN,", 120000, false, true);
        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                *len = _modem._serialParseIntClear();
                return A76XX_HTTP_OPERATION_SUCCEEDED;
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
        // get how much body is available
        // uint32_t byte_size;
        // int8_t retcode = get_content_length(&byte_size);
        // if ( retcode != A76XX_HTTP_OPERATION_SUCCEEDED)
            // return retcode;

        _modem.sendCMD("AT+HTTPREAD=", 0, ",", body_length);
        Response_t rsp = _modem.waitResponse("+HTTPREAD: ", 120000, false, true);
        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                // get <data_len>
                _modem._serial.parseInt();

                size_t nbytes = _modem._serial.readBytes(body, body_length);

                // clear stream
                _modem.waitResponse("+HTTPREAD: 0")

                if (nbytes == byte_size) {
                    return A76XX_HTTP_OPERATION_SUCCEEDED;
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
    int8_t input_data(const char* data, uint32_t length ) {
        // use a safe 5 seconds timeout
        _modem.sendCMD("AT+HTTPDATA=", length, ",", 5);

        // timeout after 10 seconds
        Response_t rsp = _modem.waitResponse("DOWNLOAD", 10, false, true);

        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                _modem._serial.write(data, length);
                switch (_modem.waitResponse()) {
                    case Response_t::A76XX_RESPONSE_OK : {
                        return A76XX_HTTP_OPERATION_SUCCEEDED;
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

#endif A76XX_MQTT_CMDS_H_