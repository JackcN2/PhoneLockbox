byte button =8;
unsigned long startTime;
unsigned long endTime;
unsigned long durat	ion;
int reset = 0;
int bizz = 1;
int maximumMoistureLevel; //The max moisture level   and current moisture levels will be needed for percentage calculations
int currentMoistureLevel;//Like   so: current/max*100 = Moisture level as a percentage
byte timerRunning;
const int buzzer = 3; //buzzer to arduino pin 9
const int waterbuzzer = 4; //buzzer to arduino pin 9

void moistureDetection(){   //Create a function for all the long code, to keep the loop free
   if (currentMoistureLevel > maximumMoistureLevel )
  {
    tone(waterbuzzer, 4000	);
  }
  else {
    noTone(waterbuzzer);    
  }
}

void setup(){
for (int i = 0; i < 4; i++)//Use a for loop, to not have to initiate all the pins   by hand
{
pinMode(i, OUTPUT);
}
pinMode (A5, INPUT); //A0   is the pin used for the Soil Moisture Sensor
maximumMoistureLevel = 100;
pinMode (button, INPUT_PULLUP);
Serial.begin(9600);
pinMode(7, OUTPUT);
pinMode(6, OUTPUT);
}
void loop(){
  currentMoistureLevel = analogRead(A5); //Keep renewing the readings   for the current moisture level
  moistureDetection();
  delay(100); //Short   delay to not overload the program
  char light = Serial.read();
  if (light == '1') {
    if (bizz == 1) {
      digitalWrite(7, HIGH);  // turn the LED on (HIGH is the voltage level)
      tone(buzzer, 1000); // Send 1KHz sound signal...
      delay(1000);
      noTone(buzzer);   
      bizz = 2;
    } 
  }
  if (light == '2') {
    if (bizz == 2) {
      digitalWrite(7, HIGH);  // turn the LED on (HIGH is the voltage level)
      digitalWrite(6, HIGH);  // turn the LED on (HIGH is the voltage level)
      tone(buzzer, 2000); // Send 1KHz sound signal...1
      delay(1000);
      noTone(buzzer);
      bizz = 0;
    }
  }
  if (light == '3') {
    digitalWrite(7, HIGH);  // turn the LED on (HIGH is the voltage level)
    digitalWrite(6, HIGH);  // turn the LED on (HIGH is the voltage level)
    tone(buzzer, 2000); // Send 1KHz sound signal...1
    delay(1000);
    noTone(buzzer);
    bizz = 0;
  }
  if (reset == 2) {
    digitalWrite(7, LOW);  // turn the LED on (HIGH is the voltage level)
    digitalWrite(6, LOW);  // turn the LED on (HIGH is the voltage level)
    reset = 1;
  }

  if (timerRunning == 0 && digitalRead(button) == LOW){ // button pressed & timer not running already
  startTime = millis();
  timerRunning = 1;
  }
  if (timerRunning == 1 && digitalRead(button) == HIGH){ // timer running, button released
  endTime = millis();
  timerRunning = 0;
  duration = endTime - startTime;
  if (duration > 25 && duration < 3000) {
    reset = 2;
    bizz = 1;
    Serial.println ("R");    
  }
  if (duration > 3000) {
    Serial.println ("E");
    tone(buzzer, 1396.91); // Send 1KHz sound signal...
    delay(1200);        // ...for 1 sec
    tone(buzzer, 1046.50); // Send 1KHz sound signal...
    delay(1200);        // ...for 1 sec
    tone(buzzer, 1396.91); // Send 1KHz sound signal...
    delay(1200);        // ...for 1 sec
    tone(buzzer, 1567.98); // Send 1KHz sound signal...
    delay(1200);        // ...for 1 sec
    tone(buzzer, 1661.22); // Send 1KHz sound signal...
    delay(1200);        // ...for 1 sec
    tone(buzzer, 1567.98); // Send 1KHz sound signal...
    delay(1200);        // ...for 1 sec
    tone(buzzer, 1396.91); // Send 1KHz sound signal...
    delay(1200);        // ...for 1 sec
    tone(buzzer, 1108.73	); // Send 1KHz sound signal...
    delay(1200);        // ...for 1 sec
    tone(buzzer, 1046.50); // Send 1KHz sound signal...
    delay(3000);        // ...for 1 sec
    tone(buzzer, 1400); // Send 1KHz sound signal...
    delay(300);        // ...for 1 sec
    tone(buzzer, 1100); // Send 1KHz sound signal...
    delay(300);        // ...for 1 
    tone(buzzer, 1400); // Send 1KHz sound signal...
    delay(300);        // ...for 1 sec
    tone(buzzer, 400); // Send 1KHz sound signal...    
    noTone(buzzer);     // Stop sound...
  }
  }
}




