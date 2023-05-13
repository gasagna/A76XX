#ifndef A76XXMODEM_H_
#define A76XXMODEM_H_

class A76XX {
  public:
    InternetServiceCommands<A76XX>       internetService;
    NetworkCommands<A76XX>                       network;
    PacketDomainCommands<A76XX>             packetDomain;
    SerialInterfaceCommands<A76XX>       serialInterface;
    SIMCommands<A76XX>                               sim;
    StatusControlCommands<A76XX>           statusControl;
    V25TERCommands<A76XX>                         v25ter;
    int8_t                              _last_error_code;

  private:
    Stream&                                             _stream;

  public:
    /*
        @brief Construct the modem instance.
        @param [IN] serial Serial interface to the SIMCOM module. Must be
            initialised separately with `begin` and the appropriate TX/RX pins.
    */
    A76XX(Stream& stream);

    /*
        @brief Wait for the serial communication with the SIMCOM module to
            become available and initialise the modem. Currently, only sims
            with no pin are supported.
    */
    bool init();

    //////////////////////////////////////////////////////////////////////////////////////
    // Connection functions
    //////////////////////////////////////////////////////////////////////////////////////

    /*
        @brief Connect to network via apn
    */
    bool connect(const char* apn, const char* username = NULL, const char* password = NULL);

    // disconnect - this does not seem to work
    bool disconnect();

    // check context has been activated
    bool isConnected();

    //////////////////////////////////////////////////////////////////////////////////////
    // Network functions
    //////////////////////////////////////////////////////////////////////////////////////

    /// @brief CREG - Get registration status
    /// @return -1 on error
    int8_t getRegistrationStatus();

    // check we are registered on network or roaming
    bool isRegistered();

    bool waitForRegistration(uint32_t timeout = 60000);

    /// @brief CNSMOD - Show network system mode
    /// @return -1 on error
    int8_t getNetworkSystemMode();

    //////////////////////////////////////////////////////////////////////////////////////
    // Power functions
    //////////////////////////////////////////////////////////////////////////////////////

    // CRESET
    bool reset();

    // CPOF
    bool powerOff();

    // CFUN - radio off/on
    bool setPhoneFunctionality(uint8_t fun, bool reset = false);

    bool radioOFF();

    bool radioON();

    // restart
    bool restart();

    // enable UART sleep
    bool sleep();

    // in mode 2 wake up the module by sending data through the serial port
    bool wakeUp();

    //////////////////////////////////////////////////////////////////////////////////////
    // Modem version functions
    //////////////////////////////////////////////////////////////////////////////////////
    String modelIdentification();

    String revisionIdentification();

    //////////////////////////////////////////////////////////////////////////////////////
    /// Time commands
    //////////////////////////////////////////////////////////////////////////////////////
    /*
    */
    bool syncTime(const char* host = "pool.ntp.org", int8_t timezone = 0);

    /*
        @brief Get date and time in the format "yy/MM/dd,hh:mm:ss±zz"
    */
    String getDateTime();

    /*
        @brief Get date and time.
    */
    bool getDateTime(int* year,
                     int* month,
                     int* day,
                     int* hour,
                     int* minute,
                     int* second,
                     int* timezone);

    /*
        @brief Get unix time.
    */
    uint32_t getUnixTime(bool UTC = true);

    //////////////////////////////////////////////////////////////////////////////////////
    /// Serial communication
    //////////////////////////////////////////////////////////////////////////////////////

