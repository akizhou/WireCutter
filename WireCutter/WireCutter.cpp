/* 
 * @file WireCutter.cpp
 * @version 1.0
 * This is the implementation of class WireCutter 
 * 
 * Created by Aki Zhou on 2018/10/23.
 * Copyright © 2018 Aki Zhou. All rights reserved.
 */

#include "WireCutter.h"
#include <Keypad.h>
#include <LiquidCrystal.h>
//#include <Stepper.h>
#include <CheapStepper.h>

/*
 * This section is for parts set ups and pin assignments
 */
// Bluetooth module 
// reserve D0 and D1 pin for serial communication
  
// Keypad
const int rows = 4; //four rows
const int cols = 3; //three columns
char keys[rows][cols] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[rows] = {A7, A6, A5, A4}; //connect to the row pinouts of the keypad
byte colPins[cols] = {A3, A2, A1}; //connect to the column pinouts of the keypad
Keypad keypad(makeKeymap(keys), rowPins, colPins, rows, cols);
  
// LCD
const byte rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
const byte lcdSwitch = 6;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
  
// Solenoids
byte solenoid1 = 43;
byte solenoid2 = 47;
byte solenoid3 = 51;

// Stepper motors
/*
// spr = steps per revolution
const int sprWheelStepper = 4096; // 28BYJ-48
const int sprCutterStepper = 4096; // 17HS13-0404S
int speedWheelStepper = 1;
int speedCutterStepper = 1;
Stepper wheelStepper(sprWheelStepper, 2, 3, 4, 5);
Stepper cutterStepper(sprCutterStepper, 18, 19, 20, 21);
*/
const bool CLOCKWISE = true;
const bool COUNTERCLOCKWISE = false;

CheapStepper wheelStepper(2, 3, 4, 5);
CheapStepper cutterStepper(23, 25, 27, 29);

void WireCutter::setup(){
  Serial.begin(9600);
  
  keypad.setHoldTime(1000);
  
  lcd.begin(16,2);
  pinMode(lcdSwitch, OUTPUT);
  
  pinMode(solenoid1, OUTPUT);
  pinMode(solenoid2, OUTPUT);
  pinMode(solenoid3, OUTPUT);

  //wheelStepper.setSpeed(speedWheelStepper);
  //cutterStepper.setSpeed(speedCutterStepper);
  wheelStepper.setRpm(5);
  cutterStepper.setRpm(10);
}

/*
 * This section is the class definitions of class WireCutter
 */
// Constructor
WireCutter::WireCutter(){
  currentSuperState = node::IDLE;
  currentSubState1 = node::WIRE_SELECTION;
  currentSubState2 = node::DEFINE_TASK;
  
  numTask = 0;
  slot = 0;
  lengthWire = 0;
  numWire = 0;
  
  numCut = 0;
  key = NO_KEY;
}

void WireCutter::runFSM(){
  switch(currentSuperState){
    case node::IDLE:
      Serial.println("Idle");
      idle();
      break;
    
    case node::PROMPT:
      Serial.println("Prompt");
      prompt();
      break;
      
    case node::PROCESS:
      Serial.println("Process");
      process();
      break;
  }
  return;
}

// machine process functions
void WireCutter::idle(){
  key = NO_KEY;
  while(key != '#'){
    key = keypad.getKey();
    lcd.noDisplay();
    digitalWrite(lcdSwitch, HIGH);
  }

  lcd.display();
  digitalWrite(lcdSwitch, LOW); // pnp transistor active low
  currentSuperState = node::PROMPT;
  return;
}

