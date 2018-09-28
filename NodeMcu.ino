/**
 * Author:    Johannes Mols
 * Created:   28.09.2018
 **/

#define LOGGING true          // Enable or Disable output via Serial
#define BUZZER true           // Enable or Disable sounds of buzzer when scanning tags
#define PAUSE_TIME_MS 50      // Pause time in milliseconds that processes like the RFID Reader or IR Scanner should take between loops to save resources
#define SCAN_TIMEOUT_S 30     // Time in seconds for timeout after the last scanned tag

using namespace std;

// Enumeration holding values corresponding to buttons on the infrared remote (Elegoo)
enum IR_CODE
{ 
  POWER = 0xFFA25D,
  FUNC_STOP = 0xFFE21D,
  VOL_PLUS = 0xFF629D,
  FAST_BACK = 0xFF22DD,
  PAUSE = 0xFF02FD,
  FAST_FORWARD = 0xFFC23D,
  DOWN = 0xFFE01F,
  VOL_MIN = 0xFFA857,
  UP = 0xFF906F,
  EQ = 0xFF9867,
  ST_REPT = 0xFFB04F,
  NUM_0 = 0xFF6897,
  NUM_1 = 0xFF30CF,
  NUM_2 = 0xFF18E7,
  NUM_3 = 0xFF7A85,
  NUM_4 = 0xFF10EF,
  NUM_5 = 0xFF38C7,
  NUM_6 = 0xFF5AA5,
  NUM_7 = 0xFF42BD,
  NUM_8 = 0xFF4AB5,
  NUM_9 = 0xFF52AD,
  REPETITION = 0xFFFFFFFF,
  UNKNOWN_BUTTON = 0x0
};

/* ----- FUNCTION PROTOTYPES ----- */
// NodeMcu.ino
void setup();
void loop();
// RFIDReader.ino
void setupReader();
vector<byte> readTag();
vector<String> scanForTags(int timeoutInSeconds);
// IRReceiver.ino
void setupIRReader();
IR_CODE listenToIR();
// Buzzer.ino
void buzzer(unsigned int frequency, unsigned long duration);
/* ----- FUNCTION PROTOTYPES END ----- */

void setup() 
{
  Serial.begin(9600);
  while (!Serial) { } // Wait for serial connection to be established
}

void loop() 
{
  IR_CODE pressedButton = listenToIR();
  switch (pressedButton)
  {
    case PAUSE:
      #if LOGGING
      Serial.println("Starting scan...");
      #endif
      vector<String> results = scanForTags(SCAN_TIMEOUT_S);
      break;
  }
}
