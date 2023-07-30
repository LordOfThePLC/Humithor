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
Rotaryencoder Encoder;

void OP_ModeInterval() {

  switch (STATE) {

    case INIT:                               //Startup the Statemachine
      Fan_Circulator.SwitchOff();            //Switch Fan off
      tOn_IntervallCirculation_Off.reset();  //Reset the timer
      tOn_IntervallCirculation_On.reset();   //Reset the timer
      STATE = IDLE;                          //Go in the IDLE-State
      break;

    case IDLE:
      Fan_Circulator.SwitchOff();
      tOn_IntervallCirculation_On.reset();             //Switch off the timer
      tOn_IntervallCirculation_Off.start();            //Start the timer
      if (tOn_IntervallCirculation_Off.timerOutput) {  //Wait for a certain time until the circulation starts
        STATE = CIRCULATE;                             //If the time is over, start circulation
      }
      break;

    case CIRCULATE:
      Fan_Circulator.SwitchOn();                      //Switch Fan on
      tOn_IntervallCirculation_Off.reset();           //Switch off the timer
      tOn_IntervallCirculation_On.start();            //Start the timer
      if (tOn_IntervallCirculation_On.timerOutput) {  //Wait for a certain Time until the circulation starts
        STATE = IDLE;                                 //Go to the IDLE-State
      }
      break;
  }
  tOn_IntervallCirculation_Off.WaitForMilliseconds(WAITTIME_IDLE);      //Call the Instance
  tOn_IntervallCirculation_On.WaitForMilliseconds(WAITTIME_CIRCULATE);  //Call the Instance
}

void OP_ModeConstantON() {
  STATE = INIT;               //Reset the Interval-Statemachine
  Fan_Circulator.SwitchOn();  //Switch Fan on
}

void OP_ModeSettings() {

  char buf[LCD_NROFCOLUMNS];  //Buffer for the generatet string

  switch (SETTINGSSTATE) {

    case INIT:                               //Init the Settings
      LCD_SetUpperLine("Button loslassen");  //Tell the user to release the Button
      if (!Encoder.GetButtonState()) {
        lcd.clear();
        Encoder.preloadEncoder(EEPROM.read(ADR_EEPROM));  //Preload the Encoder with the actual value of the EEPROM
        SETTINGSSTATE = SET_VALUE;                        //Next state
      }
      break;

    case SET_VALUE:
      LCD_SetUpperLine("Regelbereich");                           //Set the headline
      byte bufPos = Encoder.GetEncoderPosition();                 //Get and store the actual encoderposition in a buffer-value
      Humidity_LowerLimit = bufPos;                               //Set the encoderposition to the limitvalue. So you can check the behaviour of the fan on the fly
      sprintf(buf, "%i%s...%i%s", bufPos, "%", bufPos + 3, "%");  //Generate the lower line of the LCD
      LCD_SetLowerLine(buf);                                      //Print the LCD
      if (Encoder.GetButtonState()) {                             //If the Button is pressed
        lcd.clear();                                              //clear the lines
        EEPROM.update(ADR_EEPROM, Humidity_LowerLimit);           //Update the value in the EEPROM
        delay(5);                                                 //The writingprocess to the EEPROM takes about 3,3ms, so wait about 5ms to be sure
        MAINSTATE = DONE;                                         //go ahead in the main-statemachine
      }
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
  InitLCD(LCD_NROFCOLUMNS, LCD_NROFROWS);                                                    //Init the LCD
  Fan_Circulator.Init(DO_FAN_CIRCULATOR);                                                    //Init Circulator Fan
  Fan_Humidifier.Init(DO_FAN_HUMIDIFIER);                                                    //Init Humidifier Fan
  Environmentsensor.Init(REFRESHTIME_SENSOR);                                                //Init Sensor
  Encoder.Init(DI_ENCODER_A, DI_ENCODER_B, DI_ENCODER_BUTTON, Humidity_LowerLimit, 0, 100);  //Init Rotaryencoder
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
        Humidity_LowerLimit = EEPROM.read(ADR_EEPROM);                    //Read the EEPROM and store the value into the variable
        sprintf(buf, "Wert %i%s geladen", EEPROM.read(ADR_EEPROM), "%");  //Create the string with the loaded value
        LCD_SetLowerLine(buf);                                            //Print the text on the screen
        delay(1500);                                                      //Wait and show the loaded value
        Startup = false;                                                  //Set the startup-flag to false, the next cycle will change the state
      }
      if (!Encoder.GetButtonState()) {
        if (analogRead(DI_OPERATIONMODE)) {  //If the button is on, go to interval-mode
          MAINSTATE = INTERVAL;
        } else if (!analogRead(DI_OPERATIONMODE)) {  //If the button is not on, go to continious-mode
          MAINSTATE = CONSTANT_ON;
        }
      }
      SETTINGSSTATE = INIT;
      break;

    case INTERVAL:        //Handle the interval-mode
      OP_ModeInterval();  //Call the intervall-mode
      Mainscreen();
      if (!analogRead(DI_OPERATIONMODE)) {  //If the button is not pushed, go to continious-mode
        MAINSTATE = CONSTANT_ON;
      }
      if (Encoder.GetButtonState()) {            //if the Button is pressed...
        tOn_ActivateSettings.start();            //...start the timer
        if (tOn_ActivateSettings.timerOutput) {  //if the time is over
          lcd.clear();                           //clear all the lines
          MAINSTATE = SETTINGS;                  //and go to the settins
        }
      } else {
        tOn_ActivateSettings.reset();  //if the button is released before the time is over, reset the timer
      }
      break;

    case CONSTANT_ON:                      //Handle the continious-mode
      OP_ModeConstantON();                 //Call the continious-mode
      Mainscreen();                        //Call the mainscreen-function
      if (analogRead(DI_OPERATIONMODE)) {  //If the button is pushed, go to interval-mode
        MAINSTATE = INTERVAL;
      }
      if (Encoder.GetButtonState()) {            //if the Button is pressed...
        tOn_ActivateSettings.start();            //...start the timer
        if (tOn_ActivateSettings.timerOutput) {  //if the time is over
          lcd.clear();                           //clear all the lines
          MAINSTATE = SETTINGS;                  //and go to the settins
        }
      } else {
        tOn_ActivateSettings.reset();  //if the button is released before the time is over, reset the timer
      }
      break;

    case SETTINGS:
      tOn_ActivateSettings.reset();  //Reset the timer
      OP_ModeSettings();             //Call the settings
      break;

    case DONE:
      char buf[LCD_NROFCOLUMNS];                                       //buffer for the lines on the LCD
      LCD_SetUpperLine("Gespeichert!");                                //Set the headline
      sprintf(buf, "Neuer Wert: %i%s", EEPROM.read(ADR_EEPROM), "%");  //Generate the String for the lower line
      LCD_SetLowerLine(buf);                                           //Print the lowerline
      delay(1500);                                                     //Wait for a moment to print out the text
      lcd.clear();                                                     //clear the lines
      MAINSTATE = INIT;                                                //restart the process
      break;
  }
  Control_Humidity();                                                  //Call the function to handle the humidity
  Environmentsensor.handleSensor();                                    //Handle the sensor
  tOn_ActivateSettings.WaitForMilliseconds(WAITTIME_ACIVATESETTINGS);  //Call Instance
}