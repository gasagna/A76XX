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
    Stream&                                      _stream;

  public:
    /*
        @brief Construct an instance of the modem.

        @param [IN] serial Serial interface to the SIMCOM module. Must be
            initialised separately with `begin` and the appropriate TX/RX pins.
    */
    A76XX(Stream& stream);

    /*
        @brief Wait for the serial communication with the SIMCOM module to
            become available and initialise the modem. 
            
        @detail Currently, only sims with no PIN code are supported.

        @param [IN] timeout Give up if the module cannot be initialised within
            this time in millisecond. Default is 30000 ms.

        @return True is the device was successfully initialised within the 
            specified time. Returns false is the module does not become 
            responsive within `timeout`, if the SIM requires a PIN code or
            if any other commands run during the initialisation are unsuccessful.
    */
    bool init(uint32_t timeout = 30000);

    /*
        @brief Connect to GPRS network.
        
        @param [IN] apn The Access Point Name.
        @return True is connection was successful.
    */
    bool GPRSConnect(const char* apn);

    /*
        @brief Check status of connection to GPRS network.
        
        @return True if connection is active.
    */
    bool isGPRSConnected();

    /*
        @brief Get registration status.
        @detail Check https://stackoverflow.com/questions/71194343/creg-cgreg-cereg-at-commands
            for detail on the properties of different networks. For data applications,
            checking connection to the GPRS network makes most sense.
        
        @param [IN] net The type of network: 0 for GSM, 1 for GPRS data (default), 2 
            for LTE data.
        @return Returns a registration code, from 0 to 11 (see AT command manual for 
            details). This function returns -1 if any errors have occurred when 
            executing the relevant AT command. 
    */
    int8_t getRegistrationStatus(uint8_t net = 1);

    /*
        @brief Check the device is registered to the network.

        @param [IN] net The type of network: 0 for GSM, 1 for GPRS data (default), 2 
            for LTE data. See also ::getRegistrationStatus.
        @return True if the device is registered to the home network or roaming.
    */ 
    bool isRegistered(uint8_t net = 1);

    /*
        @brief Wait until device registers to the network.

        @param [IN] net The type of network: 0 for GSM, 1 for GPRS data (default), 2 
            for LTE data. See also ::getRegistrationStatus.
        @params [IN] timeout Return false if the device cannot register to the network
            within this time in ms.
        @return True if the device has registered to the network within the specified 
            timeout.
    */ 
    bool waitForRegistration(uint8_t net = 1, uint32_t timeout = 60000);

    /*
        @brief Get network system mode (AT+CNSMOD - READ)

        @return The <stat> code, from 0 to 8, or -1 if an error occurs.
    */ 
    int8_t getNetworkSystemMode();

    /*
        @brief Reset the module (AT+CRESET).

        @detail After reset, we wait for the module to become unresponsive to AT commands
            and then become responsive again, for a time give by the `timeout` 
            argument (in ms) for each of these two steps. This can be useful to block 
            execution, so commands sent after the reset are likely to be received by
            the device.

        @params [IN] timeout Time in milliseconds to wait for the device to become
            unresponsive and responsive again. Default is 30000 ms.
        @return True if the module has been successfully reset, satisfying the two 
            timeouts.
    */ 
    bool reset(uint32_t timeout = 30000);

    /*
        @brief Power OFF the module (AT+CPOF).

        @detail After sending the power off command, we wait `timeout` milliseconds 
            for the device to become unresponsive to AT commands. Can be set to
            zero to return immediately.

        @params [IN] timeout Time in milliseconds to wait for the device to become
            unresponsive. Default is 0 ms, i.e. return immediately.
        @return True if the module has been successfully powered off.
    */ 
    bool powerOff(uint32_t timeout = 0);

    /*
        @brief Set phone functionality mode (AT+CFUN).

        @params [IN] fun The phone functionality (see manual for details).
        @params [IN] reset Whether to reset the device before changing status.
        @return True if the device functionality as been successfully changed.
    */ 
    bool setPhoneFunctionality(uint8_t fun, bool reset = false);

    /*
        @brief Turn off device's radio (AT+CFUN=4).

        @return True if the device functionality as been successfully changed.
    */ 
    bool radioOFF();

    /*
        @brief Turn off device to full functionality (AT+CFUN=1).

        @return True if the device functionality as been successfully changed.
    */ 
    bool radioON();

    /*
        @brief Reset the module and wait for it to initialize.

        @detail Equivalent to calling ::reset and ::init sequentially.

        @return True if the device has been corrextly reset and initialised.
    */ 
    bool restart(uint32_t timeout = 30000);

    /*
        @brief Test the device until it stop responding to the `AT` command.

        @param [IN] timeout Give up if the module still responds after this timeout in 
            milliseconds. Default is 30000 ms.
        @return True if the device stopped responding within the specified timeout.
    */
    bool waitATUnresponsive(uint32_t timeout = 30000);

    /*
        @brief Test the device until it start responding to the `AT` command.

        @param [IN] timeout Give up if the module still does not respond after this 
            timeout in milliseconds. Default is 30000 ms.
        @return True if the device started responding within the specified timeout.
    */
    bool waitATResponsive(uint32_t timeout = 30000);

    /*
        @brief Set the device to sleep.

        @return True if the device successfully went on sleep.
    */
    bool sleep();

    /*
        @brief Wake up the device from sleep.

        @return True if the device successfully woke up from sleep.
    */
    bool wakeUp();

    /*
        @brief Get model identification string (AT+CGMM).

        @return The model identification string.
    */
    String modelIdentification();

    /*
        @brief Get firmware revision identification string (AT+CGMR).

        @return The firmware revision identification string.
    */
    String revisionIdentification();

    /*
        @brief Sync device clock with an NTP server.
        
        @detail This function requires an active GPRS connection.

        @param [IN] timezone The timezone of the device expressed in quarters of hour.
        @param [IN] timeout Timeout in milliseconds for the sync with the NTP server. 
            Default is 10000 ms.
        @param [IN] host The address of the NTP server. Default is "pool.ntp.org".

        @return True on successful synchronization.
    */
    bool syncTime(int8_t timezone = 0, uint32_t timeout = 10000, const char* host = "pool.ntp.org");

    /*
        @brief Get date and time.
        
        @return A string in the format "yy/MM/dd,hh:mm:ss±zz"
    */
    String getDateTime();

    /*
        @brief Get date and time.

        @param [IN] year Pointer to an integer to store the year.
        @param [IN] month Pointer to an integer to store the month.
        @param [IN] day Pointer to an integer to store the day.
        @param [IN] hour Pointer to an integer to store the hour.
        @param [IN] minute Pointer to an integer to store the minute.
        @param [IN] second Pointer to an integer to store the second.
        @param [IN] timezone Pointer to an integer to store the timezone of the device, 
            in units of quarters of hour.

        @return True on successful execution.
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

        @param [IN] UTC Whether the timestamp should be in UTC time (or in local time).
        @return An unsigned 32 bit integer with the number of seconds since the Epoch.
    */
    uint32_t getUnixTime(bool UTC = true);

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
        @brief Send data to the command to the module, with a trailing carriage return,
            line feed characters.

        @param [IN] args Items (string, numbers, ...) to be sent.
        @param [IN] finish Whether to send a terminating "\r\n" to the module.
    */
    template <typename... ARGS>
    void sendCMD(ARGS... args) {
        printCMD(args..., "\r\n");
    }

    /*
        @brief Print data to the module's serial port.

        @detail This is analogus to ::sendCMD, but without the trailing carriage return, 
            line feed characters.

        @param [IN] args Items (string, numbers, ...) to be sent.
        @param [IN] finish Whether to send a terminating "\r\n" to the module.
    */
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

    // The following functions are simply forwarding the calls to underlying stream 
    // object. If you need others, send a pull request!
    int streamAvailable() { 
        return _stream.available(); 
    }

    long streamParseInt() { 
        return _stream.parseInt(); 
    }

    void streamFlush() { 
        _stream.flush(); 
    }

    uint16_t streamRead()  { 
        return _stream.read(); 
    }

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