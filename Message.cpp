/*
   LIPOMETER v1.0
   http://johnlenfr.1s.fr
   v1.0
 
 Arduino pro Mini 5v/16mHz w/ Atmega 328
 
 */

#define HOTTV4_RXTX 3           // Pin for HoTT telemetrie output
#define LEDPIN_OFF              PORTB &= ~(1<<5);
#define LEDPIN_ON               PORTB |= (1<<5);

// Module define
#define GPS                             // GPS Modul
#define GAM                             // GAM Modul

#include "Message.h"
#include <EEPROM.h>
#include <SoftwareSerial.h>

 
SoftwareSerial SERIAL_HOTT(HOTTV4_RXTX , HOTTV4_RXTX); // RX, TX

/**
 * Enables RX and disables TX
 */
static inline void hottV4EnableReceiverMode() {
  DDRD &= ~(1 << HOTTV4_RXTX);
  PORTD |= (1 << HOTTV4_RXTX);
}

/**
 * Enabels TX and disables RX
 */
static inline void hottV4EnableTransmitterMode() {
  DDRD |= (1 << HOTTV4_RXTX);
}

static uint8_t _hott_serial_buffer[173];   //creating a buffer variable to store the struct

// pointer to the buffer structures "_hott_serial_buffer"
struct HOTT_GAM_MSG     *hott_gam_msg = (struct HOTT_GAM_MSG *)&_hott_serial_buffer[0];
struct HOTT_TEXTMODE_MSG	*hott_txt_msg =	(struct HOTT_TEXTMODE_MSG *)&_hott_serial_buffer[0];

// Alarm
int alarm_on_off_batt1 = 1; // 0=FALSE/Disable 1=TRUE/Enable   // Enable alarm by default for safety
char* alarm_on_off[13];      // For Radio OSD
char* model_config[15];      // For Radio OSD
static uint16_t alarm_min1 = 360; // Min volt for alarm in mV
// Timer for alarm
// for refresh time
int alarm_interval = 15000; // in ms
static unsigned long lastTime=0;  // in ms
unsigned long time=millis();      // in ms

// For communication
static uint8_t octet1 = 0;  // reception
static uint8_t octet2 = 0;  // reception

// For Lipo
static uint8_t nb_Lipo = 4; // Number of items Lipo battery default before detection
float  Lipo_total = 0;      // Total voltage measured
float  lipo1 = 0.0;         // Voltage on each element
float  lipo2 = 0.0;
float  lipo3 = 0.0;
float  lipo4 = 0.0; 
float  lipo5 = 0.0; 
float  lipo6 = 0.0; 
float  lipo_mini_bat1 = 0.0; // Minimum voltage value element
uint8_t Jauge = 0;           // fuel gauge


// For saving settings in EEPROM
/*
 !WARNING!
 Writing takes 3.3ms.
 Maximum life of the EEPROM is 100000 writings/readings.
 Be careful not to use it too much, it is not replacable!
 */
#define adr_eprom_test 0                 // For the test for 1st time init of the Arduino (First power on)
#define adr_eprom_alarm_min1 2           // Default alarm min is 3.60v
#define adr_eprom_nb_cells_batt1 4       // Automaticaly detected and calculated
#define adr_eprom_alarm_on_off_batt1 6   // 0=FALSE/Disable 1=TRUE/Enable
#define adr_eprom_alarm_interval 8       // Audio warning alarm interval 


GMessage::GMessage(){

}
void GMessage::init(){
  
  SERIAL_HOTT.begin(SERIAL_COM_SPEED); // 19400 FOR GRAUPNER HOTT using SoftSerial lib.
  hottV4EnableReceiverMode(); 
  
  // Test for 1st time init of the Arduino (First power on)
  int test = read_eprom(adr_eprom_test);
  if (test != 123)
  {
    write_eprom(adr_eprom_test,123);
    write_eprom(adr_eprom_alarm_min1,alarm_min1);
    write_eprom(adr_eprom_alarm_on_off_batt1,alarm_on_off_batt1);
  }
  // Read saved values from EEPROM
    // alarm min on battery
    alarm_min1 = read_eprom(adr_eprom_alarm_min1); // default is 3.60v if not change
    // Enable / Disable alarm bip
    alarm_on_off_batt1 = read_eprom(adr_eprom_alarm_on_off_batt1); // 0=FALSE/Disable 1=TRUE/Enable
    alarm_interval = read_eprom(adr_eprom_alarm_interval);

    

}

