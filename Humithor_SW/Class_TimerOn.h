class TimerOn {

public:
  bool WaitForMilliseconds(unsigned long delaytime, bool enable) {
    this->_delaytime = delaytime; //hand over the value to compare with
    this->_enable = enable; //hand over the enable-flag
    return TimeHandler(); //call the function and return the value
  }

private:
  unsigned long previousTimeValue = 0; //previous timestamp to compare with
  unsigned long _delaytime = 0; //internal variable to compare with
  bool _enable = false; //internal variable to enable the timer

  bool TimeHandler() {
    if (_enable) { //Handle the time only when it's enabled
      if (micros() - previousTimeValue >= _delaytime * 1000) {  //Compare act an previous time. If the difference equals to the wanted delay-time-->
        previousTimeValue = micros();                           //restore the new time as previous time
        return true;                                            //return a true flag to set an action
      } else {
        return false;  //else return a false flag
      }
    } else { //if it's not enabled...
      previousTimeValue = micros(); //...set the previous timestap to the actual time... 
      return false; //...and return false
    }
  }
};