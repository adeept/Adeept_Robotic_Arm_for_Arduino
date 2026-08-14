#include <Servo.h>
#include <Wire.h>
#include <SSD1306Ascii.h>
#include <SSD1306AsciiWire.h>

#define OLED_I2C_ADDR 0x3C
SSD1306AsciiWire oled;
char oledBuf[10];
unsigned long tOLED;

#define PIN_BUZZER 3                
#define BUZZER_FREQUENCY 4000  
#define SERVO_COUNT 3

Servo servo[SERVO_COUNT];
int servoPin[SERVO_COUNT] = {10, 9, 8};
int currentAngle[SERVO_COUNT] = {90, 90, 90};
int targetAngle[SERVO_COUNT] = {90, 135, 45};
int lastServoAngle[SERVO_COUNT] ={-1,-1,-1};

const float L1=110.0f, L2=70.0f, L3=140.0f, L4=100.0f;
float x=0,y=0,z=0, last_x=-1, last_y=-1;

void setup(){
  Serial.begin(115200);

  Serial.println("AT+CWMODE=3\r\n");//set to softAP+station mode
  delay(3000);     //delay 3s  
  Serial.println("AT+CWSAP=\"Adeept_ADA031\",\"12345678\",8,2\r\n");   //TCP Protocol, server IP addr, port
  delay(1000);     //delay 1s
  Serial.println("AT+RST\r\n");     //reset wifi
  delay(1000);     //delay 1s
  Serial.println("AT+CIPMUX=1\r\n");//set to multi-connection mode
  delay(1000);
  Serial.println("AT+CIPSERVER=1,4000\r\n");//set as server
  delay(1000);
  Serial.println("AT+CIPSTO=7000\r\n");//keep the wifi connecting 7000 seconds
  delay(1000);

  for(byte i=0;i<SERVO_COUNT;i++){
    servo[i].attach(servoPin[i]);
  }
  updateServo();
  
  oledInit();
  oledUpdate();   

  Buzzer_Setup();             
  Buzzer_Alert(2, 1);
}


void loop(){
  if (Serial.available()) {
    String str = Serial.readStringUntil('\n');
    if (parseXYZ(str, x, y, z)) {
      inverseKinematics(x,y,z,targetAngle[0],targetAngle[1],targetAngle[2]);      
    }
  } 
  updateServo();
  oledUpdate();  
  delay(10);
}

bool parseXYZ(String str, float &x, float &y, float &z){
    str.trim();
    int pos = str.indexOf(':');
    if (pos != -1) str = str.substring(pos + 1);
    int i1 = str.indexOf(',');
    int i2 = str.indexOf(',', i1 + 1);
    if (i1 == -1 || i2 == -1) return false;

    x = str.substring(0, i1).toFloat();
    y = str.substring(i1 + 1, i2).toFloat();
    z = str.substring(i2 + 1).toFloat();
    return true;
}

void updateServo(){
  for(int i=0;i<SERVO_COUNT;i++){
      if(currentAngle[i]<targetAngle[i]) currentAngle[i]++;
      else if(currentAngle[i]>targetAngle[i]) currentAngle[i]--;
      servo[i].write(currentAngle[i]);
  }
}

void Buzzer_Setup(){
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW); 
}

void softTone(int durationMs){
  unsigned long start = millis();
  int delayUs = 500000 / BUZZER_FREQUENCY; 
  while (millis() - start < durationMs) {
    digitalWrite(PIN_BUZZER, HIGH);
    delayMicroseconds(delayUs);
    digitalWrite(PIN_BUZZER, LOW);
    delayMicroseconds(delayUs);
  }
}

void Buzzer_Alert(int beat, int rebeat){
  beat = constrain(beat, 1, 9);
  rebeat = constrain(rebeat, 1, 255);
  for (int j = 0; j < rebeat; j++) {
    for (int i = 0; i < beat; i++) {
      softTone(100);   
      delay(100);
    }
    delay(500);
  }
}

void oledInit(){
  Wire.begin();

  oled.begin(&Adafruit128x64, OLED_I2C_ADDR);
  oled.setFont(Adafruit5x7);
  oled.clear();
  oled.set1X();

  oled.setCursor(10,0);
  oled.print("Adeept Robotic Arm");

  oled.setCursor(10,3);
  oled.print("S6:");
  oled.setCursor(10,5);
  oled.print("S5:");
  oled.setCursor(10,7);
  oled.print("S4:");

  oled.setCursor(75,3);
  oled.print("x:");
  oled.setCursor(75,5);
  oled.print("y:");
}

void oledPrintNum(int x,int y,int num){
  oled.set1X();
  oled.setCursor(x,y);
  oled.print("      ");
  oled.setCursor(x,y);
  oled.print(itoa(num,oledBuf,10));
}

void oledUpdate(){
  if(millis()-tOLED<100) return; 
  tOLED=millis();

  if(lastServoAngle[0]!=currentAngle[0]){
    oledPrintNum(30,3,currentAngle[0]);
    lastServoAngle[0]=currentAngle[0];
  }
  if(lastServoAngle[1]!=currentAngle[1]){
    oledPrintNum(30,5,currentAngle[1]);
    lastServoAngle[1]=currentAngle[1];
  }
  if(lastServoAngle[2]!=currentAngle[2]){
    oledPrintNum(30,7,currentAngle[2]);
    lastServoAngle[2]=currentAngle[2];
  }
  if(last_x != x || last_y !=y){
    oledPrintNum(90,3,x);
    oledPrintNum(90,5,y);
    last_x = x;
    last_y = y;
  }
}

bool inverseKinematics(float x,float y,float z,int &servo1,int &servo2,int &servo3){
  float theta1 = atan2(y,x);
  float px = sqrt(x*x+y*y);
  float pz = z - L1;
  float D = sqrt(px*px+pz*pz);
  if(D > L2+L3 || D < fabs(L2-L3)) return false;

  float beta = atan2(pz,px);
  float cos_gamma = (L2*L2 + D*D - L3*L3) / (2*L2*D);
  cos_gamma = constrain(cos_gamma,-1.0f,1.0f);
  float gamma = acos(cos_gamma);
  // elbow up
  float theta2 = PI - gamma - beta;
  float cos_phi = (D*D+L3*L3-L2*L2) / (2*D*L3);
  cos_phi = constrain(cos_phi,-1.0f,1.0f);
  float theta3 = theta2 - beta + acos(cos_phi);
  if((90-degrees(theta1)<0 || 90-degrees(theta1)>180) || (degrees(theta2) + 15<0 || degrees(theta2)+15>180) || (degrees(theta3)-90<0 || degrees(theta3)-90>180)) return false;
  servo1 = 90-degrees(theta1);
  servo2 = degrees(theta2) + 15;
  servo3 = degrees(theta3)-90;
  return true;
}
