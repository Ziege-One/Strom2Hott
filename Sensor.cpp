/*
   Strom2HoTT
   Ziege-One
   v1.0
 
 Arduino pro Mini 5v/16mHz w/ Atmega 328
 
 */
 
#include "Sensor.h"
#include "Message.h"

// Einfügen Message.cpp Funktionen
GMessage message2;

//Time values for Used Capacity calculation 
unsigned long current_time = 0;
unsigned long last_time = 0;
float time_elapsed = 0.0f;

//Calculate this scalse factor here once to save cycles
float secound_scale = 1000.0f / 3600.0f;

//Internal Values
float Volt = 0.0f;
float Volt_min = 50.0f;
  int VoltDigi = 0; 
float Current = 0.0f;
float Current_max = 0.0f;
  int CurrentDigi = 0;
float Capacity_Used = 0.0f;

float VCC = 0.0f;
float Temp = 0.0f;


float Sensor::getVolt() { return Volt; }
float Sensor::getVolt_min() { return Volt_min; }
int   Sensor::getVoltDigi() {return VoltDigi; }
float Sensor::getCurrent() { return Current; }
float Sensor::getCurrent_max() { return Current_max; }
int   Sensor::getCurrentDigi() {return CurrentDigi; }
float Sensor::getBattCap() { return Capacity_Used; }

float Sensor::getVCC() { return VCC; }
float Sensor::getTemp() { return Temp; }

void Sensor::ReadSensor(){
  // Spannung
  static float MV1_Volt = 0.0;
  static float MV2_Volt = 0.0;
  static float MV3_Volt = 0.0;
  static float MV4_Volt = 0.0;
  static float MV5_Volt = 0.0;
  static float MV6_Volt = 0.0;
  static float MV7_Volt = 0.0;
  static float MV8_Volt = 0.0;
  static float MV9_Volt = 0.0;
  float MV10_Volt;

  VoltDigi = analogRead(A0);
  MV10_Volt =  (VoltDigi - (message2.getVoltOffset()))* ((message2.getVoltCOEF()) * 0.0001);;
  Volt = (MV1_Volt + MV2_Volt + MV3_Volt + MV4_Volt + MV5_Volt + MV6_Volt + MV7_Volt + MV8_Volt + MV9_Volt + MV10_Volt) / 10; // filter (average of 10 samples)
  MV1_Volt = MV2_Volt; // shift
  MV2_Volt = MV3_Volt;
  MV3_Volt = MV4_Volt;
  MV4_Volt = MV5_Volt;
  MV5_Volt = MV6_Volt;
  MV6_Volt = MV7_Volt;
  MV7_Volt = MV8_Volt;
  MV8_Volt = MV9_Volt;
  MV9_Volt = MV10_Volt;
  
  // Strom
  static float MV1_Current = 0.0;
  static float MV2_Current = 0.0;
  static float MV3_Current = 0.0;
  static float MV4_Current = 0.0;
  static float MV5_Current = 0.0;
  static float MV6_Current = 0.0;
  static float MV7_Current = 0.0;
  static float MV8_Current = 0.0;
  static float MV9_Current = 0.0;
  float MV10_Current; 

  CurrentDigi = analogRead(A1);
  MV10_Current =  (CurrentDigi - (message2.getCurrentOffset()))* ((message2.getCurrentCOEF()) * 0.0001);
  Current = (MV1_Current + MV2_Current + MV3_Current + MV4_Current + MV5_Current + MV6_Current + MV7_Current + MV8_Current + MV9_Current + MV10_Current) / 10; // filter (average of 10 samples)
  MV1_Current = MV2_Current; // shift
  MV2_Current = MV3_Current;
  MV3_Current = MV4_Current;
  MV4_Current = MV5_Current;
  MV5_Current = MV6_Current;
  MV6_Current = MV7_Current;
  MV7_Current = MV8_Current;
  MV8_Current = MV9_Current;
  MV9_Current = MV10_Current;

  // Kapazität
  current_time = millis();
  //Calculate the used Capacity in [mAh]
  //Calculate elapsed Time in sec first
  time_elapsed = (float)(current_time - last_time)/1000.0f;
  //Now the used capacity
  Capacity_Used += Current * secound_scale * time_elapsed;
  last_time = current_time;
  
  // Min/Max
  if (current_time > 5000)
  {
    if (Volt < Volt_min) Volt_min = Volt;
    if (Current > Current_max) Current_max = Current;
  }
  
  VCC = ReadVCC();
  Temp = ReadTemp(); 
}  



float Sensor::ReadVCC() {  //Die Spannung wird in Volt zurückgegeben.
  long result;
  float result2;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1126400L / result; // Back-calculate AVcc in mV
  result2 = result;
  return result2/1000;
}

float Sensor::ReadTemp() { // Temperatur wird in ° C zurückgegeben.  
  unsigned int wADC;
  double t;
  // Set the internal reference and mux.
  ADMUX = (_BV(REFS1) | _BV(REFS0) | _BV(MUX3));
  ADCSRA |= _BV(ADEN);  // enable the ADC
  delay(20);            // wait for voltages to become stable.
  ADCSRA |= _BV(ADSC);  // Start the ADC
  // Detect end-of-conversion
  while (bit_is_set(ADCSRA,ADSC));
  // Reading register "ADCW" takes care of how to read ADCL and ADCH.
  wADC = ADCW;
  // The offset of 324.31 could be wrong. It is just an indication.
  t = (wADC - 324.31 ) / 1.22;
  // The returned temperature is in degrees Celcius.
  return (t);
}



