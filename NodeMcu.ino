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
  * Pink = Sending Data to the Server
  */

// RFID
#include <SPI.h>                // https://www.arduino.cc/en/Reference/SPI
#include <MFRC522.h>            // https://github.com/miguelbalboa/rfid

// WiFi
#include <ESP8266WiFi.h>        // WiFi library
#include <DNSServer.h>          // DNS Server
#include <ESP8266WebServer.h>   // Web Server
#include <WiFiManager.h>        // Prompt User to Connect to WiFi: https://github.com/tzapu/WiFiManager
#include <WiFiUdp.h>            // UPD functionality
#include <ESP8266HTTPClient.h>  // HTTP Client

// IR Receiver
#include <IRremoteESP8266.h>    // https://github.com/markszabo/IRremoteESP8266
#include <IRrecv.h>             // https://github.com/markszabo/IRremoteESP8266
#include <IRutils.h>            // https://github.com/markszabo/IRremoteESP8266

// Other
#include <ArduinoJson.h>        // JSON Library - use version 5.13.2, version 6 has code breaking changes

#define LOGGING true            // Enable or Disable output via Serial
#define BUZZER true             // Enable or Disable sounds of buzzer when scanning tags
#define PAUSE_TIME_MS 50        // Pause time in milliseconds that processes like the RFID Reader or IR Scanner should take between loops to save resources
#define SCAN_TIMEOUT_S 15       // Time in seconds for timeout after the last scanned tag
#define LED_FADING_TIME 5       // Incremental fading time steps in milliseconds

#define SS_PIN D8               // SPI - Slave Select Pin
#define RST_PIN D2              // SPI - Reset Pin - not actually connected

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
const unsigned char RED[] { 255, 0, 0 };        // Error
const unsigned char GREEN[] { 0, 255, 0 };      // System ready, waiting for input
const unsigned char BLUE[] { 0, 0, 255 };       // Scanning RFID tags
const unsigned char YELLOW[] { 255, 255, 0 };   // Setting up system
const unsigned char PURPLE[] { 127, 0, 255 };   // Searching for server via UDP Broadcast
const unsigned char PINK[] { 255, 0, 255 };     // Registering device with server

// Defining MFRC522 here, because there was an issue with the RGB LED displaying the wrong color
// during the scan, when running the scan for the first time. Initializing the MFRC522 at the
// start of the program instead of when the first scan is started, fixes the issue.
MFRC522 mfrc522(SS_PIN, RST_PIN);

/* ----- FUNCTION PROTOTYPES (only add when there is an error without it) ----- */
// RFIDReader.ino
vector<String> scanForTags(int timeoutInSeconds);
// IRReceiver.ino
IR_CODE listenToIR();
// StatusLED.ino
void changeColor(unsigned char color[3], boolean fade);
void changeColor(unsigned char red, unsigned char green, unsigned char blue, boolean fade);
// WiFi.ino
void setupWiFiDynamic(bool resetSettings);
/* ----- FUNCTION PROTOTYPES END ----- */

void setup() 
{
  changeColor(YELLOW, false); // initalizing color
  Serial.begin(115200);
  while (!Serial) { } // Wait for serial connection to be established

  SPI.begin(); // Initializing Serial Peripheral Interface to communicate with MFRC522
  mfrc522.PCD_Init(); // Initialize MFRC522
  
  setupWiFiDynamic(false); // WiFi initialization, blocks until connected

  changeColor(GREEN, false); // initialization complete
}

void loop() 
{
  // Listen to commands sent by an IR remote directly on the device's IR sensor
  IR_CODE pressedButton = listenToIR();
  if (pressedButton == NUM_0) // Start scanning for RFID tags with specific group number
  {
    scanForTagsAndSendRequest(0);
  }
  else if (pressedButton == NUM_1)
  {
    scanForTagsAndSendRequest(1);
  }
  else if (pressedButton == NUM_2)
  {
    scanForTagsAndSendRequest(2);
  }
  else if (pressedButton == NUM_3)
  {
    scanForTagsAndSendRequest(3);
  }
  else if (pressedButton == NUM_4)
  {
    scanForTagsAndSendRequest(4);
  }
  else if (pressedButton == NUM_5)
  {
    scanForTagsAndSendRequest(5);
  }
  else if (pressedButton == NUM_6)
  {
    scanForTagsAndSendRequest(6);
  }
  else if (pressedButton == NUM_7)
  {
    scanForTagsAndSendRequest(7);
  }
  else if (pressedButton == NUM_8)
  {
    scanForTagsAndSendRequest(8);
  }
  else if (pressedButton == NUM_9)
  {
    scanForTagsAndSendRequest(9);
  }
  else if (pressedButton == VOL_PLUS) // Add a single tag
  {
    #if LOGGING
    Serial.println("Starting scan for single tag...");
    #endif
    String tag = scanForTag(SCAN_TIMEOUT_S);
    String emptyString = "";
    if (tag != "") // Only continue if a tag was scanned
    {
      vector<String> toConvert;
      toConvert.push_back(tag);
      String jsonToSend = parseReadTagsToJson(toConvert, true, -1);
      sendAddTagRequest(jsonToSend);
    }
  }
  else if (pressedButton == FUNC_STOP) // Change the WiFi network
  {
    #if LOGGING
    Serial.println("Resetting WiFi Connection");
    #endif
    setupWiFiDynamic(true);
  }
}

void scanForTagsAndSendRequest(int category) 
{
  #if LOGGING
  Serial.println("Starting scan for group " + (String) category + "...");
  #endif
  vector<String> results = scanForTags(SCAN_TIMEOUT_S);
  String jsonToSend = parseReadTagsToJson(results, false, category);
  sendScannedTags(jsonToSend);
}
