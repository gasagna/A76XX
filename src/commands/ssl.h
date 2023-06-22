#ifndef A76XX_SSL_CMDS_H_
#define A76XX_SSL_CMDS_H_

/*
    @brief Commands in section 19 of the AT command manual version 1.09

    Command   | Implemented | Method | Function(s)
    --------- | ----------- | ------ |-----------------
    CSSLCFG   |      y      |        | configSSL*
    CCERTDOWN |      y      |        | certDownload
    CCERTLIST |      y      |        | certExists
    CCERTDELE |      y      |        | certDelete
    CCHSET    |      y      |        | setReportMode
    CCHMODE   |      y      |        | setDataMode
    CCHSTART  |      y      |        | start
    CCHSTOP   |      y      |        | stop
    CCHADDR   |             |        |
    CCHSSLCFG |      y      |        | setSSLContext
    CCHCFG    |             |        |
    CCHOPEN   |      y      |        | connect
    CCHCLOSE  |      y      |        | disconnect
    CCHSEND   |      y      |        | send
    CCHRECV   |      y      |        | available, recv
    CCERTMOVE |             |        |
*/

class SSLCommands {
  public:
    ModemSerial& _serial;

    SSLCommands(ModemSerial& serial)
        : _serial(serial) {}

    // CSSLCFG sslversion
    int8_t configSSLSSLversion(uint8_t ssl_ctx_index, uint8_t ssl_version) {
        _serial.sendCMD("AT+CSSLCFG=\"sslversion\",", ssl_ctx_index, ",", ssl_version);
        A76XX_RESPONSE_PROCESS(_serial.waitResponse());
    }

    // CSSLCFG authmode
    int8_t configSSLAuthmode(uint8_t ssl_ctx_index, uint8_t authmode) {
        _serial.sendCMD("AT+CSSLCFG=\"authmode\",", ssl_ctx_index, ",", authmode);
        A76XX_RESPONSE_PROCESS(_serial.waitResponse());
    }

    // CSSLCFG ignorelocaltime
    int8_t configSSLIgnorelocaltime(uint8_t ssl_ctx_index, uint8_t ignorelocaltime) {
        _serial.sendCMD("AT+CSSLCFG=\"ignorelocaltime\",", ssl_ctx_index, ",", ignorelocaltime);
        A76XX_RESPONSE_PROCESS(_serial.waitResponse());
    }

    // CSSLCFG negotiatetime
    int8_t configSSLNegotiatetime(uint8_t ssl_ctx_index, uint16_t negotiatetime) {
        _serial.sendCMD("AT+CSSLCFG=\"negotiatetime\",", ssl_ctx_index, ",", negotiatetime);
        A76XX_RESPONSE_PROCESS(_serial.waitResponse());
    }

    // CSSLCFG cacert
    int8_t configSSLCacert(uint8_t ssl_ctx_index, const char* ca_file) {
        _serial.sendCMD("AT+CSSLCFG=\"cacert\",", ssl_ctx_index, ",", ca_file);
        A76XX_RESPONSE_PROCESS(_serial.waitResponse());
    }

    // CSSLCFG clientcert
    int8_t configSSLClientcert(uint8_t ssl_ctx_index, const char* clientcert_file) {
        _serial.sendCMD("AT+CSSLCFG=\"clientcert\",", ssl_ctx_index, ",", clientcert_file);
        A76XX_RESPONSE_PROCESS(_serial.waitResponse());
    }

    // CSSLCFG clientkey
    int8_t configSSLClientkey(uint8_t ssl_ctx_index, const char* clientkey_file) {
        _serial.sendCMD("AT+CSSLCFG=\"clientkey\",", ssl_ctx_index, ",", clientkey_file);
        A76XX_RESPONSE_PROCESS(_serial.waitResponse());
    }

    // CSSLCFG password
    int8_t configSSLPassword(uint8_t ssl_ctx_index, const char* password_file) {
        _serial.sendCMD("AT+CSSLCFG=\"password\",", ssl_ctx_index, ",", password_file);
        A76XX_RESPONSE_PROCESS(_serial.waitResponse());
    }

    // CSSLCFG enableSNI
    int8_t configSSLContext(uint8_t ssl_ctx_index, uint8_t enableSNI_flag) {
        _serial.sendCMD("AT+CSSLCFG=\"enableSNI\",", ssl_ctx_index, ",", enableSNI_flag);
        A76XX_RESPONSE_PROCESS(_serial.waitResponse());
    }

