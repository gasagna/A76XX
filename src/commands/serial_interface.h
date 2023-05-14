#ifndef A76XX_SERIALINTERFACE_CMDS_H_
#define A76XX_SERIALINTERFACE_CMDS_H_


/*
    @brief Commands in section 10 of the AT command manual version 1.09

    Command  | Implemented | Method | Function(s)
    -------- | ----------- | ------ |------------
    AT&D     |             |        |
    AT&C     |             |        |
    AT+IPR   |             |        |
    AT+IPREX |             |        |
    AT+ICF   |             |        |
    AT+IFC   |             |        |
    AT+CSCLK |      y      | WRITE  | UARTSleep
    AT+CMUX  |             |        |
    AT+CATR  |             |        |
    AT+CFGRI |             |        |
    AT+CURCD |             |        |

*/
template <typename MODEM>
class SerialInterfaceCommands {
  public:
    MODEM* _modem = NULL;
 
    /*
        @brief Implementation for CSCLK - Write Command.
        @detail Control UART sleep.
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR
    */
    int8_t UARTSleep(uint8_t status) {
        _modem->sendCMD("AT+CSCLK=", status);
        A76XX_RESPONSE_PROCESS(_modem->waitResponse());
    }
};

#endif A76XX_SERIALINTERFACE_CMDS_H_