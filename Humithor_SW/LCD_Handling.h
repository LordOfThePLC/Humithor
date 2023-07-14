#include <avr/interrupt.h>
#include "WString.h"
#include "Arduino.h"
#include <LiquidCrystal.h>

LiquidCrystal lcd(LCD_RESET, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

String upperTextOld = "";
String lowerTextOld = "";

void InitLCD(int nrofcolumns, int nrofrows) {
  lcd.begin(nrofcolumns, nrofrows);  //Init and start the LCD
  pinMode(LCD_BGL, OUTPUT);          //Set the Backgroundlight-pin as an output
  digitalWrite(LCD_BGL, HIGH);       //Switch on the Backgroundlight
}

void LCD_SetUpperLine(String text) {  //Prints the upper line of the lcd
  lcd.setCursor(0, 0);                //Set the cursor to the upper right spot
  lcd.print(text);                    //print the new text
}

void LCD_SetLowerLine(String text) {  //Prints the lower line of the lcd
  lcd.setCursor(0, 1);                //Set the cursor to the lower right spot
  lcd.print(text);                    //print the new text
}

void LCD_SwitchLightOff() {  //Switch the backgroundlight off
  digitalWrite(LCD_BGL, LOW);
}

void LCD_SwitchLightOn() {  //Switch the backgroundlight on
  digitalWrite(LCD_BGL, HIGH);
}

bool LCD_Blink(int xtimes, int duration) { //Let the backgroundlight blink multiple times for a certain duration
  static unsigned long previousMillis = 0;
  static int ledState = LOW;
  static int count = 0;

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= duration) { //to the known "Blink without delay" stuff
    previousMillis = currentMillis;
    if (ledState == LOW) {
      ledState = HIGH;
      count++;
    } else {
      ledState = LOW;
    }

    digitalWrite(LCD_BGL, ledState);  //Write the actual state to the LED
  }

  if (count > xtimes) { //If the number of blinks is reached...
    count = 0; //Reset the counter
    digitalWrite(LCD_BGL, HIGH);  //Set the LED to default-high
    return true; //return the "finished"-flag
  }
  return false;
}