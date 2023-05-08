#ifndef A76XX_STATUCCONTROL_CMDS_H_
#define A76XX_STATUCCONTROL_CMDS_H_


/*
    @brief Commands in section 3 of the AT command manual version 1.09

    Command | Implemented | Method | Function(s)
    --------| ----------- | ------ |-----------------
    CFUN    |     y       | WRITE  | setPhoneFunctionality
    CSQ     |             |        |
    AUTOCSQ |             |        |
    CSQDELTA|             |        |
    CPOF    |     y       |  EXEC  | powerOff
    CRESET  |     y       |  EXEC  | reset
    CACM    |             |        |
    CAMM    |             |        |
    CPUC    |             |        |
    CCLK    |     y       |        | getDateTime
    CMEE    |     y       |        | setErrorResultCodes
    CPAS    |             |        |
    SIMEI   |             |        |
*/
class A76XX_StatusControl_Commands {
  private:
    A76XX& _modem;

  public:
    // Initialise from modem
    A76XX_StatusControl_Commands(A76XX& modem)
        : _modem(modem) {}

    /*
        @brief Implementation for CFUN - Write Command.
        @detail Set phone functionality.
        @param [IN] fun Phone functionality.
        @param [IN] reset Whether to reset the module before setting the functionality.
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR.
    */
    int8_t setPhoneFunctionality(uint8_t fun, bool reset = false) {
        _modem.sendCMD("AT+CFUN=", fun, ",", reset ? "1" : "");
        A76XX_RESPONSE_PROCESS(9000)
    }

    /*
        @brief Implementation for CPOF - Execute Command.
        @detail Power off the module.
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR.
    */
    int8_t powerOff() {
        _modem.sendCMD("AT+CPOF");
        A76XX_RESPONSE_PROCESS(9000)
    }

    /*
        @brief Implementation for CRESET - Execute Command.
        @detail Reset the module.
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR.
    */
    int8_t reset() {
        _modem.sendCMD("AT+CRESET");
        A76XX_RESPONSE_PROCESS(9000)
    }


    /*
        @brief Implementation for CCLK - READ Command.
        @detail Read date and time from module's real time clock
        @param [OUT] datetime A char buffer to store the date and time in the format 
            "yy/MM/dd,hh:mm:ss±zz". Length must be at least 20 char, or A76XX_GENERIC_ERROR
            is returned. 
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR.
    */
    int8_t getDateTime(char* datetime) {
        if (strlen(datetime) < 20) { 
            return A76XX_GENERIC_ERROR; 
        }

        sendCMD("AT+CCLK?");
        switch (waitResponse("+CCLK: ", 9000, false, true)) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                _modem._serial.readBytes(datetime, 20);
                // clear OK
                return _modem.serialClear();
            }
            case Response_t::A76XX_OPERATION_TIMEDOUT : {
                return A76XX_OPERATION_TIMEDOUT;
            }
            default : {
                return A76XX_GENERIC_ERROR;
            }
         }
    }

    /*
        @brief Implementation for CMEE - Write Command.
        @detail Set mobile equipment error result codes.
        @params [IN] n Flag to set the result code reporting.
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR.
    */
    int8_t setErrorResultCodes(uint8_t n) {
        sendCMD("AT+CMEE=", n);
        A76XX_RESPONSE_PROCESS(9000)
    }
};
#ifndef A76XX_STATUCCONTROL_CMDS_H_