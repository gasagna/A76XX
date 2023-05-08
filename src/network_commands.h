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
    CNSMOD  |      -      |        |
    CTZU    |      y      | WRITE  | setTimeZoneAutoUpdate
    CTZR    |      y      | WRITE  | setTimeZoneURC

*/
class A76XX_Network_Commands {
  private:
    A76XX& _modem;

  public:
    // Initialise from modem
    A76XX_Network_Commands(A76XX& modem)
        : _modem(modem) {}

    /*
        @brief Implementation for CREG - Read Command.
        @detail Get network registration status.
        @return The <stat> code, from 0 to 11, or an error code.
    */ 
    int8_t getNetworkRegistration() {
        _modem.sendCMD("AT+CREG?");
        Response_t rsp = waitResponse("+CREG: ", 9000, false, true);
        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                _modem._serial.find(',');
                return _modem.serialParseIntClear();
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
        @brief Implementation for CTZU - Write Command.
        @detail Enable or disable automatic time and time zone updates via NITZ.
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR 
    */ 
    int8_t setTimeZoneAutoUpdate(bool enable) {
        _modem.sendCMD("AT+CTZU=", enable ? "1" : "0");
        A76XX_RESPONSE_PROCESS(waitResponse())
    }

    /*
        @brief Implementation for CTZR - Write Command.
        @detail Enable or disable unsolicited codes for time zone change.
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR
    */
    int8_t setTimeZoneURC(bool enable) {
        _modem.sendCMD("AT+CTZR=", enable ? "1" : "0");
        A76XX_RESPONSE_PROCESS(waitResponse());
    }
};

#endif A76XX_NETWORK_CMDS_H_