    // CCERTDOWN
    int8_t certDownload(const char* cert, const char* certname) {
        _serial.sendCMD("AT+CCERTDOWN=\"", certname, "\",", strlen(cert));
        Response_t rsp = _serial.waitResponse(">", 120000);

        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                _serial.write(cert);
                _serial.flush();
                if (_serial.waitResponse() == Response_t::A76XX_RESPONSE_OK) {
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

    // CCERTLIST
    // check a file named `certname` already exists in the list of certificates
    bool certExists(const char* certname) {
        _serial.sendCMD("AT+CCERTLIST");
        Response_t rsp = _serial.waitResponse(certname, 120000);
        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                _serial.clear();
                return true;
            }
            default : {
                return false;
            }
        }
    }

    // CCERTDELE
    int8_t certDelete(const char* certname) {
        _serial.sendCMD("AT+CCERTDELE=\"", certname, "\"");
        A76XX_RESPONSE_PROCESS(_serial.waitResponse());
    }

    // CCHSSLCFG
    int8_t setSSLContext(uint8_t session_id, uint8_t ssl_ctx_index) {
        _serial.sendCMD("AT+CCHSSLCFG=", session_id, ",", ssl_ctx_index);
        A76XX_RESPONSE_PROCESS(_serial.waitResponse());
    }

