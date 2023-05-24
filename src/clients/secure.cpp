#include "A76XX.h"

A76XXSecureClient::A76XXSecureClient(A76XX& modem)
    : A76XXBaseClient(modem)
    , _ssl_cmds(_serial)
    , _ssl_ctx_index(0) {}

bool A76XXSecureClient::setCaCert(const char* cacertname) {
    int8_t retcode = _ssl_cmds.configSSLCacert(_ssl_ctx_index, cacertname);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    // only server authentication
    retcode = _ssl_cmds.configSSLAuthmode(_ssl_ctx_index, 1);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    return true;
}

bool A76XXSecureClient::writeCaCert(const char* cacert, const char* cacertname) {
    int8_t retcode = _ssl_cmds.certOverwrite(cacert, cacertname);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    return setCaCert(cacertname);
}

bool A76XXSecureClient::setClientCertAndKey(const char* clientcertname,
                                            const char* clientkeyname,
                                            const char* passwordname) {
    int8_t retcode = _ssl_cmds.configSSLClientcert(_ssl_ctx_index, clientcertname);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    retcode = _ssl_cmds.configSSLClientkey(_ssl_ctx_index, clientkeyname);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    retcode = _ssl_cmds.configSSLPassword(_ssl_ctx_index, passwordname);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    // only client authentication
    retcode = _ssl_cmds.configSSLAuthmode(_ssl_ctx_index, 3);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    return true;
}

bool A76XXSecureClient::writeClientCertAndKey(const char* clientcert,
                                              const char* clientkey,
                                              const char* password,
                                              const char* clientcertname,
                                              const char* clientkeyname,
                                              const char* passwordname) {
    int8_t retcode = _ssl_cmds.certOverwrite(clientcert, clientcertname);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    retcode = _ssl_cmds.certOverwrite(clientkey, clientkeyname);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    retcode = _ssl_cmds.certOverwrite(password, passwordname);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    return setClientCertAndKey(clientcertname, clientkeyname, passwordname);
}

bool A76XXSecureClient::setCerts(const char* cacertname,
                                 const char* clientcertname,
                                 const char* clientkeyname,
                                 const char* passwordname) {
    if (setCaCert(cacertname) == false || 
        setClientCertAndKey(clientcertname, clientkeyname, passwordname) == false) {
        return false;
    }
    // repeat server and client authentication
    int8_t retcode = _ssl_cmds.configSSLAuthmode(_ssl_ctx_index, 2);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    return true;
}

bool A76XXSecureClient::configSSL(uint8_t  sslversion,
                                uint8_t  ignorelocaltime,
                                uint16_t negotiatetime) {
    int8_t retcode = _ssl_cmds.configSSLSSLversion(_ssl_ctx_index, sslversion);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    retcode = _ssl_cmds.configSSLIgnorelocaltime(_ssl_ctx_index, ignorelocaltime);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    retcode = _ssl_cmds.configSSLNegotiatetime(_ssl_ctx_index, negotiatetime);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    return true;
}