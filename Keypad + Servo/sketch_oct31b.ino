// Variables
String entpWord = ("");
String pWord = ("1234ABC");
String pWordChk = ("");
int pos1 = 90;
int pos2 = 0;
String kypd = ("");
int lock = (3);

// includes libraries
#include <Key.h>
#include <Keypad.h>
#include <Servo.h>
Servo myservo;

// keypad setup
const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins [ROWS] = {9, 8, 7, 6};
byte colPins [COLS] = {5, 4, 3, 2};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  myservo.attach(10);  
  myservo.write(pos2); 
}

void loop() {
// put your main code here, to run repeatedly:
  if (Serial.available() > 0) {     // checks if data is available
    char input = Serial.read();     // reads serial monitor for input

    if (input == 'E') {  
      myservo.write(pos1);          // moves servo to open position 
      Serial.println("E");
    }

    else if (input == 'R') {  
      myservo.write(pos2);          // moves servo back to closed position
      Serial.println("R");
      kypd = ("");
      lock = 3;
    }
  }
 
 if (kypd != "off"){                // makes sure that the keypad is not off
  char customKey = customKeypad.getKey();
  
  if (customKey) { 
    Serial.println(customKey);      // prints the key pressed
    pWordChk = (customKey);
    
    if (pWordChk == "*"){           // makes sure * is not added to the password
    }
    
    else {
      entpWord += (customKey);      // adds last input to password
    }
  }
  
  if (pWordChk == "*"){             // checks if you entered * to end the password
      if (entpWord == pWord){       // checks if password is corect
        Serial.print("K"); 
        Serial.print("\n");
        entpWord = ("");            // resets entered password for next time
        pWordChk = ("");            // resets the check to end the password
        kypd = ("off");             // turns the keypad off
      }
      
      else {
        entpWord = ("");      
        pWordChk = (""); 
        lock -= 1; 
      }
      
      if (lock == -1){
        kypd = ("off");           // turns the keypad off if you try too many times
      }
  }
  }
}
