#ifndef A76XX_STATUSCONTROL_CMDS_H_
#define A76XX_STATUSCONTROL_CMDS_H_


/*
    @brief Commands in section 3 of the AT command manual version 1.09

    Command | Implemented | Method | Function(s)
    --------| ----------- | ------ |-----------------
    CFUN    |     y       | WRITE  | setPhoneFunctionality
    CSQ     |     -       |        |
    AUTOCSQ |     -       |        |
    CSQDELTA|     -       |        |
    CPOF    |     y       |  EXEC  | powerOff
    CRESET  |     y       |  EXEC  | reset
    CACM    |     -       |        |
    CAMM    |     -       |        |
    CPUC    |     -       |        |
    CCLK    |     y       |  READ  | getDateTime
    CMEE    |     y       | WRITE  | setErrorResultCodes
    CPAS    |     -       |        |
    SIMEI   |     -       |        |
*/
template <typename MODEM>
class StatusControlCommands {
  public:
    MODEM* _modem = NULL;

    /*
        @brief Implementation for CFUN - Write Command.
        @detail Set phone functionality.
        @param [IN] fun Phone functionality.
        @param [IN] reset Whether to reset the module before setting the functionality.
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR.
    */
    int8_t setPhoneFunctionality(uint8_t fun, bool reset = false) {
        _modem->sendCMD("AT+CFUN=", fun, reset ? ",1" : "");
        A76XX_RESPONSE_PROCESS(_modem->waitResponse(9000))
    }

    /*
        @brief Implementation for CPOF - Execute Command.
        @detail Power off the module.
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR.
    */
    int8_t powerOff() {
        _modem->sendCMD("AT+CPOF");
        A76XX_RESPONSE_PROCESS(_modem->waitResponse(9000))
    }

    /*
        @brief Implementation for CRESET - Execute Command.
        @detail Reset the module.
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR.
    */
    int8_t reset() {
        _modem->sendCMD("AT+CRESET");
        A76XX_RESPONSE_PROCESS(_modem->waitResponse(9000))
    }

    /*
        @brief Implementation for CCLK - READ Command.
        @detail Read date and time from module's real time clock
        @param [OUT] dateTime A char buffer to store the date and time in the format 
            "yy/MM/dd,hh:mm:ss±zz". Length must be at least 20 char, or A76XX_GENERIC_ERROR
            is returned. 
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR.
    */
    int8_t getDateTime(char* dateTime) {
        if (strlen(dateTime) < 20) { 
            return A76XX_GENERIC_ERROR; 
        }

        _modem->sendCMD("AT+CCLK?");
        switch (_modem->waitResponse("+CCLK: \"", 9000, false, true)) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                _modem->streamReadBytes(dateTime, 20);
                _modem->streamClear(); // clear OK
                return A76XX_OPERATION_SUCCEEDED;
            }
            case Response_t::A76XX_RESPONSE_TIMEOUT : {
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
        _modem->sendCMD("AT+CMEE=", n);
        A76XX_RESPONSE_PROCESS(_modem->waitResponse(9000))
    }
};

#endif A76XX_STATUSCONTROL_CMDS_H_