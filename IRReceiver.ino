/**
 * Author:    Johannes Mols
 * Created:   28.09.2018
 **/

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

  IR_CODE returnCode = UNKNOWN_BUTTON; // Pressed button that will be returned, default to unknown
  
  if (irrecv.decode(&results)) // Decide which button address the decodede result refers to
  {
    returnCode = (IR_CODE) results.value;
    irrecv.resume();  // Receive the next value
  }

  delay(PAUSE_TIME_MS); // Small delay to avoid taking all processing power from the WiFi processes

  return returnCode;
}
