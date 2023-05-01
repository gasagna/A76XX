#ifndef A76XX_SSL_CMDS_H_
#define A76XX_SSL_CMDS_H_

#include "modem.h"

//default certificate names
#define A76XX_DEFAULT_CACERT_FILENAME      "_A76XX_CACART_DEFAULT_FILENAME_.pem"
#define A76XX_DEFAULT_CLIENTCERT_FILENAME  "_A76XX_CLIENTCERT_DEFAULT_FILENAME_.pem"
#define A76XX_DEFAULT_CLIENTKEY_FILENAME   "_A76XX_CLIENTKEY_DEFAULT_FILENAME_.pem"
#define A76XX_DEFAULT_PASSWORD_FILENAME    "_A76XX_PASSWORD_DEFAULT_FILENAME_.pem"

#define A76XX_SSL_ALERTING_STATE               1
#define A76XX_SSL_UNKNOWN_ERROR                2
#define A76XX_SSL_BUSY                         3
#define A76XX_SSL_PEER_CLOSED                  4
#define A76XX_SSL_OPERATION_TIMEOUT            5
#define A76XX_SSL_TRANSFER_FAILED              6
#define A76XX_SSL_MEMORY_ERROR                 7
#define A76XX_SSL_INVALID_PARAMETER            8
#define A76XX_SSL_NETWORK_ERROR                9
#define A76XX_SSL_OPEN_SESSION_ERROR          10
#define A76XX_SSL_STATE_ERROR                 11
#define A76XX_SSL_CREATE_SOCKET_ERROR         12
#define A76XX_SSL_GET_DNS_ERROR               13
#define A76XX_SSL_CONNECT_SOCKET_ERROR        14
#define A76XX_SSL_HANDSHAKE_ERROR             15
#define A76XX_SSL_CLOSE_SOCKET_ERROR          16
#define A76XX_SSL_NONET                       17
#define A76XX_SSL_SEND_DATA_TIMEOUT           18
#define A76XX_SSL_NOT_SET_CERTIFICATES        19

class A76XX_SSL_Commands {
  private:
    A76XX&        _modem;
    int8_t     _last_error;

  public:
    // Initialise from modem
    A76XX_SSL_Commands(A76XX& modem)
        : _modem(modem)
        , _last_error(A76XX_OPERATION_SUCCEEDED) {}

    // CSSLCFG sslversion
    int8_t config_ssl_sslversion(uint8_t ssl_ctx_index, uint8_t ssl_version) {
        _modem.sendCMD("AT+CSSLCFG=\"sslversion\",", ssl_ctx_index, ",", ssl_version);
        A76XX_RESPONSE_RETURN(_modem.waitResponse());
    }

    // CSSLCFG authmode
    int8_t config_ssl_authmode(uint8_t ssl_ctx_index, uint8_t authmode) {
        _modem.sendCMD("AT+CSSLCFG=\"authmode\",", ssl_ctx_index, ",", authmode);
        A76XX_RESPONSE_RETURN(_modem.waitResponse());
    }

    // CSSLCFG ignorelocaltime
    int8_t config_ssl_ignorelocaltime(uint8_t ssl_ctx_index, uint8_t ignorelocaltime) {
        _modem.sendCMD("AT+CSSLCFG=\"ignorelocaltime\",", ssl_ctx_index, ",", ignorelocaltime);
        A76XX_RESPONSE_RETURN(_modem.waitResponse());
    }

    // CSSLCFG negotiatetime
    int8_t config_ssl_negotiatetime(uint8_t ssl_ctx_index, uint16_t negotiatetime) {
        _modem.sendCMD("AT+CSSLCFG=\"negotiatetime\",", ssl_ctx_index, ",", negotiatetime);
        A76XX_RESPONSE_RETURN(_modem.waitResponse());
    }

    // CSSLCFG cacert
    int8_t config_ssl_cacert(uint8_t ssl_ctx_index, const char* ca_file) {
        _modem.sendCMD("AT+CSSLCFG=\"cacert\",", ssl_ctx_index, ",", ca_file);
        A76XX_RESPONSE_RETURN(_modem.waitResponse());
    }

    // CSSLCFG clientcert
    int8_t config_ssl_clientcert(uint8_t ssl_ctx_index, const char* clientcert_file) {
        _modem.sendCMD("AT+CSSLCFG=\"clientcert\",", ssl_ctx_index, ",", clientcert_file);
        A76XX_RESPONSE_RETURN(_modem.waitResponse());
    }

    // CSSLCFG clientkey
    int8_t config_ssl_clientkey(uint8_t ssl_ctx_index, const char* clientkey_file) {
        _modem.sendCMD("AT+CSSLCFG=\"clientkey\",", ssl_ctx_index, ",", clientkey_file);
        A76XX_RESPONSE_RETURN(_modem.waitResponse());
    }

    // CSSLCFG password
    int8_t config_ssl_password(uint8_t ssl_ctx_index, const char* password_file) {
        _modem.sendCMD("AT+CSSLCFG=\"password\",", ssl_ctx_index, ",", password_file);
        A76XX_RESPONSE_RETURN(_modem.waitResponse());
    }

