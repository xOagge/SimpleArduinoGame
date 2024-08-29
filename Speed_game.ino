#include <stdint.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Pin definitions
#define X_PIN A6
#define Y_PIN A5
#define JOY_BUTTON A7
#define SERVO_PIN 3

// Create display object
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Joystick variables
bool lastButtonState = HIGH;
bool currentButtonState;
bool isMoving = true;

// Software variables
#define ROWS 4
#define COLS 20
bool grid[COLS][ROWS];        // Display full information of contents
bool gridChanged[COLS][ROWS]; // Tracks whether a grid cell has changed
uint8_t PrevCharacterPosC;
uint8_t characterPosC = 0;
uint8_t PrevCharacterPosR;
uint8_t characterPosR = 2;
const uint8_t obstacles_min_dis = 3;
uint8_t NoObstaclesCount = obstacles_min_dis;
byte emptySquare[8] = {B00000, B00000, B00000, B00000, B00000, B00000, B00000, B00000};
byte obstacleSquare[8] = {B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111};
byte characterSquare[8] = {B00000, B01110, B01010, B01010, B01010, B01110, B00000, B00000};
float generationDelayTime = 500;
unsigned long startTime = 0;
unsigned long elapsedTime = 0;

enum GameState {PLAYING, GAME_OVER};
GameState gameState = PLAYING;

void setup() {
  Serial.begin(9600);
  // Initialize the button pin
  pinMode(SERVO_PIN, INPUT_PULLUP);
  InitializeDisplay();
  InitializeGrid();
  randomSeed(analogRead(0));
}

void loop() {
  if (gameState == PLAYING) {
    // Give time for the player to move the character before updating the scenario
    startTime = millis();
    while (elapsedTime < generationDelayTime) {
      UpdateCharacterDisplay();
      if (GameOver()) {
        gameState = GAME_OVER;
        DisplayGameOver();
        return;
      }
      elapsedTime = millis() - startTime;
      delay(100);
    }
    elapsedTime = 0;

    // Update terrain and display grid
    moveGrid();
    generateTerrain();
    DisplayGrid();
    if(generationDelayTime >= 200){generationDelayTimeDecay();}
  } else if (gameState == GAME_OVER) {
    if (joystickButtonPressed()) {
      ResetGame();
      gameState = PLAYING;
      generationDelayTime = 500;
    }
  }
}

void generationDelayTimeDecay() {
  float secs = millis()/1000;
  float a = -4; // Set appropriate value for a
  float b = 500.0;  // Set appropriate value for b
  // Calculate the exponential decay value
  float decayValue = a*secs+b;
  generationDelayTime = decayValue;
}

// JOYSTICK INPUTS
bool readJoystickInputs() {
  uint16_t xVal = analogRead(X_PIN);
  uint16_t yVal = analogRead(Y_PIN);
  bool moved = false;

  // Thresholds for detecting movement
  const uint16_t thresholdLow = 100;
  const uint16_t thresholdHigh = 900;

  PrevCharacterPosC = characterPosC;
  PrevCharacterPosR = characterPosR;

  if (xVal < thresholdLow) { // Right
    if (characterPosC < COLS - 1) {
      characterPosC++;
      moved = true;
    }
  } else if (xVal > thresholdHigh) { // Left
    if (characterPosC > 0) {
      characterPosC--;
      moved = true;
    }
  } else if (yVal < thresholdLow) { // Up
    if (characterPosR > 0) {
      characterPosR--;
      moved = true;
    }
  } else if (yVal > thresholdHigh) { // Down
    if (characterPosR < ROWS - 1) {
      characterPosR++;
      moved = true;
    }
  }

  return moved;
}

bool randomBool() {
  int randomValue = random(0, 2);
  return randomValue == 1;
}

// MOVE GRID TO LEFT
void moveGrid() {
  // Move every column one index to the left
  for (uint8_t i = 0; i < COLS - 1; i++) {
    for (uint8_t j = 0; j < ROWS; j++) {
      if (grid[i][j] != grid[i + 1][j]) {
        gridChanged[i][j] = true;
      }
      grid[i][j] = grid[i + 1][j];
    }
  }
  // Clear the last column and mark it as changed
  for (uint8_t j = 0; j < ROWS; j++) {
    if (grid[COLS - 1][j] != false) {
      gridChanged[COLS - 1][j] = true;
    }
    grid[COLS - 1][j] = false;
  }
}

