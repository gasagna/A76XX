#include "A76XX.h"

A76XXHTTPClient::A76XXHTTPClient(A76XX& modem,
                                 const char* server_name,
                                 uint16_t server_port,
                                 bool use_ssl,
                                 const char* user_agent,
                                 bool send_host_header)
    : A76XXBaseClient(modem)
    , _http_cmds(modem)
    , _use_ssl(use_ssl)
    , _server_name(server_name)
    , _server_port(server_port) 
    , _send_host_header(send_host_header)
    , _user_agent(user_agent) {
        resetHeader();
    }

A76XXHTTPClient::A76XXHTTPClient(A76XX& modem,
                                 IPAddress server_address,
                                 uint16_t server_port,
                                 bool use_ssl,
                                 const char* user_agent)
    : A76XXBaseClient(modem)
    , _http_cmds(modem)
    , _use_ssl(use_ssl)
    , _server_name(NULL)
    , _server_address(server_address)
    , _server_port(server_port) 
    , _send_host_header(false)
    , _user_agent(user_agent) {
        resetHeader();
    }

bool A76XXHTTPClient::begin() {
    int8_t retcode = _http_cmds.init();
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
    return true;
}

void A76XXHTTPClient::resetHeader() {
    _header = "";
    _header.reserve(256); 
    if (_send_host_header == true) {
        _header += "Host:";
        // strip http:// or https:// in the Host header value to avoid 400 Bad Request
        const char* hostname = _server_name;
        if (strcmp(_server_name, "https://") == 0) {
            hostname = _server_name + 8;
        } 
        if (strcmp(_server_name, "http://") == 0) {
            hostname = _server_name + 7;
        } 
        _header += hostname;
        _header += "\n";
    }
   if (_user_agent != NULL) {
        _header += "User-Agent:";
        _header += _user_agent;
        _header += "\n";
    }
}

bool A76XXHTTPClient::addHeader(const char* header, const char* value) {
    if (_header.length() + strlen(header) + strlen(value) + 2 > 256)
        return false;
    _header += header;
    _header += ":";
    _header += value;
    _header += "\n";
    return true;
}

bool A76XXHTTPClient::post(const char* path,
                           const char* content_type,
                           const char* content_body,
                           const char* accept) {
    int8_t retcode;
    
    // set url with either the domain name or the IP address
    if (_server_name != NULL) {
        retcode = _http_cmds.config_http_url(_server_name, _server_port, path, _use_ssl);
    } else {
        // retcode = _http_cmds.config_http_url(_server_address.toString().c_str(), _server_port, path);
    }
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    // set accept type header if it's not the default
    if (strcmp(accept, "*/*") != 0) {
        retcode = _http_cmds.config_http_accept(accept);
        A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
    }

    // set content type
    retcode = _http_cmds.config_http_content_type(content_type);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    // set user header
    if (_header.length() > 0) {
        retcode = _http_cmds.config_http_user_data(_header.c_str());
        A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
    }

    // execute request and get status code and content length
    retcode = _http_cmds.POST(&_last_status_code, &_last_body_length); 
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    return true;
}

bool A76XXHTTPClient::get(const char* path, const char* accept) {
    int8_t retcode;
    
    // set url with either the domain name or the IP address
    if (_server_name != NULL) {
        retcode = _http_cmds.config_http_url(_server_name, _server_port, path, _use_ssl);
    } else {
        // retcode = _http_cmds.config_http_url(_server_address.toString().c_str(), _server_port, path);
    }
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    // set accept type header if it's not the default
    if (strcmp(accept, "*/*") != 0) {
        retcode = _http_cmds.config_http_accept(accept);
        A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
    }

    // set user header
    if (_header.length() > 0) {
        retcode = _http_cmds.config_http_user_data(_header.c_str());
        A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
    }

    // execute request and get status code and content length
    retcode = _http_cmds.GET(&_last_status_code, &_last_body_length); 
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