// Include the library code:
#include <LiquidCrystal.h>
#include <Wire.h>

#define device_address 0x11
#define signal_pin 13

char receivedData; // Variable to store received I2C data
bool data_ready = false; // Flag indicating if data is ready to send
int data_to_send = 0; // Data to send back to master

// Initialize the library by associating the necessary LCD interface pins
const int rs = 12, en = 10, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Function to perform countdown
void count() {
  int countdown = 11;

  while (countdown > 0) {
    lcd.clear();          // Clear the display
    lcd.setCursor(0, 0);  // Set cursor to the first column, first row
    lcd.print("Counting: "); // Display the label "Counting:"
    lcd.setCursor(0, 1);  // Move to the second row
    lcd.print(countdown); // Print the countdown value
    delay(1000);

    if (receivedData == 'R') { // Check for reset signal via I2C
      receivedData = 0; // Reset the received data
      count(); // Restart the countdown
    }

    countdown--;
  }

  // When countdown reaches 0, display the message "Box Open"
  lcd.clear();            // Clear the display
  lcd.setCursor(0, 0);    // Set cursor to the first column, first row
  lcd.print("Box Open");  // Display "Box Open"

  // Signal the master that the countdown is complete
  data_to_send = V;              // Set data to send
  data_ready = true;             // Flag data as ready
  digitalWrite(signal_pin, HIGH); // Signal using the output pin
  delay(100);                     // Allow time for detection
  digitalWrite(signal_pin, LOW);  // Reset signal line
}

// Function to send data to I2C master
void sendData() {
  if (data_ready) {
    Wire.write(data_to_send); // Send data to the master
    data_ready = false;       // Reset the flag
  } else {
    Wire.write(0); // Send a default value if no data is ready
  }
}

// I2C receive event handler
void receiveEvent(int bytes) {
  while (Wire.available()) {
    receivedData = Wire.read(); // Read the data sent by the master
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
