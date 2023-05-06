#ifndef A76XXMODEM_H_
#define A76XXMODEM_H_

class A76XX {

  friend class A76XX_MQTT_Commands;
  friend class A76XX_SSL_Commands;
  friend class A76XX_HTTP_Commands;

  public:
    Stream& _serial;

  public:
    A76XX(Stream& serial)
        : _serial(serial) {}

    bool init() {
        // wait until modem is ready
        while (true) {
            sendCMD("AT+CPIN?");
            if (waitResponse("+CPIN: READY", 9000) == Response_t::A76XX_RESPONSE_MATCH_1ST)
                break;
        }

        // turn off echoing commands
        sendCMD("ATE0");
        A76XX_RESPONSE_ASSERT_BOOL(waitResponse(120000));

        // disable reporting mobile equipment errors with numeric values
        sendCMD("AT+CMEE=0");
        A76XX_RESPONSE_ASSERT_BOOL(waitResponse());

        // disable unsolicited codes for time zone change
        sendCMD("AT+CTZR=0");
        A76XX_RESPONSE_ASSERT_BOOL(waitResponse());

        // enable automatic time and time zone updates via NITZ
        sendCMD("AT+CTZU=1");
        A76XX_RESPONSE_ASSERT_BOOL(waitResponse());

        return true;
    }

    // connect to network via apn
    bool connect(const char apn*) {
        // define PDP context
        sendCMD("AT+CGDCONT=1,\"IP\",\"", apn, "\"");
        A76XX_RESPONSE_ASSERT_BOOL(waitResponse(9000));

        // activate PDP context
        sendCMD("AT+CGACT=1,1");
        A76XX_RESPONSE_ASSERT_BOOL(waitResponse(9000));

        return true;
    }

    // disconnect - this does not seem to work
    bool disconnect() {
        sendCMD("AT+CGACT=0,1");
        A76XX_RESPONSE_ASSERT_BOOL(waitResponse(9000));
        return true;
    }

    // check context has been activated
    bool isConnected() {
        sendCMD("AT+CGACT?");
        Response_t rsp = waitResponse("+CGACT: 1,1", 9000);
        _serialClear();
        return rsp == Response_t::A76XX_RESPONSE_MATCH_1ST ? true : false;
    }

    /// @brief CREG - Get registration status
    /// @return -1 on error
    int getRegistrationStatus() {
        sendCMD("AT+CREG?");
        if (waitResponse("+CREG: ", 9000) == Response_t::A76XX_RESPONSE_MATCH_1ST) {
            _serial.find(',');
            return _serialParseIntClear();
        }
        return -1; // error
    }

    // check we are registered on network or roaming
    bool isRegistered() {
        int stat = getRegistrationStatus();
        return (stat == 1 | stat == 5);
    }

    bool waitForRegistration(uint32_t timeout = 60000) {
        uint32_t tstart = millis();
        while (millis() - tstart < timeout) {
            if (isRegistered())
                return true;
            delay(200);
        }
        return false;
    }

    /// @brief CNSMOD - Show network system mode
    /// @return -1 on error
    int getNetworkSystemMode() {
        sendCMD("AT+CNSMOD?");
        Response_t rsp = waitResponse("+CNSMOD: ");
        if (rsp == Response_t::A76XX_RESPONSE_MATCH_1ST) {
            _serial.find(',');
            return _serialParseIntClear();
        }
        return -1; // error
    }

    //////////////////////////////////////////////////////////////////////////////////////
    // Power functions
    //////////////////////////////////////////////////////////////////////////////////////

    // CRESET
    bool reset() {
        sendCMD("AT+CRESET");
        return waitResponse(9000) == Response_t::A76XX_RESPONSE_OK;
    }

    // CPOF
    bool powerOff() {
        sendCMD("AT+CPOF");
        return waitResponse(9000) == Response_t::A76XX_RESPONSE_OK;
    }

    // CFUN - radio off/on
    bool setPhoneFunctionality(uint8_t fun, bool reset = false) {
        sendCMD("AT+CFUN=", fun, reset ? "1" : "");
        return waitResponse(9000) == Response_t::A76XX_RESPONSE_OK;
    }

    bool radioOFF() { return setPhoneFunctionality(4, false); }
    bool radioON()  { return setPhoneFunctionality(1, false); }

    // restart
    bool restart() {
        if (reset() == false)
            return false;
        return init();
    }

    // enable UART sleep
    bool sleep() {
        sendCMD("AT+CSCLK=2");
        return waitResponse() == Response_t::A76XX_RESPONSE_OK;
    }

    // in mode 2 wake up the module by sending data through the serial port
    bool wakeUp() {
        sendCMD("AT");
        return waitResponse() == Response_t::A76XX_RESPONSE_OK;
    }

