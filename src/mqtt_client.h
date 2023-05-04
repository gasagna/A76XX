#ifndef A76XX_MQTT_CLIENT_H_
#define A76XX_MQTT_CLIENT_H_

class A76XXMQTTClient : public A76XXBaseClient {
  private:
    A76XX_MQTT_Commands   _mqtt_cmds;
    const char*            _clientID;
    bool                    _use_ssl;
    uint8_t            _client_index;
    uint8_t              _session_id;

  public:
    A76XXMQTTClient(A76XX& modem,
                    const char* clientID,
                    bool use_ssl = false,
                    uint8_t ssl_ctx_index = 0,
                    uint8_t client_index = 0,
                    uint8_t session_id = 0);

    bool begin();

    bool connect(const char* server, int port,
                 bool clean_session,
                 int keep_alive = 60,
                 const char* username = NULL,
                 const char* password = NULL,
                 const char* will_topic = NULL,
                 const char* will_message = NULL,
                 int will_qos = 0);

    bool disconnect(uint8_t timeout);

    bool end(uint8_t timeout = 120);

    bool publish(const char* topic,
                 const uint8_t* payload,
                 uint32_t length,
                 uint8_t qos,
                 uint8_t pub_timeout,
                 bool retained = false,
                 bool dup = false);

    bool publish(const char* topic,
                 const char* payload,
                 uint8_t qos,
                 uint8_t pub_timeout,
                 bool retained = false,
                 bool dup = false);

    bool isConnected();
};

#endif A76XX_MQTT_CLIENT_H_