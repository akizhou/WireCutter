/* @file WireCutter.cpp
 * @version 1.0
 * This is the implementation of class WireCutter 
 * 
 * Created by Aki Zhou on 2018/10/23.
 * Copyright © 2018 Aki Zhou. All rights reserved.
 */
 
#include "Arduino.h"
#include "WireCutter.h"

using namespace std;

void WireCutter::assignPins(){
  pinMode(11, OUTPUT);
}

WireCutter::WireCutter(){
  lengthWire = 0.0;
  numWire = 0;
  colourWire = 0;
}

int WireCutter::getNumWire(){
  return numWire;
}

void WireCutter::prompt(){
  
  return;
}

bool WireCutter::releaseWire(){
  bool wireReleased = false;
  
  return wireReleased;
}

bool WireCutter::feedWire(bool& released){
  bool wireFed = false;
  
  return wireFed;
}

void WireCutter::cutWire(bool& fed){
  return;
}
