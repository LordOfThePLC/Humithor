//Include Headerfiles
#include "Config.h"
#include "Class_Motor.h"
#include "Class_HYT221.h"

//Include Libraries
#include <LiquidCrystal.h>

//Create Instances
LiquidCrystal LcdScreen(LCD_RESET, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
Motor Fan_Circulator;
Motor Fan_Humidifier;
HYT_221_Environmentsensor Environmentsensor;

//Functions
unsigned long previousTimeValue = 0;

bool Wait(unsigned long delaytime) {  //Toggles the output in a given time. Timevalue in seconds

  if (micros() - previousTimeValue >= delaytime * 1000000) {  //Compare act an previous time. If the difference equals to the wanted delay-time-->
    previousTimeValue = micros();                             //restore the new time as previous time
    return true;                                              //return a true flag to set an action
  } else {
    return false;  //else return a false flag
  }
}

void OP_ModeInterval() {

  switch (STATE) {

    case INIT:                     //Startup the Statemachine
      Fan_Circulator.SwitchOff();  //Switch Fan off
      STATE = IDLE;                //Go in the IDLE-State
      break;

    case IDLE:
      Fan_Circulator.SwitchOff();  //Switch Fan off
      if (Wait(WAITTIME_IDLE)) {   //Wait for a certain Time until the circulation starts
        STATE = CIRCULATE;         //If the time is over, start circulation
      }
      break;

    case CIRCULATE:
      Fan_Circulator.SwitchOn();       //Switch Fan on
      if (Wait(WAITTIME_CIRCULATE)) {  //Wait for a certain time until circulation stops
        STATE = IDLE;                  //Go to the IDLE-State
      }
      break;
  }
}

void OP_ModeConstantON() {
  STATE = INIT;               //Reset the Interval-Statemachine
  Fan_Circulator.SwitchOn();  //Switch Fan on
}

void ControlHumidity() {
  if (Environmentsensor.getHumidity() <= HUMIDITY_LOWERLIMIT) {
    Fan_Humidifier.SwitchOn();
  } else if (Environmentsensor.getHumidity() >= HUMIDITY_UPPERLIMIT) {
    Fan_Humidifier.SwitchOff();
  }
}

void PrintEnvironmentData() {

  char buf[80];  //Buffer-Memory
  //String buf = 'Temp:' + Environmentsensor.getTemperature() + 'C';

  sprintf(buf, "Temp: %d%cC", int(Environmentsensor.getTemperature()), 0xDF);  //Generate String to print on LCD
  LcdScreen.setCursor(0, 0);                                                   //Set Cursor to the upper left point of the LCD
  LcdScreen.print(buf);                                                        //Print the String on the Screen

  //Do the same with the humidity
  sprintf(buf, "Luftf.: %3.1f%", int(Environmentsensor.getHumidity()));
  LcdScreen.setCursor(0, 1);
  LcdScreen.print(buf);
}

//Setup-Routine
void setup() {
  LcdScreen.begin(LCD_NROFCOLUMNS, LCD_NROFROWS);  //Init and Start LCD-Screen
  Fan_Circulator.Init(DO_FAN_CIRCULATOR);          //Init Circulator Fan
  Fan_Humidifier.Init(DO_FAN_HUMIDIFIER);          //Init Humdifier Fan
  Environmentsensor.Init(HYT_ADDR);                //Init Sensor
  Serial.begin(9600);
}

//Main-Loop
void loop() {

  if (digitalRead(DI_OPERATIONMODE)) {
    OP_ModeInterval();
  } else {
    OP_ModeConstantON();
  }
  ControlHumidity();
  PrintEnvironmentData();
}