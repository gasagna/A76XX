#ifndef A76XX_BASE_CLIENT_H_
#define A76XX_BASE_CLIENT_H_

/*
    @brief Base SSL/TLS functionality for other protocol clients and
    error handling functionality.
*/
class A76XXBaseClient {
  protected:
    // handle to all SSL commands
    A76XX_SSL_Commands     _ssl_cmds;

    // context id of the SSL session
    uint8_t           _ssl_ctx_index;

    // last error returned by any AT command
    int8_t          _last_error_code;

  public:
    /*
        @brief Constructor.

        @param [IN] modem An instance of the A76XX modem class.
    */
    A76XXBaseClient(A76XX& modem);

    /*
        @brief Get last error raise by any AT command.
    */
    int8_t getLastError();

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

#endif A76XX_BASE_CLIENT_H_