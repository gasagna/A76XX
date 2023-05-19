#ifndef A76XXMODEM_H_
#define A76XXMODEM_H_

class A76XX {
  public:
    ModemSerial                           _serial;
    int8_t                       _last_error_code;
    InternetServiceCommands       internetService;
    NetworkCommands                       network;
    PacketDomainCommands             packetDomain;
    SerialInterfaceCommands       serialInterface;
    SIMCommands                               sim;
    StatusControlCommands           statusControl;
    V25TERCommands                         v25ter;

    /*
        @brief Construct an instance of the modem.

        @param [IN] serial Serial interface to the SIMCOM module. Must be
            initialised separately with `begin` and the appropriate TX/RX pins.
    */
    A76XX(Stream& stream);

    /*
        @brief Get the return code of the last operation.
    */
    int8_t getLastError();

    /*
        @brief Wait for the serial communication with the SIMCOM module to
            become available and initialise the modem. 
            
        @param [IN] pincode The PIN code used to unlock the SIM if required.
        @param [IN] timeout Give up if the module cannot be initialised within
            this time in millisecond. Default is 30000 ms.

        @return True is the device was successfully initialised within the 
            specified time. Returns false is the module does not become 
            responsive within `timeout`, if the SIM requires a PIN code or
            if any other commands run during the initialisation are unsuccessful.
    */
    bool init(const char* pincode = NULL, uint32_t timeout = 30000);

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
};

#endif A76XXMODEM_H_