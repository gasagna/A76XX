#ifndef A76XX_STATICQUEUE_H_
#define A76XX_STATICQUEUE_H_

/*
    @brief Simple fully statically allocated queue.
*/
template <typename T, size_t N>
class StaticQueue {
  private:
    // store data in a standard array
    std::array<T, N>   data;
    // index of the first used slot (can be zero with no elements)
    int32_t           front;
    // index of the first available slot (can be N)
    int32_t            rear;

  public:
    /*
        @brief Constructor.
    */
    StaticQueue()
        : front(0)
        , rear(0) {}
    
    /*
        @brief Add an element to the queue. When the queue is full
            new elements are dropped. A copy of the input element
            is performed internally.

        @param [in] element The element to append to the queue.
        @return True on successful push, false if the queue is full.
    */
    bool pushEnd(T element) {
        // attempt to make space
        if (rear == N && front > 0) {
            _shiftLeft();
        }

        if (rear < N) {
            memcpy(&data[rear], &element, sizeof(T));
            rear++;
            return true;
        }

        return false;
    }

    /*
        @brief Get the first element in the queue.

        @param [OUT] element Copy the first element at this memory location.
        @return True on successful pop, false if the queue is empty.
    */
    bool popFront(T& element) {
        if (available() == 0) {
            return false;
        }
        memcpy(&element, &data[front], sizeof(T));
        front++;
        return true;
    }

    /*
        @brief Return the number of elements available in the queue.
    */
    int32_t available() {
        return rear - front;
    }

    /*
        @brief Get a copy of element at location i.
        @details No error checking is done. Mostly used for testing.
    */
    T peek(uint32_t i) { 
        return data[i]; 
    }

    /*
        @brief Index of first available element.
    */
    int32_t begin() { 
        return front; 
    }

    /*
        @brief Index of first slot available.
    */
    int32_t end() { 
        return rear; 
    }

  private:
    /*
        @brief Shift data left so that the first used slot is again at index 0.
    */ 
    void _shiftLeft() {
        for (int32_t i = front; i < rear; i++) {
            memcpy(&data[i - front], &data[i], sizeof(T));
        }
        rear -= front;
        front = 0;
    }
};

#endif A76XX_STATICQUEUE_H_