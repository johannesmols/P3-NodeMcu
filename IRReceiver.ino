/**
 * Author:    Johannes Mols
 * Created:   28.09.2018
 **/

#include <IRremoteESP8266.h> // https://github.com/markszabo/IRremoteESP8266
#include <IRrecv.h> // https://github.com/markszabo/IRremoteESP8266
#include <IRutils.h> // https://github.com/markszabo/IRremoteESP8266

#define IR_PIN D1 // IR (Infrared) Receiver Pin

IRrecv irrecv(IR_PIN); // Instantiate IR receiver pin

decode_results results; // Received signals

// Initial setup for IR Receiver
bool irSetupCompleted = false;
void setupIRReader()
{
  irrecv.enableIRIn();
  irSetupCompleted = true;
}

// Run continuously and receive any incoming infrared signals
IR_CODE listenToIR()
{
  if (!irSetupCompleted)
  {
    setupIRReader();
  }

  IR_CODE returnCode; // Pressed button that will be returned
  
  if (irrecv.decode(&results)) // Decide which button address the decodede result refers to
  {
    switch (results.value)
    {
      case POWER:         returnCode = POWER; break;
      case FUNC_STOP:     returnCode = FUNC_STOP; break;
      case VOL_PLUS:      returnCode = VOL_PLUS; break;
      case FAST_BACK:     returnCode = FAST_BACK; break;
      case PAUSE:         returnCode = PAUSE; break;
      case FAST_FORWARD:  returnCode = FAST_FORWARD; break;
      case DOWN:          returnCode = DOWN; break;
      case VOL_MIN:       returnCode = VOL_MIN; break;
      case UP:            returnCode = UP; break;
      case EQ:            returnCode = EQ; break;
      case ST_REPT:       returnCode = ST_REPT; break;
      case NUM_0:         returnCode = NUM_0; break;
      case NUM_1:         returnCode = NUM_1; break;
      case NUM_2:         returnCode = NUM_2; break;
      case NUM_3:         returnCode = NUM_3; break;
      case NUM_4:         returnCode = NUM_4; break;
      case NUM_5:         returnCode = NUM_5; break;
      case NUM_6:         returnCode = NUM_6; break;
      case NUM_7:         returnCode = NUM_7; break;
      case NUM_8:         returnCode = NUM_8; break;
      case NUM_9:         returnCode = NUM_9; break;
      case REPETITION:    returnCode = REPETITION; break;
      default:            returnCode = UNKNOWN_BUTTON; break;
    }
    
    irrecv.resume();  // Receive the next value
  }

  delay(PAUSE_TIME_MS); // Small delay to avoid taking all processing power from the WiFi processes

  return returnCode;
}
