/*
   LIPOMETER v1.0
   http://johnlenfr.1s.fr
   v1.0
 
 Arduino pro Mini 5v/16mHz w/ Atmega 328
 
 */
#include "Arduino.h"

class Sensor{
public:
//  void begin(); 
  float Lipo_Volt ();
  float Lipo_Current ();
  float Lipo_Batt_Cap ();

  
  private:
  uint16_t mesure(uint8_t pin);
};
