/*
   Strom2HoTT
   Ziege-One
   v1.0
 
 Arduino pro Mini 5v/16mHz w/ Atmega 328
 
 */
 
#include "Arduino.h"

  
//AMP Sensor
#define SCALE_Volt 510     // /10mV pro V          
#define SCALE_Current 915  // /10mV pro A      
#define OffsetVolt 0      // mV Offset
#define OffsetCurrent 0   // mV Offset              
/*
//ACS758 LCB 100U Default for Eprom
#define SCALE_Volt 181     // /10mV pro V          
#define SCALE_Current 1262 // /10mV pro A  
#define OffsetVolt 0      // mV Offset
#define OffsetCurrent 129 // mV Offset
*/
class Sensor{
public:
  void ReadSensor(); 
  
  float getVolt();        //in V
  float getVolt_min();    //in V
  float getVoltDigi();    //in Digi
  float getCurrent();     //in A
  float getCurrent_max(); //in A
  float getCurrentDigi(); //in Digi
  float getBattCap();     //im mA
  
  float getVCC();         // Interne Temperatur
  float getTemp();        // Interne Spannung
  
  private:
  float ReadTemp();       // Interne Temperatur lesen
  float ReadVCC();        // Interne Spannung lesen
};
