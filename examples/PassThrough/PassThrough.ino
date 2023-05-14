// configuration for serial port to simcom module (check your board!)
#define PIN_TX   26
#define PIN_RX   27

// Use the correct `Serial` object to connect to the simcom module
#define SerialAT Serial1

void setup() {
    Serial.begin(115200);
    SerialAT.begin(115200, SERIAL_8N1, PIN_RX, PIN_TX);
    delay(3000);
}

void loop() {
    if (Serial.available()) {
        SerialAT.write(Serial.read());
    }

    if (SerialAT.available()) {
        Serial.write(SerialAT.read());
    }
}
