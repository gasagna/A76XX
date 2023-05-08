#ifndef A76XXMODEM_H_
#define A76XXMODEM_H_

class A76XX {
  public:
    Stream& _serial;

  public:
    /*
        @brief Construct the modem instance.
        @param [IN] serial Serial interface to the SIMCOM module. Must be
            initialised separately with `begin` and the appropriate TX/RX pins.
    */
    A76XX(Stream& serial)

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
    bool connect(const char* apn, const char* username, const char* password);

    // disconnect - this does not seem to work
    bool disconnect();

    // check context has been activated
    bool isConnected();

    //////////////////////////////////////////////////////////////////////////////////////
    //Nnetwork functions
    //////////////////////////////////////////////////////////////////////////////////////

    /// @brief CREG - Get registration status
    /// @return -1 on error
    int getRegistrationStatus();

    // check we are registered on network or roaming
    bool isRegistered();

    bool waitForRegistration(uint32_t timeout = 60000);

    /// @brief CNSMOD - Show network system mode
    /// @return -1 on error
    int getNetworkSystemMode();

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
    // 0 Operation succeeded
    // 1 Unknown error
    // 2 Wrong parameter
    // 3 Wrong date and time calculated
    // 4 Network error
    // 5 Time zone error
    // 6 Time out error
    int syncTime(const char* host = "pool.ntp.org", int8_t timezone = 0);

    bool readTime(int* year, int* month, int* day, int* hour, int* minute, int* second, int* timezone);

    /*
        @brief Get unix time.
    */
    bool getUnixTime(uint32_t* timestamp, bool UTC = true);

    // "yy/MM/dd,hh:mm:ss±zz"
    bool getDateTime(String& date_time);

    //////////////////////////////////////////////////////////////////////////////////////
    // consume the data from the serial port until the string
    // `match` is found or before `timeout` ms have elapsed
    // if match_OK and match_ERROR are true, we also check if
    // the response matches with the OK and ERROR strings, but
    // precedence is given to `match`, which might be helpful
    // with some commands

    // match zero arguments - only match OK and ERROR
    Response_t waitResponse(int timeout=1000,
                            bool match_OK=true,
                            bool match_ERROR=true);

    // match one argument plus OK and ERROR
    Response_t waitResponse(const char* match_1,
                            int timeout=1000,
                            bool match_OK=true,
                            bool match_ERROR=true);

    // match two arguments plus OK and ERROR
    Response_t waitResponse(const char* match_1,
                            const char* match_2,
                            int timeout=1000,
                            bool match_OK=true,
                            bool match_ERROR=true);

    // send command
    template <typename... ARGS>
    void sendCMD(ARGS... args) {
        serialPrint(args..., "\r\n");
        _serial.flush();
    }

    // write to stream
    template <typename HEAD, typename... TAIL>
    void serialPrint(HEAD head, TAIL... tail) {
        _serial.print(head);
        serialPrint(tail...);
    }

    // write to stream
    template <typename ARG>
    void serialPrint(ARG arg) {
        _serial.print(arg);
    }

    // parse an int and then consume all data available in the stream
    int serialParseIntClear();

    // consume all data available in the stream
    void serialClear();
};

#endif A76XXMODEM_H_