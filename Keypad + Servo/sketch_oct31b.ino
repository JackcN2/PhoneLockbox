#include <Key.h>
#include <Keypad.h>
#include <Servo.h>
#include <DHT.h>
#include <Wire.h>

// Constants
#define DHTPIN A5          // Pin connected to DHT sensor
#define DHTTYPE DHT11      // Type of DHT sensor
DHT dht(DHTPIN, DHTTYPE);  // Initialize DHT sensor

#define DEVICE_ADDRESS 0x12  // I2C device address
#define SIGNAL_PIN 13        // Signal pin for external control
#define BUZZER_PIN 11        // Buzzer pin
#define SERVO_PIN 10         // Servo control pin

Servo myservo;

// Keypad setup
const byte ROWS = 4;  // Number of rows on the keypad
const byte COLS = 4;  // Number of columns on the keypad

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6};  // Connect keypad rows
byte colPins[COLS] = {5, 4, 3, 2};  // Connect keypad columns

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

// Variables
String enteredPassword = "";
String correctPassword = "1234ABC";
int lockAttempts = 3;
bool keypadActive = true;

float maxTemp = 35.0;
float minTemp = 10.0;
float maxHumid = 85.0;
float minHumid = 15.0;
bool sensorCheck = true;

char receivedData = '\0';   // Variable to store received I²C data
char dataToSend = '0';      // Default response to master

// Setup function
void setup() {
  // Initialize serial monitor (for debugging)
  Serial.begin(9600);

  // Initialize servo
  myservo.attach(SERVO_PIN);
  myservo.write(0);  // Start with servo in closed position

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(SIGNAL_PIN, OUTPUT);
  digitalWrite(SIGNAL_PIN, LOW);

  // Initialize DHT sensor
  dht.begin();

  // Start I2C communication
  Wire.begin(DEVICE_ADDRESS);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

// Main loop
void loop() {
  if (sensorCheck) {
    float t = dht.readTemperature();
    float h = dht.readHumidity();

    // Check if sensor data is valid
    if (isnan(t) || isnan(h)) {
      Serial.println("Failed to read from DHT sensor!");
      delay(2000);
      return;
    }

    // Check temperature and humidity thresholds
    if (t >= maxTemp || t <= minTemp || h >= maxHumid || h <= minHumid) {
      myservo.write(90);  // Open the servo
      tone(BUZZER_PIN, 4000, 500);  // Sound the buzzer for 500ms
    }
    delay(1150);  // Delay to prevent sensor overload
  }

  // Handle I²C commands
  if (receivedData == 'E') {
    myservo.write(90);  // Open the servo
    sensorCheck = false;  // Disable sensor checks
  } else if (receivedData == 'R') {
    myservo.write(0);    // Close the servo
    enteredPassword = "";  // Reset entered password
    lockAttempts = 3;    // Reset lock attempts
    keypadActive = true; // Reactivate keypad
    sensorCheck = true;  // Enable sensor checks
  }

  // Handle keypad input
  if (keypadActive) {
    char key = customKeypad.getKey();
    if (key) {
      if (key == '*') {  // Submit the entered password
        if (enteredPassword == correctPassword) {
          sendToMaster("K");  // Notify master of success
          myservo.write(90);  // Open the servo
          keypadActive = false;
        } else {
          lockAttempts--;
          if (lockAttempts == 0) {
            keypadActive = false;  // Disable keypad after 3 failed attempts
          }
        }
        enteredPassword = "";  // Reset the entered password
      } else {
        enteredPassword += key;  // Append key to password
      }
    }
  }
}

// Handle incoming I²C data
void receiveEvent(int bytes) {
  while (Wire.available()) {
    receivedData = Wire.read();
    if (receivedData == 'E' || receivedData == 'R') {
      dataToSend = receivedData;
    }
  }
}

// Handle outgoing I²C data
void requestEvent() {
  Wire.write(dataToSend);
}

// Send data to the I²C master
void sendToMaster(String message) {
  Wire.beginTransmission(DEVICE_ADDRESS);
  Wire.write(message.c_str());  // Send message
  Wire.endTransmission();
}