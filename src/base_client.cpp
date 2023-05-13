#include "A76XX.h"

A76XXBaseClient::A76XXBaseClient(A76XX* modem) 
    : _ssl_ctx_index(0)
    , _last_error_code(0) {
        _ssl_cmds._modem = modem;
    }

int8_t A76XXBaseClient::getLastError() {
    return _last_error_code;
}

bool A76XXBaseClient::setCaCert(const char* cacert) {
    int8_t retcode = _ssl_cmds.cert_overwrite(cacert, A76XX_DEFAULT_CACERT_FILENAME);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    retcode = _ssl_cmds.config_ssl_cacert(_ssl_ctx_index, A76XX_DEFAULT_CACERT_FILENAME);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    // only server authentication
    retcode = _ssl_cmds.config_ssl_authmode(_ssl_ctx_index, 1);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    return true;
}

bool A76XXBaseClient::setClientCertAndKey(const char* clientcert, const char* clientkey, const char* password) {
    int8_t retcode = _ssl_cmds.cert_overwrite(clientcert, A76XX_DEFAULT_CLIENTCERT_FILENAME);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    retcode = _ssl_cmds.config_ssl_clientcert(_ssl_ctx_index, A76XX_DEFAULT_CLIENTCERT_FILENAME);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    retcode = _ssl_cmds.cert_overwrite(clientkey, A76XX_DEFAULT_CLIENTKEY_FILENAME);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    retcode = _ssl_cmds.config_ssl_clientkey(_ssl_ctx_index, A76XX_DEFAULT_CLIENTKEY_FILENAME);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    retcode = _ssl_cmds.cert_overwrite(password, A76XX_DEFAULT_PASSWORD_FILENAME);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    retcode = _ssl_cmds.config_ssl_password(_ssl_ctx_index, A76XX_DEFAULT_PASSWORD_FILENAME);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    // only client authentication
    retcode = _ssl_cmds.config_ssl_authmode(_ssl_ctx_index, 3);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    return true;
}

bool A76XXBaseClient::setCerts(const char* cacert, const char* clientcert, const char* clientkey, const char* password) {
    int8_t retcode = _ssl_cmds.setCaCert(_ssl_ctx_index, cacert);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    retcode = _ssl_cmds.setClientCertAndKey(_ssl_ctx_index, clientcert, clientkey, password);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    // repeat server and client authentication
    retcode = _ssl_cmds.config_ssl_authmode(_ssl_ctx_index, 2);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
    return true;
}

bool A76XXBaseClient::configSSL(uint8_t  sslversion,
                                uint8_t  ignorelocaltime,
                                uint16_t negotiatetime) {
    int8_t retcode = _ssl_cmds.config_ssl_sslversion(_ssl_ctx_index, sslversion);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    retcode = _ssl_cmds.config_ssl_ignorelocaltime(_ssl_ctx_index, ignorelocaltime);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    retcode = _ssl_cmds.config_ssl_negotiatetime(_ssl_ctx_index, negotiatetime);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    return true;
}