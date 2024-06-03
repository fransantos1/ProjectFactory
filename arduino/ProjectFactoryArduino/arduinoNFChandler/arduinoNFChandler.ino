#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <EEPROM.h>


#define PN532_IRQ   (2)
#define PN532_RESET (3)  // Not connected by default on the NFC Shield
#define RED_LED 9
#define GREEN_LED 10

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

#define USERTOKEN_LEN 15
char userToken[USERTOKEN_LEN + 1];
bool writeNFC= false;

void setup(void) {
  Serial.begin(115200);
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  digitalWrite(RED_LED,HIGH);
  if (EEPROM.read(0) == 0){
    char ch;
    int i = 0;
    while ((ch = EEPROM.read(1 + i)) != '\0') {
      userToken[i] = ch;
      i++;
      
    }
    Serial.print("EEPROM ");
    writeNFC = true;
    Serial.println(userToken);
    digitalWrite(RED_LED,LOW);
    digitalWrite(GREEN_LED,HIGH);
  }

  //nfc.setPassiveActivationRetries(0x10);
}

  //state machine
  // on the sender the esp just needs to send a \0 before sending the command and the params
  //\0NFC\0TOKEN\0
  //
  //
  int state = 0; // 0 - no input, //1 - awaiting function, //2 - nfc function
  //if on 0, can only go to 1
  //if on 1, can only go to id corresponding to the function

void loop() {
  if(writeNFC){
    NFCHandler();
  }
  if (Serial.available()) { // Check if data is available to read
      Serial.println("NEW message");
      String receivedData = Serial.readStringUntil('\0'); //ignores everything before the command
      switch (state) {
        case 0:
          state = 1; 
          break;
        case 1://verify what function to preform
          if(receivedData.equals("NFC")){
            Serial.println("NFC");
            state = 2;
          }
          break;
        case 2:
          Serial.println("NEW TOKEN");
          if (receivedData.length() < sizeof(userToken)) {
            receivedData.toCharArray(userToken, sizeof(userToken)); // Convert String to char array
          } else {
            Serial.println("String is too long to fit in the char array");
            return;
          }
          digitalWrite(RED_LED,LOW);
          digitalWrite(GREEN_LED,HIGH);
          //save on eeprom information
          EEPROM.write(0, 0);
          int i;
          for (i = 1; i <= sizeof(userToken) ; i++)
              EEPROM.write(i, userToken[i-1]);
          EEPROM.write(i+1, '\0');
          writeNFC = true;
          state = 0;
          break;
      }
  }
}

void NFCHandler(){
  uint8_t success;                          // Flag to check if there was an error with the PN532
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  // Use the default NDEF keys (these would have have set by mifareclassic_formatndef.pde!)
  uint8_t keyb[6] = { 0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7 };
  uint8_t data[] = {
          0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00
        };
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 150);
  if (success)  
  {
    // Display some basic information about the card
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");

    // Make sure this is a Mifare Classic card
    if (uidLength != 4)
    {
      Serial.println("Ooops ... this doesn't seem to be a Mifare Classic card!");
      return;
    }

    // We probably have a Mifare Classic card ...
    Serial.println("Seems to be a Mifare Classic card (4 byte UID)");

    // Check if this is an NDEF card (using first block of sector 1 from mifareclassic_formatndef.pde)
    // Must authenticate on the first key using 0xD3 0xF7 0xD3 0xF7 0xD3 0xF7
    success = nfc.mifareclassic_AuthenticateBlock (uid, uidLength, 4, 0, keyb);
    if (!success)
    {
      Serial.println("Unable to authenticate block 4 ... is this card NDEF formatted?");
      return;
    }

    Serial.println("Authentication succeeded (seems to be an NDEF/NFC Forum tag) ...");
    // Authenticated seems to have worked
    // Try to write an NDEF record to sector 1
    // Use 0x01 for the URI Identifier Code to prepend "http://www."
    // to the url (and save some space).  For information on URI ID Codes
    // see http://www.ladyada.net/wiki/private/articlestaging/nfc/ndef
    if (strlen(userToken) > 38)
    {
      // The length is also checked in the WriteNDEFURI function, but lets
      // warn users here just in case they change the value and it's bigger
      // than it should be
      Serial.println("URI is too long ... must be less than 38 characters!");
      return;
    }
    success = nfc.mifareclassic_WriteDataBlock(4, data);
    success = nfc.mifareclassic_WriteDataBlock(5, data);
    success = nfc.mifareclassic_WriteDataBlock(6, data);
    success = nfc.mifareclassic_WriteDataBlock(4, userToken);
    if (success)
    {
      Serial.println("NDEF URI Record written to sector 1");
      Serial.println("");
    }
    else
    {
      Serial.println("NDEF Record creation failed! :(");
    }
      Serial.println("\n\nDone!");
    delay(1000);
    Serial.flush();
    while(Serial.available()) Serial.read();
    writeNFC = false;
    EEPROM.write(0, 1);
    digitalWrite(RED_LED,HIGH);
    digitalWrite(GREEN_LED,LOW);
    return;
  }
  // Wait a bit before trying again


}

