/*
   Strom2HoTT
   Ziege-One
   v1.0
 
 Arduino pro Mini 5v/16mHz w/ Atmega 328
 
 */
 
#include "Arduino.h"

/*  
//AMP Sensor
#define COEF_Volt 510     // /10mV pro Digi          
#define COEF_Current 915  // /10mA pro Digi      
#define OffsetVolt 0      // Digi Offset
#define OffsetCurrent 0   // Digi Offset              
*/
//ACS758 LCB 100U Default for Eprom
#define COEF_Volt 181     // /10mV pro Digi          
#define COEF_Current 1262 // /10mA pro Digi  
#define OffsetVolt 0      // Digi Offset
#define OffsetCurrent 129 // Digi Offset

class Sensor{
public:
  void ReadSensor(); 
  
  float getVolt();        //in V
  float getVolt_min();    //in V
  int   getVoltDigi();    //in Digi
  float getCurrent();     //in A
  float getCurrent_max(); //in A
  int   getCurrentDigi(); //in Digi
  float getBattCap();     //im mA
  
  float getVCC();         // Interne Temperatur
  float getTemp();        // Interne Spannung
  
  private:
  float ReadTemp();       // Interne Temperatur lesen
  float ReadVCC();        // Interne Spannung lesen
};
