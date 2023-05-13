#include "A76XX.h"

A76XX::A76XX(Stream& stream)
    : _stream(stream)
    , _last_error_code(0) {
        internetService._modem = this;
        network._modem = this;
        packetDomain._modem = this;
        serialInterface._modem = this;
        sim._modem = this;
        statusControl._modem = this;
        v25ter._modem = this;
    }

bool A76XX::init() {
    int8_t retcode;

    // wait until modem is ready
    PINStatus_t status = PINStatus_t::UKNOWKN;
    while (status != PINStatus_t::READY) {
        sim.getPINStatus(status);
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

    return true;
}

bool A76XX::connect(const char* apn, const char* username, const char* password) {
    int8_t retcode;

    retcode = packetDomain.setPDPContextParameters(1, apn);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode)

    retcode = packetDomain.setPDPContextActiveStatus(1, true);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode)

    return true;
}

bool A76XX::disconnect() {
    int8_t retcode = packetDomain.setPDPContextActiveStatus(1, false);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode)
    return true;
}

bool A76XX::isConnected() {
    int8_t status;
    _last_error_code = packetDomain.getPDPContextActiveStatus(1, status);
    return (_last_error_code == A76XX_OPERATION_SUCCEEDED && status == 1) ? true : false;
}

int8_t A76XX::getRegistrationStatus() {
    int8_t status;
    _last_error_code = network.getNetworkRegistrationStatus(status);
    return _last_error_code == A76XX_OPERATION_SUCCEEDED ? status : -1;
}

bool A76XX::isRegistered() {
    int8_t stat = getRegistrationStatus();
    return (stat == 1 | stat == 5);
}

bool A76XX::waitForRegistration(uint32_t timeout) {
    uint32_t tstart = millis();
    while (millis() - tstart < timeout) {
        if (isRegistered() == true)
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

bool A76XX::reset() {
    int8_t retcode = statusControl.reset();
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
    return true;
}

bool A76XX::powerOff() {
    int8_t retcode = statusControl.powerOff();
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
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

bool A76XX::restart() {
    if (reset() == false) {
        return false;
    }

    // test the serial interface until it stops responding within 1 second
    // but give up if the module has not properly reset within 30 seconds
    uint32_t tstart = millis();
    while (millis() - tstart < 30000) {
        sendCMD("AT");
        if (waitResponse(1000) == Response_t::A76XX_RESPONSE_TIMEOUT) {
            return init();
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
    sendCMD("AT");
    return waitResponse() == Response_t::A76XX_RESPONSE_OK;
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

bool A76XX::syncTime(const char* host, int8_t timezone) {
    int8_t retcode = internetService.setNTPParams(host, timezone);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    retcode = internetService.updateSystemTime();
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


Response_t A76XX::waitResponse(const char* match_1,
                               const char* match_2,
                               int timeout,
                               bool match_OK,
                               bool match_ERROR) {
    // store data here
    String data; data.reserve(64);

    // start timer
    auto tstart = millis();

    while (millis() - tstart < timeout) {
        if (streamAvailable() > 0) {
            data += static_cast<char>(streamRead());
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

Response_t A76XX::waitResponse(const char* match_1, 
                               int timeout,
                               bool match_OK,
                               bool match_ERROR) {
    return waitResponse(match_1,
                        "_ThIs_Is_AlMoSt_NeVeR_GoNnA_MaTcH_2_",
                        timeout, match_OK, match_ERROR);
}

Response_t A76XX::waitResponse(int timeout,
                               bool match_OK,
                               bool match_ERROR) {
    return waitResponse("_ThIs_Is_AlMoSt_NeVeR_GoNnA_MaTcH_1_",
                        "_ThIs_Is_AlMoSt_NeVeR_GoNnA_MaTcH_2_",
                        timeout, match_OK, match_ERROR);
}

int32_t A76XX::streamParseIntClear(uint32_t timeout) {
    int32_t retcode = streamParseInt();
    streamClear(timeout);
    return retcode;
}

void A76XX::streamClear(uint32_t timeout) {
    waitResponse(timeout);
}

int A76XX::streamAvailable() { 
    return _stream.available(); 
}

long A76XX::streamParseInt() { 
    return _stream.parseInt(); 
}

void A76XX::streamFlush() { 
    _stream.flush(); 
}

uint16_t A76XX::streamRead() { 
    return _stream.read(); 
}
