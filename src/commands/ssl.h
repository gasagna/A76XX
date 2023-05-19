#ifndef A76XX_SSL_CMDS_H_
#define A76XX_SSL_CMDS_H_

//default certificate names
#define A76XX_DEFAULT_CACERT_FILENAME      "_A76XX_CACART_DEFAULT_FILENAME_.pem"
#define A76XX_DEFAULT_CLIENTCERT_FILENAME  "_A76XX_CLIENTCERT_DEFAULT_FILENAME_.pem"
#define A76XX_DEFAULT_CLIENTKEY_FILENAME   "_A76XX_CLIENTKEY_DEFAULT_FILENAME_.pem"
#define A76XX_DEFAULT_PASSWORD_FILENAME    "_A76XX_PASSWORD_DEFAULT_FILENAME_.pem"

/*
    @brief Commands in section 19 of the AT command manual version 1.09

    Command   | Implemented | Method | Function(s)
    --------- | ----------- | ------ |-----------------
    CSSLCFG   |      y      |        | cconfigSSLSSL
    CCERTDOWN |      y      |        | certDownload
    CCERTLIST |      y      |        | certExists
    CCERTDELE |      y      |        | certDelete
    CCHSET    |             |        |
    CCHMODE   |             |        |
    CCHSTART  |             |        |
    CCHSTOP   |             |        |
    CCHADDR   |             |        |
    CCHSSLCFG |      y      |        | setSSLContext
    CCHCFG    |             |        |
    CCHOPEN   |             |        |
    CCHCLOSE  |             |        |
    CCHSEND   |             |        |
    CCHRECV   |             |        |
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