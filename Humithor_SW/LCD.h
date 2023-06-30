#include <LiquidCrystal.h>

class LCD {

private:
  LiquidCrystal lcd;  //Create instance of the LCD
  int pin_rs;         //RS-Pin
  int pin_en;         //Enablepin
  int pin_d4;         //Datapin 4
  int pin_d5;         //Datapin 5
  int pin_d6;         //Datapin 6
  int pin_d7;         //Datapin 7

public:
  LCD() {
    //Set the pins on a default value
    pin_rs = 12;
    enable = 11;
    d4 = 4;
    d5 = 5;
    d6 = 6;
    d7 = 7;
  }

  void Init(int rs, int enable, int d4, int d5, int d6, int d7, int rows, int columns) {
    this->pin_rs = rs;  //Handover the Pinnumbers to the internal variables
    this->pin_en = enable;
    this->pin_d4 = d4;
    this->pin_d5 = d5;
    this->pin_d6 = d6;
    this->pin_d7 = d7;

    lcd = LiquidCrystal(pin_rs, pin_en, pin_d4, pin_d5, pin_d6, pin_d7);  //Init the Library-Object
    lcd.begin(columns, rows);                                             //Init the LCD with 16 signs and two rows
  }

  void SetUpperLine(const String& text) {  //Function to set the text in the upper line
    lcd.setCursor(0, 0);
    lcd.print(text);
  }

  void SetLowerLine(const String& text) {  //Function to set the text in the lower line
    lcd.setCursor(0, 1);
    lcd.print(text);
  }
};