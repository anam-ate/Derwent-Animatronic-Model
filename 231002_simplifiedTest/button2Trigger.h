
//variables will change:

const int button2Pin = 3;     // the number of the pushbutton pin

boolean Button2WasPressed = false;
int buttonPressCount = 0;    // counter for button presses
int maxPressCount = 3;       // maximum number of button presses before reset
const unsigned long DebounceTime = 10;
unsigned long Button2StateChangeTime = 0; // Debounce timer



void button2TriggerSetup()
{
  // initialize the pushbutton pin as an input:
  pinMode(button2Pin, INPUT_PULLUP);

}

bool button2TriggerSensor()
{
  unsigned long currentTime = millis();

  // read the state of the pushbutton value:
  bool button2IsPressed = digitalRead(button2Pin) == HIGH;  //read state of push button //LOW

  // Check if the button state has changed and passed the debounce time
  if (button2IsPressed != Button2WasPressed && currentTime - Button2StateChangeTime > DebounceTime)
  {
    // Button state has changed
    Button2StateChangeTime = currentTime;

    if (button2IsPressed)
    {
      // Increment the button press count
      buttonPressCount++;

      // If the count reaches the maximum, reset it to 0
      if (buttonPressCount > maxPressCount) 
      {
        buttonPressCount = 0;
      }

      //Serial.print("Button pressed, count: ");
      //Serial.println(buttonPressCount);

      Button2WasPressed = button2IsPressed; // Update the previous button state
      return true; // Return true for a single button press
    }
    else
    {
      // Button has been released, reset the button press count
      //buttonPressCount = 0;
    }
  }

  Button2WasPressed = button2IsPressed; // Update the previous button state
  return false; // No button press detected
}

int getButton2PressCount()
{
  return buttonPressCount;
}
