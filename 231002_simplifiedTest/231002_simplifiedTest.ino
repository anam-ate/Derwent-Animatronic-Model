
#include "buttonTrigger.h"
#include "button2Trigger.h"
#include "AnamServo.h"


long lastPulse2;  // the time in milliseconds of the last pulse
bool servosActive = false;  
bool servoTopMoving = false;
bool servoBottomMoving = false;
//=====================SEQUENCER=============================================================
int x = 0;                      // for iterating through the array of sequences
int checkState = 0;             // check where you are in the sequence
int internalSequenceState = 0;  // checking internal timers run out to begin main sequences (states)

int MainRoutine[3] = { 25, 26, 27 };

int sequencePicked = 0;

int lenRoutine1 = 3;  // length of the routine - 4 sequences long so 4 state changes
int choiceState = 0;

//=======================================================================


// AnamServo(String _servoName, int _relayPin, int _dataPin, int _servoSpeed, int _startPos, int _firstPos, int _secondPos)
AnamServo bottomServo("bottomServo", 1, 10, 9, 1, 2500, 1650, 1650,2550);       //10,9 (actual servo) //12,42
AnamServo topServo("topServo", 2, 5, 6, 1, 2500, 1650, 800,2550);  //7/8(actual servo) //5,6


//===== Settings variables===========================================================================
int state = -1;   // keeps track of what part of the sequence is currently running
long timer = 0;  // use this to store times for timers
int y = 0;

int waitTime = 10;                // time in seconds to wait in between opening and closing sequence for test
unsigned long currentMillis = 0;  // stores the value of millis() in each iteration of looDOp()
bool button1Pushed = false;
bool button2Pushed = false;
bool button2State = false;
int lockButton2 = false;       //variable to check if you are already in the button2 sequence states
bool checkStateMover = false;  //checks if you are in the first button or second button
//===============================================================================================
void stateMover(String msg, long t, int s, int cS) {
  Serial.println(msg);
  timer = t;
  state = s;
  checkState = cS;
}




//TEMPORARY REMOVE CODE LATER:==============================================================


void setup() {
  

  Serial.begin(19200);
  Serial3.begin(9600); 
  button1TriggerSetup();
  button2TriggerSetup();

  bottomServo.setup();
  topServo.setup();

  Serial.println("Ready...");
  Serial.println("Ready to Reset");
  delay(5000);

  
}

