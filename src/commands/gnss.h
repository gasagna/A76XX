#ifndef A76XX_GNSS_CMDS_H_
#define A76XX_GNSS_CMDS_H_

/*
    @brief Commands in section 24 of the AT command manual version 1.09

    Command         | Implemented | Method     | Function(s)
    --------------- | ----------- | ---------- | -----------
    CGNSSPWR        |      y      |     W      | powerControl
    CGPSCOLD        |      y      |     E      | coldStart
    CGPSWARM        |      y      |     E      | warmStart
    CGPSHOT         |      y      |     E      | hotStart
    CGNSSIPR        |      y      |     W      | setUART3BaudRate
    CGNSSMODE       |      y      |     W      | setSupportMode
    CGNSSNMEA       |      y      |     W      | setNMEASentence
    CGPSNMEARATE    |      y      |     W      | setNMEARate
    CGPSFTM         |      y      |     W      | startTestMode, stopTestMode
    CGPSINFO        |      y      |     E      | getGPSInfo
    CGNSSINFO       |      y      |     E      | getGNSSInfo
    CGNSSCMD        |             |            |
    CGNSSTST        |      y      |     W      | enableNMEAOutput
    CGNSSPORTSWITCH |      y      |     W      | selectOutputPort
    CAGPS           |      y      |     E      | getAGPSData
    CGNSSPROD       |      y      |     E      | getGPSProductInfo
*/

struct GNSSInfo_t {
    int   mode;         // Fix mode 2=2D fix 3=3D fix
    int   GPS_SVs;      // GPS satellite visible numbers
    int   GLONASS_SVs;  // GLONASS satellite visible numbers
    int   BEIDOU_SVs;   // BEIDOU satellite visible numbers
    float lat;          // Latitude of current position. Output format is dd.ddddd
    char  NS;           // N/S Indicator, N=north or S=south.
    float lon;          // Longitude of current position. Output format is ddd.ddddd 
    char  EW;           // E/W Indicator, E=east or W=west.
    char  date[7];      // Date. Output format is ddmmyy.
    char  UTC_TIME[10]; // UTC Time. Output format is hhmmss.ss.
    float alt;          // MSL Altitude. Unit is meters.
    float speed;        // Speed Over Ground. Unit is knots.
    float course;       // Course. Degrees.
    float PDOP;         // Position Dilution Of Precision.
    float HDOP;         // Horizontal Dilution Of Precision.
    float VDOP;         // Vertical Dilution Of Precision.
};

struct GPSInfo_t {
    float lat;          // Latitude of current position. Output format is dd.ddddd
    char  NS;           // N/S Indicator, N=north or S=south.
    float lon;          // Longitude of current position. Output format is ddd.ddddd 
    char  EW;           // E/W Indicator, E=east or W=west.
    char  date[7];      // Date. Output format is ddmmyy.
    char  UTC_TIME[10]; // UTC Time. Output format is hhmmss.ss.
    float alt;          // MSL Altitude. Unit is meters.
    float speed;        // Speed Over Ground. Unit is knots.
    float course;       // Course. Degrees.
};

class GNSSCommands {
  public:
    ModemSerial& _serial;

    GNSSCommands(ModemSerial& serial)
        : _serial(serial) {}

