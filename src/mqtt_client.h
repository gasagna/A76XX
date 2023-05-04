#ifndef A76XX_MQTT_CLIENT_H_
#define A76XX_MQTT_CLIENT_H_

#include "modem.h"
#include "mqtt_cmds.h"
#include "ssl_cmds.h"

class A76XXMQTTClient {
  private:
    A76XX_MQTT_Commands   _mqtt_cmds;
    A76XX_SSL_Commands     _ssl_cmds;
    const char*            _clientID;
    bool                    _use_ssl;
    uint8_t           _ssl_ctx_index;
    uint8_t            _client_index;
    uint8_t              _session_id;
    int8_t               _last_error;

  public:
    A76XXMQTTClient(A76XX& modem, const char* clientID, bool use_ssl = false,
                    uint8_t ssl_ctx_index=0, uint8_t client_index=0, uint8_t session_id=0)
        : _mqtt_cmds(modem)
        , _ssl_cmds(modem)
        , _clientID(clientID)
        , _use_ssl(use_ssl)
        , _ssl_ctx_index(ssl_ctx_index)
        , _client_index(client_index)
        , _session_id(session_id)
        , _last_error(0) {}

  public:
    int8_t getLastError() {
        return _last_error;
    }

    // must be called once
    bool begin() {
        // start
        int8_t retcode = _mqtt_cmds.start();
        A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

        // acquire client
        uint8_t server_type = _use_ssl ? 1 : 0;
        retcode = _mqtt_cmds.acquire_client(_client_index, _clientID, server_type);
        A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

        // set ssl context for mqtt
        if (_use_ssl) {
            retcode = _mqtt_cmds.set_SSL_context(_session_id, _ssl_ctx_index);
            A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
        }

        return true;
    }

    bool connect(const char* server, int port,
                 bool clean_session,
                 int keepalive = 60,
                 const char* username = NULL,
                 const char* password = NULL,
                 const char* will_topic = NULL,
                 const char* will_message = NULL,
                 int will_qos = 0) {
        int8_t retcode;

        if (will_message != NULL && will_topic != NULL) {
            retcode = _mqtt_cmds.set_will_topic(_client_index, will_topic);
            A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
            retcode = _mqtt_cmds.set_will_message(_client_index, will_message, will_qos);
            A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
        }

        retcode = _mqtt_cmds.connect(_client_index, server, port, clean_session, keepalive, username, password);
        A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

        return true;
    }

    bool disconnect(uint8_t timeout) {
        int8_t retcode = _mqtt_cmds.disconnect(_client_index, timeout);
        A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
        return true;
    }

    bool end(uint8_t timeout = 120) {
        int8_t retcode = _mqtt_cmds.release_client(_client_index);
        A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

        retcode = _mqtt_cmds.stop();
        A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

        return true;
    }

    bool publish(const char* topic, const uint8_t* payload, uint32_t length, uint8_t qos,
                 uint8_t pub_timeout, bool retained = false, bool dup = false) {
        int8_t retcode = _mqtt_cmds.set_topic(_client_index, topic);
        A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

        retcode = _mqtt_cmds.set_payload(_client_index, payload, length);
        A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

        retcode = _mqtt_cmds.publish(_client_index, qos, pub_timeout, retained, dup);
        A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

        return true;
    }

    bool publish(const char* topic, const char* payload, uint8_t qos,
                 uint8_t pub_timeout, bool retained = false, bool dup = false) {
        return publish(topic, reinterpret_cast<const uint8_t*>(payload), strlen(payload), qos,
                pub_timeout, retained, dup);
    }

    bool isConnected() {
        return _mqtt_cmds.is_connected(_client_index);
    }
};


#endif A76XX_MQTT_CLIENT_H_