#define LOGGING true // Enable or Disable output via Serial

void setup() 
{
  Serial.begin(9600);
}

void loop() 
{
  scanForTags(1337);
  delay(1000);
}

void scanForTags(int durationInSeconds)
{
  byte tmpReturnedUid[4]; // 4 bytes is the standard size of a RFID UID
  readTag(tmpReturnedUid); // give memory address to function to write scanned UID into array
}