uint16_t GMessage::read_eprom(int address){
  return  (uint16_t) EEPROM.read(address) * 256 + EEPROM.read(address+1) ;
}

void GMessage::write_eprom(int address,uint16_t val){
  EEPROM.write(address, val  / 256);
  EEPROM.write(address+1,val % 256 );
}


void GMessage::init_gam_msg(){
  //puts to all Zero, then modifies the constants
  memset(hott_gam_msg, 0, sizeof(struct HOTT_GAM_MSG));   
  hott_gam_msg->start_byte = 0x7c;
  hott_gam_msg->gam_sensor_id = HOTT_TELEMETRY_GAM_SENSOR_ID;
  hott_gam_msg->sensor_id = 0xd0;
  hott_gam_msg->stop_byte = 0x7d;
}


// Sending the frame
void GMessage::send(int lenght){ 
  uint8_t sum = 0;
  hottV4EnableTransmitterMode(); 
  delay(5);
  for(int i = 0; i < lenght-1; i++){
    sum = sum + _hott_serial_buffer[i];
    SERIAL_HOTT.write (_hott_serial_buffer[i]);
    delayMicroseconds(HOTTV4_TX_DELAY);
  }  
  //Emision checksum
  SERIAL_HOTT.write (sum);
  delayMicroseconds(HOTTV4_TX_DELAY);

  hottV4EnableReceiverMode();
}

