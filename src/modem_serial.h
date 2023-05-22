#ifndef A76XX_MODEMUART_H_
#define A76XX_MODEMUART_H_

class ModemSerial {
  private:
    Stream&                                                        _stream;
    EventHandler_t*              _event_handlers[A76XX_MAX_EVENT_HANDLERS];
    uint8_t                                            _num_event_handlers;
    StaticQueue<A76XXURC_t, A76XX_URC_QUEUE_SIZE>        _events_urc_queue;

  public:

    /*
        @brief Construct a ModemSerial object.

        @details This is a standard Arduino Serial object on steroids, with 
            additional functionality to send AT commands and parse the response
            from the module.

        @param [IN] stream The underlying serial object. It must be initialized 
            externally by the user with, e.g., a call to begin, with the appropriate 
            TX and RX pins. The modem cannot be used if the serial connection is 
            not established.
    */
    ModemSerial(Stream& stream)
        : _stream(stream) 
        , _num_event_handlers(0) {}

    /*
        @brief Wait for modem to respond.

        @detail Consume data from the serial port until a match is found with one of the 
            two input string or before the operation times out. If the parameters
            `match_OK` and `match_ERROR` are true, we also check if the response
            matches with the default OK and ERROR strings, but precedence is given
            to matching the input strings, which is useful for some commands.
        @param [IN] match_1 The first string to match.
        @param [IN] match_2 The second string to match.
        @param [IN] match_3 The third string to match.
        @param [IN] timeout Return if no match is found within this time in milliseconds.
            Default is 1000 milliseconds.
        @param [IN] match_OK Whether to match the string "OK\r\n". Default is true.
        @param [IN] match_ERROR Whether to match the string "ERROR\r\n". Default is true.

        @return A Response_t object.
    */
    Response_t waitResponse(const char* match_1,
                            const char* match_2,
                            const char* match_3,
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

                // parse modem output for any URCs that we need to process
                for (uint8_t i = 0; i < _num_event_handlers; i++) {
                    if (data.endsWith(_event_handlers[i]->match_string)) {
                        _event_handlers[i]->process(this);
                        _events_urc_queue.pushEnd(_event_handlers[i]->urc);
                    }
                }

                if (match_1 != NULL && data.endsWith(match_1) == true)
                    return Response_t::A76XX_RESPONSE_MATCH_1ST;

                if (match_2 != NULL && data.endsWith(match_2) == true)
                    return Response_t::A76XX_RESPONSE_MATCH_2ND;

                if (match_3 != NULL && data.endsWith(match_3) == true)
                    return Response_t::A76XX_RESPONSE_MATCH_3RD;

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
        return waitResponse(match_1, match_2, NULL, timeout, match_OK, match_ERROR);
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
        return waitResponse(match_1, NULL, NULL, timeout, match_OK, match_ERROR);
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
        return waitResponse(NULL, NULL, NULL, timeout, match_OK, match_ERROR);
    }

    A76XXURC_t listen(uint32_t timeout = 100) {
        if (_events_urc_queue.available() == 0) {
            waitResponse(timeout, false, false);
        }

        if (_events_urc_queue.available() > 0) {
            return _events_urc_queue.popFront();
        }

        return A76XXURC_t::NONE;
    }

    void registerEventHandler(EventHandler_t* handler) {
        _event_handlers[_num_event_handlers++] = handler;
    }

    /*
        @brief Send data to the command to the module, with a trailing carriage return,
            line feed characters.

        @param [IN] args Items (string, numbers, ...) to be sent.
        @param [IN] finish Whether to send a terminating "\r\n" to the module.
    */
    template <typename... ARGS>
    void sendCMD(ARGS... args) {
        printCMD(args..., "\r\n");
    }

    /*
        @brief Print data to the module's serial port.

        @detail This is analogus to ::sendCMD, but without the trailing carriage return, 
            line feed characters.

        @param [IN] args Items (string, numbers, ...) to be sent.
        @param [IN] finish Whether to send a terminating "\r\n" to the module.
    */
    template <typename HEAD, typename... TAIL>
    void printCMD(HEAD head, TAIL... tail) {
        _stream.print(head); 
        flush();
        printCMD(tail...);
    }

    // single argument case
    template <typename ARG>
    void printCMD(ARG arg) {
        _stream.print(arg); 
        flush();
    }

    /*
        @brief Parse an integer number and then consume all data available in the 
            serial interface until the default OK or ERROR strings are found, or 
            until the operation times out.

        @param [IN] timeout Time out in milliseconds. Default is 50 milliseconds.
        @return The integer.
    */
    int32_t parseIntClear(uint32_t timeout = 500) {
        int32_t retcode = parseInt();
        clear(timeout);
        return retcode;
    }

    /*
        @brief Consume all data available in the stream, until the default
            OK or ERROR strings are found, or until the operation times out.
            
        @param [IN] timeout Time out in milliseconds. Default is 50 milliseconds.
    */ 
    void clear(uint32_t timeout = 500) {
        // TODO: this should not be used anywhere!
        waitResponse(timeout);
    }

    // The following functions are simply forwarding the calls to underlying stream 
    // object. If you need others, send a pull request!
    int available() {
        return _stream.available(); 
    }

    long parseInt() { 
        return _stream.parseInt(); 
    }

    void flush() { 
        _stream.flush(); 
    }

    uint16_t read() { 
        return _stream.read(); 
    }

    template <typename ARG>
    bool find(ARG arg) { 
        return _stream.find(arg); 
    }

    template <typename... ARGS>
    uint16_t write(ARGS... args) { 
        return _stream.write(args...); 
    }

    template <typename... ARGS>
    uint16_t readBytesUntil(ARGS... args) { 
        return _stream.readBytesUntil(args...); 
    }

    template <typename... ARGS>
    uint16_t readBytes(ARGS... args) { 
        return _stream.readBytes(args...); 
    }
};


#endif A76XX_MODEMUART_H_