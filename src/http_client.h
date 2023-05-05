#ifndef A76XX_HTTP_CLIENT_H_
#define A76XX_HTTP_CLIENT_H_

class A76XXHTTPClient : public A76XXBaseClient {
  private:
    A76XX_HTTP_Commands   _http_cmds;
    bool                    _use_ssl;
    const char*         _server_name;
    IPAddress        _server_address;
    uint16_t            _server_port;
    bool           _send_host_header;
    const char*          _user_agent;
    String                   _header;
    uint32_t       _last_body_length;
    uint16_t       _last_status_code;

  public:
    /*

    @param server_name The domain name of the HTTP server to connect to.

    */
    A76XXHTTPClient(A76XX& modem,
                    const char* server_name,
                    uint16_t server_port,
                    bool use_ssl = false,
                    const char* user_agent = NULL,
                    bool send_host_header = true);

    A76XXHTTPClient(A76XX& modem,
                    IPAddress server_address,
                    uint16_t server_port,
                    bool use_ssl = false,
                    const char* user_agent = NULL);

  public:

    /*
        @brief Reset the request header to its default state. By default the header
            "Host:SERVERNAME" is sent, where "SERVERNAME" is the server address passed
            to the class constructor.
    */
    void resetHeader();

    /*
        @brief Add a custom header to the http request. This function can be called
            repeatedly to add multiple headers, before the request is made. Note that the
            "Host" header is set by default, unless `sendHostHeader` is set to false in
            the class constructor. The headers "Content-Type" and "Accept" can be set at
            the call site of the HTTP request function.

        @param header The header string, e.g. "Content-Encoding" for "Content-Encoding: gzip".
        @param value The value string, e.g. "gzip" for "Content-Encoding: gzip".
        @return True if the resulting total header size is not greater than the 256 character
            limit of the SIMCOM firmware API, false otherwise. If false, the original header is not
            modified.
    */
    bool addHeader(const char* header, const char* value);

    /*
        @brief Execute a POST request.

        @param path The path to the resource, EXCLUDING the leading "/".
        @param content_type The value of the "Content-Type" header.
        @param content_body The body of the post request.
        @param accept The value of the "Accept" header. By default it is "*\/*".
        @return True on a successful operation. If false, use getLastError() to get
            details on the error.
    */
    bool post(const char* path, const char* content_type, const char* content_body, const char* accept = "*/*");


    bool get(const char* path, const char* accept = "*/*");

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

        @param header Read the header into this string.
        @return True if the header is successfully read.
    */
    bool getResponseHeader(String& header);

    /*
        @brief Get response body of the last successful request.

        @param header Read the body into this string.
        @return True if the body is successfully read, false if the string
            reserve operation failed or the read.
    */
    bool getResponseBody(String& body);
};

#endif A76XX_HTTP_CLIENT_H_
