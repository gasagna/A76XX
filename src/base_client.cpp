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
    int8_t retcode = _ssl_cmds.certOverwrite(cacert, A76XX_DEFAULT_CACERT_FILENAME);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    retcode = _ssl_cmds.configSSLCacert(_ssl_ctx_index, A76XX_DEFAULT_CACERT_FILENAME);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    // only server authentication
    retcode = _ssl_cmds.configSSLAuthmode(_ssl_ctx_index, 1);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    return true;
}

bool A76XXBaseClient::setClientCertAndKey(const char* clientcert, const char* clientkey, const char* password) {
    int8_t retcode = _ssl_cmds.certOverwrite(clientcert, A76XX_DEFAULT_CLIENTCERT_FILENAME);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    retcode = _ssl_cmds.configSSLClientcert(_ssl_ctx_index, A76XX_DEFAULT_CLIENTCERT_FILENAME);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    retcode = _ssl_cmds.certOverwrite(clientkey, A76XX_DEFAULT_CLIENTKEY_FILENAME);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    retcode = _ssl_cmds.configSSLClientkey(_ssl_ctx_index, A76XX_DEFAULT_CLIENTKEY_FILENAME);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    retcode = _ssl_cmds.certOverwrite(password, A76XX_DEFAULT_PASSWORD_FILENAME);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    retcode = _ssl_cmds.configSSLPassword(_ssl_ctx_index, A76XX_DEFAULT_PASSWORD_FILENAME);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    // only client authentication
    retcode = _ssl_cmds.configSSLAuthmode(_ssl_ctx_index, 3);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    return true;
}

bool A76XXBaseClient::setCerts(const char* cacert, const char* clientcert, const char* clientkey, const char* password) {
    if (setCaCert(cacert) == false || setClientCertAndKey(clientcert, clientkey, password) == false) {
        return false;
    }
    // repeat server and client authentication
    int8_t retcode = _ssl_cmds.configSSLAuthmode(_ssl_ctx_index, 2);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    return true;
}

bool A76XXBaseClient::configSSL(uint8_t  sslversion,
                                uint8_t  ignorelocaltime,
                                uint16_t negotiatetime) {
    int8_t retcode = _ssl_cmds.configSSLSslversion(_ssl_ctx_index, sslversion);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    retcode = _ssl_cmds.configSSLIgnorelocaltime(_ssl_ctx_index, ignorelocaltime);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    retcode = _ssl_cmds.configSSLNegotiatetime(_ssl_ctx_index, negotiatetime);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    return true;
}