const char __SYMBOLS__[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*
    @brief Encode a binary string in base 64.

    @param [IN] input Pointer to the input string to be encoded.
    @param [IN] length The length of the input string.
    @param [OUT] output Pointer to the output string. It is assumed that it points
        to a memory location of sufficient space. For a string with `n` characters to 
        be encoded, the required output length is `4 * ((n/3) + 1)`.
*/
void encodeBase64(char* input,  uint32_t length, char* output);