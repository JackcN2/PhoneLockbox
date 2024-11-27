// Include the library code:
#include <LiquidCrystal.h>
#include <Wire.h> 
#define devide_address 0x11
#define signal_pin 13

char reset = ("");

// Initialize the library by associating the necessary LCD interface pin
const int rs = 12, en = 10, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


void count(){
   // Countdown from 30 to 1
 int countdown = 11;
 for (countdown > 0; countdown--;) {
   lcd.clear();          // Clear the display
   lcd.setCursor(0, 0);      // Set cursor to the first column, first row
   lcd.print("Counting: "); // Display the label "Counting:"
   lcd.setCursor(0, 1);      // Move to the second row
   lcd.print(countdown);       // Print the countdown value
   delay(1000);
   char reset = Serial.read();
   if (reset == 'R'){
     countdown = 0;
     count();
   }
   // Wait for 1 second
 }

 // When countdown reaches 0, display the message "Box Open"
 lcd.clear();           // Clear the display
 lcd.setCursor(0, 0);       // Set cursor to the first column, first row
 lcd.print("Box Open");       // Display "Box Open"

    // Send a message to the Serial Monitor
   data_to_send = 2;              // Set data to send
    data_ready = true;              // Flag data as ready
    digitalWrite(signal_pin, HIGH); // Signal the Raspberry Pi
    delay(100);                     // Allow time for detection
    digitalWrite(signal_pin, LOW);  // Reset signal line
  }
}


void sendData() {
  if (data_ready) {
    Wire.write(data_to_send);  // Send data over I2C
    data_ready = false;        // Reset data ready flag
  } else {
    Wire.write(0);  // Send default if no data
  }
}


void setup() {
 // Set up the LCD's number of columns and rows (16x2):
 lcd.begin(16, 2);

    // Start serial communication at 9600 baud rate
    Serial.begin(9600);
    count();
    pinmode(signal_pin,OUTPUT);
    digitalWrite(signal_pin,LOW);
}
void loop() {


    // Wait for 3 seconds before restarting the countdown


    // reset timer
    char reset = Serial.read();

    }