void WireCutter::prompt(){
  switch(currentSubState1){
    case node::WIRE_SELECTION:
      lcd.clear();
      Serial.println("wireSelection");
      lcd.print("Select wire");
      userInput(1, 1, 3, modeON, modeOFF, node::WIRE_SELECTION);
      return;
      
    case node::DEFINE_LENGTH:
      lcd.clear();
      Serial.println("Define length");
      lcd.print("Input length");
      userInput(3, 10, 250, modeOFF, modeON, node::DEFINE_LENGTH);
      return;
      
    case node::DEFINE_NUMBER:
      lcd.clear();
      Serial.println("Define number");
      lcd.print("Input number");
      userInput(2, 1, 99, modeOFF, modeON, node::DEFINE_NUMBER);
      return;
   
    case node::CONFIRM_TASKS:
      lcd.clear();
      Serial.println("Confirm task");
      lcd.print("Confirm task = #");
      lcd.setCursor(0,2);
      lcd.print("Back = *");  
      
      key = NO_KEY; // reset the key
      while(key == NO_KEY){
        key = keypad.getKey();
      }
      
      if(key == '*'){
        currentSubState1 = node::DEFINE_NUMBER;
      }
      else if(key == '#'){
        
        //Wire newWire(slot, lengthWire, numWire);
        //tasks.push_back(newWire);
        tasks[numTask][0] = slot;
        tasks[numTask][1] = lengthWire;
        tasks[numTask][2] = numWire;
        numTask++;

        currentSubState1 = node::WIRE_SELECTION;
        currentSuperState = node::PROCESS;
      }
      else{
        
        //Wire newWire(slot, lengthWire, numWire);
        //tasks.push_back(newWire);
        tasks[numTask][0] = slot;
        tasks[numTask][1] = lengthWire;
        tasks[numTask][2] = numWire;
        numTask++;

        if(numTask == 2){
          lcd.clear();
          lcd.print("MaxTasks reached");
          lcd.setCursor(0,2);
          lcd.print("Start processing");
          delay(1000);
          currentSubState1 = node::WIRE_SELECTION;
          currentSuperState = node::PROCESS;
          return;
        }
        
        currentSubState1 = node::WIRE_SELECTION;
        currentSuperState = node::PROMPT;
      }
      return;
  }
}

void WireCutter::userInput(int nMax, int lowLim, int upLim, bool offTimer, bool back, node::SubStates1 subState){
  char keyArray[nMax] = {'0'};
  int numFilled = 0;
  int value = 0;
  int numTry = 0;
  key = NO_KEY;
  KeyState state = IDLE;
  
  while(value < lowLim || value > upLim){
    if(numTry != 0){
      lcd.clear();
      lcd.print("Invalid input");
      delay(500);
      lcd.clear();
      lcd.print("Try again");
      
      numFilled = 0;
      value = 0;
      key = NO_KEY;
      for(int i = 0; i < nMax; i++){
        keyArray[i] = '0';
      }
    }
    
    while(numFilled < nMax){
      if(offTimer){
        key = timer(10000); // 10s
      }
      else{
        key = keypad.getKey();
        state = keypad.getState();
      }
/*
      if(key == '*'){
        if(back && ){
           if it detects released then trigger
        }
        else if(state == HOLD){
          clear input
        }
      }else 
      */
      if(key == '#'){
        break;
      }
      else if(key == '*'){
        switch(subState){
          case node::WIRE_SELECTION:
            return;
          case node::DEFINE_LENGTH:
            currentSubState1 = node::WIRE_SELECTION;
            return;
          case node::DEFINE_NUMBER:
            currentSubState1 = node::DEFINE_LENGTH;
            return;
        }
      }
      else if(key == NO_KEY){
        if(offTimer){
          currentSubState1 = node::WIRE_SELECTION;
          currentSuperState = node::IDLE;
          key = NO_KEY;
          Serial.println("going idle");
          return;
        }
      }
      else{
        lcd.setCursor(numFilled, 2);
        lcd.print(key);
        keyArray[numFilled] = key;
        numFilled++;
      }
      
    }
    
    value = atoi(keyArray);
    Serial.println(value);
    numTry++;
  }

  // assign value to the corresponding variable
  switch(subState){
    case node::WIRE_SELECTION:
      slot = value;
      currentSubState1 = node::DEFINE_LENGTH;
      break;
    case node::DEFINE_LENGTH:
      lengthWire = value;
      currentSubState1 = node::DEFINE_NUMBER;
      break;
    case node::DEFINE_NUMBER:
      numWire = value;
      currentSubState1 = node::CONFIRM_TASKS;
      break;
  }
  
  return;
}

