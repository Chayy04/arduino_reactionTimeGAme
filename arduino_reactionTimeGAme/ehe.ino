#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Pin Assignments
const int leftLedPin = 7;   // LED for left direction
const int rightLedPin = 6;  // LED for 2nd left direction
const int upLedPin = 5;     // LED for right direction
const int downLedPin = 4;   // LED for 2right direction

const int startButtonPin = 8; // Pin for the start button
const int leftButtonPin = 10;   // Pin for the left button
const int rightButtonPin = 11;  // Pin for the 2left button
const int left2ButtonPin = 12;     // Pin for the right button
const int righ2ButtonPin = 13;   // Pin for the 2 right button

LiquidCrystal_I2C lcd(0x27, 16, 2);  // 16x2 LCD display

int score = 0;  // Initialize score
unsigned long gameStartTime; // To track the game start time
bool gameActive = false; // To check if the game is active

// Debouncing variables
const int debounceDelay = 50;  // Adjust debounce time as needed
unsigned long lastDebounceTime[4] = {0, 0, 0, 0};  // Last time button state was checked
int lastButtonState[4] = {HIGH, HIGH, HIGH, HIGH}; // Previous button states

void setup() {
  pinMode(leftLedPin, OUTPUT);
  pinMode(rightLedPin, OUTPUT);
  pinMode(upLedPin, OUTPUT);
  pinMode(downLedPin, OUTPUT);
  
  pinMode(startButtonPin, INPUT_PULLUP); // Start button
  pinMode(leftButtonPin, INPUT_PULLUP);   // Left button
  pinMode(rightButtonPin, INPUT_PULLUP);  // Right button
  pinMode(left2ButtonPin, INPUT_PULLUP);     // Up button
  pinMode(righ2ButtonPin, INPUT_PULLUP);   // Down button
  
  lcd.init();               // Initialize the LCD
  lcd.backlight();         // Turn on the backlight
  Serial.begin(9600);      // Initialize serial communication

  // Welcome message
  lcd.setCursor(0, 0);
  lcd.print("Welcome to the");
  lcd.setCursor(0, 1);
  lcd.print("Reaction Game!");
  delay(1000);              
  lcd.clear();

  // Inform the user to press the start button
  lcd.setCursor(0, 0);
  lcd.print("Press Start to");
  lcd.setCursor(0, 1);
  lcd.print("begin the game!");
  delay(2000);  // Show instruction for 2 seconds
  lcd.clear();

  // Wait for the start button press
  waitForStartButton();
}

void loop() {
  if (gameActive) {
    // Check if 20 seconds have passed
    if (millis() - gameStartTime >= 20000) {
      // Game over, display final score
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Final Score: ");
      lcd.print(score);
      lcd.setCursor(0, 1);
      lcd.print("Press Start");
      delay(2000);  // Show final score for 2 seconds
      gameActive = false; // Reset game state
      waitForStartButton(); // Wait for start button again
      return; // Exit loop to prevent further execution
    }

    // Generate a new LED direction
    int direction = random(0, 4);  // Randomly choose a direction
    unsigned long startTime;
    bool correctInput = false;      // Flag for correct input

    // Light up the corresponding LED
    digitalWrite(leftLedPin, LOW);
    digitalWrite(rightLedPin, LOW);
    digitalWrite(upLedPin, LOW);
    digitalWrite(downLedPin, LOW);

    switch (direction) {
      case 0:
        digitalWrite(leftLedPin, HIGH);
        break;
      case 1:
        digitalWrite(rightLedPin, HIGH);
        break;
      case 2:
        digitalWrite(upLedPin, HIGH);
        break;
      case 3:
        digitalWrite(downLedPin, HIGH);
        break;
    }

    startTime = millis();  // Record the time when the LED lights up

    // Wait for the correct button input
    while (true) {
      // Check button states with debouncing
      checkButtonStates(direction, correctInput);
      
      // Break if we have found a correct input
      if (correctInput) {
        break;
      }
    }

    // Turn off all LEDs
    digitalWrite(leftLedPin, LOW);
    digitalWrite(rightLedPin, LOW);
    digitalWrite(upLedPin, LOW);
    digitalWrite(downLedPin, LOW);

    // Display result based on input correctness
    if (correctInput) {
      score++;  // Increment score
      unsigned long reactionTime = millis() - startTime;  // Calculate reaction time

      // Display reaction time and score on the LCD
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Time: ");
      lcd.print(reactionTime);
      lcd.print(" ms");
      lcd.setCursor(0, 1);
      lcd.print("Score: ");
      lcd.print(score);
      delay(1000);  // Show result for 1 second
    } else {
      // Fail Screen
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Wrong Direction!");
      lcd.setCursor(0, 1);
      lcd.print("Score: ");
      lcd.print(score);  // Show the current score
      delay(2000);  // Show fail message for 2 seconds
    }

    // Wait a moment before the next round if the game is still active
    delay(500); // Adjust delay as needed
  }
}

// Function to wait for the player to press the start button
void waitForStartButton() {
  // Wait for start button press (active LOW)
  while (digitalRead(startButtonPin) == HIGH) {
    // Button not pressed, do nothing, wait here
  }
  // Button pressed, clear LCD and proceed
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Get Ready!");
  delay(1000);  // Brief delay before starting the game
  lcd.clear();
  score = 0; // Reset score at the beginning of the game
  gameActive = true; // Set game active flag
  gameStartTime = millis(); // Start the game timer
}

// Function to check button states with debouncing
void checkButtonStates(int direction, bool &correctInput) {
  int buttonStates[4] = {
    digitalRead(leftButtonPin),
    digitalRead(rightButtonPin),
    digitalRead(left2ButtonPin),
    digitalRead(righ2ButtonPin)
  };

  for (int i = 0; i < 4; i++) {
    if (buttonStates[i] != lastButtonState[i]) {
      lastDebounceTime[i] = millis();  // Reset debounce timer
    }
    
    if ((millis() - lastDebounceTime[i]) > debounceDelay) {
      if (buttonStates[i] == LOW) {  // Button is pressed
        if (i == direction) {
          correctInput = true;
          delay(50);  // Optional delay to give time before next input
        } else {
          
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Wrong Button!");
          lcd.setCursor(0, 1);
          lcd.print("Try Again");
          correctInput = false; // Set correct input to false
          return; // Exit function if incorrect input is detected
        }
      }
    }

    lastButtonState[i] = buttonStates[i];  // Save the state for next loop
  }
}
