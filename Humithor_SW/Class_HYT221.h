#include <Wire.h>

class HYT_221_Environmentsensor {

public:

  int I2Cadress;

  void Init(int Sensoradress) {
    Wire.begin();
    this->I2Cadress = Sensoradress;
  }

  double getHumidity() {
    this->ReadSensorData();  //Read the sensordata
    return this->humidity;   //return the sensordata
  }

  double getTemperature() {
    this->ReadSensorData();    //read the sensordata
    return this->temperature;  //return the sensordata
  }

private:

  double temperature;
  double humidity;

  void ReadSensorData() {

    Wire.beginTransmission(I2Cadress);  //Start reading from the given adress
    Wire.requestFrom(I2Cadress, 4);     //Get 4 Bytes from the Sensor

    if (Wire.available() == 4) {  //If Sensor is reachable, read the data

      int b1 = Wire.read();  //Read and save the 4 Bytes into the memory
      int b2 = Wire.read();
      int b3 = Wire.read();
      int b4 = Wire.read();

      Wire.endTransmission();  //End up the connection

      int rawHumidity = b1 << 8 | b2;               //Combine Byte1 and Byte2 to a rawvalue by shifting b1 by 8
      rawHumidity = (rawHumidity &= 0x3FFF);        //Set all bits over the place 13 to 0
      humidity = 100.0 / pow(2, 14) * rawHumidity;  //Calculate the actual humidity-value

      b4 = (b4 << 2);                                          //Shift the place of b4 by 2.
      int rawTemperature = b3 << 6 | b4;                       //Combine Byte3 and Byte4 to a rawvalue
      temperature = 165.0 / pow(2, 14) * rawTemperature - 40;  //Calculate the actual temperaturevalue
    }
  }
};