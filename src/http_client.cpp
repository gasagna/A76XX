#include "A76XX.h"

A76XXHTTPClient::A76XXHTTPClient(A76XX& modem,
                                 const char* server_name,
                                 uint16_t server_port,
                                 bool use_ssl,
                                 const char* user_agent)
    : A76XXBaseClient(modem)
    , _http_cmds(modem)
    , _use_ssl(use_ssl)
    , _server_name(server_name)
    , _server_port(server_port)
    , _user_agent(user_agent) { }

bool A76XXHTTPClient::begin() {
    int8_t retcode = _http_cmds.init();
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
    return true;
}

bool A76XXHTTPClient::end() {
    int8_t retcode = _http_cmds.term();
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
    return true;
}

bool A76XXHTTPClient::addHeader(const char* header, const char* value) {
    int8_t retcode = _http_cmds.config_http_user_data(header, value);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
    return true;
}

uint16_t A76XXHTTPClient::getResponseStatusCode() {
    return _last_status_code;
}

uint32_t A76XXHTTPClient::getResponseBodyLength() {
    return _last_body_length;
}

bool A76XXHTTPClient::getResponseHeader(String& header) {
    int8_t retcode = _http_cmds.read_header(header);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
    return true;
}

bool A76XXHTTPClient::getResponseBody(String& body) {
    int8_t retcode = _http_cmds.read_response_body(body, _last_body_length);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
    return true;
}

bool A76XXHTTPClient::request(uint8_t method,
                              const char* path,
                              const char* content_body,
                              const char* content_type,
                              const char* accept) {
    int8_t retcode;
    
    // set url
    retcode = _http_cmds.config_http_url(_server_name, _server_port, path, _use_ssl);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    // set user agent
    if (_user_agent != NULL) {
        addHeader("User-Agent", _user_agent);
    }

    // set Accept: header
    if (accept != NULL) {
        retcode = _http_cmds.config_http_accept(accept);
        A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
    }

    // set Content-Type: header
    if (content_type != NULL) {
        retcode = _http_cmds.config_http_content_type(content_type);
        A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
    }

    // write request body
    if (content_body != NULL) {
        retcode = _http_cmds.input_data(content_body, strlen(content_body));
        A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
    }

    // execute request and get status code and content length
    retcode = _http_cmds.action(method, &_last_status_code, &_last_body_length); 
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    return true;
}