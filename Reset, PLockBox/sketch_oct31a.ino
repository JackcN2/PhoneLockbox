#include <Wire.h>

#define DEVICE_ADDRESS 0x14 // I2C address
#define SIGNAL_PIN 13
#define BUTTON1_PIN 8 // Button 1 for Reset (R)
#define BUTTON2_PIN 9 // Button 2 for Emergency Open (V)
#define LED1_PIN 7    // LED 1
#define LED2_PIN 6    // LED 2
#define BUZZER_PIN 3  // Buzzer pin

int resetFlag = 0;
char receivedData;   // Variable to store received data
char dataBuffer;     // Buffer to hold data to send to the master

void setup() {
  pinMode(SIGNAL_PIN, OUTPUT);
  pinMode(BUTTON1_PIN, INPUT_PULLUP);  // Button 1 (R)
  pinMode(BUTTON2_PIN, INPUT_PULLUP);  // Button 2 (Emergency Open)
  pinMode(LED1_PIN, OUTPUT);           // LED1 is on pin 7
  pinMode(LED2_PIN, OUTPUT);           // LED2 is on pin 6
  pinMode(BUZZER_PIN, OUTPUT);         // Buzzer pin

  Wire.begin(DEVICE_ADDRESS); // Initialize as I2C slave with address 0x14
  Wire.onRequest(sendData);   // Handle data request from the master
}

void loop() {
  delay(1150); // Prevent overload

  // Button 1 (R) - Reset: Send "R" over I2C
  if (digitalRead(BUTTON1_PIN) == LOW) {
    dataBuffer = 'R';          // Store "R" in buffer for reset command
    digitalWrite(SIGNAL_PIN, HIGH); // Trigger signal pin
    resetLEDs();               // Reset LEDs
    delay(500); // Debounce delay
  }

  // Button 2 (Emergency Open) - Send "V" over I2C, turn on both LEDs, and trigger buzzer sequence
  if (digitalRead(BUTTON2_PIN) == LOW) {
    dataBuffer = 'V';          // Store "V" in buffer for emergency open command
    digitalWrite(SIGNAL_PIN, HIGH); // Trigger signal pin
    digitalWrite(LED1_PIN, HIGH); // Turn on LED1
    digitalWrite(LED2_PIN, HIGH); // Turn on LED2
    longBeepSequence();        // Trigger long beep sequence
    delay(500); // Debounce delay
  }
  
  // Reset signal pin to LOW after processing
  digitalWrite(SIGNAL_PIN, LOW);
}

void resetLEDs() {
  digitalWrite(LED1_PIN, LOW);  // Turn off LED1
  digitalWrite(LED2_PIN, LOW);  // Turn off LED2
}

void sendData() {
  Wire.write(dataBuffer);  // Send data stored in buffer to the master
}

void longBeepSequence() {
  tone(BUZZER_PIN, 1396.91);   // 1st tone
  delay(1200);
  tone(BUZZER_PIN, 1046.50);   // 2nd tone
  delay(1200);
  tone(BUZZER_PIN, 1396.91);   // 3rd tone
  delay(1200);
  tone(BUZZER_PIN, 1567.98);   // 4th tone
  delay(1200);
  tone(BUZZER_PIN, 1661.22);   // 5th tone
  delay(1200);
  tone(BUZZER_PIN, 1567.98);   // 6th tone
  delay(1200);
  tone(BUZZER_PIN, 1396.91);   // 7th tone
  delay(1200);
  tone(BUZZER_PIN, 1108.73);   // 8th tone
  delay(1200);
  tone(BUZZER_PIN, 1046.50);   // 9th tone
  delay(3000); // Long delay before the next tones
  
  // Short sequence to end the sound
  tone(BUZZER_PIN, 1400);       // Final tone 1
  delay(300);
  tone(BUZZER_PIN, 1100);       // Final tone 2
  delay(300);
  tone(BUZZER_PIN, 1400);       // Final tone 3
  delay(300);
  tone(BUZZER_PIN, 400);        // Short tone
  noTone(BUZZER_PIN);           // Stop the buzzer
}
