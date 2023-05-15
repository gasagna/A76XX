#ifndef A76XX_INTERNETSERVICE_CMDS_H_
#define A76XX_INTERNETSERVICE_CMDS_H_


/*
    @brief Commands in section 14 of the AT command manual version 1.09

    Command       | Implemented | Method     | Function(s)
    ------------- | ----------- | ---------- | -----------
    AT+CHTPSERV   |             |            |
    AT+CHTPUPDATE |             | WRITE/EXEC | setNTPParams,
    AT+CNTP       |     y       |            |
*/
template <typename MODEM>
class InternetServiceCommands {
  public:
    MODEM* _modem = NULL;

    /*
        @brief Implementation for CNTP - WRITE Command.
        @detail Set NTP server and timezone.
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR.
    */
    int8_t setNTPParams(const char* host = "pool.ntp.org", int8_t timezone = 0) {
        _modem->sendCMD("AT+CNTP=\"", host, "\",", timezone);
        A76XX_RESPONSE_PROCESS(_modem->waitResponse());
    }

    /*
        @brief Implementation for CNTP - EXEC Command.
        @detail Set NTP server and timezone.
        @param [IN] timeout Timeout in milliseconds for the sync with the NTP server.
        @return Any of the results codes: 0 for Operation succeeded, 1 for Unknown error, 
            2 for Wrong parameter, 3 for Wrong date and time calculated, 4 for Network 
            error, 5 for Time zone error, 6 for Time out error, plus 
            A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR.
    */
    int8_t updateSystemTime(uint32_t timeout) {
        _modem->sendCMD("AT+CNTP");
        switch (_modem->waitResponse("+CNTP: ", timeout, false, true)) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                return _modem->streamParseIntClear();
            }
            case Response_t::A76XX_RESPONSE_TIMEOUT : {
                return A76XX_OPERATION_TIMEDOUT;
            }
            default : {
                return A76XX_GENERIC_ERROR;
            }
        }
    }

};

#endif A76XX_INTERNETSERVICE_CMDS_H_