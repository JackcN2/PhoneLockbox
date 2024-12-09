#include <Keypad.h>
#include <Servo.h>
#include <Wire.h>
#include <DHT.h>

//DHT Setup
#define DHTPIN A5       // Define the pin used to connect the sensor
#define DHTTYPE DHT11  // Define the sensor type
DHT dht(DHTPIN, DHTTYPE);  // Create a DHT object

// I2C address
#define DEVICE_ADDRESS 0x12
#define SIGNAL_PIN 13

// Servo and buzzer pins
Servo myservo;
const int BUZZER = 11;

// Keypad variables
String enteredPassword = "";
const String correctPassword = "1234ABC";
String keyInput = "";
int remainingAttempts = 3;
const int LOCK_POSITION = 90;
const int UNLOCK_POSITION = 0;
bool keypadEnabled = true;

// Data for I2C communication
char dataToSend = '0'; // Data to send to the master
char receivedData;     // Data received from the master

// Keypad configuration
const byte ROWS = 4;
const byte COLS = 4;
char hexaKeys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

void setup() {
  // Initialize serial for debugging
  Serial.begin(9600);

  // Servo setup
  myservo.attach(10);
  myservo.write(UNLOCK_POSITION);

  // Buzzer and signal pin setup
  pinMode(BUZZER, OUTPUT);
  pinMode(SIGNAL_PIN, OUTPUT);
  digitalWrite(SIGNAL_PIN, LOW);

  // Initialize I2C as slave
  Wire.begin(DEVICE_ADDRESS);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

void loop() {

// Sensor checking logic
  if (sensChk == 1) {
    float t = dht.readTemperature();
    float h = dht.readHumidity();

    // Check if temperature or humidity is outside the allowed range
    if (t >= maxTemp || t <= minTemp || h >= maxHumid || h <= minHumid) {
      myservo.write(pos1);
      tone(buzzer, 4000);
      delay(500);
      noTone(buzzer);
    }
    delay(1150); // Short delay to not overload the sensor
  }
    
  if (keypadEnabled) {
    char customKey = customKeypad.getKey();
    if (customKey) {
      keyInput = customKey;
      if (keyInput != "*") {
        enteredPassword += customKey; // Add key to the entered password
      } else if (keyInput == "*") {  // Check password when '*' is pressed
        if (enteredPassword == correctPassword) {
          dataToSend = 'K'; // Notify master of success
          Serial.println("Password correct. Lock opened.");
          enteredPassword = "";
          keypadEnabled = false; // Disable keypad after success

          // Trigger signal to master
          triggerSignal();
        } else {
          enteredPassword = ""; // Clear the entered password
          remainingAttempts--;

          if (remainingAttempts <= 0) {
            Serial.println("Keypad locked due to too many failed attempts.");
            keypadEnabled = false;

            // Trigger signal to master
            triggerSignal();
          } else {
            Serial.println("Incorrect password. Attempts remaining: " + String(remainingAttempts));

            // Trigger signal to master
            triggerSignal();
          }
        }
      }
    }
  }
}

void receiveEvent(int bytes) {
  while (Wire.available()) {
    receivedData = Wire.read();
  }
  Serial.print("Received via I2C: ");
  Serial.println(receivedData);

  if (receivedData == 'E') { // Master command to lock
    myservo.write(LOCK_POSITION);
    Serial.println("Servo moved to lock position.");
    triggerSignal();
  } else if (receivedData == 'R') { // Master command to reset
    myservo.write(UNLOCK_POSITION);
    keypadEnabled = true;
    remainingAttempts = 3;
    enteredPassword = "";
    Serial.println("System reset. Keypad re-enabled.");
    triggerSignal();
  }
}

void requestEvent() {
  Serial.print("Master requested data: Sending ");
  Serial.println(dataToSend);
  Wire.write(dataToSend); // Send current status to the master
}

// Function to trigger the signal line
void triggerSignal() {
  digitalWrite(SIGNAL_PIN, HIGH);
  delay(50); // Small pulse duration
  digitalWrite(SIGNAL_PIN, LOW);
}