    /*
        @brief Wait for modem to respond.

        @detail Consume data from the serial port until a match is found with one of the 
            two input string or before the operation times out. If the parameters
            `match_OK` and `match_ERROR` are true, we also check if the response
            matches with the default OK and ERROR strings, but precedence is given
            to matching the input strings, which is useful for some commands.
        @param [IN] match_1 The first string to match.
        @param [IN] match_2 The second string to match.
        @param [IN] timeout Return if no match is found within this time in milliseconds.
            Default is 1000 milliseconds.
        @param [IN] match_OK Whether to match the string "OK\r\n". Default is true.
        @param [IN] match_ERROR Whether to match the string "ERROR\r\n". Default is true.

        @return A Response_t object.
    */
    Response_t waitResponse(const char* match_1,
                            const char* match_2,
                            int timeout = 1000,
                            bool match_OK = true,
                            bool match_ERROR = true);

    /*
        @brief Wait for modem to respond.

        @detail Consume data from the serial port until a match is found with the 
            input string or before the operation times out. If the parameters
            `match_OK` and `match_ERROR` are true, we also check if the response
            matches with the default OK and ERROR strings, but precedence is given
            to matching the input string, which is useful for some commands.
        @param [IN] match_1 The string to match.
        @param [IN] timeout Return if no match is found within this time in milliseconds.
            Default is 1000 milliseconds.
        @param [IN] match_OK Whether to match the string "OK\r\n". Default is true.
        @param [IN] match_ERROR Whether to match the string "ERROR\r\n". Default is true.

        @return A Response_t object.
    */
    Response_t waitResponse(const char* match_1,
                            int timeout = 1000,
                            bool match_OK = true,
                            bool match_ERROR = true);

    /*
        @brief Wait for modem to respond.

        @detail Consume data from the serial port until a match with the default 
            OK and ERROR strings, if the parameters `match_OK` and `match_ERROR` are true.
        @param [IN] timeout Return if no match is found within this time in milliseconds.
            Default is 1000 milliseconds.
        @param [IN] match_OK Whether to match the string "OK\r\n". Default is true.
        @param [IN] match_ERROR Whether to match the string "ERROR\r\n". Default is true.

        @return A Response_t object.
    */
    Response_t waitResponse(int timeout = 1000,
                            bool match_OK = true,
                            bool match_ERROR = true);

    /*
        @brief Send data to the command to the module.
        @param [IN] args Items (string, numbers, ...) to be sent.
        @param [IN] finish Whether to send a terminating "\r\n" to the module.
    */
    template <typename... ARGS>
    void sendCMD(ARGS... args) {
        printCMD(args..., "\r\n");
    }

    // print any number of commands to the module serial interface
    template <typename HEAD, typename... TAIL>
    void printCMD(HEAD head, TAIL... tail) {
        _stream.print(head); 
        streamFlush();
        printCMD(tail...);
    }

    // single argument case
    template <typename ARG>
    void printCMD(ARG arg) {
        _stream.print(arg); 
        streamFlush();
    }

    /*
        @brief Parse an integer number and then consume all data available in the 
            serial interface until the default OK or ERROR strings are found, or 
            until the operation times out.
        @param [IN] timeout Time out in milliseconds. Default is 50 milliseconds.
        @return The integer.
    */
    int32_t streamParseIntClear(uint32_t timeout = 500);

    /*
        @brief Consume all data available in the stream, until the default
            OK or ERROR strings are found, or until the operation times out.
        @param [IN] timeout Time out in milliseconds. Default is 50 milliseconds.
    */ 
    void streamClear(uint32_t timeout = 500);

    // forward calls to underlying stream object
    int streamAvailable();

    long streamParseInt();

    void streamFlush();

    uint16_t streamRead();

    template <typename ARG>
    bool streamFind(ARG arg) { 
        return _stream.find(arg); 
    }

    template <typename... ARGS>
    uint16_t streamWrite(ARGS... args) { 
        return _stream.write(args...); 
    }

    template <typename... ARGS>
    uint16_t streamReadBytesUntil(ARGS... args) { 
        return _stream.readBytesUntil(args...); 
    }

    template <typename... ARGS>
    uint16_t streamReadBytes(ARGS... args) { 
        return _stream.readBytes(args...); 
    }
};

#endif A76XXMODEM_H_