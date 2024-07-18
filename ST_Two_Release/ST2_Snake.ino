//*******************************************************************************************************************
// 								                           Snake Game
//*******************************************************************************************************************
int8_t direction = 1; // 0, 1, 2, 3 mean North, East, South, West

// This will hold the coordinates of each snake segment in order
uint8_t snakeCoords[140][2] = {0};
// Where in the array the head and tail are
uint8_t head = 0, tail = 0;

bool snakeDead(uint8_t nextHeadRow, uint8_t nextHeadCol) {
  for (int i = tail; i != head; i = (i + 1) % 139) {
    if (snakeCoords[i][0] == nextHeadRow &&
        snakeCoords[i][1] == nextHeadCol) {
      return true;
    }
  }
  return false;
}

void clearGame() {
  head = 0;
  tail = 0;
  for (int i = 0; i < 140; i++) {
    snakeCoords[i][0] = 0;
    snakeCoords[i][1] = 0;
  }
  snakeCoords[0][0] = 3; // Make start in the middle row
  direction = 1;
  
  clearmatrix();
}

void drawSnake() {
  clearmatrix();
  
  for (int i = 0; i <= head; i++) {
    int row = snakeCoords[i][0];
    int col = snakeCoords[i][1];
    LEDMAT[col] |= 1 << row;
  }
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
      head++;
      
      if (head == 139) {
        SUBSTATE == 99;
        break;
      }
      
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
      } else {
        snakeCoords[head][0] = nextHeadRow;
        snakeCoords[head][1] = nextHeadCol;

        drawSnake();
      }

      break;
    
    case 99: // Exit Snake
      clearGame();
      NextState();
      break;
  }
}
