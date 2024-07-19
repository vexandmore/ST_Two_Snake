#include "SnakeBuffer.h"

/*
 * Point Implementation
 */
Point::Point(uint8_t row, uint8_t col): row(row), col(col) {
}

Point::Point(): row(0), col(0) {
}

bool operator==(const Point& lhs, const Point& rhs)
{
    return lhs.row == rhs.row && lhs.col == rhs.col;
}



/**
 * Snake buffer implementation
 */
CircularSnakeBuffer::CircularSnakeBuffer() {
}

void CircularSnakeBuffer::reset() {
  head = 0;
  tail = 0;
  // Reset array to all 0
  for (int i = 0; i < 140; i++) {
    snakeCoords[i] = 0;
    snakeCoords[i] = 0;
  }
  snakeCoords[0] = 3; // Make start in the middle row
}

Point CircularSnakeBuffer::getHead() {
  return get(head);
}

uint8_t CircularSnakeBuffer::begin() {
  return tail;
}

uint8_t CircularSnakeBuffer::end() {
  return (head + 1) % 140;
}

uint8_t CircularSnakeBuffer::length() {
  if (head > tail) {
    return head - tail + 1;
  } else {
    return 139 - tail + 1 + head + 1;
  }
}

Point CircularSnakeBuffer::get(uint8_t index) {
  uint8_t point = snakeCoords[index];
  return Point(point & 0b00000111, (point & 0b11111000) >> 3);
}

void CircularSnakeBuffer::addHead(Point p) {
  head++;
  head %= 140;
  snakeCoords[head] = p.row;
  snakeCoords[head] |= p.col << 3;
}

void CircularSnakeBuffer::removeTail() {
  tail++;
  tail %= 140;
}

bool CircularSnakeBuffer::snakeDead(Point h) {
  for (int i = begin(); i != end(); i = (i + 1) % 140) {
    if (get(i) == h) {
      return true;
    }
  }
  return false;
}

bool CircularSnakeBuffer::atPellet(Point pellet) {
  return getHead() == pellet;
}
