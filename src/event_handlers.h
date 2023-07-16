#ifndef A76XX_EVENTHANDLER_H_
#define A76XX_EVENTHANDLER_H_

// forward declaration
class ModemSerial;

/*
    @brief Base class for URC event handlers.

    @details SIMCOM modules produce certain unsolicited result codes (URC)
        that signal certain events have occurred, e.g., when the device 
        disconnects from the network or an MQTT message is received. 
        These codes are emitted on the serial port that connects the 
        module to the micro-controller and thus need to be captured
        as they occur, to avoid them being lost in the output of 
        other AT commands that might be issued to the module by the user.

        An EventHandler_t object is composed of a match string, a corresponding
        element of the enum A76XXURC_t, i.e. an URC code, and a processing function.

        When communicating with the module, the output of the serial connection
        is monitored. When the match string of any of the active event handler is 
        found in the data stream the processing function is executed. This can be 
        useful to further information about some URCs that also contain additional
        characters, e.g. the payload of a MQTT message. Then the URC code of the 
        event is pushed to a queue for later processing. This queue can be accessed
        by calling A76XX::listen and events can be processed by the user's application.
*/
class EventHandler_t {
  public:
    /*
        The URC string produces by the module that we attempt to match.
    */
    const char* match_string;
    
    /*
        Construct from the URC code and a match string.
    */
    EventHandler_t(const char* _match_string)
        : match_string(_match_string) {}
    
    /*
        Function to be executed as soon as the match string is found in the 
        stream of characters from the serial connection. Can be a no-op if no
        immediate action is required.
    */
    virtual void process(ModemSerial* serial) = 0;
};

#endif A76XX_EVENTHANDLER_H_