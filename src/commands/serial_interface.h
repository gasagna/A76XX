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
    AT+CMUX  |      y      | WRITE  | enableMUX
    AT+CATR  |      y      | WRITE  | setURCInterface
    AT+CFGRI |             |        |
    AT+CURCD |             |        |

*/

class SerialInterfaceCommands {
  public:
    ModemSerial& _serial;

    SerialInterfaceCommands(ModemSerial& serial)
        : _serial(serial) {}

    /*
        @brief Implementation for CSCLK - Write Command.
        @detail Control UART sleep.
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR
    */
    int8_t UARTSleep(uint8_t status) {
        _serial.sendCMD("AT+CSCLK=", status);
        A76XX_RESPONSE_PROCESS(_serial.waitResponse());
    }

    /*
        @brief Implementation for CMUX - Write Command.
        @detail Enable the multiplexer over the UART.
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR
    */
    int8_t enableMUX() {
        _serial.sendCMD("AT+CMUX=0");
        A76XX_RESPONSE_PROCESS(_serial.waitResponse());
    }

    /*
        @brief Implementation for CATR - Write Command.
        @detail Configure URC destination interface.
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR
    */
    int8_t setURCInterface(uint8_t port) {
        _serial.sendCMD("AT+CATR=", port);
        A76XX_RESPONSE_PROCESS(_serial.waitResponse());
    }

};

#endif A76XX_SERIALINTERFACE_CMDS_H_