/* @file WireCutter.h
 * @version 1.0
 * This is the interface of class WireCutter 
 * 
 * Created by Aki Zhou on 2018/10/23.
 * Copyright © 2018 Aki Zhou. All rights reserved.
 */
 
#ifndef WIRECUTTER_H
#define WIRECUTTER_H

class WireCutter{
  public:
    void assignPins();
    WireCutter();
    int getNumWire();
    
    void prompt();
    bool releaseWire();
    bool feedWire(bool& released);
    void cutWire(bool& fed);

  private:
    double lengthWire;
    int numWire;
    int colourWire;
};

#endif //WIRECUTTER_H
