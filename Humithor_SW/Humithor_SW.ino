//Include Headerfiles
#include "Config.h"
#include "Class_Motor.h"
#include "Class_Sensorhandling.h"
#include "Rotaryencoder.h"
#include "LCD_Handling.h"
#include <EEPROM.h>

//Create Instances
Motor Fan_Circulator;
Motor Fan_Humidifier;
Sensorhandling Environmentsensor;
TimerOn tOn_IntervallCirculation_On;
TimerOn tOn_IntervallCirculation_Off;
TimerOn tOn_ActivateSettings;
TimerOn tOn_SaveSettings;
Rotaryencoder Encoder;

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

void OP_ModeSettings() {

  char buf[LCD_NROFCOLUMNS];              //Char-array for storing the string to print
  byte membuf = EEPROM.read(ADR_EEPROM);  //Memorybuffer to store the actual EEPROM-value

  switch (SETTINGSSTATE) {

    case INIT:
      LCD_SetUpperLine("IST-Regelbereich");                       //Set up the headline
      sprintf(buf, "%i%s...%i%s", membuf, "%", membuf + 3, "%");  //Show the actual controller-range
      LCD_SetLowerLine(buf);                                      //Print the headline
      if (!Encoder.GetButtonState()) {                            //Wait until the button is released
        SETTINGSSTATE = IDLE;                                     //and go into the next state
      }
      break;

    case IDLE:
      if (Encoder.GetButtonState()) {     //If the button gets pressed again...
        lcd.clear();                      //...clear the lcd and go to the next state
        if (!Encoder.GetButtonState()) {  //Release the button to jump to the next state, so you cannot "over-jump" the next state
          SETTINGSSTATE = SET_VALUE;
        }
      }
      break;

    case SET_VALUE:
      LCD_SetUpperLine("Regelbereich NEU");  //Set up the headline

      /*BIG TO DO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      if (Encoder.GetStatePinA()) {
        if (Humidity_LowerLimit <= 99) {
          Humidity_LowerLimit++;
        } else if (Humidity_LowerLimit > 99)
          Humidity_LowerLimit = 0;
      }*/

      sprintf(buf, "%i%s...%i%s", Humidity_LowerLimit, "%", Humidity_LowerLimit + 3, "%");  //Show the new set controll range
      LCD_SetLowerLine(buf);                                                                //Print the lower line

      if (Encoder.GetButtonState()) {                            //If the button is pressed
        if (tOn_SaveSettings.WaitForMilliseconds(3000, true)) {  //activate the timer, and if it's time is over, go to the next state
          SETTINGSSTATE = STORE_VALUE;
        }
      } else {
        tOn_SaveSettings.WaitForMilliseconds(3000, false);  //If the button is not pressed, reset the timer
      }
      break;

    case STORE_VALUE:
      EEPROM.update(ADR_EEPROM, Humidity_LowerLimit);  //Save the value to the EEPROM if it differs from the old value
      delay(5);                                        //The saving-process takes about 3,3ms to finish. So wait about 5ms
      if (LCD_Blink(3, 250)) {
        SETTINGSSTATE = DONE;
      }
      break;

    case DONE:
      lcd.clear();           //Clear the screen
      SETTINGSSTATE = INIT;  //Reset the statemachine
      MAINSTATE = INIT;      //Start in the INIT-State of the Main-Statemachine and come back to the mainscreen
      break;
  }
}

void Control_Humidity() {
  if (Environmentsensor.getHumidity() <= Humidity_LowerLimit) {  //If the humidityvalue is below the limit, switch the fan on
    Fan_Humidifier.SwitchOn();
  } else if (Environmentsensor.getHumidity() >= (Humidity_LowerLimit + Humidity_Delta)) {  //If the humidityvalue is on or over the upper limit, switch the fan off. The upper limit is the lower limit + delta
    Fan_Humidifier.SwitchOff();
  }
}

