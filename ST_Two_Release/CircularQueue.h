#include <stdint.h>

template<typename T, int n>

class CircularQueue {
private:
  uint8_t tail, length;
  T buf[n] = {0};
public:
  CircularQueue(): tail(0), length(0) {
  }

  void reset() {
    tail = 0;
    length = 0;
  };

  bool enqueue(T t) {
    if (!isFull()) {
      uint8_t nextSpot = (tail + length + 1) % n;
      buf[nextSpot] = t;
      // If was empty, need to reset tail
      if (length == 0) {
        tail = nextSpot;
      }
      length++;
    } else {
      return false;
    }
  }

  T dequeue() {
    if (length > 0) {
      T t = buf[tail];
      length--;
      tail++;
      tail %= n;
      return t;
    } else {
      return T();
    }
  }

  bool isFull() {
    return length == n;
  }

  uint8_t getLength() {
    return length;
  }
};
