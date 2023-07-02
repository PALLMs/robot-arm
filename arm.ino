#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET 4
#define NUM_SERVOS 5

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Servo servos[5];
const int servoPins[5] = {2, 3, 4, 5, 6}; // Digital pins for servos
const int potPins[5] = {A0, A1, A2, A3, A5};
const int recordingButtonPin = 10; // Button pin for recording
const int playbackButtonPin = 11;  // Button pin for playback
const int minPotValues[5] = {430, 30, 140, 1023, 0};    // Minimum potentiometer values for each servo
const int maxPotValues[5] = {840, 615, 870, 400, 1023};  // Maximum potentiometer values for each servo
const int minAngles[5] = {0, 250, 150, 0, 170};  // Minimum servo angles
const int maxAngles[5] = {120, 100, 90, 100, 90};  // Maximum servo angles

int servotrack[NUM_SERVOS][100];

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
  Serial.begin(9600);

  for (int i = 0; i < 5; i++) {
    servos[i].attach(servoPins[i]);
    servos[i].write(minAngles[i]);
    pinMode(potPins[i], INPUT);
  }
  pinMode(recordingButtonPin, INPUT_PULLUP);
  pinMode(playbackButtonPin, INPUT_PULLUP);
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
  if ((digitalRead(playbackButtonPin) == HIGH) && (digitalRead(recordingButtonPin) == HIGH)){
    currentState = IDLE;
  }
  // Check the state of the buttons and set the current state accordingly
  else if (digitalRead(recordingButtonPin) == LOW) {
    currentState = RECORDING;
    for(int i =0; i < 100; i++){
       for (int j=0;j<5;j++)
       {
        servotrack[j][i] = map(analogRead( potPins[j]), minPotValues[j], maxPotValues[j], minAngles[j],  maxAngles[j]);
        }
      delay(500);
    } 
 

  } else if (digitalRead(playbackButtonPin) == LOW) {
    currentState = PLAYBACK;
    for(int i =0; i < 100; i++){
       for (int j=0;j<5;j++)
       {
        servos[j].write( servotrack[j][i] );
        }
      delay(500);
    }

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
  delay(20);
}

