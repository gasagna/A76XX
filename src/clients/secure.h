#ifndef A76XX_SECURE_CLIENT_H_
#define A76XX_SECURE_CLIENT_H_

/*
    @brief Base SSL/TLS functionality for other protocol clients.
*/
class A76XXSecureClient : public A76XXBaseClient {
  public:
    // handle to all SSL commands
    SSLCommands         _ssl_cmds;

    // context id of the SSL session
    uint8_t        _ssl_ctx_index;

    /*
        @brief Constructor.

        @param [IN] FIXME:
    */
    A76XXSecureClient(A76XX& modem);

    /*
        @brief Write root CA certificate to the module and enable server only
            authentication. See the command AT+CSSLCFG for additional details.

        @param [IN] cacert A NULL terminated string containing the root CA certificate.
    */
    bool setCaCert(const char* cacert);

    /*
        @brief Write client certificate, key and the password to decrypt the key to
            the module and enable client only authentication. See the command AT+CSSLCFG
            for additional details.

        @param [IN] clientcert A NULL terminated string containing the client certificate.
        @param [IN] clientkey A NULL terminated string containing the client key.
        @param [IN] password A NULL terminated string containing password to decrypt the key.
    */
    bool setClientCertAndKey(const char* clientcert, const char* clientkey, const char* password);

    /*
        @brief Write root CA certificate, client certificate, key and the password
            to decrypt the key to the module and enable client and server authentication.
            See the command AT+CSSLCFG for additional details.

        @param [IN] cacert A NULL terminated string containing the root CA certificate.
        @param [IN] clientcert A NULL terminated string containing the client certificate.
        @param [IN] clientkey A NULL terminated string containing the client key.
        @param [IN] password A NULL terminated string containing password to decrypt the key.
    */
    bool setCerts(const char* cacert, const char* clientcert, const char* clientkey, const char* password);

    /*
        @brief Configure SSL authentication parameters. See the command AT+CSSLCFG for
            additional details.

        @param [IN] ssl_version the version of the SSL/TLS protocol. Default is all version.
        @param [IN] ignore_local_time flag to deal with expired certificates. Default is to
            ignore the expiration date.
        @param [IN] negotiate_time timeout value in the SSL negotiation stage. Default is
            300 seconds.
    */
    bool configSSL(uint8_t ssl_version = 4, uint8_t ignore_local_time = 1, uint16_t negotiate_time = 300);
};

#endif A76XX_SECURE_CLIENT_H_