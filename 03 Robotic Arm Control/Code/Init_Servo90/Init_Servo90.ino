/***********************************************************
Description: Power on, the servo rotates 90 to straighten the robotic arm
Website: www.adeept.com
E-mail: support@adeept.com
***********************************************************/
#include <Servo.h>。
int servopin1 = 5;  
int servopin2 = 6;  
int servopin3 = 7;  
int servopin4 = 8;  
int servopin5 = 9;  
int servopin6 = 10; 

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;
Servo servo6;
int angle = 90;       

void setup() {
  servo1.attach(servopin1);
  servo2.attach(servopin2);
  servo3.attach(servopin3);
  servo4.attach(servopin4);
  servo5.attach(servopin5);
  servo6.attach(servopin6);

  servo1.write(angle);
  servo2.write(angle);
  servo3.write(angle);
  servo4.write(angle);
  servo5.write(angle); 
  servo6.write(angle); 
  delay(20);
}

void loop()
{

}
