// Adeept_Buzzer_For_Arduino.h
#ifndef ADEEPT_BUZZER_FOR_ARDUINO_H
#define ADEEPT_BUZZER_FOR_ARDUINO_H

class AdeeptBuzzer {
  public:
    AdeeptBuzzer(int pin);
    void playTone(int frequency, int duration);
  private:
    int buzzerPin;
};

#endif