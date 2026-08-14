#include "Adeept_Buzzer_For_Arduino.h"

#define G4 392
#define A4 440
#define B4 494
#define C5 523
#define D5 587
#define F5 698


int tune[] = {
  G4, G4, A4, G4, C5, B4,
  G4, G4, A4, G4, D5, C5,
  G4, G4, C5, B4, C5, B4, A4,
  F5, F5, B4, C5, D5, C5
};
float durt[] = {
  0.3, 0.3, 0.3, 0.3, 0.3, 0.6,
  0.3, 0.3, 0.3, 0.3, 0.3, 0.6,
  0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.6,
  0.3, 0.3, 0.3, 0.3, 0.3, 0.6
};
const int buzzerPin = 3;

AdeeptBuzzer buzzer(buzzerPin);
int length = sizeof(tune) / sizeof(tune[0]);
void setup() {

}

void loop() {

  for (int i = 0; i < length; i++) {
    int duration = durt[i] * 1000;
    buzzer.playTone(tune[i], duration);
    delay(10); 
  }
  delay(2000); 
}