#include <Adafruit_AHTX0.h>
#include "Class_TimerOn.h"  //Headerfile is included here, so it's also included in the Main-Script
class Sensorhandling {

  //Create Instances
  Adafruit_AHTX0 Sensor;    //Create Sensor-Instance
  TimerOn tOn_Refreshtime;  //Create Timerinstance for refreshing the Sensordata

public:

  bool Init(unsigned long refreshtime) {
    this->_refreshtime = refreshtime;  //Starts up the Sensor
    if (!Sensor.begin()) {             //If the Sensor doesn't responds,
      return false;                    //return false
    } else {
      return true;  //else return true
    }
  }

  double getHumidity() {  //returns the humidity of the sensor
    return this->humidity.relative_humidity;
  }

  double getTemperature() {  //returns the temperature of the sensor
    return this->temperature.temperature;
  }
  void handleSensor() {

    if (tOn_Refreshtime.WaitForMilliseconds(_refreshtime, true)) {  //when the time is over
      Sensor.getEvent(&humidity, &temperature);                     //...refresh the sensordata
    }
  }

private:
  sensors_event_t humidity, temperature;  //create two specific datatyps for the humidity and temperature
  unsigned long _refreshtime = 500;       //Timeintervall to refresh the sensordata -- default value for startup
};