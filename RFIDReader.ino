/**
 * Author:    Johannes Mols
 * Created:   28.09.2018
 **/

// Read a single RFID tag and return it's UID
vector<byte> readTag()
{
  vector<byte> uid; // Create vector that will store the UID

  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) // proceed if a new tag has been found
  {
    for (byte i = 0; i < mfrc522.uid.size; i++) // loop through the scanned tag and write UID to vector
    {
      uid.push_back(mfrc522.uid.uidByte[i]); // Add byte to vector
    }

    mfrc522.PICC_HaltA(); // Put the read RFID chip to a halt to avoid reading it twice and allowing other chips to be read
  }

  return uid;
}

// Scan for a single RFID tag in proximity and return it's UID as a String
String scanForTag(int timeoutInSeconds)
{
  // Change LED Status color
  changeColor(BLUE, false);

  vector<byte> newUid = readTag();
  unsigned long timeSinceScanStarted = millis();
  while (newUid.size() <= 0) 
  {
    delay(PAUSE_TIME_MS); // Small delay to avoid taking all processing power from the WiFi processes
    if (millis() - timeSinceScanStarted > timeoutInSeconds * 1000) // stop scanning if the time limit is reached
    {
      #if LOGGING
      Serial.println("Time ran out. Returning to normal operation mode.");
      #endif
      return "";
    }
    newUid = readTag();
  }

  String uid;
  // Convert byte vector to String
  for (byte tmp : newUid)
  {
    uid += tmp < 0x10 ? "0" + String(tmp, HEX) : String(tmp, HEX); // Write bytes in 2-char pairs as hexadecimal numbers
  }
  uid.toUpperCase();

  #if LOGGING
  Serial.println("Found Tag: " + uid);
  #endif

  #if BUZZER
  buzzer(250, 250); // Play a short, high pitched sound to show acceptance
  #endif

  // Change LED Status color back to normal
  changeColor(GREEN, false);

  return uid;
}

// Scan for RFID tags in proximity and return a vector of UID's of the found RFID tags
vector<String> scanForTags(int timeoutInSeconds)
{
  // Change LED Status color
  changeColor(BLUE, false);
  
  vector<String> foundTags;
  
  unsigned long timeSinceLastScannedTag = millis();
  while (millis() - timeSinceLastScannedTag < timeoutInSeconds * 1000) // Scan until timeout is reached
  {
    vector<byte> newUid = readTag();
    if (newUid.size() > 0)
    {
      // Convert byte vector to String
      String tmpUid;
      for (byte tmp : newUid)
      {
        tmpUid += tmp < 0x10 ? "0" + String(tmp, HEX) : String(tmp, HEX); // Write bytes in 2-char pairs as hexadecimal numbers
      }
      tmpUid.toUpperCase();

      // Determine if the tag has already been scanned
      boolean duplicate = false;
      for (String tag : foundTags)
      {
        if (tag == tmpUid)
        {
          duplicate = true;
          break;
        }
      }

      if (!duplicate)
      {
        foundTags.push_back(tmpUid); // Add tag to list of found tags if it is no duplicate

        #if LOGGING
        Serial.println("Found Tag: " + tmpUid);
        #endif

        #if BUZZER
        buzzer(250, 250); // Play a short, high pitched sound to show acceptance
        #endif
      }
      #if LOGGING
      else
      {
        Serial.println("Tag " + tmpUid + " has already been scanned.");

        #if BUZZER
        buzzer(100, 250); // Play a short, low pitched sound to show declination
        #endif
      }
      #endif

      timeSinceLastScannedTag = millis(); // Reset timeout timer
    }

    delay(PAUSE_TIME_MS); // Small delay to avoid taking all processing power from the WiFi processes
  }

  #if LOGGING
  Serial.println("RFID Scan Complete");
  if (!foundTags.size() > 0)
  {
    Serial.println("No tags found");
  }
  #endif

  // Change LED Status color back to normal
  changeColor(GREEN, false);

  return foundTags;
}
