# Arduino Joystick-Controlled Game

This project involves an Arduino system that uses a joystick to control a character displayed on an I2C LCD. The game features a grid with obstacles and allows the player to navigate the character around the grid. The game continues until the character collides with an obstacle, at which point a "GAME OVER" screen is displayed. The game can be restarted by pressing a button.

## Components Required

- **Arduino Uno** (or similar)
- **Joystick Module** (with X, Y axes and a button)
- **Servo Motor** (for control, but not actively used in the provided code)
- **I2C LCD Display** (20x4 or similar)
- **Connecting wires and breadboard**

## Code Overview

### Arduino Code

- **Joystick Input**:
  - **X Axis**: Controls horizontal movement of the character.
  - **Y Axis**: Controls vertical movement of the character.
  - **Button**: Used to restart the game after a game over.

- **LCD Display**:
  - **Display Grid**: Shows the game environment with obstacles and the character.
  - **Character**: Controlled by the joystick and moves within the grid.
  - **Obstacles**: Randomly generated within the grid.

- **Functions**:
  - **`setup()`**: Initializes the serial communication, LCD, and grid, and sets up the joystick button.
  - **`loop()`**: Manages game state, updates display, and handles game logic.
  - **`generationDelayTimeDecay()`**: Gradually reduces the time between grid updates.
  - **`readJoystickInputs()`**: Reads joystick input to move the character.
  - **`moveGrid()`**: Shifts the grid leftward and generates new columns.
  - **`generateTerrain()`**: Adds new columns with obstacles or empty spaces.
  - **`generateObstaclesColumn()`**: Creates a column with obstacles.
  - **`generateEmptyColumn()`**: Creates an empty column.
  - **`DisplayGrid()`**: Updates the LCD to show the current state of the grid.
  - **`DisplayGameOver()`**: Displays the game over message on the LCD.
  - **`UpdateCharacterDisplay()`**: Updates the character's position on the LCD.
  - **`InitializeDisplay()`**: Sets up the LCD and character symbols.
  - **`InitializeGrid()`**: Initializes the grid and marks all cells as changed.
  - **`copyColumn()`**: Copies data from one column to another.
  - **`GameOver()`**: Checks if the character has collided with an obstacle.
  - **`joystickButtonPressed()`**: Detects when the joystick button is pressed.
  - **`ResetGame()`**: Resets game variables and state for a new game.

### Libraries Used

- **`Wire.h`**: For I2C communication with the LCD.
- **`LiquidCrystal_I2C.h`**: For controlling the I2C LCD display.

## Setup

1. **Connect Components**:
   - **Joystick Module**:
     - Connect the X axis to analog pin `A6`.
     - Connect the Y axis to analog pin `A5`.
     - Connect the button to analog pin `A7`.
   - **I2C LCD Display**:
     - Connect to the I2C bus (typically SDA to A4 and SCL to A5 on Arduino Uno).

2. **Upload Arduino Code**: Use the Arduino IDE to upload the provided code to your Arduino board.

3. **Open Serial Monitor**: Set to 9600 baud to view debugging messages (if any).

## Game Mechanics

- **Movement**:
  - Use the joystick to move the character up, down, left, or right.
  - The character's position updates based on joystick input.

- **Obstacles**:
  - Obstacles are generated randomly and move leftward across the grid.
  - If the character collides with an obstacle, the game ends.

- **Game Over**:
  - The LCD displays "GAME OVER" and instructions to restart.
  - Press the joystick button to reset the game and start a new round.

## Troubleshooting

- **Incorrect Display**:
  - Ensure the I2C address for the LCD is correct.
  - Verify connections and check for proper library installation.

- **Joystick Not Responding**:
  - Check the wiring and make sure the joystick is correctly connected.
  - Verify that the analog pins are properly defined and working.

- **LCD Issues**:
  - Confirm the I2C address and wiring.
  - Ensure the LCD library is correctly installed and compatible with your display.

## License

This project is open-source and licensed under the [MIT License](https://opensource.org/licenses/MIT).
