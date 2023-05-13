#include "A76XX.h"

A76XXBaseClient::A76XXBaseClient(A76XX* modem) 
    , _last_error_code(0) {}

int8_t A76XXBaseClient::getLastError() {
    return _last_error_code;
}