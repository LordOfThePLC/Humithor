#include "Arduino.h"
#include <Encoder.h>

class Rotaryencoder {
private:
  int pinA;
  int pinB;
  int pinButton;
  int lowerLimit;
  int upperLimit;
  Encoder* encoder;
  byte oldPosition = -128;
  byte newPosition;

public:
  void Init(int pinA, int pinB, int pinButton, int ppr, long start, long lowerLimit, long upperLimit) {
    this->pinA = pinA;
    this->pinB = pinB;
    this->pinButton = pinButton;
    this->lowerLimit = lowerLimit;
    this->upperLimit = upperLimit;

    pinMode(pinButton, INPUT);
    pinMode(pinA, INPUT);
    pinMode(pinB, INPUT);

    encoder = new Encoder(pinA, pinB);

    encoder->write(start);
  }

  void preloadEncoder(long preload_value) {
    encoder->write(preload_value);
  }

  long GetEncoderPosition() {
    newPosition = (byte)encoder->read() / 3;
    if (newPosition != oldPosition) {

      oldPosition = newPosition;
      if (newPosition > this->upperLimit) {
        newPosition = this->upperLimit;
        encoder->write(this->upperLimit * 3);
      } else if (newPosition < this->lowerLimit) {
        newPosition = this->lowerLimit;
        encoder->write(this->lowerLimit * 3);
      }
      return newPosition;
    }
  }
  bool GetButtonState() {
    return !digitalRead(pinButton);
  }
};
