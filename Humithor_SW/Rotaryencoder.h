#include "Arduino.h"

class Rotaryencoder {
private:
  int pinA;
  int pinB;
  int pinButton;



public:
  void Init(int pinA, int pinB, int pinButton, int ppr, long start, long min, long max) {
    this->pinA = pinA;
    this->pinB = pinB;
    this->pinButton = pinButton;

    pinMode(pinButton, INPUT);
    pinMode(pinA, INPUT);
    pinMode(pinB, INPUT);

  }

  bool GetStatePinA(){
    return !digitalRead(pinA);
  }

  bool GetButtonState() {
    return !digitalRead(pinButton);
  }
};