    /*
        @brief Implementation for CCHSET - WRITE Command.
        @detail Configure the report mode of sending and receiving data.
        @param [IN] report_send Whether to report result of CCHSEND.
        @param [IN] report_recv Whether to report an event when data is received.
        @return Any of A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or
            A76XX_GENERIC_ERROR.
    */
    int8_t setReportMode(bool report_send, bool report_recv) {
        _serial.sendCMD("AT+CCHSET=", 
                        report_send == true ? 1 : 0,  ",", 
                        report_recv == true ? 1 : 0, );
        switch (_serial.waitResponse(120000)) {
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

    /*
        @brief Implementation for CCHMODE - WRITE Command.
        @detail Configure the mode of sending and receiving data.
        @param [IN] is_normal_mode Whether to use normal mode or transparent mode.
        @return Any of A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or
            A76XX_GENERIC_ERROR.
    */
    int8_t setDataMode(bool is_normal_mode) {
        _serial.sendCMD("AT+CCHMODE=", is_normal_mode == true ? 0 : 1);
        switch (_serial.waitResponse(120000)) {
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

    /*
        @brief Implementation for CCHSTART - EXEC Command.
        @detail Start the SSL service.
        @return Any of A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT, 
            A76XX_GENERIC_ERROR or one of the error codes at the end of chapter 19
            of the A76XX AT command manual.
    */
    int8_t start() {
        _serial.sendCMD("AT+CCHSTART");
        switch (_serial.waitResponse("+CCHSTART: ", 120000, false, true)) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                // we matched the OK +CCHSTART: 0
                return _serial.parseIntClear();
            }
            case Response_t::A76XX_RESPONSE_TIMEOUT : {
                return A76XX_OPERATION_TIMEDOUT;
            }
            case Response_t::A76XX_RESPONSE_ERROR : {
                // try reading <err> immediately, otherwise it's a generic error
                switch (_serial.waitResponse("+CCHSTART: ", 100, false, true)) {
                    case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                        return _serial.parseIntClear();
                    }
                    case Response_t::A76XX_RESPONSE_TIMEOUT : {
                        return A76XX_GENERIC_ERROR;
                    }
                }
            }
            default : {
                return A76XX_GENERIC_ERROR;
            }
        }
    }

    /*
        @brief Implementation for CCHSTOP - EXEC Command.
        @detail Stop the SSL service.
        @return Any of A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT,
            A76XX_GENERIC_ERROR or one of the error codes at the end of chapter 19
            of the A76XX AT command manual.
    */
    int8_t stop() {
        _serial.sendCMD("AT+CCHSTOP");
        switch (_serial.waitResponse("+CCHSTOP: ", 120000, false, true)) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
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

    /*
        @brief Implementation for CCHOPEN - EXEC Command.
        @detail Connect to server. Only available in normal (i.e. not transparent) mode.
        @param [IN] session_id The session index to operate. It’s from 0 to 1.
        @param [IN] host The server address, maximum length is 256 bytes.
        @param [IN] port The server port which to be connected, the range is from 1 to 65535.
        @return Any of A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT,
            A76XX_GENERIC_ERROR or one of the error codes at the end of chapter 19
            of the A76XX AT command manual.
    */
    int8_t connect(uint8_t session_id,
                   const char* host,
                   uint16_t port,
                   bool is_SSL_TLS_client,
                   uint16_t bind_port) {
        _serial.sendCMD("AT+CCHOPEN=", session_id, ",", host, ",", port);
        switch (_serial.waitResponse("+CCHOPEN: ", 120000, false, true)) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                // get session id
                int _session_id = _serial.parseIntClear(); _serial.find(',');
                // return error code or zero
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

    /*
        @brief Implementation for CCHCLOSE - WRITE Command.
        @detail Disconnect from server. Only available in normal mode.
        @param [IN] session_id The session index to operate. It’s from 0 to 1.
        @return Any of A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT,
            A76XX_GENERIC_ERROR or one of the error codes at the end of chapter 19
            of the A76XX AT command manual.
    */
    int8_t disconnect(uint8_t session_id) {
        _serial.sendCMD("AT+CCHCLOSE=", session_id);
        switch (_serial.waitResponse("+CCHCLOSE: ", 120000, false, true)) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                // get session id
                int _session_id = _serial.parseIntClear(); _serial.find(',');
                // return error code or zero
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

    /*
        @brief Implementation for CCHSEND - WRITE Command.
        @detail Send data to server.
        @param [IN] session_id The session index to operate. It’s from 0 to 1.
        @param [IN] data Pointer to byte array to send.
        @param [IN] length Number of bytes to send.
        @return Any of A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT,
            A76XX_GENERIC_ERROR or one of the error codes at the end of chapter 19
            of the A76XX AT command manual.
    */
    int8_t send(uint8_t session_id, const uint8_t* data, uint32_t length) {
        _serial.sendCMD("AT+CCHSEND=", session_id, ",", length);
        switch (_serial.waitResponse(">", 9000, false, true)) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                _serial.write(data, length);
                _serial.flush();
                if (_serial.waitResponse() == Response_t::A76XX_RESPONSE_OK) {
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

    /*
        @brief Implementation for CCHRECV - READ Command.
        @detail Get length of bytes received from the server in the cache.
        @param [IN] session_id The session index to operate. It’s from 0 to 1.
        @param [OUT] length Number of bytes available.
        @return Any of A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT,
            A76XX_GENERIC_ERROR or one of the error codes at the end of chapter 19
            of the A76XX AT command manual.
    */
    int8_t available(uint8_t session_id, uint32_t* length) {
        _serial.sendCMD("AT+CCHRECV?");
        switch (_serial.waitResponse("+CCHRECV: LEN,", 9000, false, true)) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                int cache_len_0 = _serial.parseIntClear(); _serial.find(',');
                int cache_len_1 = _serial.parseIntClear();
                *length = session_id == 0 ? cache_len_0 : cache_len_1;
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

    /*
        @brief Implementation for CCHRECV - WRITE Command.
        @detail Read cached data received from the server.
        @param [IN] session_id The session index to operate. It’s from 0 to 1.
        @param [IN] data Pointer to byte array to send.
        @param [IN] length Number of bytes to send.
        @return Any of A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT,
            A76XX_GENERIC_ERROR or one of the error codes at the end of chapter 19
            of the A76XX AT command manual.
    */
    int8_t recv(uint8_t session_id, const uint8_t* data, uint32_t length) {
        _serial.sendCMD("AT+CCHRECV=", session_id, ",", length);
        switch (_serial.waitResponse("+CCHRECV: ", 120000)) {
            case Response_t::A76XX_RESPONSE_OK : {
                uint8_t* _data = data;
                int _session_id, _len;
                while (true) {
                    switch (_serial.waitResponse("+CCHRECV: DATA", 1000)) { 
                        case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                            _session_id = _serial.parseInt(); _serial.find(',');
                            _len = _serial.parseInt();
                            if (_len > 0) {
                                _serial.readBytes(_data, _len);
                                _data += _len;
                            } else {
                                return A76XX_OPERATION_SUCCEEDED;
                            }
                            break;
                        }
                        default: {
                            return A76XX_GENERIC_ERROR;
                        }
                    }
                }
            }
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                // this is an error case
                int _session_id = _serial.parseIntClear(); _serial.find(',');
                // return error code
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


    ///////////////////////////////////////////////////////////////
    // HIGH LEVEL COMMANDS
    ///////////////////////////////////////////////////////////////

    // delete certificate if exists, then download
    int8_t certOverwrite(const char* cert, const char* certname) {
        int8_t retcode;

        if (certExists(certname)) {
            retcode = certDelete(certname);
            A76XX_RETCODE_ASSERT_RETURN(retcode);
        }

        retcode = certDownload(cert, certname);
        A76XX_RETCODE_ASSERT_RETURN(retcode);

        return retcode;
    }
};

#endif A76XX_SSL_CMDS_H_