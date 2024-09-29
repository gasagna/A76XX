#ifndef A76XX_GNSS_CLIENT_H_
#define A76XX_GNSS_CLIENT_H_

/*
    @brief The type of NMEA messages.
*/
struct NMEAMessage_t {
    char payload[NMEA_MESSAGE_SIZE];
};

/*
    @brief Handler for NMEA messages.

    @details This object is responsible of detecting, parsing and storing 
        incoming NMEA messages sent by the SIMCOM module. It has one important 
        member, "_nmea_queue", i.e. a CircularBuffer object used to store 
        incoming NMEA if they arrive at a high rate and the user is not parsing
        them appropriately. The size of this buffer is defines by the variable 
        GNSS_NMEA_QUEUE_SIZE. If messages arrive at a faster rate than they 
        are read, older messages are dropped.
*/
class GNSSOnNMEAMessage : public EventHandler_t {
  public:
    CircularBuffer<NMEAMessage_t, GNSS_NMEA_QUEUE_SIZE>&   _nmea_queue;

    /*
        @brief Constructor

        @param [IN] match_string the NMEA string to match
        @param [IN] queue a CircularBuffer for storing NMEA messages
    
    */
    GNSSOnNMEAMessage(const char* match_string, 
        CircularBuffer<NMEAMessage_t, GNSS_NMEA_QUEUE_SIZE>& queue)
        : EventHandler_t(match_string)
        , _nmea_queue(queue) {}
    
    void process(ModemSerial* serial) {
        NMEAMessage_t msg;

        // copy the match string at the beginning of the message to have the full message
        for (uint i = 0; i < strlen(match_string); i++)
            msg.payload[i] = match_string[i];
        
        // keep reading from that point until <CR>, then read <LF> and close the string
        for (uint i = strlen(match_string); i < NMEA_MESSAGE_SIZE; i++) {
            // wait until a char becomes available
            while (serial->available() == 0) {}

            // read and process
            char c = static_cast<char>(serial->read());
            if (c == '\r') {
                serial->read(); 
                msg.payload[i] = '\0';
                break;
            } else {
                msg.payload[i] = c;
            }
        }

        _nmea_queue.push(msg);
    }
};

class A76XXGNSSClient : public A76XXBaseClient {
  private:
    GNSSCommands                                                  _gnss_cmds;

    // use a single queue for all types of messages
    CircularBuffer<NMEAMessage_t, GNSS_NMEA_QUEUE_SIZE>          _nmea_queue;

    // array of handlers
    GNSSOnNMEAMessage                                      _nmea_handlers[6];

  public:
    /*
        @brief
        @param
        @param
        @param
        @param
        @param
    */
    A76XXGNSSClient(A76XX& modem) 
        : A76XXBaseClient(modem)
        , _gnss_cmds(_serial)
        , _nmea_handlers {{"$GP", _nmea_queue},
                          {"$GA", _nmea_queue},
                          {"$GB", _nmea_queue},
                          {"$GN", _nmea_queue},
                          {"$GL", _nmea_queue},
                          {"$BD", _nmea_queue}} {
    }

    bool enableGNSS(GPSStart_t start,
                    uint8_t mode = 3,
                    uint32_t baud_rate = 9600) {
        int8_t retcode = _gnss_cmds.powerControl(true);
        A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

        retcode = _gnss_cmds.setUART3BaudRate(baud_rate);
        A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

        retcode = _gnss_cmds.setSupportMode(mode);
        A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
        
        retcode = _gnss_cmds.start(start);
        A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
        
        return true;
    }

    bool disableGNSS() {
        int8_t retcode = _gnss_cmds.powerControl(false);
        A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
        
        return true;
    }

    bool enableNMEAStream(uint8_t nmea_rate_Hz = 1,
                          uint8_t nmea_mask = A76XX_GNSS_nGGA | A76XX_GNSS_nRMC) {

        int8_t retcode = _gnss_cmds.selectOutputPort(true, true);
        A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

        // register handlers then enable NMEA output
        for (auto& h : _nmea_handlers) {
            _serial.registerEventHandler(&h);
        }

        retcode = _gnss_cmds.enableNMEAOutput(true);
        A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

        // this does not seem to do anything, although it 
        // does not produce errors
        retcode = _gnss_cmds.setNMEASentence(nmea_mask);
        A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

        // this too does not seem to do anything on my boards
        retcode = _gnss_cmds.setNMEARate(nmea_rate_Hz);
        A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

        return true;
    }

    uint32_t nmeaAvailable() {
        return _nmea_queue.size();
    }

    NMEAMessage_t getNMEAMessage() {
        return _nmea_queue.shift();
    }

    bool disableNMEAStream(bool exhaust = true) {
        // stop output
        int8_t retcode = _gnss_cmds.enableNMEAOutput(false);
        A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
        
        // deregister handlers
        for (auto& h : _nmea_handlers) {
            _serial.deRegisterEventHandler(&h);
        }

        // then exhaust the queue if required
        if (exhaust == true) {
            _nmea_queue.clear();
        }

        return true;
    }

    bool getGNSSInfo(GNSSInfo_t &info) {
        int8_t retcode = _gnss_cmds.getGNSSInfo(info);
        A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
        return true;
    }

    bool getGPSInfo(GPSInfo_t &info) {
        int8_t retcode = _gnss_cmds.getGPSInfo(info);
        A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
        return true;
    }
};

#endif A76XX_GNSS_CLIENT_H_
