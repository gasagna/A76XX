#include "A76XX.h"

// determined by the length of the longest input string in the examples below
char output[37];

void setup() {
    Serial.begin(115200); delay(5000);

    encodeBase64("Many hands make light work.", 27, output);
    Serial.println(output); // should be "TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdvcmsu"
    memset(output, ' ', sizeof(output));

    encodeBase64("hello:world", 11, output);
    Serial.println(output); // should be "aGVsbG86d29ybGQ="
    memset(output, ' ', sizeof(output));

    encodeBase64("Man", 3, output);
    Serial.println(output); // should be "TWFu"
    memset(output, ' ', sizeof(output));

    encodeBase64("Ma",  2, output);
    Serial.println(output); // should be "TWE="
    memset(output, ' ', sizeof(output));

    encodeBase64("M",   1, output);
    Serial.println(output); // should be "TQ=="
    memset(output, ' ', sizeof(output));

}

void loop() {}