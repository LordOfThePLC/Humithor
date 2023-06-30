

class Control_Humidity {

public:
  void Init() {
  
  }

void SetHumidityLimit(int LowerLimitValue){



}

private:
  void Controller() {
    if (Environmentsensor.getHumidity() <= Humidity_UpperLimit) {
      Fan_Humidifier.SwitchOn();
    } else if (Environmentsensor.getHumidity() >= (Humidity_UpperLimit + 3)) {
      Fan_Humidifier.SwitchOff();
    }
  }
};