#include "A76XX.h"

class A76XXCOAPClient : public A76XXBaseClient {
  private:
    COAPCommands   _coap_cmds;
    uint8_t       _session_id;

  public:
    A76XXCOAPClient(A76XX& modem) 
        : A76XXBaseClient(modem)
        , _coap_cmds(_serial) {}

    bool begin() {
        int8_t retcode = _coap_cmds.start();
        A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
        return true;
    }

    bool end() {
        int8_t retcode = _coap_cmds.stop();
        A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
        return true;
    }

    bool connect(const char* server_name, uint16_t server_port) {
        int8_t retcode = _coap_cmds.connect(server_name, server_port, _session_id);
        A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
        return true;
    }

    bool disconnect() {
        int8_t retcode = _coap_cmds.disconnect(_session_id);
        A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
        return true;
    }
};