/*
 
 Arduino pro Mini 5v/16mHz w/ Atmega 328
 
 */
 
#include "Arduino.h"

/*  
//AMP Sensor
#define COEF_Volt 0.05104656              
#define COEF_Current 0.091551155        
#define OffsetVolt 0
#define OffsetCurrent 0                 
*/
//ACS758 LCB 100U Default for Eprom
#define COEF_Volt 181               
#define COEF_Current 1262   
#define OffsetVolt 0
#define OffsetCurrent 129 

class Sensor{
public:
  void ReadSensor(); 
  
  float getVolt();    //in V
  int   getVoltDigi();//in Digi
  float getCurrent(); //in mA
  int   getCurrentDigi(); //in Digi
  float getBattCap(); //im mA
  
  float getVCC();
  float getTemp();
  
  private:
  uint16_t mesure(uint8_t pin);
  float ReadTemp();
  float ReadVCC();
};
