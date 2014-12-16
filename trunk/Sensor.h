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
//ACS758 LCB 100U 
#define COEF_Volt 0.05104656              
#define COEF_Current     0.126213592233   
#define OffsetVolt 0
#define OffsetCurrent 126 

class Sensor{
public:
  void ReadSensor(); 
  
  float getVolt();    //in V
  float getCurrent(); //in Am
  float getBattCap(); //im mA
  
  float getVCC();
  float getTemp();
  
  private:
  uint16_t mesure(uint8_t pin);
  float ReadTemp();
  float ReadVCC();
};
