#include <Adafruit_AHTX0.h>
#include "Class_TimerOn.h" 
class Sensorhandling {

  //Create Instances
  Adafruit_AHTX0 Sensor;          //Create Sensor-Instance
  TimerOn tOn_RefreshSensorData;  //Create Timer-Instance

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

    switch (state) {
      case 0:
        tOn_RefreshSensorData.start(); //Start the timer
        if (tOn_RefreshSensorData.timerOutput) { //If the time is over, go into the next state
          state = 1;
        }
        break;

      case 1:
        tOn_RefreshSensorData.reset(); //Reset the timer and...
        Sensor.getEvent(&humidity, &temperature);  //...refresh the sensordata
        state = 0; //back to idle
        break;
    }

    tOn_RefreshSensorData.WaitForMilliseconds(_refreshtime); //Call the instance
  }

private:
  sensors_event_t humidity, temperature;  //create two specific datatyps for the humidity and temperature
  unsigned long _refreshtime = 500;       //Timeintervall to refresh the sensordata -- default value for startup
  int state = 0;
};