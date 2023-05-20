#include <StreamDebugger.h>
#include <A76XX.h>

// dump all communication with the module to the standard serial port
#define DEBUG_AT false

// Use the correct `Serial` object to connect to the simcom module
#if DEBUG_AT
    StreamDebugger SerialAT(Serial1, Serial);
#else
    #define SerialAT Serial1
#endif

// MQTT detailed
const char* clientID      = "test_client";
const char* server        = "test.mosquitto.org";
const int   port          = 1883;
const int   keepalive     = 60;
const bool  clean_session = true;
const bool  use_ssl       = false;

// replace with your apn
const char* apn           = "simbase";

A76XX modem(SerialAT);
A76XXMQTTClient mqtt(modem, clientID, use_ssl);

// configuration for serial port to simcom module (check your board!)
#define PIN_TX   26
#define PIN_RX   27

// queue for intertask communication
QueueHandle_t queue;

// This function is an example of what the processing task might do. We simply 
// wait forever for MQTT messages to appear in the queue and we then simply
// print them on the serial port. Other logic can be implemented too.
// The important thing not to forget is to use vTaskDelay as often as possible
// to avoid triggering the ESP32 watchdog.
void processMQTTMessage(void* parameter) {
    MQTTMessage_t msg;
    while (true) {
        while (uxQueueMessagesWaiting(queue) > 0) {
            xQueueReceive(queue, &msg, 0);
            Serial.println("Received message ...");
            Serial.print("  topic: "); Serial.println(msg.topic);
            Serial.print("  payload: "); Serial.println(msg.payload);

            // allow other tasks on CPU0 to run (e.g. petting the watchdog), plus
            // simulate some delay which might result in some messages being dropped
            vTaskDelay(500);
        }
        vTaskDelay(100);
    }
}

void setup() {
    // begin serial port
    Serial.begin(115200);

    // must begin UART communicating with the SIMCOM module
    Serial1.begin(115200, SERIAL_8N1, PIN_RX, PIN_TX);

    // wait a little so we can see the output
    delay(3000);

    Serial.print("Waiting for modem ... ");
    if (modem.init() == false) {
        Serial.println("error");
        while (true) {}
    }
    Serial.println("OK");

    Serial.print("Waiting for modem to register on network ... ");
    if (modem.waitForRegistration() == false) {
        Serial.println("registration timed out");
        while (true) {}
    }
    Serial.println("done");

    Serial.print("Connecting  ... ");
    if (modem.GPRSConnect(apn) == false){
        Serial.println("cannot connect");
        while (true) {}
    }
    Serial.println("connected");

    Serial.print("Starting client  ... ");
    if (mqtt.begin() == false) {
        Serial.println("error");
        while (true) {}
    }
    Serial.println("done");

    Serial.print("Connecting to mosquitto test server  ... ");
    if (mqtt.connect(server, port, clean_session, keepalive) == false) {
        Serial.println("error");
        while (true) {}
    }
    Serial.println("done");

    Serial.print("Subscribe to topic  ... ");
    if (mqtt.subscribe("_this_is_a_test_topic_") == false) {
        Serial.println("error");
        while (true) {}
    }
    Serial.println("done");

    // Create a queue that can store up to 20 messages
    queue = xQueueCreate(20, sizeof(MQTTMessage_t));

    // Create task on another CPU to processes packets in the queue
    xTaskCreatePinnedToCore(processMQTTMessage,   /* Task function. */
                            "processMQTTMessage", /* String with name of task. */
                            10000,                /* Stack size in words. */
                            NULL,                 /* Parameter passed as input of the task */
                            1,                    /* Priority of the task. */
                            NULL,                 /* Task handle. */
                            0);                   /* CPU Core. */
}

void loop() {
    // main loop – we parse messages as fast as we can from the serial interface
    // with the SIMCOM module, and put them in a queue for the processing task
    // running on another core. If messages are received more quickly than the
    // processing task can process them, some messages might get dropped. Creating
    // a larger queue that can store more messages might alleviate such issues.
    A76XXURC_t urc = modem.listen(1000);
    
    switch (urc) {
        case A76XXURC_t::MQTT_HAS_MESSAGE : {
            MQTTMessage_t msg = modem.MQTT_GetMessage();
            xQueueSend(queue, &msg, 0);
            break;
        }

        case A76XXURC_t::MQTT_CONNECTION_LOST : {
            break;
        }

        case A76XXURC_t::PDP_DETACH : {
            break;
        }

        case A76XXURC_t::NONE : {
            // no-op
        }
    }
}
