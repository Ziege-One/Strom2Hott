/*
 
 Arduino pro Mini 5v/16mHz w/ Atmega 328
 
 */
 
#include "Arduino.h"

#define COEF_Volt 0.05104656
#define COEF_Current 0.091551155

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
