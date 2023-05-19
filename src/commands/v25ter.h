#ifndef A76XX_V25TER_CMDS_H_
#define A76XX_V25TER_CMDS_H_


/*
    @brief Commands in section 2 of the AT command manual version 1.09

    Command  | Implemented | Method | Function(s)
    -------- | ----------- | ------ |-----------------------
    ATD      |             |        |
    ATA      |             |        |
    ATH      |             |        |
    ATS0     |             |        |
    +++      |             |        |
    ATO      |             |        |
    ATI      |             |        |
    ATE      |     y       | WRITE  | commandEcho
    AT&V     |             |        |
    ATV      |             |        |
    AT&F     |             |        |
    ATQ      |             |        |
    ATX      |             |        |
    AT&W     |             |        |
    ATZ      |             |        |
    AT+CGMI  |             |        |
    AT+CGMM  |     y       | READ   | modelIdentification
    AT+CGMR  |     y       | READ   | revisionIdentification
    AT+CGSN  |             |        |
    AT+CSCS  |             |        |
    AT+GCAP  |             |        |
*/

class V25TERCommands {
  public:
    ModemSerial& _serial;

    V25TERCommands(ModemSerial& serial)
        : _serial(serial) {}

    /*
        @brief Implementation for ATE - WRITE Command.
        @detail Enable/Disable command echo.
        @param [IN] enable whether to enable command echo.
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR
    */
    int8_t commandEcho(bool enable) {
        _serial.sendCMD("ATE", enable ? "1" : "0");
        switch(_serial.waitResponse(120000)) {
            case Response_t::A76XX_RESPONSE_OK : {
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
        @brief Implementation for CGMM - WRITE Command.
        @detail Get model identification string
        @param [IN] model will contain the model string on successful execution.
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR
    */
    int8_t modelIdentification(String& model) {
        // clear stream before sending command, then get rid of the first line
        _serial.clear();
        _serial.sendCMD("AT+CGMM");
        _serial.find("\r\n");
        uint32_t tstart = millis();
        while (millis() - tstart < 5000) {
            if (_serial.available() > 0) {
                char c = static_cast<char>(_serial.read());
                if (c == '\r') {
                    _serial.clear();
                    return A76XX_OPERATION_SUCCEEDED;
                }
                model += c;
            }
        }
        return A76XX_OPERATION_TIMEDOUT;
    }

    /*
        @brief Implementation for CGMR - WRITE Command.
        @detail Get model revision string
        @param [IN] model will contain the revision string on successful execution.
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR
    */
    int8_t revisionIdentification(String& revision) {
        _serial.sendCMD("AT+CGMR");
        switch (_serial.waitResponse("+CGMR: ", 9000, false, false)) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                uint32_t tstart = millis();
                while (millis() - tstart < 5000) {
                    if (_serial.available() > 0) {
                        char c = static_cast<char>(_serial.read());
                        if (c == '\r') {
                            _serial.clear();
                            return A76XX_OPERATION_SUCCEEDED;
                        }
                        revision += c;
                    }
                }
                _serial.clear();
                return A76XX_OPERATION_TIMEDOUT;
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

#endif A76XX_V25TER_CMDS_H_