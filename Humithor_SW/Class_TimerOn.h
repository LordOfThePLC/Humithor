class TimerOn {
public:
  void WaitForMilliseconds(unsigned long long delayTime) {  //Setup and start the timer to wait for a certain amound of milliseconds - the enable-variable needs to be true the whole time
    this->delayTime = delayTime; 
    this->enable = enable;
    this->TimeHandler();
  }

  void reset() { //Resets the timer
    enable = false;
    timerOutput = false;
  }

  void start() { //Starts the timer
    enable = true;
  }

  bool timerOutput; //Output of the timer, gets high when the timer is over

private:
  unsigned long previousTimeValue = 0;
  unsigned long long delayTime = 0;
  bool enable = false;

  bool TimeHandler() {
    if (enable) {                                              //If the timer is enabled...
      if (micros() - previousTimeValue >= delayTime * 1000) {  //...do the "blink without delay"-thing
        previousTimeValue = micros();
        timerOutput = true; //set the output high
      }
    } else {
      previousTimeValue = micros();  //if enable is false, reset the timer
    }
  }
};
