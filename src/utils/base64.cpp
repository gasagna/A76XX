#include "A76XX.h"

void encodeBase64(char* input, uint32_t length, char* output) {
    while (length >= 3) {
        uint8_t b1 = *(input + 0);
        uint8_t b2 = *(input + 1);
        uint8_t b3 = *(input + 2);

        *(output + 0) = __SYMBOLS__[  b1 >> 2 ];
        *(output + 1) = __SYMBOLS__[((b1 & 0b00000011) << 4) + (b2 >> 4)];
        *(output + 2) = __SYMBOLS__[((b2 & 0b00001111) << 2) + (b3 >> 6)];
        *(output + 3) = __SYMBOLS__[  b3 & 0b00111111 ];
        
        length -= 3; input += 3; output += 4;
    }

    if (length == 2) {
        uint8_t b1 = *(input + 0);
        uint8_t b2 = *(input + 1);

        *(output + 0) = __SYMBOLS__[  b1 >> 2 ];
        *(output + 1) = __SYMBOLS__[((b1 & 0b00000011) << 4) + (b2 >> 4)];
        *(output + 2) = __SYMBOLS__[((b2 & 0b00001111) << 2)];
        *(output + 3) = '=';
    }

    if (length == 1) {
        uint8_t b1 = *(input + 0);

        *(output + 0) = __SYMBOLS__[  b1 >> 2 ];
        *(output + 1) = __SYMBOLS__[ (b1 & 0b00000011) << 4];
        *(output + 2) = '=';
        *(output + 3) = '=';
    }
}