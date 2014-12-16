/*
   LIPOMETER v1.0
   http://johnlenfr.1s.fr
   v1.0
 
 Arduino pro Mini 5v/16mHz w/ Atmega 328
 
 */

#include "Sensor.h"

//Time values for Used Capacity calculation 
unsigned long current_time = 0;
unsigned long last_time = 0;
float time_elapsed = 0.0f;

//Calculate this scalse factor here once to save cycles
float secound_scale = 1000.0f / 3600.0f;

//Internal Values
float Current = 0.0f;
float Capacity_Used = 0.0f;

float Sensor::Lipo_Volt (){
  float val;
  
  val = 12.5;
  
  return val;
}

float Sensor::Lipo_Current (){
  float val;
  Current = 10.0;
  val = 10.0;
  
  return val;
}


float Sensor::Lipo_Batt_Cap (){
  float val;
  //Get the Current time
  current_time = millis();
  //Calculate the used Capacity in [mAh]
  //Calculate elapsed Time in sec first
  time_elapsed = (float)(current_time - last_time)/1000.0f;
  //Now the used capacity
  Capacity_Used += Current * secound_scale * time_elapsed;
 
  
  val = Capacity_Used;
  last_time = current_time;
  return val;
}