    //////////////////////////////////////////////////////////////////////////////////////
    // Modem version functions
    //////////////////////////////////////////////////////////////////////////////////////
    String modelIdentification() {
        // check the command works
        sendCMD("AT+CGMM");
        if (waitResponse() != Response_t::A76XX_RESPONSE_OK) { return ""; }

        // send again
        sendCMD("AT+CGMM");
        String out = "";

        // go at the start of the version number
        _serial.find('\n');

        uint32_t tstart = millis();
        while (millis() - tstart < 5000) {
            if (_serial.available() > 0) {
                char c = static_cast<char>(_serial.read());
                if (c == '\r') {
                    break;
                }
                out += c;
            }
        }
        _serialClear();
        return out;
    }


    String revisionIdentification() {
        sendCMD("AT+CGMR");
        if (waitResponse("+CGMR: ") != Response_t::A76XX_RESPONSE_MATCH_1ST) {
            return "";
        }
        String out = "";
        uint32_t tstart = millis();
        while (millis() - tstart < 5000) {
            if (_serial.available() > 0) {
                char c = static_cast<char>(_serial.read());
                if (c == '\r') {
                    break;
                }
                out += c;
            }
        }
        _serialClear();
        return out;
    }
    //////////////////////////////////////////////////////////////////////////////////////
    // consume the data from the serial port until the string
    // `match` is found or before `timeout` ms have elapsed
    // if match_OK and match_ERROR are true, we also check if
    // the response matches with the OK and ERROR strings, but
    // precedence is given to `match`, which might be helpful
    // with some commands

    // match zero arguments - only match OK and ERROR
    Response_t waitResponse(int timeout=1000, bool match_OK=true, bool match_ERROR=true) {
        return waitResponse("_ThIs_Is_AlMoSt_NeVeR_GoNnA_MaTcH_1_",
                            "_ThIs_Is_AlMoSt_NeVeR_GoNnA_MaTcH_2_",
                            "_ThIs_Is_AlMoSt_NeVeR_GoNnA_MaTcH_3_",  timeout, match_OK, match_ERROR);
    }

    // match one argument plus OK and ERROR
    Response_t waitResponse(const char* match_1, int timeout=1000, bool match_OK=true, bool match_ERROR=true) {
        return waitResponse(match_1,
                            "_ThIs_Is_AlMoSt_NeVeR_GoNnA_MaTcH_2_",
                            "_ThIs_Is_AlMoSt_NeVeR_GoNnA_MaTcH_3_",  timeout, match_OK, match_ERROR);
    }

    // match two arguments plus OK and ERROR
    Response_t waitResponse(const char* match_1, const char* match_2, int timeout=1000, bool match_OK=true, bool match_ERROR=true) {
        return waitResponse(match_1, match_2,
                            "_ThIs_Is_AlMoSt_NeVeR_GoNnA_MaTcH_3_",  timeout, match_OK, match_ERROR);
    }

    // match three arguments plus OK and ERROR
    Response_t waitResponse(const char* match_1, const char* match_2, const char* match_3, int timeout=1000, bool match_OK=true, bool match_ERROR=true) {
        // store data here
        String data; data.reserve(64);

        // start timer
        auto tstart = millis();

        while (millis() - tstart < timeout) {
            if (_serial.available() > 0) {
                data += static_cast<char>(_serial.read());
                if (data.endsWith(match_1) == true)
                    return Response_t::A76XX_RESPONSE_MATCH_1ST;
                if (data.endsWith(match_2) == true)
                    return Response_t::A76XX_RESPONSE_MATCH_2ND;
                if (data.endsWith(match_3) == true)
                    return Response_t::A76XX_RESPONSE_MATCH_3RD;
                if (match_ERROR && data.endsWith(RESPONSE_ERROR) == true)
                    return Response_t::A76XX_RESPONSE_ERROR;
                if (match_OK && data.endsWith(RESPONSE_OK) == true)
                    return Response_t::A76XX_RESPONSE_OK;

            }
        }

        return Response_t::A76XX_RESPONSE_TIMEOUT;
    }

    // send command
    template <typename... ARGS>
    void sendCMD(ARGS... args) {
        _serialPrint(args..., "\r\n");
        _serial.flush();
    }

  protected:

    // parse an int and then consume all data available in the stream
    int _serialParseIntClear() {
        int retcode = _serial.parseInt(); _serialClear();
        return retcode;
    }

    // consume all data available in the stream
    void _serialClear() {
        waitResponse();
    }

    // write to stream
    template <typename ARG>
    void _serialPrint(ARG arg) {
        _serial.print(arg);
    }

    // write to stream
    template <typename HEAD, typename... TAIL>
    void _serialPrint(HEAD head, TAIL... tail) {
        _serial.print(head);
        _serialPrint(tail...);
    }

};

#endif A76XXMODEM_H_