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
    CGNSSIPR        |             |            |
    CGNSSMODE       |      y      |            | setSupportMode
    CGNSSNMEA       |             |            |
    CGPSNMEARATE    |      y      |            | setNMEARate
    CGPSFTM         |             |            |
    CGPSINFO        |             |            |
    CGNSSINFO       |      y      |            | getGNSSInfo
    CGNSSCMD        |             |            |
    CGNSSTST        |             |            |
    CGNSSPORTSWITCH |             |            |
    CAGPS           |             |            |
    CGNSSPROD       |             |            |
*/

struct GNSSInfo_t {
    int   mode,        // Fix mode 2=2D fix 3=3D fix
    int   GPS_SVs,     // GPS satellite visible numbers
    int   GLONASS_SVs, // GLONASS satellite visible numbers
    int   BEIDOU_SVs,  // BEIDOU satellite visible numbers
    float lat          // Latitude of current position. Output format is dd.ddddd
    char  NS           // N/S Indicator, N=north or S=south.
    float lon          // Longitude of current position. Output format is ddd.ddddd 
    char  EW           // E/W Indicator, E=east or W=west.
    char  date[7],     // Date. Output format is ddmmyy.
    char  UTC_TIME[10],// UTC Time. Output format is hhmmss.ss.
    float alt,         // MSL Altitude. Unit is meters.
    float speed,       // Speed Over Ground. Unit is knots.
    float course,      // Course. Degrees.
    float PDOP,        // Position Dilution Of Precision.
    float HDOP,        // Horizontal Dilution Of Precision.
    float VDOP         // Vertical Dilution Of Precision.
};

class GNSSCommands {
  public:
    ModemSerial& _serial;

    GNSSCommands(ModemSerial& serial)
        : _serial(serial) {}

    int8_t powerControl(bool enableGNSS) {
        _serial.sendCMD("AT+CGNSSPWR=", enableGNSS == true ? 1 : 0);
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

    int8_t coldStart() { return start("COLD"); }
    int8_t warmStart() { return start("WARM"); }
    int8_t hotStart()  { return start("HOT");  }

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

    int8_t setNMEARate(uint8_t rate) {
        _serial.sendCMD("AT+CGPSNMEARATE=", rate);
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

    int8_t getGNSSInfo(GNSSInfo_t& info) {
        _serial.sendCMD("AT+CGNSSINFO");
        switch (_serial.waitResponse("+CGNSSINFO:", 9000, false, true)) {
            case Response_t::A76XX_RESPONSE_MATCH_1ST : {
                // when we do not have a fix
                if (_serial.peek() == ',') { return A76XX_GNSS_NO_FIX; }

                // parse all
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