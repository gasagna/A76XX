#ifndef A76XX_PACKETDOMAIN_CMDS_H_
#define A76XX_PACKETDOMAIN_CMDS_H_


/*
    @brief Commands in section 5 of the AT command manual version 1.09

    Command | Implemented | Method | Function(s)
    ------- | ----------- | ------ |-----------------
    CGREG   |      y      | READ   | getNetworkRegistrationStatus
    CEREG   |      -      |        |
    CGATT   |      -      |        |
    CGACT   |      y      | WRITE  | setPDPContextActiveStatus
    CGDCONT |      y      | WRITE  | setPDPContextParameters
    CGDSCONT|      -      |        |
    CGTFT   |      -      |        |
    CGQREQ  |      -      |        |
    CGEQREQ |      -      |        |
    CGQMIN  |      -      |        |
    CGEQMIN |      -      |        |
    CGDATA  |      -      |        |
    CGPADDR |      -      |        |
    CGCLASS |      -      |        |
    CGEREP  |      -      |        |
    CGAUTH  |      y      | WRITE  | setPDPAuthentication
    CPING   |      -      |        |
*/
template <typename MODEM>
class A76XX_PacketDomain_Commands {
  public:
    MODEM* _modem = NULL;

    /*
        @brief Implementation for CGREG - Read Command.
        @detail Get GPRS network registration status
        @return The <stat> code, from 0 to 11, or an error code.
    */
    int8_t getNetworkRegistrationStatus() {
        _modem->sendCMD("AT+CGREG?");
        Response_t rsp = _modem->waitResponse("+CGREG: ", 9000, false, true);
        switch (rsp) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                _modem->streamFind(',');
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

    /*
        @brief Implementation for CGACT - Write Command.
        @detail Activate or Deactivate PDP context.
        @param [IN] cid Context identifier.
        @param [IN] activate Flag to activate/deactive context.
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR.
    */
    int8_t setPDPContextActiveStatus(uint8_t cid, bool activate) {
        _modem->sendCMD("AT+CGACT=", activate ? "1" : "0", ",", cid);
        A76XX_RESPONSE_PROCESS(_modem->waitResponse(9000))
    }

    int8_t getPDPContextActiveStatus(uint8_t cid, int8_t status) {
        _modem->sendCMD("AT+CGACT?");
        char buff[10] = "+CGACT: X"; buff[8] = cid;
        switch (_modem->waitResponse(buff, 9000)) {
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
        @brief Implementation for CGDCONT - Write Command.
        @detail Set PDP context parameters.
        @param [IN] cid Context identifier.
        @param [IN] apn Access Point Name.
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR.
    */
    int8_t setPDPContextParameters(uint8_t cid, const char* apn) {
        _modem->sendCMD("AT+CGDCONT=", cid,",\"IP\",\"", apn, "\"");
        A76XX_RESPONSE_PROCESS(_modem->waitResponse(9000))
    }

    /*
        @brief Implementation for CGAUTH - Write Command.
        @detail Set type of authentication for PDP-IP connections of GPRS.
        @param [IN] cid Context identifier.
        @param [IN] auth_type Type of authentication.
        @param [IN] password The password used for authentication.
        @param [IN] username The username used for authentication.
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR.
    */
    int8_t setPDPAuthentication(uint8_t cid,
                                uint8_t auth_type = 0,
                                const char* password = NULL,
                                const char* username = NULL) {
        _modem->printCMD("AT+CGAUTH=", cid);
        if (auth_type != 0)    { _modem->printCMD(",", auth_type);}
        if (password  != NULL) { _modem->printCMD(",", password);}
        if (username  != NULL) { _modem->printCMD(",", username);}
        _modem->printCMD("\r\n");
        A76XX_RESPONSE_PROCESS(_modem->waitResponse(9000))
    }

};

#endif A76XX_PACKETDOMAIN_CMDS_H_