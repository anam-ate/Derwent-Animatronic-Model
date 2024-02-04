#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include "lights01.h"

const uint8_t PROGMEM gamma8[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2,
  2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5,
  5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10,
  10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
  17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
  25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
  37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
  51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
  69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
  90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 109, 110, 112, 114,
  115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142,
  144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175,
  177, 180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
  215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 255
};

SoftwareSerial softSerial(10, 11);  // RX, TX for DFPlayer
DFRobotDFPlayerMini myDFPlayer;
long timer = 0;
int internalSequenceState = 0;
#define POT_PIN A0  // Define the analog pin where the potentiometer is connected
int instantOffOnSpeed = 0;
// light colour value

const long intervalRoof = 1000;        // the time we wait after roof lights fade down
const long intervalFirstFloor = 1000;  // the time we wait after first floor lights fade down
//const long intervalGround = 13000;      // the time we wait after ground lights fade down

int r = 254;  //255
int g = 165;  //77
int b = 0;
int w = 150;  //32
int i = 0;

// Global variable to control fading
bool isFadingGround = false;
bool isFadingFirstFloor = false;
bool isFadingTopFloor = false;

Lights groundLights(20, 5, gamma8[r], gamma8[g], gamma8[b], gamma8[w], 1);      //26 (actual pin) //7
Lights firstFloorLights(20, 7, gamma8[r], gamma8[g], gamma8[b], gamma8[w], 1);  //26 (actual pin) //7
Lights topFloorLights(20, 6, gamma8[r], gamma8[g], gamma8[b], gamma8[w], 1);    //26 (actual pin) //7

void setup() {
  Serial.begin(9600);      // Start serial communication at 9600 bps
  softSerial.begin(9600);  // Start the software serial for DFPlayer
  Serial.println("start");
  groundLights.setup();
  firstFloorLights.setup();
  topFloorLights.setup();


   if (!myDFPlayer.begin(softSerial)) {
    //Serial.println("Initialization of DFPlayer failed!");
    while (true)
      ;
    }
    //Serial.println("DFPlayer initialized!");

    myDFPlayer.volume(20);  // Set volume
}

void loop() {

  if (Serial.available()) {        // Check if data is available to read from the serial port
    char command = Serial.read();  // Read the incoming byte as char

    Serial.println("Data received: " + String(command));  // Feedback on what data is received

    switch (command)
    {
      case '1':
        {
          myDFPlayer.play(1);  // Play track 1
          delay(100);          // delay for 100ms
          break;
        }
      case '2':
        {
          myDFPlayer.stop();  // Stop playing
          delay(100);         // delay for 100ms
          break;
        }
      case '3':
        {
          int i = 0;
          for (i = 0; i < 20; i = i + 1)  // fade lights on the ground floor only
          {
            groundLights.fadeUpStart(i, 1);
          }
          isFadingGround = true;
          break;
        }

      case '4':
        {
          i = 0;
          for (i = 0; i < 20; i = i + 1)  // fade lights on the ground floor only
          {
            firstFloorLights.fadeUpStart(i, 1);
          }
          isFadingFirstFloor = true;
          break;
        }
      case '5':
        {
          i = 0;
          for (i = 0; i < 20; i = i + 1)  // fade lights on the ground floor only
          {
            topFloorLights.fadeUpStart(i, 1);
          }
          isFadingTopFloor = true;
          break;
        }

      case '6':  // Starting the sequential fade-down process
        {
          Serial.println("fade off lights triggered");
          if (internalSequenceState == 0) {
            timer = millis();
            internalSequenceState = 1;
          }
          break;
        }
      case '7':
        {
          for (i = 0; i < 20; i = i + 1)  // fade lights on the ground floor only
          {
            groundLights.instantOn(i);
            firstFloorLights.instantOn(i);
            topFloorLights.instantOn(i);
          }
          break;
    
        }

      case '8':
        {
          for (i = 0; i < 20; i = i + 1)  // fade lights on the ground floor only
          {
            groundLights.instantOff(i);
            firstFloorLights.instantOff(i);
            topFloorLights.instantOff(i);
          }
          break;

        }

    }
  }

  // Place this just below the entire switch block

  if (internalSequenceState == 1 && millis() - timer >= intervalRoof) {
    for (int i = 0; i < 20; i++) {
      topFloorLights.fadeDownStart(i, 1);
      firstFloorLights.fadeDownStart(i, 1);
      groundLights.fadeDownStart(i, 1);
    }
    
    isFadingTopFloor = true;
    isFadingFirstFloor = true;
    isFadingGround = true;
    timer = millis();  // Reset timer
  }

  // This part continuously updates the fading process
  if (isFadingGround) {
    bool done = groundLights.lightSequence();
    if (done) {
      isFadingGround = false;  // End the fading process once done
      internalSequenceState = 0;
    
    }
  }

  if (isFadingFirstFloor) {
    bool done = firstFloorLights.lightSequence();
    if (done) {
      isFadingFirstFloor = false;  // End the fading process once done
      internalSequenceState = 0;
      
    }
  }
  if (isFadingTopFloor) {
    bool done = topFloorLights.lightSequence();
    if (done) {
      isFadingTopFloor = false;  // End the fading process once done
      internalSequenceState = 0;
      
    }
  }
}
