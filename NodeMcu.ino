#define LOGGING true // Enable or Disable output via Serial

using namespace std;

/* ----- FUNCTION PROTOTYPES ----- */
// NodeMcu.ino
void setup();
void loop();
// RFIDReader.ino
void setupReader();
vector<byte> readTag();
vector<String> scanForTags(int timeoutInSeconds);
/* ----- FUNCTION PROTOTYPES END ----- */

void setup() 
{
  Serial.begin(9600);

  scanForTags(10);
}

void loop() 
{
  
}
