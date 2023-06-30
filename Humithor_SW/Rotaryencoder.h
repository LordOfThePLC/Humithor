class Rotaryencoder {
private:
  int pinA;
  int pinB;
  int pinButton;
  int previousStateA;
  int previousStateB;
  int direction;

public:
  void Init(int pinA, int pinB, int pinButton) {
    this->pinA = pinA;
    this->pinB = pinB;
    this->pinButton = pinButton;

    pinMode(pinA, INPUT);
    pinMode(pinB, INPUT);
    pinMode(pinButton, INPUT_PULLUP);

    previousStateA = digitalRead(pinA);
    previousStateB = digitalRead(pinB);
    direction = 0;
  }

  int GetDirection() {
    int currentStateA = digitalRead(pinA);
    int currentStateB = digitalRead(pinB);

    if (currentStateA != previousStateA) {
      if (currentStateB == currentStateA) {
        direction = 1;
      } else {
        direction = -1;
      }
    }

    previousStateA = currentStateA;
    previousStateB = currentStateB;

    return direction;
  }

  bool GetButtonState() {
    return !digitalRead(pinButton);  // Der Zustand des Buttons wird negiert zurückgegeben, um die Logik zu invertieren (gedrückt = true, nicht gedrückt = false)
  }
};
