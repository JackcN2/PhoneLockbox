#include <Keypad.h>
#include <Servo.h>
#include <DHT.h>
#include <Wire.h>

// Libraries for DHT and Servo
#define DHTPIN A5       // Define the pin used to connect the sensor
#define DHTTYPE DHT11  // Define the sensor type
DHT dht(DHTPIN, DHTTYPE);  // Create a DHT objectA5
#define device_address 0x12 // Slave I2C address
#define signal_pin 13
Servo myservo;

// Variables
String entpWord = "";
String pWord = "1234ABC";
String pWordChk = "";
int pos1 = 90;
int pos2 = 0;
String kypd = "on";
int lock = 3;
float maxTemp = 35.0;
float minTemp = 10.0;
float maxHumid = 85.0;
float minHumid = 15.0;
int sensChk = 1;
const int buzzer = 11;

char dataToSend = '0'; // Data to send to the master
char receivedData;     // Data received from the master

// Keypad setup
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
  Serial.begin(9600); // Debugging output
  myservo.attach(10);  
  myservo.write(pos2);

  pinMode(buzzer, OUTPUT);
  pinMode(signal_pin, OUTPUT);
  digitalWrite(signal_pin, LOW);

  Wire.begin(device_address);       // Initialize I2C as a slave
  Wire.onReceive(receiveEvent);     // Callback for receiving data
  Wire.onRequest(requestEvent);     // Callback for sending data
}

void loop() {
  // Sensor logic
    if (sensChk == false) {
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

  // Keypad logic
  if (kypd == "on") {
    char customKey = customKeypad.getKey();
    if (customKey) {
      pWordChk = customKey;
      if (pWordChk != "*") {
        entpWord += customKey;
      }

      if (pWordChk == "*") {
        if (entpWord == pWord) {
          dataToSend = 'K'; // Notify master of success
          entpWord = "";
          pWordChk = "";
          kypd = "off";
        } else {
          entpWord = "";
          pWordChk = "";
          lock -= 1;
        }

        if (lock == -1) {
          kypd = "off";
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

  if (receivedData == 'E') {
    myservo.write(pos1); 
    sensChk = 0;
  } else if (receivedData == 'R') {
    myservo.write(pos2); 
    kypd = "on";
    lock = 3;
    sensChk = 1;
  }
}

void requestEvent() {
  Serial.print("Master requested data: Sending ");
  Serial.println(dataToSend);
  Wire.write(dataToSend); // Send the value of `dataToSend` to the master
}