// GENERATE NEW COLUMNS
void generateTerrain() {
  bool newCol[ROWS];
  // If current count permits, create obstacles column, else create empty column
  if (NoObstaclesCount >= obstacles_min_dis) {
    copyColumn(newCol, generateObstaclesColumn());
  } else {
    copyColumn(newCol, generateEmptyColumn());
  }
  // Add new column to end of grid and mark it as changed
  copyColumn(grid[COLS - 1], newCol);
  for (uint8_t j = 0; j < ROWS; j++) {
    gridChanged[COLS - 1][j] = true;
  }
}

// GENERATE LINE OF OBSTACLES
bool* generateObstaclesColumn() {
  static bool obstacles[ROWS];
  uint8_t count = 0;
  // Create an obstacles column where at least one slot needs to not represent an obstacle
  for (uint8_t i = 0; i < ROWS; i++) {
    if (randomBool() && count < 3) {
      obstacles[i] = true;
      count++;
    } else {
      obstacles[i] = false;
    }
  }
  // If this new column has obstacles, reset NoObstaclesCount
  if (count != 0) {
    NoObstaclesCount = 0;
  }
  return obstacles;
}

// GENERATE LINE WITHOUT OBSTACLES
bool* generateEmptyColumn() {
  NoObstaclesCount++;
  static bool empty[ROWS] = {false, false, false, false};
  return empty;
}

// DISPLAY GRID AT DISPLAY
void DisplayGrid() {
  // Iterate through the grid and update only the changed cells
  for (uint8_t i = 0; i < COLS; i++) {
    for (uint8_t j = 0; j < ROWS; j++) {
      if (gridChanged[i][j]) {
        lcd.setCursor(i, j);
        lcd.write(grid[i][j] ? byte(1) : byte(0));
        gridChanged[i][j] = false;
      }
    }
  }
  // Display the character
  lcd.setCursor(characterPosC, characterPosR);
  lcd.write(byte(2));
}

void DisplayGameOver() {
  // Display "GAME OVER" message
  lcd.clear();
  lcd.setCursor(7, 1);
  lcd.print("GAME OVER");
  lcd.setCursor(4, 2);
  lcd.print("Press to Restart");
}

void UpdateCharacterDisplay() {
  if (readJoystickInputs()) { // Read input and return if needs to update
    // Clear previous character position from the grid
    lcd.setCursor(PrevCharacterPosC, PrevCharacterPosR);
    lcd.write(byte(0));
    // Write character to the new position
    lcd.setCursor(characterPosC, characterPosR);
    lcd.write(byte(2));
  }
}

// INITIALIZERS
void InitializeDisplay() {
  lcd.begin();
  lcd.backlight();
  // Add new characters to display
  lcd.createChar(0, emptySquare);
  lcd.createChar(1, obstacleSquare);
  lcd.createChar(2, characterSquare);
  // Initialize the character position on the display
  lcd.setCursor(characterPosC, characterPosR);
  lcd.write(byte(2));
}

void InitializeGrid() {
  for (uint8_t i = 0; i < COLS; i++) {
    for (uint8_t j = 0; j < ROWS; j++) {
      grid[i][j] = false;
      gridChanged[i][j] = true; // Initialize as changed to draw the initial grid
    }
  }
}

// GENERAL ABSTRACTION
void copyColumn(bool* dest, const bool* src) {
  for (uint8_t i = 0; i < ROWS; i++) {
    dest[i] = src[i];  // Copy each element from src to dest
  }
}

bool GameOver() {
  // Check if the character's position coincides with an obstacle
  return grid[characterPosC][characterPosR];
}

bool joystickButtonPressed() {
    currentButtonState = digitalRead(JOY_BUTTON);

    // Check for a transition from HIGH (unpressed) to LOW (pressed)
    if (currentButtonState == LOW && lastButtonState == HIGH) {
        delay(50); // Debounce delay
        currentButtonState = digitalRead(JOY_BUTTON); // Read the state again after delay
        if (currentButtonState == LOW) {
            lastButtonState = LOW; // Update the last state
            return true; // Button was just pressed
        }
    } else if (currentButtonState == HIGH) {
        lastButtonState = HIGH; // Update last state when button is released
    }

    return false; // Button was not pressed or still being held down
}
void ResetGame() {
  // Reset grid, character position, and other game variables
  InitializeGrid();
  characterPosC = 0;
  characterPosR = 2;
  NoObstaclesCount = obstacles_min_dis;
  InitializeDisplay();
  lastButtonState = HIGH;  // Reset the button state to HIGH (unpressed)
}
