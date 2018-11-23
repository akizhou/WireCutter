/* 
 * @file WireCutter.ino
 * @version 1.0
 * This is the application file of LA26 auto wire cutter
 * 
 * Created by Aki Zhou on 2018/10/23.
 * Copyright © 2018 Aki Zhou. All rights reserved.
 */
 
#include "WireCutter.h"

// object declaration
WireCutter bob; // yes our wire cutter is named Bob

void setup() {
  bob.setup();
}

void loop() {
  Serial.println("");
  Serial.println("Session starts");
  bob.runFSM();
  delay(1000);
}
