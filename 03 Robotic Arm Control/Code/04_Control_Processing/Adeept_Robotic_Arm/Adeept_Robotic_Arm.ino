#include <Servo.h>
#include <Wire.h>
#include <SSD1306Ascii.h>
#include <SSD1306AsciiWire.h>

#define OLED_I2C_ADDR 0x3C
SSD1306AsciiWire oled;
char oledBuf[10];
#define PIN_BUZZER 3                
#define BUZZER_FREQUENCY 4000   
#define SERVO_NUM 5

const int servoPins[SERVO_NUM] = {10, 9, 8, 7, 6};
int step = 5;
int servoAngle[SERVO_NUM] = {90,90,90,90,90};
int servoTarget[SERVO_NUM] = {90,90,90,90,90};
Servo servo[SERVO_NUM];
int lastServoAngle[SERVO_NUM] ={-1,-1,-1,-1,-1};
unsigned long tOLED;

void moveServo(int index,int dir){
  servoTarget[index]+=dir*step;
  servoTarget[index]=constrain(servoTarget[index],0,180);
  if(index == 4){
    servoTarget[index]=constrain(servoTarget[index],90,180);
  }
}

void updateServo(){
  for(int i=0;i<SERVO_NUM;i++){
      if(servoAngle[i]<servoTarget[i]) servoAngle[i]++;
      else if(servoAngle[i]>servoTarget[i]) servoAngle[i]--;
      servo[i].write(servoAngle[i]);
  }
}

void setup()
{
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

  for(int i=0;i<SERVO_NUM;i++)
  {
    servo[i].attach(servoPins[i]);
    servo[i].write(servoAngle[i]);
  }

  oledInit();
  oledUpdate();   

  Buzzer_Setup();                //Buzzer initialization
  Buzzer_Alert(2, 1);
}


void handleCommand(char cmd)
{
  switch(cmd)
  {
    case 'o':
    case 'O':
      moveServo(0,+1);
      break;
    case 'p':
    case 'P':
      moveServo(0,-1);
      break;

    case 'u':
    case 'U':
      moveServo(1,+1);
      break;
    case 'i':
    case 'I':
      moveServo(1,-1);
      break;

    case 't':
    case 'T':
      moveServo(2,+1);
      break;
    case 'y':
    case 'Y':
      moveServo(2,-1);
      break;

    case 'e':
    case 'E':
      moveServo(3,+1);
      break;
    case 'r':
    case 'R':
      moveServo(3,-1);
      break;

    case 'q':
    case 'Q':
      moveServo(4,+1);
      break;
    case 'w':
    case 'W':
      moveServo(4,-1);
      break;

  }
}


void loop()
{
  while(Serial.available()){
    char c = Serial.read();
    if(c==':'){
        while(!Serial.available());
        char cmd=Serial.read();
        handleCommand(cmd);
    }
  }
  updateServo();
  oledUpdate();   
  delay(10);
}


void Buzzer_Setup(){
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW); 
}

void softTone(int durationMs){
  unsigned long start = millis();
  int delayUs = 500000 / BUZZER_FREQUENCY; // 半周期
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
  oled.setCursor(75,3);
  oled.print("S5:");

  oled.setCursor(10,5);
  oled.print("S4:");
  oled.setCursor(75,5);
  oled.print("S3:");

  oled.setCursor(10,7);
  oled.print("S2:");
}

void oledPrintNum(int x,int y,int num){
  oled.set1X();
  oled.setCursor(x,y);
  oled.print("   ");
  oled.setCursor(x,y);
  oled.print(itoa(num,oledBuf,10));
}

void oledUpdate(){
  if(millis()-tOLED<100)return; 
  tOLED=millis();
  if(lastServoAngle[0]!=servoAngle[0])
  {
    oledPrintNum(30,3,servoAngle[0]);
    lastServoAngle[0]=servoAngle[0];
  }

  if(lastServoAngle[1]!=servoAngle[1])
  {
    oledPrintNum(95,3,servoAngle[1]);
    lastServoAngle[1]=servoAngle[1];
  }

  if(lastServoAngle[2]!=servoAngle[2])
  {
    oledPrintNum(30,5,servoAngle[2]);
    lastServoAngle[2]=servoAngle[2];
  }

  if(lastServoAngle[3]!=servoAngle[3])
  {
    oledPrintNum(95,5,servoAngle[3]);
    lastServoAngle[3]=servoAngle[3];
  }

  if(lastServoAngle[4]!=servoAngle[4])
  {
    oledPrintNum(30,7,servoAngle[4]);
    lastServoAngle[4]=servoAngle[4];
  }
}