//*******************************************************************************************************************
// 								                           Snake Game
//*******************************************************************************************************************
volatile int8_t direction = 0; // 0, 1, 2, 3 mean North, East, South, West
volatile int8_t locationRow = 0, locationCol = 0;

void snake()
{
  switch (SUBSTATE) 
  {  
    case 0:
      SUBSTATE = 1;
      displayString("Snak");
      delay(250);
      clearmatrix();
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

      delay(100); // prevent from moving too fast
      if (direction == 0) {
        locationRow--;
        locationRow = locationRow < 0 ? 6 : locationRow;
      } else if (direction == 1) {
        locationCol++;
        locationCol %= 20;
      } else if (direction == 2) {
        locationRow++;
        locationRow %= 7;
      } else if (direction == 3) {
        locationCol--;
        locationCol = locationCol < 0 ? 19 : locationCol;
      }

      // Draw dot
      clearmatrix();
      LEDMAT[locationCol] = 1 << locationRow;

      break;
    
    case 99: // Exit Snake

      NextState();
      clearmatrix();

      break;
  }
}