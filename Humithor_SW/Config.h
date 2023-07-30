#include "Arduino.h"
//Digital Inputs
#define DI_OPERATIONMODE A6   //Digitaleingang Betriebsartenwahlschalter 0 = Dauerbetrieb 1 = Intervallbetrieb
#define DI_ENCODER_BUTTON 17  //Button Rotaryencoder
#define DI_ENCODER_A 2        //Spur A Rotaryencoder
#define DI_ENCODER_B 3        //Spur B Rotaryencoder

//Digital Outputs
#define DO_FAN_CIRCULATOR 5  //Digitalausgang Lüfter Umluft
#define DO_FAN_HUMIDIFIER 4  //Digitalausgang Befeuchter

//LCD Pins and Config
#define LCD_RESET 6   //Resetpin LCD
#define LCD_ENABLE 8  //Enablepin LCD
#define LCD_D4 12     //D4-Pin LCD
#define LCD_D5 11     //D5-Pin LCD
#define LCD_D6 10     //D6-Pin LCD
#define LCD_D7 9      //D7-Pin LCD
#define LCD_BGL 13    //Backgroundlight LCD

#define LCD_NROFROWS 2      //LCD Zeilenanzahl
#define LCD_NROFCOLUMNS 16  //LCD Spaltenanzahl

//States Fan
#define INIT 0       //Initialstate
#define IDLE 1       //Leerlauf
#define CIRCULATE 2  //Umluft
int STATE = INIT;

//States Main-Statemachine
#define INTERVAL 1
#define CONSTANT_ON 2
#define SETTINGS 3
int MAINSTATE = INIT;

//States Settings-Statemachine
#define SET_VALUE 2
#define STORE_VALUE 4
#define DONE 5
int SETTINGSSTATE = INIT;


//Waittimes
#define WAITTIME_IDLE 600000            //Wartezeit Leerlauf
#define WAITTIME_CIRCULATE 60000       //Ablaufzeit Umluft
#define WAITTIME_ACIVATESETTINGS 3000  //Haltezeit Button für Einstellungen
#define REFRESHTIME_SENSOR 500         //Refresh-Zeit der Sensorwerte in Millisekunden
#define NUMBER_OF_BLINKS_BGL 3         //Anzahl wie oft der LCD blinkt beim speichern
#define BLINKTIME 250                  //Blinkdauer des LCD beim speichern

//Humidity Control
byte Humidity_LowerLimit_default = 67;  //Unterer Grenzwert Luftfeuchte - defaultwert
byte Humidity_LowerLimit = 67;          //Unterer Grenzwert Luftfeuchte
byte Humidity_Delta = 3;                //Hysteresegrenze unten Luftfeuchte

//EEPROM
byte ADR_EEPROM = 0;

//Globals
bool Startup = true;  //Flag für ersten Zyklus