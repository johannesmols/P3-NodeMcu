/**
 * Author:    Johannes Mols
 * Created:   28.09.2018
 **/

#define BUZZER_PIN D0

// Initial setup for Buzzer
bool buzzerSetupCompleted = false;
void setupBuzzer()
{
  pinMode(BUZZER_PIN, OUTPUT);
  buzzerSetupCompleted = true;
}

void buzzer(unsigned int frequency, unsigned long duration)
{
  tone(BUZZER_PIN, frequency, duration);
}