void GMessage::main_loop(){ 
  
  // STARTING MAIN PROGRAM
  static byte page_settings = 1; // page number to display settings
  


  if(SERIAL_HOTT.available() >= 2) {
    uint8_t octet1 = SERIAL_HOTT.read();
    switch (octet1) {
    case HOTT_BINARY_MODE_REQUEST_ID:
      { 
        uint8_t  octet2 = SERIAL_HOTT.read();
        
        // Demande RX Module =	$80 $XX
        switch (octet2) {
        
        #ifdef GAM
        case HOTT_TELEMETRY_GAM_SENSOR_ID: //0x8D
          {    
               LEDPIN_ON
           
            // init structure
               init_gam_msg();
            
            // Set values to 0 for clean screen
               setTemp (0,2);                       // not use
               setTemp (0,1);                       // not use
               setClimbrate_M3(120);                // not use
               setVoltage2(0);                      // not use
              
            // Use values of the GPS
               setAltitudeInMeters (500);     // use gps infos
               setClimbrate(30000); // use gps infos
               setSpeed(0);                 // use gps infos

            // Voltage Measurement
             lipo1 = 0;
             lipo2 = 0;
             lipo3 = 0;
             lipo4 = 0;
             lipo5 = 0;
             lipo6 = 0;

              // Set Lipo       
              setLipo (lipo1,1);
              setLipo (lipo2,2);
              setLipo (lipo3,2);
              setLipo (lipo4,2);
              setLipo (lipo5,2);
              setLipo (lipo6,2);
              
              // set lipo voltages on Battery 1 and Main voltage
              setVoltage1(Lipo_total);    // Batt 1
              setMainVoltage(Lipo_total); // Main

               // Searching Minus
               // Search the weakest element
               // to gauge display
               // radio and alarm management
               lipo_mini_bat1 = 0;
               setVoltage2(lipo_mini_bat1);    // Batt 2
                
               time=millis();
               if (time-lastTime>alarm_interval)  // if at least alarm_interval in ms have passed
               {
                     // Check for alarm beep
                     if (((lipo_mini_bat1*100) < alarm_min1) && alarm_on_off_batt1 == 1)
                     {
                     hott_txt_msg->warning_beeps =  ALARME_TENSION_SEUIL    ; // alarm beep or voice
                     }
                 lastTime=time;  // reset timer
               }
            
               // Fuel gauge display
               setFuelPercent (0);
               // View percentage of lipo voltage behind fuel in place of milliliters
               setFuelMilliliters(0);
               // current
               setCurrent (2.5);
               // batt_cap
               setBatt_Cap (3.33); 
               
            // sending all data
            send(sizeof(struct HOTT_GAM_MSG));
            break;
          } //end case GAM*/
          #endif
        } //end case octet 2
        break;
      }

    case HOTT_TEXT_MODE_REQUEST_ID:
      {
        //LEDPIN_ON
        uint8_t  octet3 = SERIAL_HOTT.read();
        byte id_sensor = (octet3 >> 4);
        byte id_key = octet3 & 0x0f;
        static byte ligne_select = 4 ;
        static int8_t ligne_edit = -1 ;
        hott_txt_msg->start_byte = 0x7b;
        hott_txt_msg->esc = 0;
        hott_txt_msg->warning_beeps = 0;
        
        memset((char *)&hott_txt_msg->text, 0x20, HOTT_TEXTMODE_MSG_TEXT_LEN);
        hott_txt_msg->stop_byte = 0x7d;

        if (id_key == HOTT_KEY_LEFT && page_settings == 1)
        {   
          hott_txt_msg->esc = 0x01;
        }
        else
        {
          if (id_sensor == (HOTT_TELEMETRY_GAM_SENSOR_ID & 0x0f)) 
          {
            switch (page_settings) { //SETTINGS
              
              case 1://PAGE 1 SETTINGS
              
                    {
                    // test if alarm active for display ON or OFF the screen
                    if (alarm_on_off_batt1 == 0)
                      *alarm_on_off = " Alarm : OFF";
                      else
                      *alarm_on_off = " Alarm :  ON";
                                           
                    if (id_key == HOTT_KEY_UP && ligne_edit == -1)
                    ligne_select = min(6,ligne_select+1); // never gets above line 6 max
                    else if (id_key == HOTT_KEY_DOWN && ligne_edit == -1)
                    ligne_select = max(4,ligne_select-1); // never gets above line 4 min
                    else if (id_key == HOTT_KEY_SET && ligne_edit == -1)
                    ligne_edit =  ligne_select ;
                    else if (id_key == HOTT_KEY_RIGHT && ligne_edit == -1)
                      {
                        if (page_settings >=1)// change it if you want more pages
                          page_settings = 1;
                        else
                          page_settings+=1;
                      }
                      
                      
                      
                    //LINE 4 SELECTED = text[4]
                    else if (id_key == HOTT_KEY_UP && ligne_select == 4 )
                      {
                        alarm_on_off_batt1 = 1;
                        *alarm_on_off = " Alarm :  ON";
                      }
                      
                    else if (id_key == HOTT_KEY_DOWN && ligne_select == 4 )
                      {
                        alarm_on_off_batt1 = 0;
                        *alarm_on_off = " Alarm : OFF";
                       }
                      
                    else if (id_key == HOTT_KEY_SET && ligne_edit == 4)
                      {
                       ligne_edit = -1 ;
                       write_eprom(adr_eprom_alarm_on_off_batt1,alarm_on_off_batt1);
                       }
                    
                    
                    //LINE 5 SELECTED = text[5]
                    else if (id_key == HOTT_KEY_UP && ligne_select == 5 )
                      alarm_min1+=5;
                    else if (id_key == HOTT_KEY_DOWN && ligne_select == 5 )
                      alarm_min1-=5;
                    else if (id_key == HOTT_KEY_SET && ligne_edit == 5)
                      {
                       ligne_edit = -1 ;
                       write_eprom(adr_eprom_alarm_min1,alarm_min1);
                       }

                    else if (alarm_min1>420) // not over 4.2v
                        {
                          alarm_min1=5;
                        } 
                    else if (alarm_min1<1)  // not behind 0v
                        {
                       alarm_min1=420;
                        }

                    //LINE 6 SELECTED = text[6]
                    else if (id_key == HOTT_KEY_UP && ligne_select == 6 )
                      alarm_interval+=1000;
                    else if (id_key == HOTT_KEY_DOWN && ligne_select == 6 )
                      alarm_interval-=1000;
                    else if (id_key == HOTT_KEY_SET && ligne_edit == 6)
                      {
                       ligne_edit = -1 ;
                       write_eprom(adr_eprom_alarm_interval,alarm_interval);
                       }
                    else if (alarm_interval>60000)
                    {
                       alarm_interval=1000;  
                    }
                    else if (alarm_interval<0)
                       {alarm_interval=0; }
                              
                    // Showing page 1
                    
                    //line 0:
                    snprintf((char *)&hott_txt_msg->text[0],21," LIPO    <");
                    //line 1:
                    snprintf((char *)&hott_txt_msg->text[1],21,"Lipo Total: %i.%iv",(int) Lipo_total, ((int) (Lipo_total*100)) % 100);
                    //line 2:
                    snprintf((char *)&hott_txt_msg->text[2],21,"Cells : %iS",nb_Lipo);
                    //line 3:
                    snprintf((char *)&hott_txt_msg->text[3],21,"Change settings :");
                    //line 4:
                    snprintf((char *)&hott_txt_msg->text[4],21,*alarm_on_off);
                    //line 5:
                    if (((int) (alarm_min1 % 100)) == 5 || ((int) (alarm_min1 % 100)) == 0)// Display management to put a 0 after the comma and the X.05v X.00v values ​​because the modulo return 0 or 5 for typical values ​​100,105,200,205,300,305, etc ...
                    snprintf((char *)&hott_txt_msg->text[5],21," Alarm value : %i.0%iv",(int) (alarm_min1/100),(int) (alarm_min1 % 100)); // adding a 0
                    else // normal display
                    snprintf((char *)&hott_txt_msg->text[5],21," Alarm value : %i.%iv",(int) (alarm_min1/100),(int) (alarm_min1 % 100)); // no need of adding 0
                    //line 6:
                    snprintf((char *)&hott_txt_msg->text[6],21," Alarm repeat: %is",(alarm_interval/1000));
                    //line 7:
                    snprintf((char *)&hott_txt_msg->text[7],21,"Strom2HoTT  %d/1",page_settings); //Showing page number running down the screen to the right
                    
                    hott_txt_msg->text[ligne_select][0] = '>';
                    _hott_invert_ligne(ligne_edit);
                    break;                    
                    }//END PAGE 1
                  
                  default://PAGE 
                  {
                    break;
                  }
                  
                  
                  
            }//END SETTINGS

          } // END IF
          
             
          else {
            snprintf((char *)&hott_txt_msg->text[0],21,"Unknow sensor module <");
            snprintf((char *)&hott_txt_msg->text[1],21,"Nothing here");
          }
        }
        _hott_send_text_msg();
        //LEDPIN_OFF
        break;

      }
    }	
  }
}


