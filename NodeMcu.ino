
/**
 * Author:    Johannes Mols
 * Created:   28.09.2018
 **/

 /**
  * Color Codes
  * -----------
  * Yellow = Initializing
  * Red = Error
  * Green = Ready, waiting for input
  * Blue = Scanning mode (RFID tags)
  * Purple = Adding mode (RFID tags)
  */

#include <SPI.h> // https://www.arduino.cc/en/Reference/SPI
#include <MFRC522.h> // https://github.com/miguelbalboa/rfid

#define LOGGING true          // Enable or Disable output via Serial
#define BUZZER true           // Enable or Disable sounds of buzzer when scanning tags
#define PAUSE_TIME_MS 50      // Pause time in milliseconds that processes like the RFID Reader or IR Scanner should take between loops to save resources
#define SCAN_TIMEOUT_S 15     // Time in seconds for timeout after the last scanned tag
#define LED_FADING_TIME 5    // Incremental fading time steps in milliseconds

#define SS_PIN D8   // SPI - Slave Select Pin
#define RST_PIN D2  // SPI - Reset Pin - not actually connected

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

// LED Colors
const unsigned char RED[] { 255, 0, 0 };
const unsigned char GREEN[] { 0, 255, 0 };
const unsigned char BLUE[] { 0, 0, 255 };
const unsigned char YELLOW[] { 255, 255, 0 };
const unsigned char PURPLE[] { 127, 0, 255 };
const unsigned char ORANGE[] { 255, 127, 0 };

// Defining MFRC522 here, because there was an issue with the RGB LED displaying the wrong color
// during the scan, when running the scan for the first time. Initializing the MFRC522 at the
// start of the program instead of when the first scan is started, fixes the issue.
MFRC522 mfrc522(SS_PIN, RST_PIN);

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
// StatusLED.ino
void setupLED();
void changeColor(unsigned char color[3], boolean fade);
void changeColor(unsigned char red, unsigned char green, unsigned char blue, boolean fade);
// WiFi.ino
void setupWiFi();
void discoverServer();
/* ----- FUNCTION PROTOTYPES END ----- */

void setup() 
{
  changeColor(YELLOW, false); // initalizing color
  Serial.begin(115200);
  while (!Serial) { } // Wait for serial connection to be established

  SPI.begin(); // Initializing Serial Peripheral Interface to communicate with MFRC522
  mfrc522.PCD_Init(); // Initialize MFRC522
  
  setupWiFi(); // WiFi initialization, blocks until connected
  //discoverServer();

  changeColor(GREEN, false); // initialization complete
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
