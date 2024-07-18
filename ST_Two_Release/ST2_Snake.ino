//*******************************************************************************************************************
// 								                           Snake Game
//*******************************************************************************************************************
int8_t direction = 1; // 0, 1, 2, 3 mean North, East, South, West

// This will hold the coordinates of each snake segment in order
uint8_t snakeCoords[140][2] = {0};
// Where in the array the head and tail are
uint8_t head = 0, tail = 0;
// Where the pellet is
uint8_t pelletRow, pelletColumn;

bool snakeDead(uint8_t nextHeadRow, uint8_t nextHeadCol) {
  for (int i = tail; i != ((head + 1) % 140); i = (i + 1) % 140) {
    if (snakeCoords[i][0] == nextHeadRow &&
        snakeCoords[i][1] == nextHeadCol) {
      return true;
    }
  }
  return false;
}

bool atPellet(uint8_t nextHeadRow, uint8_t nextHeadCol) {
  return nextHeadRow == pelletRow && nextHeadCol == pelletColumn;
}

// Place pellet somewhere the snake isn't
void placePellet() {
  while (true) {
    pelletRow = random(7);
    pelletColumn = random(19);

    bool pelletIsOnSnake = false;
    
    for (int i = tail; i != ((head + 1) % 140); i = (i + 1) % 140) {
      if (snakeCoords[i][0] == pelletRow &&
          snakeCoords[i][1] == pelletColumn) {
        pelletIsOnSnake = true;
        break; // need to redraw; just exit loop
      }
    }

    if (!pelletIsOnSnake) {
      // exit if we've picked a valid location
      break;
    }
  }
}

void clearGame() {
  head = 0;
  tail = 0;
  // Reset array to all 0
  for (int i = 0; i < 140; i++) {
    snakeCoords[i][0] = 0;
    snakeCoords[i][1] = 0;
  }
  snakeCoords[0][0] = 3; // Make start in the middle row
  direction = 1;

  pelletRow = random(7);
  pelletColumn = random(4, 19);
  
  clearmatrix();
}

void drawSnake() {
  clearmatrix();
  
  for (int i = tail; i != ((head + 1) % 140); i = (i + 1) % 140) {
    int row = snakeCoords[i][0];
    int col = snakeCoords[i][1];
    LEDMAT[col] |= 1 << row;
  }

  // Draw pellet
  LEDMAT[pelletColumn] |= 1 << pelletRow;
}


void snake()
{
  switch (SUBSTATE) 
  {  
    case 0:
      SUBSTATE = 1;
      displayString("Snak");
      delay(250);
      clearGame();
      break;
    case 1:
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

      delay(75); // prevent from moving too fast
      int nextHeadRow = snakeCoords[head][0];
      int nextHeadCol = snakeCoords[head][1];
      
      if (direction == 0) {
        nextHeadRow--;
        nextHeadRow = nextHeadRow < 0 ? 6 : nextHeadRow;
      } else if (direction == 1) {
        nextHeadCol++;
        nextHeadCol %= 20;
      } else if (direction == 2) {
        nextHeadRow++;
        nextHeadRow %= 7;
      } else if (direction == 3) {
        nextHeadCol--;
        nextHeadCol = nextHeadCol < 0 ? 19 : nextHeadCol;
      }

      // Now that we have the coords of the next position, check if 
      // the snake has run into itself
      if (snakeDead(nextHeadRow, nextHeadCol)) {
        clearGame();
        break;
      } else {
        head++;
        head %= 140;
        snakeCoords[head][0] = nextHeadRow;
        snakeCoords[head][1] = nextHeadCol;
      }

      // Check if that puts the snake head on the pellet
      if (atPellet(nextHeadRow, nextHeadCol)) {
        // If so, snake grows and re-put pellet
        placePellet();
      } else {
        // If not, snake doesn't grow
        tail++;
        tail %= 140;
      }

      drawSnake();


      break;
    
    case 99: // Exit Snake
      clearGame();
      NextState();
      break;
  }
}
