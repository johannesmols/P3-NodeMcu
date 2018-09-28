#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN D4   // SPI - Slave Select Pin
#define RST_PIN D2  // SPI - Reset Pin - not actually connected

MFRC522 mfrc522(SS_PIN, RST_PIN);

// Initial setup for SPI and MFRC522
bool rfidSetupCompleted = false;
void setupReader()
{
  SPI.begin();
  mfrc522.PCD_Init();
  rfidSetupCompleted = true;
}

// Read a RFID tag and return it's UID
void readTag(byte toReturn[])
{
  // Run setup if it hasn't been run yet
  if (!rfidSetupCompleted)
  {
    setupReader();
  }

  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) // proceed if a new tag has been found
  {
    #if LOGGING
    Serial.print("Found UID: ");
    #endif

    if (mfrc522.uid.size == sizeof(toReturn) / sizeof(toReturn[0])) // only continue if the scanned tag size is as large as a predefined empty tag
    {
      for (byte i = 0; i < mfrc522.uid.size; i++) // loop through the scanned tag and write UID to array
      {
        toReturn[i] = mfrc522.uid.uidByte[i];

        #if LOGGING
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
        #endif
      }

      #if LOGGING
      Serial.println();
      #endif

      mfrc522.PICC_HaltA(); // Put the read RFID chip to a halt to avoid reading it twice and allowing other chips to be read
    }
    else
    {
      for (byte i = 0; i < sizeof(toReturn) / sizeof(toReturn[0]); i++)
      {
        toReturn[i] = 0; // Change every item in the array to zero to indicate that no tag was found
      }
    }
  }
  else
  {
    for (byte i = 0; i < sizeof(toReturn) / sizeof(toReturn[0]); i++)
    {
      toReturn[i] = 0; // Change every item in the array to zero to indicate that no tag was found
    }
  }
}
