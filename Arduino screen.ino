Output = Orange, Input = Blue.
/*
  LiquidCrystal Library - Countdown with "Box Open" Message and Serial Signal

 Demonstrates a countdown on a 16x2 LCD display. When the countdown
 hits zero, the display will show the message "Box Open", send a signal
 to the Serial Monitor, and then restart the countdown.

  The circuit:
  * LCD RS pin to digital pin 12
  * LCD Enable pin to digital pin 10=
  * LCD D4 pin to digital pin 5
  * LCD D5 pin to digital pin 4
  * LCD D6 pin to digital pin 3
  * LCD D7 pin to digital pin 2
  * LCD R/W pin to ground
  * LCD VSS pin to ground
  * LCD VCC pin to 5V
  * 10K resistor:
  * ends to +5V and ground
  * wiper to LCD VO pin (pin 3)
*/

String reset = ("30* ");

// Include the library code:
#include <LiquidCrystal.h>

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
Output = Orange, Input = Blue.
 lcd.clear();           // Clear the display
 lcd.setCursor(0, 0);       // Set cursor to the first column, first row
 lcd.print("Box Open");       // Display "Box Open"

    // Send a message to the Serial Monitor
    Serial.println("Box Open");
}

void setup() {
 // Set up the LCD's number of columns and rows (16x2):
 lcd.begin(16, 2);

    // Start serial communication at 9600 baud rate
    Serial.begin(9600);
    count();
}
void loop() {


    // Wait for 3 seconds before restarting the countdown


    // reset timer
    char reset = Serial.read();


    if (reset == '1'){
      Serial.println("counting");
      count();

    }
}




Keypad output: 1, 2, 3, 4, 5, 6, 7, 8, 9, A, B, C, D, *, #, E, R, K
Output = Orange, Input = Blue.
/*
  LiquidCrystal Library - Countdown with "Box Open" Message and Serial Signal

 Demonstrates a countdown on a 16x2 LCD display. When the countdown
 hits zero, the display will show the message "Box Open", send a signal
 to the Serial Monitor, and then restart the countdown.

  The circuit:
  * LCD RS pin to digital pin 12
  * LCD Enable pin to digital pin 10=
  * LCD D4 pin to digital pin 5
  * LCD D5 pin to digital pin 4
  * LCD D6 pin to digital pin 3
  * LCD D7 pin to digital pin 2
  * LCD R/W pin to ground
  * LCD VSS pin to ground
  * LCD VCC pin to 5V
  * 10K resistor:
  * ends to +5V and ground
  * wiper to LCD VO pin (pin 3)
*/

String reset = ("30* ");

// Include the library code:
#include <LiquidCrystal.h>

// Initialize the library by associating the necessary LCD interface pin
const int rs = 12, en = 10, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


void count(){
   // Countdown from 30 to 1
 int countdown = 31;
 for (countdown > 0; countdown--;) {
   lcd.clear();          // Clear the display
   lcd.setCursor(0, 0);      // Set cursor to the first column, first row
   lcd.print("Time Until Open:"); // Display the label "Counting:"
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
Output = Orange, Input = Blue.
 lcd.clear();           // Clear the display
 lcd.setCursor(0, 0);       // Set cursor to the first column, first row
 lcd.print("Box Open");       // Display "Box Open"

    // Send a message to the Serial Monitor
    Serial.println("Box Open");
}

void setup() {
 // Set up the LCD's number of columns and rows (16x2):
 lcd.begin(16, 2);

    // Start serial communication at 9600 baud rate
    Serial.begin(9600);
    count();
}
void loop() {


    // Wait for 3 seconds before restarting the countdown


    // reset timer
    char reset = Serial.read();


    if (reset == '1'){
      Serial.println("counting");
      count();

    }
}
