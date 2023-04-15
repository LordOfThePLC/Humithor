//Include Headerfiles
#include "Config.h"
#include "Class_Motor.h"
#include "Class_Sensorhandling.h"

//Include Libraries
#include <LiquidCrystal.h>

//Create Instances
LiquidCrystal LcdScreen(LCD_RESET, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
Motor Fan_Circulator;
Motor Fan_Humidifier;
Sensorhandling Environmentsensor;
TimerOn tOn_IntervallCirculation_On;
TimerOn tOn_IntervallCirculation_Off;

void OP_ModeInterval() {

  switch (STATE) {

    case INIT:                                                                     //Startup the Statemachine
      Fan_Circulator.SwitchOff();                                                  //Switch Fan off
      tOn_IntervallCirculation_Off.WaitForMilliseconds(WAITTIME_IDLE, false);      //Hand over the waittime and switch off the timer
      tOn_IntervallCirculation_On.WaitForMilliseconds(WAITTIME_CIRCULATE, false);  //Hand over the waittime and switch off the timer
      STATE = IDLE;                                                                //Go in the IDLE-State
      break;

    case IDLE:
      Fan_Circulator.SwitchOff();                                                   //Switch Fan off
      if (tOn_IntervallCirculation_Off.WaitForMilliseconds(WAITTIME_IDLE, true)) {  //Wait for a certain Time until the circulation starts
        tOn_IntervallCirculation_Off.WaitForMilliseconds(WAITTIME_IDLE, false);     //Switch off the timer
        STATE = CIRCULATE;                                                          //If the time is over, start circulation
      }
      break;

    case CIRCULATE:
      Fan_Circulator.SwitchOn();                                                        //Switch Fan on
      if (tOn_IntervallCirculation_On.WaitForMilliseconds(WAITTIME_CIRCULATE, true)) {  //Wait for a certain time until circulation stops
        tOn_IntervallCirculation_On.WaitForMilliseconds(WAITTIME_CIRCULATE, false);     //Switch off the timer
        STATE = IDLE;                                                                   //Go to the IDLE-State
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