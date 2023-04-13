 /*****************************************************************
File:         readAmbientAndProximity.ino
Description:  repeatedly obtain the Ambient and Proximity value 
              through IIC and display the value in the serial port.
******************************************************************/
#include "BMS33M332.h"

BMS33M332  Alsps(8);   //Select Pin8 as INTPIN
uint16_t alsValue;
uint16_t psValue;

void setup() 
{
   Serial.begin(9600);               
   Alsps.begin();

}

void loop()
{
   psValue = Alsps.readRawProximity();
   Serial.print("Data_PS : "); 
   Serial.print(psValue);
   alsValue = Alsps.readRawAmbient();
   Serial.print("   Data_ALS : ");
   Serial.print(alsValue); 
   Serial.println();  
   delay(1000);
}
