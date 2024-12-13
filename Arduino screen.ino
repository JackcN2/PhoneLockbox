// Include the library code:
#include <LiquidCrystal.h>
#include <Wire.h>

#define device_address 0x11
#define signal_pin 13

char receivedData;        // Variable to store received I2C data
char data_to_send = 0;    // Variable to store data to send (V when countdown is done)
int countdown = 11;       // Countdown starting from 11 seconds

// Initialize the library by associating the necessary LCD interface pins
const int rs = 12, en = 10, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Function to perform countdown
void count() {
  while (countdown > 0) {
    lcd.clear();          // Clear the display
    lcd.setCursor(0, 0);  // Set cursor to the first column, first row
    lcd.print("Counting: "); // Display the label "Counting:"
    lcd.setCursor(0, 1);  // Move to the second row
    lcd.print(countdown); // Print the countdown value
    delay(1000);

    // Check for reset signal via I2C
    if (receivedData == 'R') { 
      receivedData = 0; // Reset the received data
      countdown = 11;   // Restart countdown
    }

    countdown--; // Decrease the countdown
  }

  // When countdown reaches 0, display the message "Box Open"
  lcd.clear();            // Clear the display
  lcd.setCursor(0, 0);    // Set cursor to the first column, first row
  lcd.print("Box Open");  // Display "Box Open"

  // Set data to 'V' when countdown finishes
  data_to_send = 'V';

  // Trigger the signal before sending 'V'
  triggerSignal();        // Trigger external signal to master
}

// I2C receive event handler
void receiveEvent(int bytes) {
  while (Wire.available()) {
    receivedData = Wire.read(); // Read the data sent by the master
  }
}

// Function to trigger signal to master
void triggerSignal() {
  digitalWrite(signal_pin, HIGH); // Set signal pin HIGH
  delay(50);                      // Short pulse duration
  digitalWrite(signal_pin, LOW);  // Reset signal pin to LOW
}

// Function to send data when requested by master
void sendData() {
  if (data_to_send != 0) {
    Wire.write(data_to_send);  // Send the current data ('V' when countdown finishes)
    data_to_send = 0;          // Reset the data to prevent repeated sending
  }
}

void setup() {
  // Set up the LCD's number of columns and rows (16x2):
  lcd.begin(16, 2);

  // Initialize I2C as a slave
  Wire.begin(device_address);
  Wire.onReceive(receiveEvent); // Register receive event handler
  Wire.onRequest(sendData);     // Register send event handler

  // Initialize signal pin
  pinMode(signal_pin, OUTPUT);
  digitalWrite(signal_pin, LOW);

  // Start the countdown
  count();
}

void loop() {
  // Main loop logic is now handled through I2C communication and events
}
