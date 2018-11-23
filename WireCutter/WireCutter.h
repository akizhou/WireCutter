/* 
 * @file WireCutter.h
 * @version 1.0
 * This is the interface of class WireCutter 
 * 
 * Created by Aki Zhou on 2018/10/23.
 * Copyright © 2018 Aki Zhou. All rights reserved.
 */
 
#ifndef WIRECUTTER_H
#define WIRECUTTER_H

#include <Vector.h> 
//#include "Wire.h"

/*
 * This section is for declaring the states of WireCutter FSM
 */
namespace node{
  enum SuperStates{IDLE, PROMPT, PROCESS};
  enum SubStates1{WIRE_SELECTION, DEFINE_LENGTH, DEFINE_NUMBER, CONFIRM_TASKS};
  enum SubStates2{DEFINE_TASK, RELEASE, FEED, CUT};
}

const bool modeON = true;
const bool modeOFF = false;

/*
 * This section is the implementation of class WireCutter
 */
class WireCutter{
  public:
    void setup();
    WireCutter();
    void runFSM();
    
  private:
    // states
    node::SuperStates currentSuperState;
    node::SubStates1 currentSubState1;
    node::SubStates2 currentSubState2;

    // variables
    //Vector<Wire> wires;
    int slot;
    int lengthWire;
    int numWire;
    int numCut;
    char key;

    // functions
    void idle();
    void prompt();
    void process();

    void userInput(int nMax, int lowLim, int upLim, bool timer, bool back, node::SubStates1 subState);
    
    void defineTasks();
    void releaseWire();
    void feedWire();
    void cutWire();
};

char timer(int lengthTime);

#endif // WIRECUTTER_H
