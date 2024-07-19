#include <stdint.h>

// Point has signed integers to make mod arithmetic easier
class Point {
public:
  int8_t row, col;

  Point(uint8_t row, uint8_t col);

  Point();
};

bool operator==(const Point& lhs, const Point& rhs);


// Store snake coordinates, bit-packed (so
// the lower 3 bits is row, higher 5 bits
// are column).
class CircularSnakeBuffer {
private:
  uint8_t head = 0, tail = 0;
  uint8_t snakeCoords[140] = {0};
public:
  CircularSnakeBuffer();

  void reset();

  Point getHead();

  uint8_t begin();

  uint8_t end();

  uint8_t length();

  Point get(uint8_t index);

  void addHead(Point p);

  void removeTail();

  bool snakeDead(Point h);

  bool atPellet(Point pellet);
};