char timer(int lengthTime){
  char key = NO_KEY;
  KeyState state = IDLE;
  unsigned long initialTime = millis();
  unsigned long finalTime = 0;
  
  while(key == NO_KEY){
    key = keypad.getKey(); 
    finalTime = millis();
    
    if((finalTime - initialTime) > lengthTime){
      break;
    }
  }
  return key;
}

void WireCutter::process(){
  switch(currentSubState2){
    case node::DEFINE_TASK:
                                                                        Serial.print("numer of tasks stored: ");  Serial.println(numTask);
      if(numTaskCompleted < numTask){
        Serial.print("Processing task "); Serial.println(numTaskCompleted + 1);
        lcd.clear();
        lcd.print("Task "); lcd.print(numTaskCompleted + 1); lcd.print(" / "); lcd.print(numTask);
        
        //slot = tasks[numTaskCompleted].getSlot();
        //lengthWire = tasks[numTaskCompleted].getLength();
        //numWire = tasks[numTaskCompleted].getNumber();
        slot = tasks[numTaskCompleted][0];
        lengthWire = tasks[numTaskCompleted][1];
        numWire = tasks[numTaskCompleted][2];
                                                                          Serial.print("working on array element: "); Serial.println(numTaskCompleted);
                                                                          Serial.print("Slot: "); Serial.println(slot);
                                                                    Serial.print("Length: "); Serial.println(lengthWire);
                                                                      Serial.print("Number: "); Serial.println(numWire);

        currentSubState2 = node::RELEASE;
        return;
      }
      
      numTask = 0;
      numTaskCompleted = 0;                                   
      currentSuperState = node::PROMPT;
      
      return;

      
    case node::RELEASE:
      Serial.println("Release");
      releaseWire();
      return;
    case node::FEED:
      Serial.println("Feed");
      feedWire();
      return;
    case node::CUT:
      Serial.println("Cut");
      cutWire();
      return;
  }
}

void WireCutter::releaseWire(){
  lcd.setCursor(0,2);
  lcd.print(numCut); lcd.print(" / "); lcd.print(numWire); lcd.print(" cut");
    
  switch(slot){
    case 1:
      digitalWrite(solenoid1, HIGH);
      break;
    case 2:
      digitalWrite(solenoid2, HIGH);
      break;
    case 3:
      digitalWrite(solenoid3, HIGH);
      break;
  }
  // need to lock later

  currentSubState2 = node::FEED;
  return;
}

void WireCutter::feedWire(){
  lcd.setCursor(0,2);
  lcd.print(numCut); lcd.print(" / "); lcd.print(numWire); lcd.print(" cut");
    
  //wheelStepper.step(lengthWire*10);
  Serial.println(lengthWire);
  delay(50);
  wheelStepper.moveDegrees(CLOCKWISE, 45);
  
  // lock whichever wire is unlocked
  switch(slot){
    case 1:
      digitalWrite(solenoid1, LOW);
      break;
    case 2:
      digitalWrite(solenoid2, LOW);
      break;
    case 3:
      digitalWrite(solenoid3, LOW);
      break;
  }
  
  currentSubState2 = node::CUT;
  return;
}

void WireCutter::cutWire(){
  /*
  cutterStepper.step(100);
  delay(50);
  cutterStepper.step(-100);
  */
  cutterStepper.moveDegrees(CLOCKWISE, 90);
  delay(50);
  cutterStepper.moveDegrees(COUNTERCLOCKWISE, 90);
  numCut++;
  Serial.print(numCut); Serial.print(" / "); Serial.print(numWire); Serial.println(" cut");
  
  if(numCut < numWire){
    lcd.setCursor(0,2);
    lcd.print(numCut); lcd.print(" / "); lcd.print(numWire); lcd.print(" cut");
    
    currentSubState2 = node::FEED;
  }
  else{
    lcd.clear();
    Serial.println("Session ending");
    lcd.print("Task complete");
    delay(5000); // 5s

    numCut = 0;
    numTaskCompleted++;
    currentSubState2 = node::DEFINE_TASK;
    currentSuperState = node::PROCESS;
  }
  return;
}
