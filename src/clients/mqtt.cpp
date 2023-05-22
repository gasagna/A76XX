#include "A76XX.h"


void MQTTOnMessageRx::process(ModemSerial* serial) {
    MQTTMessage_t msg;

    // skip the client index
    serial->find(','); uint16_t   topic_length = serial->parseInt();
    serial->find(','); uint16_t payload_length = serial->parseInt();

    serial->waitResponse("+CMQTTRXTOPIC: "); serial->find('\n');
    if (topic_length < sizeof(msg.topic)) {
        serial->readBytes(msg.topic, topic_length);
        msg.topic[topic_length] = '\0';
    } else {
        // read what we can't store, then overwrite
        serial->readBytes(msg.topic, topic_length - sizeof(msg.topic) + 1);
        serial->readBytes(msg.topic, sizeof(msg.topic) - 1);
        msg.topic[sizeof(msg.topic)] = '\0';
    }

    serial->waitResponse("+CMQTTRXPAYLOAD: "); serial->find('\n');
    if (payload_length < sizeof(msg.payload)) {
        serial->readBytes(msg.payload, payload_length);
        msg.payload[payload_length] = '\0';
    } else {
        // read what we can't store, then overwrite
        serial->readBytes(msg.payload, payload_length - sizeof(msg.payload) + 1);
        serial->readBytes(msg.payload, sizeof(msg.payload) - 1);
        msg.payload[sizeof(msg.payload)] = '\0';
    }

    serial->waitResponse("+CMQTTRXEND: "); serial->find('\n');

    messageQueue.pushEnd(msg);
}

A76XXMQTTClient::A76XXMQTTClient(A76XX& modem, const char* clientID, bool use_ssl)
    : A76XXSecureClient(modem)
    , _mqtt_cmds(_serial)
    , _clientID(clientID)
    , _use_ssl(use_ssl)
    , _client_index(0)
    , _session_id(0) {
        // enable parsing MQTT URCs
        _serial.registerEventHandler(&_on_message_rx_handler);
        _serial.registerEventHandler(&_on_connection_lost_handler);
        _serial.registerEventHandler(&_on_no_net_handler);
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

uint32_t A76XXMQTTClient::messageAvailable() {
    return _on_message_rx_handler.messageQueue.available();
}

MQTTMessage_t A76XXMQTTClient::getMessage() {
    return _on_message_rx_handler.messageQueue.popFront();
}

bool A76XXMQTTClient::isConnected() {
    return _mqtt_cmds.isConnected(_client_index);
}