#include "A76XX.h"

A76XXMQTTClient::A76XXMQTTClient(A76XX* modem, const char* clientID, bool use_ssl)
    : A76XXSecureClient(modem)
    , _clientID(clientID)
    , _use_ssl(use_ssl)
    , _client_index(0)
    , _session_id(0) {
        _mqtt_cmds._modem = modem;
    }

bool A76XXMQTTClient::begin() {
    // start
    int8_t retcode = _mqtt_cmds.start();
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    // acquire client
    uint8_t server_type = _use_ssl ? 1 : 0;
    retcode = _mqtt_cmds.acquireClient(_client_index, _clientID, server_type);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    // set ssl context for mqtt
    if (_use_ssl) {
        retcode = _mqtt_cmds.setSSLContext(_session_id, _ssl_ctx_index);
        A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
    }

    return true;
}

bool A76XXMQTTClient::connect(const char* server_name, int port,
                              bool clean_session,
                              int keepalive,
                              const char* username,
                              const char* password,
                              const char* will_topic,
                              const char* will_message,
                              int will_qos) {
    int8_t retcode;

    if (will_message != NULL && will_topic != NULL) {
        retcode = _mqtt_cmds.setWillTopic(_client_index, will_topic);
        A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
        retcode = _mqtt_cmds.setWillMessage(_client_index, will_message, will_qos);
        A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
    }

    retcode = _mqtt_cmds.connect(_client_index, server_name, port, clean_session, keepalive, username, password);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    return true;
}

bool A76XXMQTTClient::disconnect(uint8_t timeout) {
    int8_t retcode = _mqtt_cmds.disconnect(_client_index, timeout);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
    return true;
}

bool A76XXMQTTClient::end() {
    int8_t retcode = _mqtt_cmds.releaseClient(_client_index);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    retcode = _mqtt_cmds.stop();
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    return true;
}

bool A76XXMQTTClient::publish(const char* topic,
                              const uint8_t* payload,
                              uint32_t length,
                              uint8_t qos,
                              uint8_t pub_timeout,
                              bool retained,
                              bool dup) {
    int8_t retcode = _mqtt_cmds.setTopic(_client_index, topic);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    retcode = _mqtt_cmds.setPayload(_client_index, payload, length);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    retcode = _mqtt_cmds.publish(_client_index, qos, pub_timeout, retained, dup);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    return true;
}

bool A76XXMQTTClient::publish(const char* topic,
                              const char* payload,
                              uint8_t qos,
                              uint8_t pub_timeout,
                              bool retained,
                              bool dup) {
    return publish(topic, reinterpret_cast<const uint8_t*>(payload), strlen(payload), qos,
            pub_timeout, retained, dup);
}

bool A76XXMQTTClient::subscribe(const char* topic, uint8_t qos) {
    int8_t retcode = _mqtt_cmds.subscribe(_client_index, topic, qos);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
    return true;
}

bool A76XXMQTTClient::mainLoop(int timeout) {
    Response_t rsp = modem->waitResponse("+CMQTTRXSTART: ", "+CMQTTNONET", "+CMQTTCONNLOST");
    switch (rsp) {
        case Response_t::A76XX_RESPONSE_MATCH_1ST : {
            parseMessage();
            this->onMessage(_topic_buf, _pload_buf);
        }
        case Response_t::A76XX_RESPONSE_MATCH_2ND : {

        }
    }
}

bool A76XXMQTTClient::parseMessage() {
    // +CMQTTRXSTART: 0,22,7
    //
    // +CMQTTRXTOPIC: 0,22
    // _this_is_a_test_topic_
    // +CMQTTRXPAYLOAD: 0,7
    // testone
    // +CMQTTRXEND: 0

    // we read the data until "+CMQTTRXSTART: "
    // skip the client index
    modem->streamFind(','); uin16_t topic_length = modem->streamParseInt();
    modem->streamFind(','); uin16_t pload_length = modem->streamParseInt();

    modem->waitResponse("+CMQTTRXTOPIC: "); modem->streamFind('\n');
    if (topic_length < MQTT_TOPIC_BUFF_LEN) {
        modem->streamReadBytes(_topic_buf, topic_length);
        _topic_buf[topic_length] = '\0';
    } else {
        // read what we can't store, then overwrite
        modem->streamReadBytes(_topic_buf, topic_length - MQTT_TOPIC_BUFF_LEN + 1);
        modem->streamReadBytes(_topic_buf, MQTT_TOPIC_BUFF_LEN - 1);
        _topic_buf[MQTT_TOPIC_BUFF_LEN] = '\0';
    }

    modem->waitResponse("+CMQTTRXPAYLOAD: "); modem->streamFind('\n');
    if (pload_length < MQTT_PLOAD_BUFF_LEN) {
        modem->streamReadBytes(_pload_buf, pload_length);
        _pload_buf[pload_length] = '\0';
    } else {
        // read what we can't store, then overwrite
        modem->streamReadBytes(_pload_buf, pload_length - MQTT_PLOAD_BUFF_LEN + 1);
        modem->streamReadBytes(_pload_buf, MQTT_PLOAD_BUFF_LEN - 1);
        _pload_buf[MQTT_PLOAD_BUFF_LEN] = '\0';
    }

    modem->waitResponse("+CMQTTRXEND: "); modem->streamFind('\n');
}


bool A76XXMQTTClient::isConnected() {
    return _mqtt_cmds.isConnected(_client_index);
}