void loop() 
{
  // EVER 20 MS SEND A PULSE OF EACH SERVO
  long t = millis();
  char command = Serial.read();  // Read the incoming byte as char

  // if callupdate gets called stop calling holdUpdate
  
  //if the top one is moving only hold the bottomServo position
  if(servosActive == true && servoTopMoving == true && servoBottomMoving == false)
  {
    if (t - lastPulse2 >= 20) 
    {
      
      bottomServo.holdUpdate();
      lastPulse2 = t;
    }
  }
  //if neither servo is moving  and the servos are active still in the sequence hold both
  if(servosActive == true && servoBottomMoving == false && servoTopMoving == false)
  {
    if (t - lastPulse2 >= 20) 
    {
      topServo.holdUpdate();
      bottomServo.holdUpdate();
      lastPulse2 = t;
    }
  }
  if(state == -1 && choiceState == 0 && x == 0)
  {

    Serial3.write('7'); // turn lights on for whole model
    
    
    Serial.println("model is on default STATE -1 ");
    state = 0;
  }
  // put your main code here, to run repeatedly:
  if (state == 0 && choiceState == 0 && x == 0) {
    
    //CHECK IF MAIN SEQ BUTTON IS TRIGGERED
    if (button1TriggerSensor() == true && lockButton2 == false) {
      button1Pushed = true;
      Serial.println("button 1 pushed");
    }
    if (button1Pushed == true) {
      Serial3.write('8'); //turn off all lights start sequence
      Serial.println("model is off default");
      button1Pushed = false;
      Serial.println("choiceState = 1");
      choiceState = 1;
      sequencePicked = 1;
    }
    //CHECK IF INDIVIDUAL STATE BUTTON IS TRIGGERED
    bool button2Pressed = button2TriggerSensor();
    if (button2Pressed) {
      buttonPressCount = getButton2PressCount();
      button2Pushed = true;
      lockButton2 = true;
    }

    //check if button 2 has been pressed first
    if (button2Pushed == true && lockButton2 == true) {
      
      button2Pushed = false;
      checkStateMover = false;
      choiceState = 2;
      button2State = true;
      Serial.println("choiceState = 2");
    }
  }
  //======================MAIN SEQUENCE TRIGGER=========================================
  if (choiceState == 1) {
    //end sequence with button press 1
    if (x >= lenRoutine1 && checkState == 0)  // if the last seq number is complete then it is the end of seq reset everything
    {
      Serial.println("repick a button");
      choiceState = 0;
      state = 0;
      x = 0;  // reset x
      Serial.println("resetting");
      Serial3.write('7'); //turn on all lights start sequence // end of sequence
      Serial.println("model is on default");
    }
    if (sequencePicked == 1 && choiceState == 1)  // TRIGGER ROUTINE 1
    {
      //move to the next state in the MainRoutine
      if (x <= lenRoutine1 && checkState == 0) {
        Serial.println("Trigger button 1 routine");
        stateMover("move to state " + String(MainRoutine[x]) + "!", millis(), MainRoutine[x], 1);
        checkStateMover = true;
      }
    }
  }
  //INDIVIDUAL BUTTON TRIGGERS
  if (choiceState == 2) {
    if (buttonPressCount == 1 && button2State == true) {
      Serial3.write('8');// turn off all lights start sequence
      Serial.println("model is off default");
      Serial.println("button was pushed once trigger state 28");
      state = 28;
      button2State = false;
    }
    if (buttonPressCount == 2 && button2State == true) {
      Serial.println("button was pushed twice trigger state 29");
      state = 29;
      button2State = false;
    }
    if (buttonPressCount == 3 && button2State == true) {
      Serial.println("button was pushed thrice trigger state 30");
      buttonPressCount = 0;  // Reset the press count
      button2State = false;
      state = 30;
    }
  }
  // ==========================ground floor lights fade start moving into 1st position=============
  //====================================================================================================
  if (state == 25) {

    if (internalSequenceState == 0)  //start
    {
      
      Serial.println("state = 25");
      Serial3.write('1'); // 1 can be a predefined command for playing sound
      timer = millis();
      internalSequenceState = 1;
    }
    
    if(internalSequenceState == 1 && millis() >= timer + 5 * 1000)
    {

      timer = millis();
      internalSequenceState = 2;
      Serial3.write('3'); // turn lights on ground floor on
    }
    
    if (internalSequenceState == 2)  // gradual fade up to lights give it a few seconds before it parts and lifts
    {
      
      servosActive = true; // to trigger the hold servo position from now
      servoBottomMoving = true;
      servoTopMoving = true;
      bottomServo.openStart();
      topServo.openStart();
      
      internalSequenceState = 3;
      timer = millis();
      
    }
    if (internalSequenceState == 3) 
    {
      
      bool bottomServoDone = bottomServo.callUpdate();
      bool topServoDone = topServo.callUpdate();
      

      if (bottomServoDone == true && topServoDone == true) {
        servoBottomMoving = false;
        servoTopMoving = false;
        internalSequenceState = 4;
        timer = millis();
      }
    }
   

    //short pause state before second part opens
    if (internalSequenceState == 4 && millis() >= timer + 12 * 1000) {

      if (checkStateMover == true) {
        x++;  // get the next index value for the array
        state = 0;
        checkState = 0;
        internalSequenceState = 0;
        timer = millis();
        Serial.println(" continue to next state");
      } else {
        x = 0;
        state = 0;
        checkState = 0;
        internalSequenceState = 0;
        timer = millis();
        Serial.println("wait for next state trigger");
        choiceState = 0;
      }
    }
  }

  //====================================================================================================
  //====================================================================================================
  //====================================================================================================
  // ==========================move into second position turn 1st floor lights on and top floor and pause=============
  //====================================================================================================
  else if (state == 26) 
  {
    if (internalSequenceState == 0)  //start
    {
      Serial3.write('4');//fade up 1st floor
      Serial.println("state = 26");
      timer = millis();
      internalSequenceState = 1;
    }

    if (internalSequenceState == 1 )  // gradual fade up to lights give it a few seconds before it parts and lifts
    {
      
      servoTopMoving = true;
      topServo.secondStart();  //this is actually the 3rd position of the top servo
      internalSequenceState = 2;
      timer = millis();
      
    }
    if (internalSequenceState == 2) {
      bool topServoDone = topServo.callUpdate();
      if ( topServoDone == true ) {
        internalSequenceState = 3;
        timer = millis();
        servoTopMoving = false;
        Serial3.write('5');//fade top floor
      }
    }
    
    //pause for 12 seconds
    if (internalSequenceState == 3 && millis() >= timer + 16 * 1000) 
    {
      if (checkStateMover == true) {
        x++;  // get the next index value for the array
        state = 0;
        checkState = 0;
        internalSequenceState = 0;
        timer = millis();
        Serial.println(" continue to next state");
      } else 
      {
        x = 0;
        state = 0;
        checkState = 0;
        internalSequenceState = 0;
        timer = millis();
        Serial.println("wait for next state trigger");
        choiceState = 0;
      }
    }

  }

  else if (state == 27) {

    if (internalSequenceState == 0)  //start
    {
      
      Serial.println("state = 27");
      timer = millis();
      internalSequenceState = 1;
    }

    if (internalSequenceState == 1)  // gradual fade up to lights give it a few seconds before it parts and lifts
    {
      
      
      servoBottomMoving = true;
      servoTopMoving = true;

      bottomServo.thirdStart();
      topServo.thirdStart();  //this is actually the 3rd position of the top servo

     
        internalSequenceState = 2;
        timer = millis();
      
    }
    if (internalSequenceState == 2) {
     
      bool topServoDone = topServo.callUpdate();
      bool bottomServoDone = bottomServo.callUpdate();

      if (topServoDone == true && bottomServoDone == true) {
        
        internalSequenceState = 3;
        Serial3.write('6');//fade down all lightss
        timer = millis();
      }
    }
    if (internalSequenceState == 3 && millis() >= timer + 20 * 1000) 
    {
      internalSequenceState = 4;
      timer = millis();

    }
    if (internalSequenceState == 4 ) 
    {
      if (checkStateMover == true) {
      
        x++;  // get the next index value for the array
        Serial3.write('2'); // 1 can be a predefined command for stopping sound
        state = -1;
        checkState = 0;
        internalSequenceState = 0;
        timer = millis();
        Serial.println(" end re pick a button to push");
        lockButton2 = false;  // unlock the ability to push the main sequence button1 after the third button press has be activated and completed put this statement after the last sequence state has completed
        servosActive = false;
        servoBottomMoving = false;
        servoTopMoving = false;
        
      } else {
        
        x = 0;
        Serial3.write('2'); // 1 can be a predefined command for stopping sound
        state = -1;
        checkState = 0;
        internalSequenceState = 0;
        timer = millis();
        Serial.println("end re pick a button to push");
        choiceState = 0;
        lockButton2 = false;  // unlock the ability to push the main sequence button1 after the third button press has be activated and completed put this statement after the last sequence state has completed
        servosActive = false;
        servoBottomMoving = false;
        servoTopMoving = false;
        
      }
    }
  }

  else if(state == 28)
  {
    if (internalSequenceState == 0)  //start
    {
      Serial.println("state = 28");
      timer = millis();
      internalSequenceState = 1;
    }
    if (internalSequenceState == 1)  // gradual fade up to lights give it a few seconds before it parts and lifts
    {
      Serial3.write('3');//fade down top floor
      servosActive = true; // to trigger the hold servo position from now
      servoBottomMoving = true;
      servoTopMoving = true;
      bottomServo.openStart();
      topServo.openStart();
      internalSequenceState = 2;
      timer = millis();
    
    }
    if (internalSequenceState == 2) 
    {
      bool bottomServoDone = bottomServo.callUpdate();
      bool topServoDone = topServo.callUpdate();
  
      if (bottomServoDone == true && topServoDone == true) {
        servoBottomMoving = false; //hold positions for both motors
        servoTopMoving = false;//hold positions for both motors
        internalSequenceState = 3;
        timer = millis();
        Serial3.write('4');//fade down top floor
      }
    }
    //short pause state before second part opens
    if (internalSequenceState == 3 ) {

      x = 0;
      state = 0;
      checkState = 0;
      internalSequenceState = 0;
      timer = millis();
      Serial.println("wait for next state trigger");
      choiceState = 0;
      
    }
  }

  else if(state == 29)
  {
    if (internalSequenceState == 0)  //start
    {
      Serial.println("state = 29");
      timer = millis();
      internalSequenceState = 1;
    }

    if (internalSequenceState == 1 )  // gradual fade up to lights give it a few seconds before it parts and lifts
    {
      Serial3.write('5');//fade down top floor
      servoTopMoving = true;
      topServo.secondStart();  //this is actually the 3rd position of the top servo
      internalSequenceState = 2;
      timer = millis();
      
    }
    if (internalSequenceState == 2) {
      bool topServoDone = topServo.callUpdate();
      if (topServoDone == true ) {
        servoTopMoving = false;
        internalSequenceState = 3;
        timer = millis();
      }
    }
    
    if (internalSequenceState == 3 ) 
    {
      x = 0;
      state = 0;
      checkState = 0;
      internalSequenceState = 0;
      timer = millis();
      Serial.println("wait for next state trigger");
      choiceState = 0;
      
    }
  }

  else if(state == 30)
  {
    if (internalSequenceState == 0)  //start
    {
      Serial.println("state = 30");
      timer = millis();
      internalSequenceState = 1;
    }

    if (internalSequenceState == 1)  // gradual fade up to lights give it a few seconds before it parts and lifts
    {
      
      Serial.println("fade all lights");
      servoBottomMoving = true;
      servoTopMoving = true;
      bottomServo.thirdStart();
      topServo.thirdStart();  //this is actually the 3rd position of the top servo
      internalSequenceState = 2;
      timer = millis();
    
    }
    if (internalSequenceState == 2) 
    {
  
      bool topServoDone = topServo.callUpdate();
      bool bottomServoDone = bottomServo.callUpdate();

      if (topServoDone == true && bottomServoDone == true) {
        Serial3.write('6');//fade down top floor
        internalSequenceState = 3;
        timer = millis();
      }
    }
    if (internalSequenceState == 3 && millis() >= timer + 20 * 1000) 
    {
        
        x = 0;
        state = -1;
        checkState = 0;
        internalSequenceState = 0;
        timer = millis();
        Serial.println("end re pick a button to push");
        choiceState = 0;
        lockButton2 = false;  // unlock the ability to push the main sequence button1 after the third button press has be activated and completed put this statement after the last sequence state has completed
        servosActive = false;
        servoBottomMoving = false;
        servoTopMoving = false;
        

    }
  }
}
