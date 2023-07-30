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
  void Init(int pinA, int pinB, int pinButton,long start, long lowerLimit, long upperLimit) { //Init and startup the encoder
    this->pinA = pinA; //Handover all the parameters
    this->pinB = pinB;
    this->pinButton = pinButton;
    this->lowerLimit = lowerLimit;
    this->upperLimit = upperLimit;

    //Declare and set the pins as Inputs
    pinMode(pinButton, INPUT);
    pinMode(pinA, INPUT);
    pinMode(pinB, INPUT);

    encoder = new Encoder(pinA, pinB); //create a new instance of the encoder-library

    encoder->write(start*3); //set the startvalue of the encoder-position for the first time
  }

  void preloadEncoder(long preload_value) { //set the startvalue of the encoder manually
    encoder->write(preload_value*3);
  }

  long GetEncoderPosition() { //Returns the actual encoderposition
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
  bool GetButtonState() { //Returns the actual state if the button is pressed or not
    return !digitalRead(pinButton);
  }
};
