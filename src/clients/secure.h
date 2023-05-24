#ifndef A76XX_SECURE_CLIENT_H_
#define A76XX_SECURE_CLIENT_H_

//default certificate names
#define A76XX_DEFAULT_CACERT_FILENAME      "_A76XX_CACERT_DEFAULT_FILENAME_.pem"
#define A76XX_DEFAULT_CLIENTCERT_FILENAME  "_A76XX_CLIENTCERT_DEFAULT_FILENAME_.pem"
#define A76XX_DEFAULT_CLIENTKEY_FILENAME   "_A76XX_CLIENTKEY_DEFAULT_FILENAME_.pem"
#define A76XX_DEFAULT_PASSWORD_FILENAME    "_A76XX_PASSWORD_DEFAULT_FILENAME_.pem"

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

        @param [IN] An A76XX modem instance.
    */
    A76XXSecureClient(A76XX& modem);

   /*
        @brief Enable server only authentication with a CA cert previously stored in
            the module's file system. See the command AT+CSSLCFG for additional details.

        @param [IN] cacertname A NULL terminated string containing the name of
            the root CA certificate file to use.
    */
    bool setCaCert(const char* cacertname);

    /*
        @brief Write root CA certificate to the module's file system and enable server
            only SSL/TLS authentication. See the command AT+CSSLCFG for additional details.

        @details You only need to call this function once, to download the certificate 
            file to the module, e.g. with a custom sketch. The function
            A76XXSecureClient::setCaCert can then be used to load a specific certificate
            in subsequent usage. File names must end with ".der" or
            ".pem". If the file exists, it is overwritten.

        @param [IN] cacert A NULL terminated string containing the root CA certificate.
        @param [IN] cacertname A NULL terminated string containing the name of the root
            CA certificate. Default is A76XX_DEFAULT_CACERT_FILENAME.

        @return True on success, false otherwise.
    */
    bool writeCaCert(const char* cacert, 
                     const char* cacertname = A76XX_DEFAULT_CACERT_FILENAME);

   /*
        @brief Enable client only authentication with a client certificate, client key and
            password files previously stored in the module's file system. See the command
            AT+CSSLCFG for additional details.

        @param [IN] clientcertname A NULL terminated string containing the name of
            the root CA certificate file to use.
        @param [IN] clientkeyname A NULL terminated string containing name the client 
            key file.
        @param [IN] passwordname A NULL terminated string containing the password file 
            to decrypt the key.
    */
    bool setClientCertAndKey(const char* clientcertname,
                             const char* clientkeyname,
                             const char* passwordname);

   /*
        @brief Write client certificate, key and password files to the module's file
            system and enable server only SSL/TLS authentication. See the command 
            AT+CSSLCFG for additional details.

        @details You only need to call this function once, to download the certificate, 
            key and password files to the module, e.g. with a custom sketch. The function
            A76XXSecureClient::setClientCertAndKey can then be used to load a specific 
            certificate and key in subsequent usage. File names must end with ".der" or
            ".pem". If the file exists, it is overwritten.

        @param [IN] clientcert A NULL terminated string containing the client certificate.
        @param [IN] clientcertname A NULL terminated string containing the name of
            the root CA certificate file to use. Default is A76XX_DEFAULT_CLIENTCERT_FILENAME.
        @param [IN] clientkey A NULL terminated string containing the client key.
        @param [IN] clientkeyame A NULL terminated string containing name the client 
            key file. Default is A76XX_DEFAULT_CLIENTKEY_FILENAME.
        @param [IN] password A NULL terminated string containing password to decrypt the key.
        @param [IN] passwordname A NULL terminated string containing the password file 
            to decrypt the key. Default is A76XX_DEFAULT_PASSWORD_FILENAME.

        @return True on success, false otherwise.
    */
    bool writeClientCertAndKey(const char* clientcert,
                               const char* clientkey,
                               const char* password,
                               const char* clientcertname = A76XX_DEFAULT_CLIENTCERT_FILENAME,
                               const char* clientkeyname = A76XX_DEFAULT_CLIENTKEY_FILENAME,
                               const char* passwordname = A76XX_DEFAULT_PASSWORD_FILENAME);

    /*
        @brief Enable client and server authentication with certificates previously stored
            in the module. See A76XXSecureClient::writeClientCertAndKey and
            A76XXSecureClient::writeCaCert for saving certificates to the module.

        @param [IN] cacert A NULL terminated string containing the name of the root
             CA certificate file.
        @param [IN] clientcert A NULL terminated string containing the name of the client
             certificate file.
        @param [IN] clientkey A NULL terminated string containing the name of the client
             key file.
        @param [IN] password A NULL terminated string containing the name of password file 
            to decrypt the key.
    */
    bool setCerts(const char* cacertname,
                  const char* clientcertname,
                  const char* clientkeyname,
                  const char* passwordname);

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