void Mainscreen() {

  char buf[LCD_NROFCOLUMNS];    //create a char-array as text-buffer
  char value[LCD_NROFCOLUMNS];  //create a chart array as value-buffer

  dtostrf(Environmentsensor.getTemperature(), 3, 1, value);  //Read the temperaturevalue and convert it to a string

  sprintf(buf, "Temp.:    %s%cC", value, 0xDF);  //Generate string for upper line on the LCD. The value 0xDF shows the °-sign on the screen
  LCD_SetUpperLine(buf);                         //Print the string on the upper line

  dtostrf(Environmentsensor.getHumidity(), 3, 1, value);  //Read the humidityvalue and convert it to a string
  sprintf(buf, "Luftf.:    %s%s", value, "%");            //Generate string for lower line on the LCD.
  LCD_SetLowerLine(buf);                                  //Print the string on the lower line
}

//Setup-Routine
void setup() {
  InitLCD(LCD_NROFCOLUMNS, LCD_NROFROWS);                                       //Init the LCD
  Encoder.Init(DI_ENCODER_A, DI_ENCODER_B, DI_ENCODER_BUTTON, 24, 15, 0, 100);  //Init Rotaryencoder
  Fan_Circulator.Init(DO_FAN_CIRCULATOR);                                       //Init Circulator Fan
  Fan_Humidifier.Init(DO_FAN_HUMIDIFIER);                                       //Init Humidifier Fan
  Environmentsensor.Init(REFRESHTIME_SENSOR);                                   //Init Sensor
}

//Main-Loop
void loop() {

  switch (MAINSTATE) {

    case INIT:                                                     //Startup the controller and the statemachine
      if (Startup) {                                               //If you start the controller new, always init the statemachine an re-read the memory
        LCD_SetUpperLine("Startup...");                            //Set the Startuptext in the Upper-line
        if (EEPROM.read(ADR_EEPROM) == 0) {                        //Read the value out of the EEPROM, if it's 0...
          EEPROM.update(ADR_EEPROM, Humidity_LowerLimit_default);  //...write the default value. This is usefull for the very first inital startup
          Startup = false;                                         //Set the startup-flag to false, the next cycle will change the state
        }
      }
      if (digitalRead(DI_OPERATIONMODE)) {  //If the button is on, go to interval-mode
        MAINSTATE = INTERVAL;
      } else if (!digitalRead(DI_OPERATIONMODE)) {  //If the button is not on, go to continious-mode
        MAINSTATE = CONSTANT_ON;
      }
      break;

    case INTERVAL:        //Handle the interval-mode
      OP_ModeInterval();  //Call the intervall-mode
      Mainscreen();
      if (!digitalRead(DI_OPERATIONMODE)) {  //If the button is not pushed, go to continious-mode
        MAINSTATE = CONSTANT_ON;
      }
      if (Encoder.GetButtonState()) {                                                    //If the encoderbutton is pressed...
        if (tOn_ActivateSettings.WaitForMilliseconds(WAITTIME_ACIVATESETTINGS, true)) {  //...and hold for at least a certain time...
          tOn_ActivateSettings.WaitForMilliseconds(WAITTIME_ACIVATESETTINGS, false);     //Reset the timer
          lcd.clear();                                                                   //Clear the Display once
          MAINSTATE = SETTINGS;                                                          //...switch to the settings-menu
        };
      } else {
        tOn_ActivateSettings.WaitForMilliseconds(WAITTIME_ACIVATESETTINGS, false);  //Else - do nothing
      }
      break;

    case CONSTANT_ON:                       //Handle the continious-mode
      OP_ModeConstantON();                  //Call the continious-mode
      Mainscreen();                         //Call the mainscreen-function
      if (digitalRead(DI_OPERATIONMODE)) {  //If the button is pushed, go to interval-mode
        MAINSTATE = INTERVAL;
      }
      if (Encoder.GetButtonState()) {                                                    //If the encoderbutton is pressed...
        if (tOn_ActivateSettings.WaitForMilliseconds(WAITTIME_ACIVATESETTINGS, true)) {  //...and hold for at least a certain time...
          tOn_ActivateSettings.WaitForMilliseconds(WAITTIME_ACIVATESETTINGS, false);     //Reset the timer
          lcd.clear();                                                                   //Clear the Display once
          MAINSTATE = SETTINGS;                                                          //...switch to the settings-menu
        };
      } else {
        tOn_ActivateSettings.WaitForMilliseconds(WAITTIME_ACIVATESETTINGS, false);  //Else - do nothing
      }
      break;

    case SETTINGS:
      OP_ModeSettings();  //Call the settings
      break;
  }
  Environmentsensor.handleSensor();  //Handle the sensor
}