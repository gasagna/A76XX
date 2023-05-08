#ifndef A76XX_SERIAL_H_
#define A76XX_SERIAL_H_

enum Response_t {
    A76XX_RESPONSE_OK        = 0,
    A76XX_RESPONSE_MATCH_1ST = 1,
    A76XX_RESPONSE_MATCH_2ND = 2,
    A76XX_RESPONSE_ERROR     = 3,
    A76XX_RESPONSE_TIMEOUT   = 4
};

#define RESPONSE_OK "OK\r\n"
#define RESPONSE_ERROR "ERROR\r\n"

class SerialAT : public Stream {
    /*
        @brief Wait for modem to respond.

        @detail Consume data from the serial port until a match is found with one of the 
            two input string or before the operation times out. If the parameters
            `match_OK` and `match_ERROR` are true, we also check if the response
            matches with the default OK and ERROR strings, but precedence is given
            to matching the input strings, which is useful for some commands.
        @param [IN] match_1 The first string to match.
        @param [IN] match_2 The second string to match.
        @param [IN] timeout Return if no match is found within this time in milliseconds.
            Default is 1000 milliseconds.
        @param [IN] match_OK Whether to match the string "OK\r\n". Default is true.
        @param [IN] match_ERROR Whether to match the string "ERROR\r\n". Default is true.

        @return A Response_t object.
    */
    Response_t waitResponse(const char* match_1,
                            const char* match_2,
                            int timeout = 1000,
                            bool match_OK = true,
                            bool match_ERROR = true) {
        // store data here
        String data; data.reserve(64);

        // start timer
        auto tstart = millis();

        while (millis() - tstart < timeout) {
            if (available() > 0) {
                data += static_cast<char>(read());
                if (data.endsWith(match_1) == true)
                    return Response_t::A76XX_RESPONSE_MATCH_1ST;
                if (data.endsWith(match_2) == true)
                    return Response_t::A76XX_RESPONSE_MATCH_2ND;
                if (match_ERROR && data.endsWith(RESPONSE_ERROR) == true)
                    return Response_t::A76XX_RESPONSE_ERROR;
                if (match_OK && data.endsWith(RESPONSE_OK) == true)
                    return Response_t::A76XX_RESPONSE_OK;
            }
        }

        return Response_t::A76XX_RESPONSE_TIMEOUT;
    }

    /*
        @brief Wait for modem to respond.

        @detail Consume data from the serial port until a match is found with the 
            input string or before the operation times out. If the parameters
            `match_OK` and `match_ERROR` are true, we also check if the response
            matches with the default OK and ERROR strings, but precedence is given
            to matching the input string, which is useful for some commands.
        @param [IN] match_1 The string to match.
        @param [IN] timeout Return if no match is found within this time in milliseconds.
            Default is 1000 milliseconds.
        @param [IN] match_OK Whether to match the string "OK\r\n". Default is true.
        @param [IN] match_ERROR Whether to match the string "ERROR\r\n". Default is true.

        @return A Response_t object.
    */
    Response_t waitResponse(const char* match_1,
                            int timeout = 1000,
                            bool match_OK = true,
                            bool match_ERROR = true) {
        return waitResponse(match_1,
                            "_ThIs_Is_AlMoSt_NeVeR_GoNnA_MaTcH_2_",
                            timeout, match_OK, match_ERROR);
    }

    /*
        @brief Wait for modem to respond.

        @detail Consume data from the serial port until a match with the default 
            OK and ERROR strings, if the parameters `match_OK` and `match_ERROR` are true.
        @param [IN] timeout Return if no match is found within this time in milliseconds.
            Default is 1000 milliseconds.
        @param [IN] match_OK Whether to match the string "OK\r\n". Default is true.
        @param [IN] match_ERROR Whether to match the string "ERROR\r\n". Default is true.

        @return A Response_t object.
    */
    Response_t waitResponse(int timeout = 1000,
                            bool match_OK = true,
                            bool match_ERROR = true) {
        return waitResponse("_ThIs_Is_AlMoSt_NeVeR_GoNnA_MaTcH_1_",
                            "_ThIs_Is_AlMoSt_NeVeR_GoNnA_MaTcH_2_",
                            timeout, match_OK, match_ERROR);
    }


    /*
        @brief Send data to the command to the module.
        @param [IN] args Items (string, numbers, ...) to be sent.
        @param [IN] finish Whether to send a terminating "\r\n" to the module.
    */
    template <typename... ARGS>
    void sendCMD(ARGS... args, bool finish = true) {
        if (finish) {
            _print(args..., "\r\n");
        } else {
            _print(args...);
        }
        flush();
    }

    /*
        @brief Parse an integer number and then consume all data available in the 
            serial interface until the default OK or ERROR strings are found, or 
            until the operation times out.
        @param [IN] timeout Time out in milliseconds. Default is 50 milliseconds.
        @return The integer.
    */
    auto parseIntClear(uint32_t timeout = 500) {
        auto retcode = parseInt();
        clear(timeout);
        return retcode;
    }

    /*
        @brief Consume all data available in the stream, until the default
            OK or ERROR strings are found, or until the operation times out.
        @param [IN] timeout Time out in milliseconds. Default is 50 milliseconds.
    */ 
    void clear(uint32_t timeout = 500) {
        waitResponse(timeout);
    }

  protected:
    // print any number of commands to the module serial interface
    template <typename HEAD, typename... TAIL>
    void _print(HEAD head, TAIL... tail) {
        print(head);
        _print(tail...);
    }

    // single argument case
    template <typename ARG>
    void _print(ARG arg) {
        print(arg);
    }

}

#endif A76XX_SERIAL_H_