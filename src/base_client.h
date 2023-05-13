#ifndef A76XX_BASE_CLIENT_H_
#define A76XX_BASE_CLIENT_H_

/*
    @brief Basic error handling functionality.
*/
class A76XXBaseClient {
  protected:
    // last error returned by any AT command
    int8_t  _last_error_code;

  public:
    /*
        @brief Constructor.

        @param [IN] FIXME:
    */
    A76XXBaseClient();

    /*
        @brief Get last error raise by any AT command.
    */
    int8_t getLastError();
};

#endif A76XX_BASE_CLIENT_H_