#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>

#define NUM_SERVOS 5
#define RECORD_BUTTON_PIN 10
#define PLAYBACK_BUTTON_PIN 11

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET 4
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Servo servos[NUM_SERVOS];
int servoPins[NUM_SERVOS] = {2, 3, 4, 5, 6}; // Digital pins for servos

int servoValues[NUM_SERVOS][100];
int servoIndex = 0;
bool isRecording = false;
int recordButtonState = HIGH;
int playbackButtonState = HIGH;
int potPins[5] = {A0, A1, A2, A3, A5};///

int minPotValues[5] = {430, 30, 140, 1023, 0};    // Minimum potentiometer values for each servo
int maxPotValues[5] = {840, 615, 870, 400, 1023};  // Maximum potentiometer values for each servo
int minAngles[5] = {0, 250, 150, 0, 170};  // Minimum servo angles
int maxAngles[5] = {120, 100, 90, 100, 90};  // Maximum servo angles

enum SystemState {
  IDLE,
  RECORDING,
  PLAYBACK
};

SystemState currentState = IDLE;

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  

  
  pinMode(RECORD_BUTTON_PIN, INPUT_PULLUP);
  pinMode(PLAYBACK_BUTTON_PIN, INPUT_PULLUP);

  for (int i = 0; i < NUM_SERVOS; i++) {
    servos[i].attach(servoPins[i]);
}
}

void loop() {

  
  display.clearDisplay();

  for (int i = 0; i < 5; i++) {
    int potValue = analogRead(potPins[i]);
    int angle = map(potValue, minPotValues[i], maxPotValues[i], minAngles[i], maxAngles[i]);
    servos[i].write(angle);

    display.setCursor(0, i * 16);
    display.println(potValue);
  }

  // Check the state of the buttons and set the current state accordingly
  if (digitalRead(RECORD_BUTTON_PIN) == LOW) {
    currentState = RECORDING;
  } else if (digitalRead(PLAYBACK_BUTTON_PIN) == LOW) {
    currentState = PLAYBACK;
  } else {
    currentState = IDLE;
  }

  // Display the corresponding letter based on the current state
  display.setTextSize(2);
  display.setCursor(SCREEN_WIDTH - 60, SCREEN_HEIGHT - 16);
  if (currentState == RECORDING) {
    display.println("REC");
  } else if (currentState == PLAYBACK) {
    display.println("PLAY");
  } else {
    display.println("IDLE");
  }
  display.display();

  recordButtonState = digitalRead(RECORD_BUTTON_PIN);
  playbackButtonState = digitalRead(PLAYBACK_BUTTON_PIN);

  if (recordButtonState == LOW && !isRecording) {
    recordServoValues();
  }

  if (playbackButtonState == LOW && !isRecording) {
    playbackServoValues();
  }
}
void recordServoValues() {
  isRecording = true;
  servoIndex = 0;

  while (servoIndex < 100) {  // Maximum of 100 recorded values
    for (int i = 0; i < NUM_SERVOS; i++) {
      servoValues[i][servoIndex] = servos[i].read();  // Store current servo position
    }
    delay(10);
    servoIndex++;

    recordButtonState = digitalRead(RECORD_BUTTON_PIN);
    if (recordButtonState == HIGH) {
      break;
    }
  }

  isRecording = false;
}

void playbackServoValues() {
  for (int i = 0; i < servoIndex; i++) {
    for (int j = 0; j < NUM_SERVOS; j++) {
      servos[j].write(servoValues[j][i]);
    }
    delay(10);


}
}