    /*
        @brief Implementation for CGNSSPWR - Write Command.
        @detail GNSS power control.
        @param [IN] enable_GNSS Set to true to power on the GNSS module.
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT, A76XX_GENERIC_ERROR 
            or A76XX_GNSS_NOT_READY.
    */
    int8_t powerControl(bool enable_GNSS) {
        _serial.sendCMD("AT+CGNSSPWR=", enable_GNSS == true ? 1 : 0);
        switch (_serial.waitResponse("+CGNSSPWR: READY!", 9000)) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                return A76XX_OPERATION_SUCCEEDED;
            }
            case Response_t::A76XX_RESPONSE_OK : {
                return A76XX_GNSS_NOT_READY;
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
        @brief Implementation for CGPSCOLD - Write Command.
        @detail Cold start GPS.
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR.
    */
    int8_t coldStart() { return start("COLD"); }

    /*
        @brief Implementation for CGPSWARM - Write Command.
        @detail Warm start GPS.
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR.
    */
    int8_t warmStart() { return start("WARM"); }

    /*
        @brief Implementation for CGPSHOT - Write Command.
        @detail Hot start GPS.
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR.
    */
    int8_t hotStart() { return start("HOT"); }

    /*
        @brief Implementation for CGNSSIPR - Write Command.
        @detail Configure the baud rate of UART3 and GPS module.
        @param [IN] baud_rate The baud rate to set.
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR.
    */
    int8_t setUART3BaudRate(uint32_t baud_rate) {
        _serial.sendCMD("AT+CGNSSIPR=", baud_rate);
        switch (_serial.waitResponse(9000)) {
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
        @brief Implementation for CGNSSMODE - Write Command.
        @detail Configure GNSS support mode.
        @param [IN] mode The suport mode, from 1 to 7.
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR.
    */
    int8_t setSupportMode(uint8_t mode) {
        _serial.sendCMD("AT+CGNSSMODE=", mode);
        switch (_serial.waitResponse(9000)) {
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
        @brief Implementation for CGNSSNMEA - Write Command.
        @detail Configure NMEA sentence type. A mask can be constructed by `|`-ing
            several of the A76XX_GNSS_nXXX flags. For instance, to only output GGA 
            and RMC NMEA sentences set nmea_mask to A76XX_GNSS_nGGA | A76XX_GNSS_nRMC.
        @param [IN] nmea_mask An 8 bit mask to select the sentences to output.
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR.
    */
    int8_t setNMEASentence(uint8_t nmea_mask) {
        uint8_t nGGA = (nmea_mask & A76XX_GNSS_nGGA) >> 0;
        uint8_t nGLL = (nmea_mask & A76XX_GNSS_nGLL) >> 1;
        uint8_t nGSA = (nmea_mask & A76XX_GNSS_nGSA) >> 2; 
        uint8_t nGSV = (nmea_mask & A76XX_GNSS_nGSV) >> 3;
        uint8_t nRMC = (nmea_mask & A76XX_GNSS_nRMC) >> 4; 
        uint8_t nVTG = (nmea_mask & A76XX_GNSS_nVTG) >> 5;
        uint8_t nZDA = (nmea_mask & A76XX_GNSS_nZDA) >> 6; 
        uint8_t nGST = (nmea_mask & A76XX_GNSS_nGST) >> 7;
        _serial.sendCMD("AT+CGNSSNMEA=", nGGA, ",", nGLL, ",", nGSA, ",", nGSV, ",",
                                         nRMC, ",", nVTG, ",", nZDA, ",", nGST);
        switch (_serial.waitResponse(9000)) {
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
        @brief Implementation for CGNSSNMEA - Write Command.
        @detail Set NMEA output rate.
        @param [IN] nmea_rate Rate in outputs per second - 1, 2, 4, 5 or 10
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR.
    */
    int8_t setNMEARate(uint8_t nmea_rate) {
        _serial.sendCMD("AT+CGPSNMEARATE=", nmea_rate);
        switch (_serial.waitResponse(9000)) {
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
        @brief Implementation for CGPSFTM - Write Command.
        @detail Start GPS test mode.
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR.
    */
    int8_t startTestMode() {
        _serial.sendCMD("AT+CGPSFTM=1");
        switch (_serial.waitResponse(9000)) {
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
        @brief Implementation for CGPSFTM - Write Command.
        @detail Stop GPS test mode.
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR.
    */
    int8_t stopTestMode() {
        _serial.sendCMD("AT+CGPSFTM=0");
        switch (_serial.waitResponse(9000)) {
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
        @brief Implementation for CGNSSINFO - Write Command.
        @detail Get GNSS fixed position information.
        @param [OUT] info A GNSSInfo_t structure.
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR.
    */
    int8_t getGNSSInfo(GNSSInfo_t& info) {
        _serial.sendCMD("AT+CGNSSINFO");
        switch (_serial.waitResponse("+CGNSSINFO:", 9000, false, true)) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                // when we do not have a fix
                if (_serial.peek() == ',') { return A76XX_GNSS_NO_FIX; }
                info.mode        = _serial.parseInt();   _serial.find(',');
                info.GPS_SVs     = _serial.parseInt();   _serial.find(',');
                info.GLONASS_SVs = _serial.parseInt();   _serial.find(',');
                info.BEIDOU_SVs  = _serial.parseInt();   _serial.find(',');
                info.lat         = _serial.parseFloat(); _serial.find(',');
                info.NS          = _serial.read();       _serial.find(',');
                info.lon         = _serial.parseFloat(); _serial.find(',');
                info.EW          = _serial.read();       _serial.find(',');
                _serial.readBytes(info.date, 6);         _serial.find(',');
                _serial.readBytes(info.UTC_TIME, 9);     _serial.find(',');
                info.alt         = _serial.parseFloat(); _serial.find(',');
                info.speed       = _serial.parseFloat(); _serial.find(',');
                info.course      = _serial.parseFloat(); _serial.find(',');
                info.PDOP        = _serial.parseFloat(); _serial.find(',');
                info.HDOP        = _serial.parseFloat(); _serial.find(',');
                info.VDOP        = _serial.parseFloat();
                // get last OK
                if (serial.waitResponse(9000) == Response_t::A76XX_RESPONSE_OK) {
                    return A76XX_OPERATION_SUCCEEDED;
                } else {
                    return A76XX_GENERIC_ERROR;
                }
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
        @brief Implementation for CGPSSINFO - Write Command.
        @detail Get GPS fixed position information.
        @param [OUT] info A GPSInfo_t structure.
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR.
    */
    int8_t getGPSInfo(GPSInfo_t& info) {
        _serial.sendCMD("AT+CGPSINFO");
        switch (_serial.waitResponse("+CGPSINFO:", 9000, false, true)) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                // when we do not have a fix
                if (_serial.peek() == ',') { return A76XX_GNSS_NO_FIX; }
                info.lat         = _serial.parseFloat(); _serial.find(',');
                info.NS          = _serial.read();       _serial.find(',');
                info.lon         = _serial.parseFloat(); _serial.find(',');
                info.EW          = _serial.read();       _serial.find(',');
                _serial.readBytes(info.date, 6);         _serial.find(',');
                _serial.readBytes(info.UTC_TIME, 9);     _serial.find(',');
                info.alt         = _serial.parseFloat(); _serial.find(',');
                info.speed       = _serial.parseFloat(); _serial.find(',');
                info.course      = _serial.parseFloat();
                // get last OK
                if (serial.waitResponse(9000) == Response_t::A76XX_RESPONSE_OK) {
                    return A76XX_OPERATION_SUCCEEDED;
                } else {
                    return A76XX_GENERIC_ERROR;
                }
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
        @brief Implementation for CGNSSTST - Write Command.
        @detail Send data received from GNSS module to the serial port.
        @param [IN] enabled Whether to send NMEA data to the serial port.
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR.
    */
    int8_t enableNMEAOutput(bool enabled) {
        _serial.sendCMD("AT+CGNSSTST=", enabled == true ? "1" : "0");
        switch (_serial.waitResponse(9000)) {
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
        @brief Implementation for CGNSSPORTSWITCH - Write Command.
        @detail Select the output port for NMEA sentence.
        @param [IN] output_parsed_data If true, output parsed data to the serial port.
        @param [IN] output_nmea_data If true, output raw nmea data to the serial port.
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR.
    */
    int8_t selectOutputPort(bool output_parsed_data, bool output_nmea_data) {
        _serial.sendCMD("AT+CGNSSPORTSWITCH=", 
                        output_parsed_data == true ? "1" : "0", ",",
                        output_nmea_data   == true ? "1" : "0");
        switch (_serial.waitResponse(9000)) {
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
        @brief Implementation for CAGPS - EXEC Command.
        @detail Get AGPS data from the AGNSS server for assisted positioning.
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR.
    */
    int8_t getAGPSData() {
        _serial.sendCMD("AT+CAGPS");
        switch (_serial.waitResponse("+AGPS: ", 9000, false, true)) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                // when the output is "+AGPS: success"
                if (_serial.peek() == 's') {
                    _serial.clear();
                    return A76XX_OPERATION_SUCCEEDED;
                }
                // error case
                return _serial.parseIntClear();
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
        @brief Implementation for CGNSSPROD - EXEC Command.
        @detail Get GNSS receiver product information.
        @return A76XX_OPERATION_SUCCEEDED, A76XX_OPERATION_TIMEDOUT or A76XX_GENERIC_ERROR.
    */
    int8_t getGPSProductInfo(const char* info) {
        _serial.sendCMD("AT+CGNSSPROD");
        switch (_serial.waitResponse("PRODUCT: ", 9000, false, true)) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                _serial.readBytesUntil(info, '\r');
                _serial.clear();
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

  private:
    int8_t start(const char* start_type) {
        _serial.sendCMD("AT+CGPS", start_type);
        switch (_serial.waitResponse(9000)) {
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


};

#endif A76XX_GNSS_CMDS_H_