//////////////////////////////////////////////////
// Main voltage (0.1V resolution)
void GMessage::setMainVoltage(float tension){
  hott_gam_msg->main_voltage = (uint16_t) (tension * 10);
}

// battery 1 (0.1V resolution)
void GMessage::setVoltage1(float volt){
  hott_gam_msg->Battery1 =  (uint16_t) (volt * 10); 
}

// battery 2 (0.1V resolution)
void GMessage::setVoltage2(float volt){
  hott_gam_msg->Battery2 = (uint16_t) (volt * 10); 
}


// Element n Lipo (0.2V resolution)
void GMessage::setLipo(float volt, int lipo){

  if (lipo >= 1 and lipo <= 6)
  {
    lipo--;
    hott_gam_msg->cell[lipo] =  (uint16_t) 100 * volt / 2 ; 
    
    if (volt/2 <= hott_gam_msg->min_cell_volt || hott_gam_msg->min_cell_volt ==0)
    {
      hott_gam_msg->min_cell_volt = (uint16_t) 100 * volt/2 ;
      hott_gam_msg->min_cell_volt_num = lipo;
    }
  }
}


// Relative altitude in meters : -500 / +9999 [m]
void GMessage::setAltitudeInMeters(uint16_t alti){
  hott_gam_msg->altitude = alti + 500;
}

