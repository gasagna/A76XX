#ifndef A76XX_NETWORK_CMDS_H_
#define A76XX_NETWORK_CMDS_H_


/*
    @brief Commands in section 4 of the AT command manual version 1.09

    Command | Implemented | Method | Function(s)
    ------- | ----------- | ------ |-----------------------
    CREG    |      y      |  READ  | getNetworkRegistration
    COPS    |      -      |        |
    CUSD    |      -      |        |
    CSSN    |      -      |        |
    CPOL    |      -      |        |
    COPN    |      -      |        |
    CNMP    |      -      |        |
    CNBP    |      -      |        |
    CPSI    |      -      |        |
    CNSMOD  |      y      | READ   | getNetworkSystemMode
    CTZU    |      y      | WRITE  | setTimeZoneAutoUpdate
    CTZR    |      y      | WRITE  | setTimeZoneURC

*/
template <typename MODEM>
class NetworkCommands {
  public:
    MODEM* _modem = NULL;

    /*
        @brief Implementation for CREG - Read Command.
        @detail Get GSM network registration status.
        @param [OUT] status the registration code (i.e. <stat> in the manual)
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR.
    */ 
    int8_t getNetworkRegistrationStatus(int8_t& status) {
        _modem->sendCMD("AT+CREG?");
        Response_t rsp = _modem->waitResponse("+CREG: ", 9000, false, true);
        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                _modem->streamFind(',');
                status = _modem->streamParseIntClear();
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
        @brief Implementation for CNSMOD - Read Command.
        @detail Show network system mode.
        @return The <stat> code, from 0 to 8, or an error code.
    */ 
    int8_t getNetworkSystemMode(int8_t& mode) {
        _modem->sendCMD("AT+CNSMOD?");
        Response_t rsp = _modem->waitResponse("+CNSMOD: ");
        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                _modem->streamFind(',');
                mode = _modem->streamParseIntClear();
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
        @brief Implementation for CTZU - Write Command.
        @detail Enable or disable automatic time and time zone updates via NITZ.
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR 
    */ 
    int8_t setTimeZoneAutoUpdate(bool enable) {
        _modem->sendCMD("AT+CTZU=", enable ? "1" : "0");
        A76XX_RESPONSE_PROCESS(_modem->waitResponse())
    }

    /*
        @brief Implementation for CTZR - Write Command.
        @detail Enable or disable unsolicited codes for time zone change.
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR
    */
    int8_t setTimeZoneURC(bool enable) {
        _modem->sendCMD("AT+CTZR=", enable ? "1" : "0");
        A76XX_RESPONSE_PROCESS(_modem->waitResponse());
    }
};

#endif A76XX_NETWORK_CMDS_H_