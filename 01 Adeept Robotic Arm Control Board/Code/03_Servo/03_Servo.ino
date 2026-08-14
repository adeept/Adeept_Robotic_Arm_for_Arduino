/***********************************************************
 Website: www.adeept.com
 E-mail: support@adeept.com
***********************************************************/
#include <Servo.h>
#define SERVO_NUM 6
Servo servos[SERVO_NUM];
uint8_t servoPins[SERVO_NUM] = {5, 6, 7, 8, 9, 10};

void setup() {
  for (int i = 0; i < SERVO_NUM; i++) {
    servos[i].attach(servoPins[i]);
    servos[i].write(90);        
  }
  delay(1000);
}

void loop() {
  servoMoveAll(0);
  delay(2000);
  servoMoveAll(180);
  delay(2000);
}

void servoMoveAll(int angle) {
  for (int i = 0; i < SERVO_NUM; i++) {
    servos[i].write(angle);
    delay(10);  
  }
}