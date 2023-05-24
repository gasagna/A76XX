#ifndef A76XX_HTTP_CLIENT_H_
#define A76XX_HTTP_CLIENT_H_

/*
    @brief Handler of the URC "+HTTP_PEER_CLOSED".

    @details This is a stateless URC handler. It produces the 
        URC A76XXURC_t::HTTP_PEER_CLOSED when A76XX::listen is called.
*/
class HTTPOnPeerClosed : public EventHandler_t {
  public:
    HTTPOnPeerClosed()
        : EventHandler_t(A76XXURC_t::HTTP_PEER_CLOSED, "+HTTP_PEER_CLOSED: ") {}
    
    void process(ModemSerial* serial) {}
};

/*
    @brief Handler of the URC "+HTTP_NONET_EVENT".

    @details This is a stateless URC handler. It produces the 
        URC A76XXURC_t::HTTP_NO_NET when A76XX::listen is called.
*/
class HTTPOnNoNet : public EventHandler_t {
  public:
    HTTPOnNoNet()
        : EventHandler_t(A76XXURC_t::HTTP_NO_NET, "+HTTP_NONET_EVENT: ") {}
    
    void process(ModemSerial* serial) {}
};
 

class A76XXHTTPClient : public A76XXSecureClient {
  private:
    HTTPCommands              _http_cmds;
    bool                        _use_ssl;
    const char*             _server_name;
    uint16_t                _server_port;
    const char*              _user_agent;
    uint32_t           _last_body_length;
    uint16_t           _last_status_code;

  public:
    /*
        @brief Construct an HTTP client.

        @param [IN] An A76XX modem instance.
        @param [IN] server_name The domain name of the HTTP server to connect to. For instance,
            "https://www.bbc.co.uk" or "www.google.com". When the domain name does not
            start with either "http://" or "https://" the flag `use_ssl` determines whether
            secure or unsecure connections are made.
        @param [IN] server_port The port to connect to.
        @param [IN] use_ssl Whether to enable SSL/TLS encrypted connections. If using
            encryption, appropriate certificates must be set to the module. Default is
            false, i.e. use unsecure connections.
        @param [IN] user_agent If provided, it is the value of the "User-Agent" header.
    */
    A76XXHTTPClient(A76XX& modem,
                    const char* server_name,
                    uint16_t server_port,
                    bool use_ssl = false,
                    const char* user_agent = NULL);

  public:

    /*
        @brief Start the HTTP service

        @detail This function must be called before any call to request functions.
        @return True on success. If false, use getLastError() to get detail on the error.
    */
    bool begin();

    /*
        @brief Stop the HTTP service

        @return True on success. If false, use getLastError() to get detail on the error.
    */
    bool end();

    /*
        @brief Reset the request header to its default state. By default the header
            "Host:SERVERNAME" is sent, where "SERVERNAME" is the server address passed
            to the class constructor. If the `user_agent` parameter is passed to the
            constructor, the "User-Agent" header is also included by default.
    */
    void resetHeader();

    /*
        @brief Add a custom header to the http request. This function can be called
            repeatedly to add multiple headers, before the request is made. Note that the
            "Host" header is set by default, unless `sendHostHeader` is set to false in
            the class constructor. The headers "Content-Type" and "Accept" can be set at
            the call site of the HTTP request function.

        @param [IN] header The header string, e.g. "Content-Encoding" for "Content-Encoding: gzip".
        @param [IN] value The value string, e.g. "gzip" for "Content-Encoding: gzip".
        @return True if the resulting total header size is not greater than the 256 character
            limit of the SIMCOM firmware API, false otherwise. If false, the original header is not
            modified.
    */
    bool addHeader(const char* header, const char* value);

    /*
        @brief Add basic credentials for authenticating the client.

        @details You should not use this authentication method for unsecure connections. See
            https://developer.mozilla.org/en-US/docs/Web/HTTP/Authentication for details.
        @param [IN] username The username for the authentication. Maximum length is 32 characters.
        @param [IN] password The password for the authentication. Maximum length is 32 characters.
    */
    bool addBasicAuthentication(const char* username, const char* password);

    /*
        @brief Execute a GET request.

        @param [IN] path The path to the resource, EXCLUDING the leading "/".
        @param [IN] accept The value of the "Accept" header. If NULL, it defaults to "*\/*".
        @return True on a successful operation. If false, use getLastError() to get
            details on the error.
    */
    bool get(const char* path, const char* accept = NULL) {
        return request(0, path, NULL, NULL, accept);
    }

    /*
        @brief Execute a POST request.

        @param [IN] path The path to the resource, EXCLUDING the leading "/".
        @param [IN] content_body The body of the post request.
        @param [IN] content_type The value of the "Content-Type" header. If NULL, it
            defaults to "text/plain".
        @param [IN] accept The value of the "Accept" header. If NULL, it defaults to "*\/*".
        @return True on a successful operation. If false, use getLastError() to get
            details on the error.
    */
    bool post(const char* path,
              const char* content_body,
              const char* content_type = NULL,
              const char* accept = NULL) {
        return request(1, path, content_body, content_type, accept);
    }

    /*
        @brief Return the status code of the last request. If the request
            was unsuccessful, the result of this function is undetermined.

        @return The status code of the last request, e.g. 404.
    */
    uint16_t getResponseStatusCode();

    /*
        @brief Return the decimal length in bytes of the response body of the
            last request. If the request was unsuccessful, the result of this
            function is undetermined.

        @return The length in bytes of the response body of the last request.
    */
    uint32_t getResponseBodyLength();

    /*
        @brief Get response header of the last successful request.

        @param [IN] header Read the header into this string.
        @return True if the header is successfully read.
    */
    bool getResponseHeader(String& header);

    /*
        @brief Get response body of the last successful request.

        @param [IN] header Read the body into this string.
        @return True if the body is successfully read, false if the string
            reserve operation failed or the read.
    */
    bool getResponseBody(String& body);

  private:
    /*
        @brief Private request function used to unify all other types of requests

        @param [IN] method The type of the request: 0 is "GET", 1 is "POST", 2 is "HEAD",
            3 is "DELETE",  4 is "PUT".
        @param [IN] path The path to the resource
        @param [IN] content_body The body content, can be NULL
        @param [IN] content_type The value of the "Content-Type" header. If NULL, it
            defaults to "text/plain".
        @param [IN] accept The value of the "Accept" header. If NULL, it defaults to "*\/*".
    */
    bool request(uint8_t method,
                 const char* path,
                 const char* content_body,
                 const char* content_type,
                 const char* accept);
};

#endif A76XX_HTTP_CLIENT_H_
