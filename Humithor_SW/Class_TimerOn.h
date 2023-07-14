class TimerOn {
public:
  bool WaitForMilliseconds(unsigned long long delayTime, bool enable) { //Setup and start the timer to wait for a certain amound of milliseconds - the enable-variable needs to be true the whole time
    this->delayTime = delayTime;
    this->enable = enable;
    return TimeHandler();
  }

private:
  unsigned long previousTimeValue = 0;
  unsigned long long delayTime = 0;
  bool enable = false;

  bool TimeHandler() {
    if (enable) { //If the timer is enabled...
      if (micros() - previousTimeValue >= delayTime * 1000) { //...to the "blink without delay"-thing
        previousTimeValue = micros();
        return true; //and return true if the time is over
      } else {
        return false; //else return false
      }
    } else {
      previousTimeValue = micros(); //if enable is false, reset the timer
      return false;
    }
  }
};