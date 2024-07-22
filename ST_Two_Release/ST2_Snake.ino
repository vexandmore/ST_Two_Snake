//*******************************************************************************************************************
// 								                           Snake Game
//*******************************************************************************************************************
#include "SnakeBuffer.h"
#include "CircularQueue.h"
#include <EEPROM.h>

// Store the whole snake
CircularSnakeBuffer snake;
// Where the pellet is
Point pellet;
// 0, 1, 2, 3 mean North, East, South, West
int8_t direction;
// Keep previous state of the buttons, to tell if on rising or falling edge
uint8_t previousNextState, previousNextSubstate;
// Keep track of last update to keep consistent rate of snake updates
unsigned long timeLastUpdate;
// How many ms between frames
#define UPDATE_RATE 150
// Only quit if hold both buttons for long enough
int8_t heldBothButtonsBefore;
#define NUMBER_OF_TIMES_HOLD_BOTH 6
// Constants for the queue
enum Button: uint8_t {NEXTSTATE, NEXTSUBSTATE};
// Buffer inputs
CircularQueue<Button, 3> inputBuffer;

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

void moveSnake() {
  Point head = snake.getHead();
  
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
    beepsound(600, 300);
    beepsound(400, 300);
    beepsound(350, 300);

    char scoreBuf[5] = {0, 0, 0, 0, 0};
    uint8_t score = snake.length();
    snprintf(scoreBuf, sizeof(scoreBuf), "%d   ", score);
    uint8_t old_score;
    EEPROM.get(2, old_score);
    clearGame();

    if (score > old_score) {
      displayString("New ");
      delay(600);
      displayString("high");
      delay(600);
      EEPROM.put(2, score);
    }
    
    displayString("scor");
    delay(600);
    displayString(scoreBuf);
    delay(600);
    // Wait to press a button before continuing
    SUBSTATE = 4;

    return;
  } else {
    snake.addHead(head);
  }

  // Check if that puts the snake head on the pellet
  if (snake.atPellet(pellet)) {
    beepsound(500, 60);
    // If so, snake grows and re-put pellet
    placePellet();
  } else {
    // If not, snake doesn't grow
    snake.removeTail();
  }

  drawSnake();
}

void clearGame() {
  direction = 1;
  heldBothButtonsBefore = 0;

  snake.reset();
  timeLastUpdate = millis();
  pellet.row = random(7);
  pellet.col = random(4, 19);

  previousNextState = 0;
  previousNextSubstate = 0;

  uint8_t val;
  if (EEPROM.get(0, val) != 'H' || EEPROM.get(1, val) != 'i'){
    // Initialize hi score if not already.
    EEPROM.write(0, 'H');
    EEPROM.write(1, 'i');
    EEPROM.write(2, 0);
  }
  
  clearmatrix();
}

void drawSnake() {
  noInterrupts();
  
  clearmatrix();

  // Loop over all snake segments
  for (int i = snake.begin(); i != snake.end(); i = (i + 1) % 140) {
    Point point = snake.get(i);
    LEDMAT[point.col] |= 1 << point.row;
  }

  // Draw pellet
  LEDMAT[pellet.col] |= 1 << pellet.row;

  interrupts();
}

bool timeCheck() {
  if (millis() - timeLastUpdate < UPDATE_RATE) {
    // If not time to update the game yet,
    // and break
    return true;
  } else {
    // If time to update, update the time
    // (it adds UPDATE_RATE to prevent long term drift),
    // and continue.
    timeLastUpdate = timeLastUpdate + UPDATE_RATE;
    return false;
  }
}

bool shouldQuit() {
  if(NextStateRequest && NextSUBStateRequest) {
    if (heldBothButtonsBefore < NUMBER_OF_TIMES_HOLD_BOTH) {
      // If only just pressed both, wait until next time to see if still
      // holding. The snake won't move since it will move both left and right.
      heldBothButtonsBefore++;
      return false; 
    } else {
      heldBothButtonsBefore = 0;
      SUBSTATE = 99;
      NextStateRequest = false;
      NextSUBStateRequest = false;
      return true; 
    }
  }
  return false;
}

void handleInput() {
  if (NextStateRequest && !previousNextState) {
    // Rising edge of right button
    inputBuffer.enqueue(NEXTSTATE);
  }
  if (NextSUBStateRequest && !previousNextSubstate) {
    // Rising edge of left button
    inputBuffer.enqueue(NEXTSUBSTATE);
  }
  previousNextState = NextStateRequest;
  previousNextSubstate = NextSUBStateRequest;
  
  // Since we're handling buttons, set these to false
  NextStateRequest = false;
  NextSUBStateRequest = false;
}


void doSnake()
{
  char scoreBuf[5] = {0, 0, 0, 0, 0};
  switch (SUBSTATE)
  {  
    case 0:
      SUBSTATE = 1;
      uint8_t score;
      EEPROM.get(2, score);
      snprintf(scoreBuf, sizeof(scoreBuf), "%d   ", score);

      displayString("Snke");
      delay(600);
      clearGame();
      displayString("High");
      delay(600);
      displayString("scor");
      delay(600);
      displayString(scoreBuf);
      delay(600);
      break;

    case 1:
      // When press both, quit. This necessitated
      // a modification to ST2_Main
      if (shouldQuit()) {
        break;
      }

      // Queue button presses
      handleInput();

      // Only update snake if it's time
      if(timeCheck()) {
        break;
      }
      
      // If time to render the snake, remove one action from the queue
      if (inputBuffer.getLength() > 0) {
        uint8_t nextPress = inputBuffer.dequeue();
        if (nextPress == NEXTSUBSTATE) {
          direction--;
          direction = direction < 0 ? 3 : direction;
        }
        if (nextPress == NEXTSTATE) {
          direction++;
          direction %= 4;
        }
      }

      moveSnake();
      
      break;

    case 4:
      if (NextStateRequest || NextSUBStateRequest) {
        SUBSTATE = 0;
        NextStateRequest = false;
        NextSUBStateRequest = false;
      }
      break;

    case 99: // Exit Snake
      clearGame();
      NextState();
      break;
  }
}
