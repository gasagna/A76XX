#include <A76XX.h>

template <typename T, size_t N>
void print(StaticQueue<T, N>& queue) {
    Serial.print("Queue: ");
    for (int i = queue.begin(); i < queue.end(); i++) {
        Serial.print(queue.peek(i));
        Serial.print(" ");
    }
    Serial.println();
}

void setup() {
    Serial.begin(115200); delay(5000);
    
    // define a queue of ints
    StaticQueue<uint8_t, 4> queue;
    print(queue);

    queue.pushEnd(1);
    print(queue);

    queue.pushEnd(2);
    print(queue);

    queue.pushEnd(3);
    print(queue);

    queue.popFront();
    print(queue);

    queue.pushEnd(4);
    print(queue);

    queue.pushEnd(5);
    print(queue);

    queue.pushEnd(6);
    print(queue);

    queue.popFront();
    print(queue);

    queue.pushEnd(7);
    print(queue);

    queue.popFront();
    print(queue);

    queue.popFront();
    print(queue);

    queue.popFront();
    print(queue);

    queue.popFront();
    print(queue);
    
    queue.pushEnd(8);
    print(queue);
}

void loop() {}