// temperature : -20 a +235° C 
// 
void GMessage::setTemp(int temp, int capteur){
  if(temp < -20)
    temp = -20;
  else if(temp > 234)
    temp = 235;
  if (capteur < 1)
    capteur = 1;
  if (capteur > 2)
    capteur = 2; 
  if (capteur == 1)
    hott_gam_msg->temperature1 = temp + 20;
  else if (capteur == 2)
    hott_gam_msg->temperature2 = temp + 20;
}

// tank level : 0%, 25%, 50%, 75%, 100%  
// 
void GMessage::setFuelPercent(uint8_t pourcent){
  if(pourcent > 100)
    pourcent = 100;
  else if(pourcent < 0)
    pourcent = 0;
  hott_gam_msg->fuel_procent = pourcent ;
}

// tank level ml : 0 a 65535 ml
// 
void GMessage::setFuelMilliliters(uint16_t ml){
  hott_gam_msg->fuel_ml = ml ;
}

// rotation : 0 a 655350 tr/min
void GMessage::setRPM(uint16_t rpm){
  hott_gam_msg->rpm = rpm ;
} 

// Set climbrates
void GMessage::setClimbrate(uint16_t climb_L){
  hott_gam_msg->climbrate_L =  climb_L;
}
void GMessage::setClimbrate_M3(int climb3s){
  hott_gam_msg->climbrate3s =  climb3s;
}

// Set speed 
void GMessage::setSpeed(float speed){
  hott_gam_msg->speed =  (int) speed;
}

// Set current 1 = 0.1A
void GMessage::setCurrent(float a){
  hott_gam_msg->current = (uint16_t) (a * 10); 
}
// Set batt_cap 1 = 10mAh
void GMessage::setBatt_Cap(float ma){
  hott_gam_msg->batt_cap = (uint16_t) (ma * 100); 
}


//
// Audible alarm on radio
//
void GMessage::alarme (uint8_t son){
  hott_gam_msg->warning_beeps = son;
}



void GMessage::_hott_send_text_msg() {
  for(byte *_hott_msg_ptr = hott_txt_msg->text[0]; _hott_msg_ptr < &hott_txt_msg->stop_byte ; _hott_msg_ptr++){
    if (*_hott_msg_ptr == 0)
      *_hott_msg_ptr = 0x20;
  }  
  //_hott_send_msg(_hott_serial_buffer, sizeof(struct HOTT_TEXTMODE_MSG));
  send(sizeof(struct HOTT_TEXTMODE_MSG));
}


char * GMessage::_hott_invert_all_chars(char *str) {
  return _hott_invert_chars(str, 0);
}


char * GMessage::_hott_invert_chars(char *str, int cnt) {
  if(str == 0) return str;
  int len = strlen(str);
  if((len < cnt)  && cnt > 0) len = cnt;
  for(int i=0; i< len; i++) {
    str[i] = (byte)(0x80 + (byte)str[i]);
  }
  return str;
}


void GMessage::_hott_invert_ligne(int ligne) {
  if (ligne>= 0 && ligne<= 7)
    for(int i=0; i< 21; i++) {
      if (hott_txt_msg->text[ligne][i] == 0)   //reversing the null character (end of string)
        hott_txt_msg->text[ligne][i] = (byte)(0x80 + 0x20);
      else
        hott_txt_msg->text[ligne][i] = (byte)(0x80 + (byte)hott_txt_msg->text[ligne][i]);
    }
}


uint32_t GMessage::seconds() {
  return millis() / 1000;
}


void GMessage::debug(){
   // FOR DEBUG
    Serial.println("------------------------");
    Serial.print("Alt:"); Serial.println(0);


}
