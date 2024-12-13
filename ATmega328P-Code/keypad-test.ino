// Includes libraries
#include <Keypad.h>
#include <Servo.h>
#include <DHT.h>
#include <Wire.h>

dht DHT;
#define DHT11_PIN A5
#define device_address 0x12
#define signal_pin 13
Servo myservo;

// Variables
String entpWord = ("");
String pWord = ("1234ABC");
String pWordChk = ("");
int pos1 = 90;
int pos2 = 0;
String kypd = ("");
int lock = (3);
float maxTemp = 35.0;
float minTemp = 10.0;
float maxHumid = 85.0;
float minHumid = 15.0;
int sensChk = 1;
const int buzzer = 11;

// Keypad setup
const byte ROWS = 4;     // Defines the number of rows on the keypad
const byte COLS = 4;     // Defines the number of columns on the keypad

// Defines what each button on the keypad is
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// Connects the keypad rows and columns to specific pins on the Arduino
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

char receivedData; // Variable to store data received via I2C

void setup() {
  // Initialize Servo
  myservo.attach(10);  
  myservo.write(pos2); 

  pinMode(buzzer, OUTPUT);
  pinMode(signal_pin, OUTPUT);
  digitalWrite(signal_pin, LOW);

  // Start I2C communication as a slave
  Wire.begin(device_address);
  Wire.onReceive(receiveEvent); // Handle incoming data
}

void loop() {
  // Sensor checking logic
  if (sensChk == 1) {
    float chk = DHT.read11(DHT11_PIN);

    // Check if temperature or humidity is outside the allowed range
    if (DHT.temperature >= maxTemp || DHT.temperature <= minTemp || DHT.humidity >= maxHumid || DHT.humidity <= minHumid) {
      myservo.write(pos1);
      tone(buzzer, 4000);
      delay(500);
      noTone(buzzer);
    }
    delay(1150); // Short delay to not overload the sensor
  }

  // Process received I2C command
  if (receivedData == 'E') {
    myservo.write(pos1); // Move servo to open position
    sensChk = 0;
  } else if (receivedData == 'R') {
    myservo.write(pos2); // Move servo back to closed position
    kypd = ("");
    lock = 3;
    sensChk = 1;
  }

  // Keypad functionality
  if (kypd != "off") { // Ensure keypad is active
    char customKey = customKeypad.getKey();
    if (customKey) {
      pWordChk = customKey;
      if (pWordChk != "*") {
        entpWord += customKey; // Add last input to password
      }

      if (pWordChk == "*") { // Check if password entry is completed
        if (entpWord == pWord) {
          sendToMaster("K"); // Notify master of success
          entpWord = ("");
          pWordChk = ("");
          kypd = ("off"); // Deactivate keypad
        } else {
          entpWord = ("");
          pWordChk = ("");
          lock -= 1;
        }

        if (lock == -1) {
          kypd = ("off"); // Deactivate keypad after too many failed attempts
        }
      }
    }
  }
}

// Function to handle incoming I2C data
void receiveEvent(int bytes) {
  while (Wire.available()) {
    receivedData = Wire.read(); // Read the received byte
  }
}

// Function to send data to the I2C master
void sendToMaster(String message) {
  Wire.beginTransmission(device_address);
  Wire.write(message.c_str()); // Send string data as bytes
  Wire.endTransmission();
}
