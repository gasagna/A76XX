#include "A76XX.h"

A76XX::A76XX(Stream& stream)
    : serial(stream)
    , _last_error_code(0)
    , internetService(serial)
    , network(serial)
    , packetDomain(serial)
    , serialInterface(serial)
    , sim(serial)
    , statusControl(serial)
    , v25ter(serial) {}

int8_t A76XX::getLastError() {
    return _last_error_code;
}

bool A76XX::init(const char* pincode, uint32_t timeout) {
    int8_t retcode;

    // wait until modem is ready
    if (waitATResponsive(timeout) == false) {
        return false;
    }

    // turn off echoing commands
    retcode = v25ter.commandEcho(false);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode)

    // disable reporting mobile equipment errors with numeric values
    retcode = statusControl.setErrorResultCodes(0);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode)

    // disable unsolicited codes for time zone change
    retcode = network.setTimeZoneURC(false);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode)

    // enable automatic time and time zone updates via NITZ
    retcode = network.setTimeZoneAutoUpdate(true);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode)

    PINStatus_t status;
    retcode = sim.getPINStatus(status);
    // catch errors with the command
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode)

    switch (status) {
        case PINStatus_t::READY : {
            break;
        }
        // some error occurred in reading the response
        case PINStatus_t::UKNOWN : {
            _last_error_code = A76XX_SIM_PIN_MODEM_ERROR;
            return false;
        }
        // all other cases where PIN is required
        default : {
            if (pincode == NULL) {
                _last_error_code = A76XX_SIM_PIN_REQUIRED;
                return false;
            }
            retcode = sim.enterPIN(pincode);
            A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode)
        }
    }

    return true;
}

bool A76XX::GPRSConnect(const char* apn) {
    int8_t retcode;

    retcode = packetDomain.setPDPContextParameters(1, apn);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode)

    retcode = packetDomain.setPDPContextActiveStatus(1, true);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode)

    return true;
}

bool A76XX::isGPRSConnected() {
    int8_t status;
    _last_error_code = packetDomain.getPDPContextActiveStatus(1, status);
    return (_last_error_code == A76XX_OPERATION_SUCCEEDED && status == 1) ? true : false;
}

int8_t A76XX::getRegistrationStatus(uint8_t net) {
    int8_t status;
    switch (net) {
        case 0 : { // circuit switched network - CREG
            _last_error_code = network.getNetworkRegistrationStatus(status);
            break;
        }
        case 1 : { // packet network - GPRS - CGREG
            _last_error_code = packetDomain.getGPRSNetworkRegistrationStatus(status);
            break;
        }
        case 2 : { // packet network - LTE - CEREG
            _last_error_code = packetDomain.getLTENetworkRegistrationStatus(status);
            break;
        }
    }
    return _last_error_code == A76XX_OPERATION_SUCCEEDED ? status : -1;
}

bool A76XX::isRegistered(uint8_t net) {
    int8_t stat = getRegistrationStatus(net);
    return (stat == 1 || stat == 5);
}

bool A76XX::waitForRegistration(uint8_t net, uint32_t timeout) {
    uint32_t tstart = millis();
    while (millis() - tstart < timeout) {
        if (isRegistered(net) == true)
            return true;
        delay(200);
    }
    return false;
}

int8_t A76XX::getNetworkSystemMode() {
    int8_t mode;
    _last_error_code = network.getNetworkSystemMode(mode);
    return _last_error_code == A76XX_OPERATION_SUCCEEDED ? mode : -1;
}

bool A76XX::reset(uint32_t timeout) {
    if (statusControl.reset() != A76XX_OPERATION_SUCCEEDED) {
        return false;
    }
    if (waitATUnresponsive(timeout) == false) {
        return false;
    }
    if (waitATResponsive(timeout) == false) {
        return false;
    }
    return true;
}

bool A76XX::powerOff(uint32_t timeout) {
    if (statusControl.powerOff() != A76XX_OPERATION_SUCCEEDED) {
        return false;
    }
    // only return error if we wanted to wait 
    if (timeout > 0 && waitATUnresponsive(timeout) == false) {
        return false;
    }
    return true;
}

bool A76XX::setPhoneFunctionality(uint8_t fun, bool reset) {
    int8_t retcode = statusControl.setPhoneFunctionality(fun, reset);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
    return true;
}

bool A76XX::radioOFF() {
    return setPhoneFunctionality(4, false);
}

bool A76XX::radioON()  {
    return setPhoneFunctionality(1, false);
}

bool A76XX::restart(uint32_t timeout) {
    if (reset(timeout) == false) {  return false; }
    return init();
}

bool A76XX::waitATUnresponsive(uint32_t timeout) {
    uint32_t tstart = millis();
    while (millis() - tstart < timeout) {
        serial.sendCMD("AT");
        if (serial.waitResponse(1000) == Response_t::A76XX_RESPONSE_TIMEOUT) {
            return true;
        }
        delay(100);
    }
    return false;
}

bool A76XX::waitATResponsive(uint32_t timeout) {
    uint32_t tstart = millis();
    while (millis() - tstart < timeout) {
        serial.sendCMD("AT");
        if (serial.waitResponse(1000) == Response_t::A76XX_RESPONSE_OK) {
            return true;
        }
        delay(100);
    }
    return false;
}

bool A76XX::sleep() {
    int8_t retcode = serialInterface.UARTSleep(2);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
    return true;
}

bool A76XX::wakeUp() {
    serial.sendCMD("AT");
    return serial.waitResponse() == Response_t::A76XX_RESPONSE_OK;
}

String A76XX::modelIdentification() {
    String out;
    _last_error_code = v25ter.modelIdentification(out);
    return out;
}

String A76XX::revisionIdentification() {
    String out;
    _last_error_code = v25ter.revisionIdentification(out);
    return out;
}

bool A76XX::syncTime(int8_t timezone, uint32_t timeout, const char* host) {
    int8_t retcode = internetService.setNTPParams(host, timezone);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    retcode = internetService.updateSystemTime(timeout);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
    return true;
}

String A76XX::getDateTime() {
    char dateTime[] = "yy/MM/dd,hh:mm:ss+zz";
    _last_error_code = statusControl.getDateTime(dateTime);
    return dateTime;
}

bool A76XX::getDateTime(int* year,
                        int* month,
                        int* day,
                        int* hour,
                        int* minute,
                        int* second,
                        int* timezone) {
    char dateTime[] = "00/00/00,00:00:00+00";
    _last_error_code = statusControl.getDateTime(dateTime);
    if (_last_error_code == A76XX_OPERATION_SUCCEEDED) {
        sscanf(dateTime, "%d/%d/%d,%d:%d:%d%d", year, month, day, hour, minute, second, timezone);
        *year += 2000;
        return true;
    }
    return false;
}

uint32_t A76XX::getUnixTime(bool UTC) {
    int year, month, day, hour, minute, second, timezone;
    if (getDateTime(&year, &month, &day, &hour, &minute, &second, &timezone) == false) {
        return 0;
    }

    struct tm ts = {0};         // Initalize to all 0's
    ts.tm_year = year - 1900;   // This is year-1900, so for 2023 => 123
    ts.tm_mon  = month - 1;     // january is 0, not 1
    ts.tm_mday = day;
    ts.tm_hour = hour;
    ts.tm_min  = minute;
    ts.tm_sec  = second;

    uint32_t time = mktime(&ts);
    if (UTC == true) {
        time -= static_cast<uint32_t>(timezone*15*60);
    }
    return time;
}

A76XXURC_t A76XX::listen(uint32_t timeout) {
    return serial.listen(timeout);
}