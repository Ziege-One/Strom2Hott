/*
   DJILEDS+LIPOMETER
   http://johnlenfr.1s.fr
   v6.0
 
 Arduino pro Mini 5v/16mHz w/ Atmega 328
 
V4 incorporating the latest Lipomètre of Thyzoon v0.531 (October 2013) with low voltage alarm management.
 
  Added detection GPS DJI Naza.
 
  Youtube link demonstration:
 
 http://
 
 !!WARNING!!
 When using Arduino Board that have USB on board such a connection with a common RX/TX line does not work.
 The RX/TX on the Arduino are internally connected to the USB.
 I now used a Mini Pro 328 that has no USB on board and it works like a charm!
 (So DON'T USE Arduino UNO or similar boards, it will not work)
 
 /////PIN MAPPING////
 D0: RX Serial
 D1: TX Serial
 D2: Remote Control (detecting interupt whith a button on the radio)
 D3: 
 D4: RX Softserial
 D5: TX Softserial
 D6: 
 D7: 
 D8: 
 D9: 
 D10: 
 D11: 
 D12: 
 D13: green led to visualize communication
 
 A0: Lipo 1S
 A1: Lipo 2S
 A2: Lipo 3S
 A3: Lipo 4S
 A4: SDA not used (Can be use for I2C communications with sensors or for Lipo 5S) Support I2C (TWI) communication using the Wire library.
 A5: SCL not used (Can be use for I2C communications with sensors or for Lipo 6S) Support I2C (TWI) communication using the Wire library.
 
 
 */
 
// ======== LIPOMETRE  =======================================

#include <EEPROM.h>
#include <SoftwareSerial.h>
#include "Message.h"
#include <inttypes.h>

// Green LED on pin 13
#define LEDPIN_PINMODE    pinMode (13, OUTPUT);
#define LEDPIN_SWITCH     PINB |= 1<<5;     //switch LEDPIN state (digital PIN 13)
#define LEDPIN_OFF        PORTB &= ~(1<<5);
#define LEDPIN_ON         PORTB |= (1<<5);

//#define Debug                             // on off debuging

// Time interval [ms] for display updates:
const unsigned long DISPLAY_INTERVAL = 5000;
static unsigned long lastTime=0;  // in ms
unsigned long timer=millis();      // in ms


// Include Message.cpp functions for Init and Main program of LIPOMETER and GPS
GMessage message;


// ======== SETUP & CHECK =======================================
void setup()
{
  
  Serial.begin (115200); // 115200 FOR NAZA DJI GPS RX
  
  LEDPIN_PINMODE
  LEDPIN_ON
  delay(200);
  LEDPIN_OFF
  delay(200);
  LEDPIN_ON
  delay(200);
  LEDPIN_OFF
  delay(200);
  LEDPIN_ON
  delay(200);
  LEDPIN_OFF
  
  // Init GRAUPNER HOTT PROTOCOL
  message.init();
  
}

// ======== MAIN LOOP  =======================================
void loop()  {
  
    #ifdef Debug
      //FOR DEBUG ON SERIAL 115200
      timer=millis();
      if (timer-lastTime>DISPLAY_INTERVAL)  // if at least DISPLAY_INTERVAL ms have passed
      {
      message.debug();
      lastTime=timer;
      }
    #endif
  // No communication
  LEDPIN_OFF 
  
  // Sending and update GRAUPNER HOTT TELEMETRY
  message.main_loop();
}
