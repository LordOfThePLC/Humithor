class Motor {

private:
  int Motorpin;

public:

  void Init(int Pinnumber) {       //Inits the Motor-Class
    pinMode(Pinnumber, OUTPUT);    //Set Motorpin as Output
    digitalWrite(Pinnumber, LOW);  //Switch Off the Pin
    this->Motorpin = Pinnumber;    //Safe the pinnumber to the private memory
  }

  void SwitchOn() {
    digitalWrite(this->Motorpin, HIGH);  //Set Motorpin HIGH
  }

  void SwitchOff() {
    digitalWrite(this->Motorpin, LOW);  //Set Motorpin LOW
  }
};