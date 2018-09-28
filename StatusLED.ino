/**
 * Author:    Johannes Mols
 * Created:   28.09.2018
 **/

#define LED_BLUE D8
#define LED_GREEN D3
#define LED_RED D2

unsigned char redValue; // value from 0 to 255
unsigned char greenValue;
unsigned char blueValue;

// Initial setup for RGB LED
bool ledSetupCompleted = false;
void setupLED()
{
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  ledSetupCompleted = true;
}

// Change the RGB LED's color by providing RGB color values from 0-255
void changeColor(const unsigned char color[3], boolean fade)
{
  changeColor(color[0], color[1], color[2], fade);
}

// Change the RGB LED's color by providing RGB color values from 0-255
void changeColor(unsigned char red, unsigned char green, unsigned char blue, boolean fade)
{
  if (!ledSetupCompleted)
  {
    setupLED();
  }
  
  if (!fade)
  {
    redValue = red;
    greenValue = green;
    blueValue = blue;
    analogWrite(LED_RED, redValue);
    analogWrite(LED_GREEN, greenValue);
    analogWrite(LED_BLUE, blueValue);
  }
  else
  {
    for (int i = 0; i < 255; i++)
    {
      // Red
      if (redValue < red && redValue < 255)
      {
        analogWrite(LED_RED, ++redValue); // increment
      } 
      else if (redValue > red && redValue > 0)
      {
        analogWrite(LED_RED, --redValue); // decrement
      }

      // Green
      if (greenValue < green && greenValue < 255)
      {
        analogWrite(LED_GREEN, ++greenValue); // increment
      } 
      else if (greenValue > green && greenValue > 0)
      {
        analogWrite(LED_GREEN, --greenValue); // decrement
      }

      // Blue
      if (blueValue < blue && blueValue < 255)
      {
        analogWrite(LED_BLUE, ++blueValue); // increment
      } 
      else if (blueValue > blue && blueValue > 0)
      {
        analogWrite(LED_BLUE, --blueValue); // decrement
      }

      delay(LED_FADING_TIME); // fading time between colors
    }
  }
}
