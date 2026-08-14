#include "Adeept_Buzzer_For_Arduino.h"
#include <Arduino.h>

AdeeptBuzzer::AdeeptBuzzer(int pin) {
  buzzerPin = pin;
  pinMode(buzzerPin, OUTPUT);
}

void AdeeptBuzzer::playTone(int frequency, int duration) {
  tone(buzzerPin, frequency, duration);
  delay(duration);
  noTone(buzzerPin);
}