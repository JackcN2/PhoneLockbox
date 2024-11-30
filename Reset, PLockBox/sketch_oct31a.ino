#include <Wire.h>
#define device_address 0x10
#define signal_pin 13

byte button = 8;
unsigned long startTime;
unsigned long endTime;
unsigned long duration;
int reset = 0;
byte timerRunning = 0;
const int buzzer = 3; // buzzer to Arduino pin 3

char receivedData; // Stores data received from I2C master

void setup() {
  pinMode(signal_pin, OUTPUT);
  digitalWrite(signal_pin, LOW);

  for (int i = 0; i < 4; i++) { // Use a for loop to avoid initiating pins manually
    pinMode(i, OUTPUT);
  }

  pinMode(button, INPUT_PULLUP);
  pinMode(7, OUTPUT);
  pinMode(6, OUTPUT);

  Wire.begin(device_address); // Initialize as I2C slave with address 0x10
  Wire.onReceive(receiveEvent); // Define function to handle incoming data
}

void loop() {
  delay(1150); // Short delay to prevent program overload

  // Process received data
  if (receivedData == '1') {
    digitalWrite(7, HIGH);  // Turn on LED
    tone(buzzer, 1000);     // Send 1kHz sound signal
    delay(1000);
    noTone(buzzer);
  } else if (receivedData == '2') {
    digitalWrite(7, HIGH);
    digitalWrite(6, HIGH);
    tone(buzzer, 2000);     // Send 2kHz sound signal
    delay(1000);
    noTone(buzzer);
  }

  if (reset == 2) {
    digitalWrite(7, LOW);  // Turn off LEDs
    digitalWrite(6, LOW);
    reset = 1;
  }

  if (timerRunning == 0 && digitalRead(button) == LOW) { // Button pressed, start timer
    startTime = millis();
    timerRunning = 1;
  }
  if (timerRunning == 1 && digitalRead(button) == HIGH) { // Button released, stop timer
    endTime = millis();
    timerRunning = 0;
    duration = endTime - startTime;

    if (duration > 25 && duration < 3000) {
      reset = 2;
      Wire.beginTransmission(device_address);
      Wire.write("R"); // Send "R" to master
      Wire.endTransmission();
    }
    if (duration > 3000) {
      Wire.beginTransmission(device_address);
      Wire.write("E"); // Send "E" to master
      Wire.endTransmission();

      // Long beep sequence
      tone(buzzer, 1396.91);
      delay(1200);
      tone(buzzer, 1046.50);
      delay(1200);
      tone(buzzer, 1396.91);
      delay(1200);
      tone(buzzer, 1567.98);
      delay(1200);
      tone(buzzer, 1661.22);
      delay(1200);
      tone(buzzer, 1567.98);
      delay(1200);
      tone(buzzer, 1396.91);
      delay(1200);
      tone(buzzer, 1108.73);
      delay(1200);
      tone(buzzer, 1046.50);
      delay(3000);
      tone(buzzer, 1400);
      delay(300);
      tone(buzzer, 1100);
      delay(300);
      tone(buzzer, 1400);
      delay(300);
      tone(buzzer, 400);
      noTone(buzzer);
    }
  }
}

// Function to handle incoming data from I2C master
void receiveEvent(int bytes) {
  while (Wire.available()) {
    receivedData = Wire.read(); // Read received byte
  }
}
