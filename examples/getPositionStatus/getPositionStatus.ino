/*****************************************************************
File:         readAmbientAndProximity.ino
Description:  When PS_ ADC exceeds thdh, int_ Pin output Low level 
              and print "Find object!" .PS_ ADC lower than thdl, 
              int_ Pin output High level and print "No objects found ):"
******************************************************************/
#include "BMS33M332.h"
BMS33M332  Alsps(8);   //Select Pin8 as INTPIN

void setup() 
{
   Serial.begin(9600);             
   Alsps.begin();
   Alsps.setINT(400,200);//INT initial,thdh=400 ,thdl=200 
}

void loop()
{
   if(Alsps.getPositionStatus() == 1)
   {
      Serial.println("No objects found ):");
   }
   else Serial.println("Find object!");
   delay(1000);
}
