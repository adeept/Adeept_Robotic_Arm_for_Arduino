#include "Adeept_Buzzer_For_Arduino.h"


const int buzzerPin = 3;
AdeeptBuzzer buzzer(buzzerPin);


const int tones[] = {262, 294, 330, 349, 392, 440, 494}; // C4, D4, E4, F4, G4, A4, B4
const int duration = 500; 

void setup() {

}

void loop() {

  for (int i = 0; i < 7; i++) {
    buzzer.playTone(tones[i], duration);
    delay(100); 
  }
  delay(2000); 
}