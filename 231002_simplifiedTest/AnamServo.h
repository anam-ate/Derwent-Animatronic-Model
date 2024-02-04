#include "Arduino.h"

#define servoMax 2550  // Maximum position for servo
#define servoMin 800   // Minimum position for servo

class AnamServo {
public:

  String servoName;
  int id;
  int relayPin;
  int dataPin;
  int servoSpeed;
  int startPos;
  int firstPos;
  int secondPos;
  int thirdPos;

  int currentPos;  // Current position of the servo (in pulselength)
  int gotoPos;     // Goto position - servo will move towards this position

  long startTime;  // Starting time for this script
  long lastPulse;  // the time in milliseconds of the last pulse
  
  int holdPos;  // Position to hold servo at
  
  bool done;


  //constructor
  AnamServo(String _servoName, int _id, int _relayPin, int _dataPin, int _servoSpeed, int _startPos, int _firstPos, int _secondPos,int _thirdPos) {
    servoName = _servoName;
    id = _id;
    relayPin = _relayPin;
    dataPin = _dataPin;
    servoSpeed = _servoSpeed;
    startPos = _startPos;
    firstPos = _firstPos;
    secondPos = _secondPos;
    thirdPos = _thirdPos;
    done = false;
    int holdPos;  // Position to hold servo at
  }

  // Sets the servo to hold at a given position
  void setHoldPosition(int pos) {
    holdPos = pos;
    gotoPos = pos;  // Initially set gotoPos to holdPos
  }

  void setup() {
    pinMode(relayPin, OUTPUT);
    digitalWrite(relayPin, LOW);
    pinMode(dataPin, OUTPUT);
  }

  void openStart() {
    Serial.println(servoName + " open start");
    currentPos = startPos;  // set current position to the startint position
    gotoPos = firstPos;     // set goto position to the first position
    lastPulse = 0;
    digitalWrite(relayPin, HIGH);
    done = false;
  }

  void secondStart() {
    Serial.println(servoName + " second start");
    currentPos = firstPos;  // set current position to the startint position
    gotoPos = secondPos;    // set goto position to the first position
    lastPulse = 0;
  
    done = false;
  }

  void thirdStart() {
    Serial.println(servoName + " third start");
    currentPos = secondPos;  // set current position to the startint position
    gotoPos = thirdPos;    // set goto position to the first position
    lastPulse = 0;
  
    done = false;
  }

  bool update() {
    // we check if we need to increment the current position
    if (currentPos < gotoPos) {
      // check if it would overshoot
      if (currentPos + servoSpeed > gotoPos) {
        currentPos = gotoPos;
      } else {
        currentPos += servoSpeed;
      }
    }

    // we check if we need to deveremtn the current position
    else if (currentPos > gotoPos) {
      // check if it would overshoot
      if (currentPos - servoSpeed < gotoPos) {
        currentPos = gotoPos;
      } else {
        currentPos -= servoSpeed;
      }
    }

    // make sure the current position is not more or less than min or max
    if (currentPos > servoMax) {
      currentPos = servoMax;
    } else if (currentPos < servoMin) {
      currentPos = servoMin;
    }

    // now we have to actually set the goto position on the servo
    digitalWrite(dataPin, HIGH);    // Turn the motor on
    delayMicroseconds(currentPos);  // Length of the pulse sets the motor position
    digitalWrite(dataPin, LOW);     // Turn the motor off
  }

  /**
       Call this every frame in the main script, it will call the update function every 20 ms.
    */
  // Sets the servo to hold at a given position
  
 bool holdUpdate() 
 {
   
      // now we have to actually set the goto position on the servo
      digitalWrite(dataPin, HIGH);    // Turn the motor on
      delayMicroseconds(currentPos);  // Length of the pulse sets the motor position
      digitalWrite(dataPin, LOW); 
      //Serial.println("currentPos");   
      //Serial.println(currentPos);  
 }

  bool callUpdate() {

    // EVER 20 MS SEND A PULSE OF EACH SERVO
    long t = millis();
    if (t - lastPulse >= 20) 
    {
      // update last pulse
      lastPulse = t;

      // do the servo update
      if (done == false) {
        update();
      }

              Serial.print("Pos: ");
              Serial.println(currentPos);
          //    Serial.print(" - GotoPos ");
          //    Serial.println(gotoPos);
          //    if (id == 1) {
          //      Serial.print(" --- ");
          //    } else {
          //      Serial.println("");
          //    }
      // Additional logic to hold the servo at a certain position
      /*if (done && holdPos != -1) {
        if (currentPos != holdPos) {
          gotoPos = holdPos; // Reset gotoPos to the holding position
          done = false; // Trigger the servo to move to the holding position
        }
      }*/
      // Check if the servo reached its target position
      if (currentPos == gotoPos) {
        if (gotoPos == thirdPos) {
          digitalWrite(relayPin, LOW);
        }
        done = true;
        return done;
      }
      
    }
    return done;
  }
};
