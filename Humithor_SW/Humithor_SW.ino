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

  char buf[LCD_NROFCOLUMNS];  //Char-array for storing the string to print

  switch (SETTINGSSTATE) {

    case INIT:
      if (!Encoder.GetButtonState()) {
        SETTINGSSTATE = IDLE;
      }
      break;

    case IDLE:
      Encoder.preloadEncoder(Humidity_LowerLimit);
      LCD_SetUpperLine("Einstellungen");
      LCD_SetLowerLine("Taste druecken");
      if (Encoder.GetButtonState()) {
        lcd.clear();
        SETTINGSSTATE = PRESET;
      }
      break;

    case PRESET:
      if (!Encoder.GetButtonState()) {
        SETTINGSSTATE = SET_VALUE;
      }
      break;

    case SET_VALUE:
      LCD_SetUpperLine("Regelbereich");  //Set up the headline
      byte bufPos = Encoder.GetEncoderPosition();
      sprintf(buf, "%i%s...%i%s", bufPos, "%", bufPos + 3, "%");  //Show the new set controll range
      LCD_SetLowerLine(buf);
      if (Encoder.GetButtonState()) {
        SETTINGSSTATE = STORE_VALUE;
      }
      break;

    case STORE_VALUE:
      LCD_SetUpperLine("Speichern...");
      delay(5000);                                       //The saving-process takes about 3,3ms to finish. So wait about 5ms
      if (LCD_Blink(NUMBER_OF_BLINKS_BGL, BLINKTIME)) {  //Let the LCD blink for a certain time and amount to show the user, that the data is stored
        lcd.clear();                                     //Clear the screen
        SETTINGSSTATE = DONE;
      }
      break;

    case DONE:
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
  InitLCD(LCD_NROFCOLUMNS, LCD_NROFROWS);                                                        //Init the LCD
  Fan_Circulator.Init(DO_FAN_CIRCULATOR);                                                        //Init Circulator Fan
  Fan_Humidifier.Init(DO_FAN_HUMIDIFIER);                                                        //Init Humidifier Fan
  Environmentsensor.Init(REFRESHTIME_SENSOR);                                                    //Init Sensor
  Encoder.Init(DI_ENCODER_A, DI_ENCODER_B, DI_ENCODER_BUTTON, 24, Humidity_LowerLimit, 0, 100);  //Init Rotaryencoder
}

//Main-Loop
void loop() {

  switch (MAINSTATE) {

    case INIT:                                                     //Startup the controller and the statemachine
      if (Startup) {                                               //If you start the controller new, always init the statemachine an re-read the memory
        LCD_SetUpperLine("Startup...");                            //Set the Startuptext in the Upper-line
        if (EEPROM.read(ADR_EEPROM) == 0) {                        //Read the value out of the EEPROM, if it's 0...
          EEPROM.update(ADR_EEPROM, Humidity_LowerLimit_default);  //...write the default value. This is usefull for the very first inital startup
        }
        delay(500);                                                       //Wait until EEPROM is read
        char buf[LCD_NROFCOLUMNS];                                        //Buffervalue for buffering the string
        sprintf(buf, "Wert %i%s geladen", EEPROM.read(ADR_EEPROM), "%");  //Create the string with the loaded value
        LCD_SetLowerLine(buf);                                            //Print the text on the screen
        delay(1500);                                                      //Wait and show the loaded value
        Startup = false;                                                  //Set the startup-flag to false, the next cycle will change the state
      }
      if (!Encoder.GetButtonState()) {
        if (digitalRead(DI_OPERATIONMODE)) {  //If the button is on, go to interval-mode
          MAINSTATE = INTERVAL;
        } else if (!digitalRead(DI_OPERATIONMODE)) {  //If the button is not on, go to continious-mode
          MAINSTATE = CONSTANT_ON;
        }
      }
      break;

    case INTERVAL:        //Handle the interval-mode
      OP_ModeInterval();  //Call the intervall-mode
      Mainscreen();
      if (!digitalRead(DI_OPERATIONMODE)) {  //If the button is not pushed, go to continious-mode
        MAINSTATE = CONSTANT_ON;
      }
      /*if (Encoder.GetButtonState()) {
        lcd.clear();           //Clear the Display once
        MAINSTATE = SETTINGS;  //...switch to the settings-menu
      }*/
      break;

    case CONSTANT_ON:                       //Handle the continious-mode
      OP_ModeConstantON();                  //Call the continious-mode
      Mainscreen();                         //Call the mainscreen-function
      if (digitalRead(DI_OPERATIONMODE)) {  //If the button is pushed, go to interval-mode
        MAINSTATE = INTERVAL;
      }
      /*if (Encoder.GetButtonState()) {
        lcd.clear();           //Clear the Display once
        MAINSTATE = SETTINGS;  //...switch to the settings-menu
      }*/
      break;

    /*case SETTINGS:
      OP_ModeSettings();  //Call the settings
      break;*/
  }
  Environmentsensor.handleSensor();  //Handle the sensor
}