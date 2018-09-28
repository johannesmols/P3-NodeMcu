#define LOGGING false // Enable or Disable output via Serial
#define UID_SIZE 4 // 4 bytes is the standard size of a RFID UID

void setup() 
{
  Serial.begin(9600);

  scanForTags(10);

  std::vector<byte> uid;
}

void loop() 
{
  
}

void scanForTags(int durationInSeconds)
{
  unsigned long timeSinceLastScannedTag = millis();
  while (millis() - timeSinceLastScannedTag < durationInSeconds * 1000) // Scan until timeout is reached
  {
    byte tmpReturnedUid[UID_SIZE]; // temporary array to store the scanned tags
    readTag(tmpReturnedUid); // give memory address to function to write scanned UID into array

    delay(100); // Small delay to avoid taking all process power from the WiFi processes

    // Check if the returned UID is containing only zeroes
    int zeroes = 0;
    for (byte tmp : tmpReturnedUid)
    {
      if (tmp == 0) { zeroes++; }
    }

    // Only accept tag that has values other than zeroes
    if (zeroes < UID_SIZE)
    {
      Serial.print("Found tag: ");
      for (byte tmp : tmpReturnedUid)
      {
        Serial.print(tmp < 0x10 ? " 0" : " ");
        Serial.print(tmp, HEX);
      }
      Serial.println();

      timeSinceLastScannedTag = millis(); // Reset timeout timer
    }
  }

  Serial.println("Time ran out!");
}
