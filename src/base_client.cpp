#include "A76XX.h"

A76XXBaseClient::A76XXBaseClient(A76XX& modem) 
    : _ssl_cmds(modem)
    , _ssl_ctx_index(0)
    , _last_error_code(0) {}

int8_t A76XXBaseClient::getLastError() {
    return _last_error_code;
}

bool A76XXBaseClient::setCaCert(const char* cacert) {
    int8_t retcode = _ssl_cmds.setCaCert(_ssl_ctx_index, cacert);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
    return true;
}

bool A76XXBaseClient::setClientCertAndKey(const char* clientcert, const char* clientkey, const char* password) {
    int8_t retcode = _ssl_cmds.setClientCertAndKey(_ssl_ctx_index, clientcert, clientkey, password);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
    return true;
}

bool A76XXBaseClient::setCerts(const char* cacert, const char* clientcert, const char* clientkey, const char* password) {
    int8_t retcode = _ssl_cmds.setCerts(_ssl_ctx_index, cacert, clientcert, clientkey, password);
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