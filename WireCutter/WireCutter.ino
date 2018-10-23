/* @file WireCutter.ino
 * @version 1.0
 * This is the application file of LA26 auto wire cutter
 * 
 * Created by Aki Zhou on 2018/10/23.
 * Copyright © 2018 Aki Zhou. All rights reserved.
 */
 
#include "WireCutter.h"

WireCutter protoType; //object declaration

void setup() {
  protoType.assignPins();
}

void loop() {
  int numCut = 0;
  bool ready2Feed = false;
  bool ready2Cut = false;

  protoType.prompt();
  
  while(numCut != protoType.getNumWire()){
    ready2Feed = protoType.releaseWire();
    
    ready2Cut = protoType.feedWire(ready2Feed);
    
    protoType.cutWire(ready2Cut);

    numCut++;
  }
  
}
