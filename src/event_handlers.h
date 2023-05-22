#ifndef A76XX_EVENTHANDLER_H_
#define A76XX_EVENTHANDLER_H_

// forward declaration
class ModemSerial;

enum A76XXURC_t {
    MQTT_MESSAGE_RX,
    MQTT_CONNECTION_LOST,
    MQTT_NO_NET,
    CGEV_PDP_DETACH,
    NONE
};

class EventHandler_t {
  public:
    A76XXURC_t                                urc;
    const char*                      match_string;
    
    EventHandler_t(A76XXURC_t _urc, const char* _match_string)
        : urc(_urc)
        , match_string(_match_string) {}
        
    virtual void process(ModemSerial* serial) = 0;
};

#endif A76XX_EVENTHANDLER_H_