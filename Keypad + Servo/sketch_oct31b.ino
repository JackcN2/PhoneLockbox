// includes libraries
#include <Key.h>
#include <Keypad.h>
#include <Servo.h>
#include <dht.h>
dht DHT;
#define DHT11_PIN A5
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

// keypad setup
const byte ROWS = 4;     //defines the amount of rows on the keypad
const byte COLS = 4;     // defines the amount of columns on the keypad

//defines what each button on the keypad is
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

//connects the keypad rows and columns to specific pins on the arduino
byte rowPins [ROWS] = {9, 8, 7, 6};
byte colPins [COLS] = {5, 4, 3, 2};

//
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  myservo.attach(10);  
  myservo.write(pos2); 
  pinMode(sensBuzzer, OUTPUT);
}

void loop() {

//checks if the sensor shoould be on
if (sensChk == 1){}

//checks the output from the sensor
float chk = DHT.read11(DHT11_PIN);

//checks if the  output is less or more than the min/max temp/humidity
if(DHT.temperature >= maxTemp or DHT.temperature <= minTemp or DHT.humidity >= maxHumid or DHT.humidity <= minHumid){
  myserco.write(pos1);
  tone(sensBuzzer, 4000);
  delay(500);
  noTone(sensBuzzer);
}
delay(1150); //Short delay to not overload the sensor
}
// put your main code here, to run repeatedly:
  if (Serial.available() > 0) {     // checks if data is available
    char input = Serial.read();     // reads serial monitor for input

    if (input == 'E') {  
      myservo.write(pos1);          // moves servo to open position 
      Serial.println("E");
      sensChk = 0;
    }

    else if (input == 'R') {  
      myservo.write(pos2);          // moves servo back to closed position
      Serial.println("R");
      kypd = ("");
      lock = 3;
      sensChk = 1;
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