    // CSSLCFG enableSNI
    int8_t config_ssl_context(uint8_t ssl_ctx_index, uint8_t enableSNI_flag) {
        _modem.sendCMD("AT+CSSLCFG=\"enableSNI\",", ssl_ctx_index, ",", enableSNI_flag);
        A76XX_RESPONSE_RETURN(_modem.waitResponse());
    }

    // CCERTDOWN
    int8_t cert_download(const char* cert, const char* certname) {
        _modem.sendCMD("AT+CCERTDOWN=\"", certname, "\",", strlen(cert));
        Response_t rsp = _modem.waitResponse(">", 120000);

        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                _modem._serial.write(cert);
                _modem._serial.flush();
                if (_modem.waitResponse() == Response_t::A76XX_RESPONSE_OK) {
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
    bool cert_exists(const char* certname) {
        _modem.sendCMD("AT+CCERTLIST");
        Response_t rsp = _modem.waitResponse(certname, 120000);
        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                _modem._serialClear();
                return true;
            }
            default : {
                return false;
            }
        }
    }

    // CCERTDELE
    int8_t cert_delete(const char* certname) {
        _modem.sendCMD("AT+CCERTDELE=\"", certname, "\"");
        A76XX_RESPONSE_RETURN(_modem.waitResponse());
    }

    // CCHSSLCFG
    int8_t set_ssl_context(uint8_t session_id, uint8_t ssl_ctx_index) {
        _modem.sendCMD("AT+CCHSSLCFG=", session_id, ",", ssl_ctx_index);
        A76XX_RESPONSE_RETURN(_modem.waitResponse());
    }


    ///////////////////////////////////////////////////////////////
    // HIGH LEVEL COMMANDS
    ///////////////////////////////////////////////////////////////

    // delete certificate if exists, then download
    int8_t cert_overwrite(const char* cert, const char* certname) {
        int8_t retcode;

        if (cert_exists(certname)) {
            retcode = cert_delete(certname);
            A76XX_RETCODE_ASSERT_RETURN(retcode);
        }

        retcode = cert_download(cert, certname);
        A76XX_RETCODE_ASSERT_RETURN(retcode);

        return retcode;
    }

    // save certificate `cacert` to the module.
    int8_t setCaCert(uint8_t ssl_ctx_index, const char* cacert) {
        int8_t retcode = cert_overwrite(cacert, A76XX_DEFAULT_CACERT_FILENAME);
        A76XX_RETCODE_ASSERT_RETURN(retcode);

        retcode = config_ssl_cacert(ssl_ctx_index, A76XX_DEFAULT_CACERT_FILENAME);
        A76XX_RETCODE_ASSERT_RETURN(retcode);

        // only server authentication
        retcode = config_ssl_authmode(ssl_ctx_index, 1);
        A76XX_RETCODE_ASSERT_RETURN(retcode);

        return retcode;
    }

    // set client certificate, key and key file password
    int8_t setClientCertAndKey(uint8_t ssl_ctx_index, const char* clientcert, const char* clientkey, const char* password) {
        int8_t retcode = cert_overwrite(clientcert, A76XX_DEFAULT_CLIENTCERT_FILENAME);
        A76XX_RETCODE_ASSERT_RETURN(retcode);

        retcode = config_ssl_clientcert(ssl_ctx_index, A76XX_DEFAULT_CLIENTCERT_FILENAME);
        A76XX_RETCODE_ASSERT_RETURN(retcode);

        retcode = cert_overwrite(clientkey, A76XX_DEFAULT_CLIENTKEY_FILENAME);
        A76XX_RETCODE_ASSERT_RETURN(retcode);

        retcode = config_ssl_clientkey(ssl_ctx_index, A76XX_DEFAULT_CLIENTKEY_FILENAME);
        A76XX_RETCODE_ASSERT_RETURN(retcode);

        retcode = cert_overwrite(password, A76XX_DEFAULT_PASSWORD_FILENAME);
        A76XX_RETCODE_ASSERT_RETURN(retcode);

        retcode = config_ssl_password(ssl_ctx_index, A76XX_DEFAULT_PASSWORD_FILENAME);
        A76XX_RETCODE_ASSERT_RETURN(retcode);

        // only client authentication
        retcode = config_ssl_authmode(ssl_ctx_index, 3);
        A76XX_RETCODE_ASSERT_RETURN(retcode);

        return retcode;
    }

    // set ca and client certificate, key and key file password
    int8_t setCerts(uint8_t ssl_ctx_index, const char* cacert, const char* clientcert, const char* clientkey, const char* password) {
        int8_t retcode = setCaCert(ssl_ctx_index, cacert);
        A76XX_RETCODE_ASSERT_RETURN(retcode);

        retcode = setClientCertAndKey(ssl_ctx_index, clientcert, clientkey, password);
        A76XX_RETCODE_ASSERT_RETURN(retcode);

        // repeat server and client authentication
        retcode = config_ssl_authmode(ssl_ctx_index, 2);
        A76XX_RETCODE_ASSERT_RETURN(retcode);

        return retcode;
    }
};

#endif A76XX_SSL_CMDS_H_