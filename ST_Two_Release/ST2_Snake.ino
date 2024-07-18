//*******************************************************************************************************************
// 								                           Snake Game
//*******************************************************************************************************************

// Point has signed integers to make mod arithmetic easier
class Point {
public:
  int8_t row, col;

  Point(uint8_t row, uint8_t col): row(row), col(col) {
  }

  Point(): row(0), col(0) {
  }
};

bool operator==(const Point& lhs, const Point& rhs)
{
    return lhs.row == rhs.row && lhs.col == rhs.col;
}


// Store snake coordinates, bit-packed (so
// the lower 3 bits is row, higher 5 bits
// are column).
class CircularSnakeBuffer {
private:
  uint8_t head = 0, tail = 0;
  uint8_t snakeCoords[140] = {0};
public:
  CircularSnakeBuffer() {
  }

  void reset() {
    head = 0;
    tail = 0;
    // Reset array to all 0
    for (int i = 0; i < 140; i++) {
      snakeCoords[i] = 0;
      snakeCoords[i] = 0;
    }
    snakeCoords[0] = 3; // Make start in the middle row
  }

  Point getHead() {
    return get(head);
  }

  uint8_t begin() {
    return tail;
  }

  uint8_t end() {
    return (head + 1) % 140;
  }

  Point get(uint8_t index) {
    uint8_t point = snakeCoords[index];
    return Point(point & 0b00000111, (point & 0b11111000) >> 3);
  }

  void addHead(Point p) {
    head++;
    head %= 140;
    snakeCoords[head] = p.row;
    snakeCoords[head] |= p.col << 3;
  }

  void removeTail() {
    tail++;
    tail %= 140;
  }

  bool snakeDead(Point h) {
    for (int i = begin(); i != end(); i = (i + 1) % 140) {
      if (get(i) == h) {
        return true;
      }
    }
    return false;
  }

  bool atPellet(Point pellet) {
    return getHead() == pellet;
  }
  
};

// Store the whole snake
CircularSnakeBuffer snake;
// Where the pellet is
Point pellet;
// 0, 1, 2, 3 mean North, East, South, West
int8_t direction = 1;


// Place pellet somewhere the snake isn't
void placePellet() {
  while (true) {
    uint8_t pelletRow = random(7);
    uint8_t pelletColumn = random(19);

    bool pelletIsOnSnake = false;
    
    for (int i = snake.begin(); i != snake.end(); i = (i + 1) % 140) {
      if (snake.get(i).row == pelletRow &&
          snake.get(i).col == pelletColumn) {
        pelletIsOnSnake = true;
        break; // need to redraw; just exit loop
      }
    }

    if (!pelletIsOnSnake) {
      // exit if we've picked a valid location
      pellet = Point(pelletRow, pelletColumn);
      break;
    }
  }
}

void clearGame() {
  direction = 1;

  snake.reset();

  pellet.row = random(7);
  pellet.col = random(4, 19);
  
  clearmatrix();
}

void drawSnake() {
  clearmatrix();

  // Loop over all snake segments
  for (int i = snake.begin(); i != snake.end(); i = (i + 1) % 140) {
    Point point = snake.get(i);
    LEDMAT[point.col] |= 1 << point.row;
  }

  // Draw pellet
  LEDMAT[pellet.col] |= 1 << pellet.row;
}


void doSnake()
{
  Point head; // initialize here to make compiler happy
  switch (SUBSTATE) 
  {  
    case 0:
      SUBSTATE = 1;
      displayString("Snak");
      delay(250);
      clearGame();
      break;
    case 1:
      // When press both, quit. This necessitated
      // a modification to ST2_Main
      if(NextStateRequest && NextSUBStateRequest) {
        SUBSTATE = 99;
        NextStateRequest = false;
        NextSUBStateRequest = false;
        break;
      }

      if (NextSUBStateRequest) {
        direction--;
        direction = direction < 0 ? 3 : direction;
        NextSUBStateRequest = false;
      }
      if (NextStateRequest) {
        direction++;
        direction %= 4;
        NextStateRequest = false;
      }

      delay(85); // prevent from moving too fast
      head = snake.getHead();
      
      if (direction == 0) {
        head.row--;
        head.row = head.row < 0 ? 6 : head.row;
      } else if (direction == 1) {
        head.col++;
        head.col %= 20;
      } else if (direction == 2) {
        head.row++;
        head.row %= 7;
      } else if (direction == 3) {
        head.col--;
        head.col = head.col < 0 ? 19 : head.col;
      }

      // Now that we have the coords of the next position, check if 
      // the snake has run into itself
      if (snake.snakeDead(head)) {
        clearGame();
        break;
      } else {
        snake.addHead(head);
      }

      // Check if that puts the snake head on the pellet
      if (snake.atPellet(pellet)) {
        // If so, snake grows and re-put pellet
        placePellet();
      } else {
        // If not, snake doesn't grow
        snake.removeTail();
      }

      drawSnake();


      break;
    
    case 99: // Exit Snake
      clearGame();
      NextState();
      break;
  }
}
