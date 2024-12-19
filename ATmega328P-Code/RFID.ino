#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

#define I2C_ADDRESS 0x13  // Set the I2C address for this device
#define AUTH_PIN 8        // Change from Pin 13 to Pin 8 for successful authentication

void setup() 
{
  pinMode(AUTH_PIN, OUTPUT); // Set AUTH_PIN as output
  digitalWrite(AUTH_PIN, LOW); // Ensure pin starts LOW

  // Start I2C communication as slave
  Wire.begin(I2C_ADDRESS); // Initialize I2C with the slave address 0x13
  Wire.onRequest(requestEvent); // Define callback for when data is requested by master
  
  SPI.begin();      // Initiate SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
}

void loop() 
{
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }

  // Compare UID and grant/deny access
  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  if (content.substring(1) == "04 D1 6C 02 8F 65 80") // Change here the UID of the card/cards that you want to give access
  {
    digitalWrite(AUTH_PIN, HIGH); // Set AUTH_PIN to HIGH for successful authentication
    delay(3000);
    digitalWrite(AUTH_PIN, LOW); // Reset AUTH_PIN to LOW after delay
  }
  else 
  {
    delay(3000); // Just wait for a while if access is denied
  }
}

// Callback function for I2C data request
void requestEvent()
{
  Wire.write('M'); // Send the character 'M' to the I2C master
}
