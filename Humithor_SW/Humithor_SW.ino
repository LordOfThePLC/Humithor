//Include Headerfiles
#include "Config.h"
#include "Class_Motor.h"
#include "Class_Sensorhandling.h"
#include "Rotaryencoder.h"
#include "LCD.h"

//Create Instances
Motor Fan_Circulator;
Motor Fan_Humidifier;
Sensorhandling Environmentsensor;
TimerOn tOn_IntervallCirculation_On;
TimerOn tOn_IntervallCirculation_Off;
TimerOn tOn_ActivateSettings;
Rotaryencoder Encoder;
LCD Lcd;

//Global Variables
int Humidity_UpperLimit = 67;

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

void HandleLCD() {
  char buf[80];  //Buffer-Memory
  int setValue_HumidityLimit;

  switch (STATE_LCD) {

    case INIT_LCD:
      setValue_HumidityLimit = Humidity_UpperLimit;  //Handover the Limitvalue once
      STATE_LCD = MAINSCREEN;
      break;

    case MAINSCREEN:
      sprintf(buf, "Temp: %d%cC", int(Environmentsensor.getTemperature()), 0xDF);  //Generate String to print on LCD
      LcdScreen.setCursor(0, 0);                                                   //Set Cursor to the upper left point of the LCD
      LcdScreen.print(buf);                                                        //Print the string on the screen

      sprintf(buf, "Luftf.: %3.1f%", int(Environmentsensor.getHumidity()));  //Generate String to print on LCD
      LcdScreen.setCursor(0, 1);                                             //Set Cursor to the lower left point of the LCD
      LcdScreen.print(buf);                                                  //Print the string on the screen

      if (tOn_ActivateSettings.WaitForMilliseconds(WAITTIME_ACIVATESETTINGS, (Encoder.GetButtonState()))) {  //Hold the encoderbutton for Waittime-Seconds to activate the settings-menu
        STATE_LCD = SETTINGS;                                                                                //Change state to settings
      }
      break;

    case SETTINGS:
      LcdScreen.setCursor(0, 0);                        //Set Cursor to the upper left point of the LCD
      LcdScreen.print("Hysteresebereich Luftfeuchte");  //Print Headline on Screen

      if (setValue_HumidityLimit <= 100) &&(setValue_HumidityLimit >= 0) {           //If the limit is below 100 and over 0 then...
          setValue_HumidityLimit = setValue_HumidityLimit + Encoder.GetDirection();  //...turn the encoder to increase or decrease the lower-limit of the humidity
        }
      else if (setValue_HumidityLimit >= 100) {  //Limit the limit with 100
        setValue_HumidityLimit = 100;
      } else if (setValue_HumidityLimit <= 0) {  //Limit the limit with 0
        setValue_HumidityLimit = 0;
      }

      sprintf(buf, "Sollwert: %3.1f%", setValue_HumidityLimit);  //Generate the string to print on LCD
      LcdScreen.setCursor(0, 1);                                 //Set the cursor to the lower left point of the LCD
      LcdScreen.print(buf);

      if (tOn_ActivateSettings.WaitForMilliseconds(WAITTIME_ACIVATESETTINGS, (Encoder.GetButtonState()))) {
        STATE_LCD = MAINSCREEN;
      }
      break;
  }
}

//Setup-Routine
void setup() {
  Lcd.Init(LCD_RESET, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7, LCD_NROFROWS, LCD_NROFCOLUMNS);  //Init and Start LCD-Screen
  Encoder.Init(DI_ENCODER_A, DI_ENCODER_B, DI_ENCODER_BUTTON);                                     //Init Rotaryencoder
  Fan_Circulator.Init(DO_FAN_CIRCULATOR);                                                          //Init Circulator Fan
  Fan_Humidifier.Init(DO_FAN_HUMIDIFIER);                                                          //Init Humidifier Fan
  Environmentsensor.Init(REFRESHTIME_SENSOR);                                                      //Init Sensor
}

//Main-Loop
void loop() {

  if (digitalRead(DI_OPERATIONMODE)) {
    OP_ModeInterval();
  } else {
    OP_ModeConstantON();
  }
  HandleLCD();
}