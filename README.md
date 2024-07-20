# Summary

Modified version of the code for the [solder time 2](https://www.spikenzielabs.com/learn/soldertimeii.html).
Adds a snake game as a last menu option.

## How to play the game
Press the MODE button until you get to the Snke option. It will display the 
high score and start the game. Press the left button to turn left (counterclockwise), and the right
button to turn the snake right (clockwise). Hold both buttons (or stop pressing any buttons for 
15-20 seconds) to stop playing.

After you game ends, it will display your score, if it's a high score. Press a button to continue.

The high score is stored in bytes 0, 1, 2 in the EEPROM, and is initialized to 0
if no score is detected (basically, if the bytes 0 and 1 aren't 'Hi' it
initializes them).
