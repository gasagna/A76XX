#ifndef A76XX_SIM_CMDS_H_
#define A76XX_SIM_CMDS_H_

enum PINStatus_t {
    READY,
    SIM_PIN,
    SIM_PUK,
    PH_SIM_PIN,
    SIM_PIN2,
    SIM_PUK2,
    PH_NET_PIN,
    UKNOWKN
};

/*
    @brief Commands in section 6 of the AT command manual version 1.09

    Command         | Implemented | Method | Function(s)
    --------------- | ----------- | ------ |-----------------------
    CICCID          |             |        |
    CPIN            |      y      | READ   |
    CLCK            |             |        |
    CPWD            |             |        |
    CIMI            |             |        |
    CSIM            |             |        |
    CRSM            |             |        |
    SPIC            |             |        |
    CSPN            |             |        |
    UIMHOTSWAPON    |             |        |
    UIMHOTSWAPLEVEL |             |        |
    SWITCHSIM       |             |        |
    DUALSIM         |             |        |
    BINDSIM         |             |        |
    DUALSIMURC      |             |        |
*/
template <typename MODEM>
class SIMCommands {
  public:
    MODEM* _modem = NULL;

    /*
        @brief Implementation for CPIN - Read Command.
        @detail Get PIN status.
        @param [OUT] status The PIN status.
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR 
    */
    int8_t getPINStatus(PINStatus_t& status) {
        _modem->sendCMD("AT+CPIN?");
        switch(_modem->waitResponse("+CPIN: ", 9000)) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                char buff[11];
                _modem->streamReadBytesUntil('\r', &buff[0], 11);

                if (strcmp(buff, "READY"))      {status = PINStatus_t::READY;}
                if (strcmp(buff, "SIM PIN"))    {status = PINStatus_t::SIM_PIN;}
                if (strcmp(buff, "SIM PUK"))    {status = PINStatus_t::SIM_PUK;}
                if (strcmp(buff, "PH-SIM PIN")) {status = PINStatus_t::PH_SIM_PIN;}
                if (strcmp(buff, "SIM PIN2"))   {status = PINStatus_t::SIM_PIN2;}
                if (strcmp(buff, "SIM PUK2"))   {status = PINStatus_t::SIM_PUK2;}
                if (strcmp(buff, "PH-NET PIN")) {status = PINStatus_t::PH_NET_PIN;}

                // default case
                status = PINStatus_t::UKNOWKN;
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
};

#endif A76XX_SIM_CMDS_H_