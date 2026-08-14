/***********************************************************
Website: www.adeept.com
E-mail: support@adeept.com
***********************************************************/
#include <EEPROM.h>
#include <Servo.h>
#include <Wire.h>
#include <SSD1306AsciiWire.h>

#define SERVO_NUM 5
#define PIN_BUZZER 3                
#define BUZZER_FREQUENCY 4000   
#define OLED_I2C_ADDR   0x3C
#define PIN_BATTERY     A7

SSD1306AsciiWire oled;
const float ADC_REF_VOLTAGE    = 5.1;
const float VOLTAGE_DIV_RATIO  = 2.0;
const float BAT_MIN_VOLTAGE    = 6.0;
const float BAT_MAX_VOLTAGE    = 8.4;
float batteryVoltage = 0.0;

Servo servos[SERVO_NUM];
const byte servoPins[SERVO_NUM]={10,9,8,7,6};
const byte adcPins[SERVO_NUM]={A0,A1,A2,A3,A6};
int servoCurrent[SERVO_NUM]={90,90,90,90,90};
int servoTarget[SERVO_NUM]={90,90,90,90,90};
const byte buttonPin=4;
int step=10,total,addr=0,address=0;
unsigned long oledTime;

void setup(){
  Serial.begin(115200);
  for(int i=0;i<SERVO_NUM;i++) servos[i].attach(servoPins[i]);
  pinMode(buttonPin,INPUT);
  OLED_Setup();
  servoMove();
  total=step*SERVO_NUM;
  Buzzer_Setup();
  Buzzer_Alert(2,1);
}

void loop(){
  int timer=0;
  if(step){
    for(int i=0;i<SERVO_NUM;i++)
      servoTarget[i]=(i==SERVO_NUM-1)?map(analogRead(adcPins[i]),0,1023,80,180):map(analogRead(adcPins[i]),0,1023,0,180);
  }

  servoMove();
  
  if(digitalRead(buttonPin)==LOW){
    while(digitalRead(buttonPin)==LOW){
      delay(50);
      if(timer<50) timer++;
    }

    if(timer==50){
      step=0;
      Buzzer_Alert(2,2);
    }else if(step){
      step--;
      for(int i=0;i<SERVO_NUM;i++) EEPROM.update(addr+i,servoTarget[i]);
      addr+=SERVO_NUM;
      Buzzer_Alert(1,1);
    }
  }

  if(step==0){
    for(int i=0;i<SERVO_NUM;i++) servoTarget[i]=EEPROM.read(address+i);
    address+=SERVO_NUM;
    if(address>=total) address=0;
  }

  OLED_display(step);
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

void servoMove(){
  bool moving;
  do{
    moving=false;
    for(int i=0;i<SERVO_NUM;i++){
      if(servoCurrent[i]<servoTarget[i]){
        servoCurrent[i]++;
        moving=true;
      }else if(servoCurrent[i]>servoTarget[i]){
        servoCurrent[i]--;
        moving=true;
      }
      servos[i].write(servoCurrent[i]);
    }
    if(moving) delay(10);
  }while(moving);
}

void OLED_display(int step){
  if(millis()-oledTime<1000) return;
  oledTime=millis();

  if(step==0){
    oledPrintStr(60,3,"Working");
    oledPrintStr(0,4,"                      ");
  }else{
    oledPrintStr(60,3,"Recording");
    oledPrintStr(0,4,"Remaining Steps:");
    oledPrintNum(100,4,step);
  }
  for(int i=0;i<SERVO_NUM;i++) oledPrintNum(5+i*25,7,servoTarget[i]);

  int batRatio = Get_Battery_Ratio();
  oledPrintNum(100, 1, batRatio);
  oledPrintStr(115, 1, "%");
}


void OLED_Setup(){
  Wire.begin();
  oled.begin(&Adafruit128x64,OLED_I2C_ADDR);
  oled.setFont(Adafruit5x7);
  oled.clear();

  oledPrintStr(15,0,"Record Actions");
  oledPrintStr(0,1,"Battery Percent:");
  oledPrintStr(0,3,"CurState:");
  oledPrintStr(0,6,"(S6)(S5)(S4)(S3)(S2)");
}

void oledPrintNum(int x,int y,int num){
  oled.set1X();
  oled.setCursor(x,y);
  oled.print(F("     "));
  oled.setCursor(x,y);
  oled.print(num);
}

void oledPrintStr(int x,int y,const char *text){
  oled.set1X();
  oled.setCursor(x,y);
  oled.print(F("                    "));
  oled.setCursor(x,y);
  oled.print(text);
}

int Get_Battery_Voltage_ADC(){
  int adcValue = 0;
  for (int i = 0; i < 5; i++){
    adcValue += analogRead(PIN_BATTERY);
    delayMicroseconds(200);
  }
  return adcValue / 5;
}

float Get_Battery_Voltage(){
  int adcValue = Get_Battery_Voltage_ADC();
  batteryVoltage = ((float)adcValue / 1023.0) * ADC_REF_VOLTAGE * VOLTAGE_DIV_RATIO;
  batteryVoltage = constrain(batteryVoltage, 0, BAT_MAX_VOLTAGE);
  return batteryVoltage;
}

int Get_Battery_Ratio(){
  float voltage = Get_Battery_Voltage();
  int ratio = map(voltage * 100, BAT_MIN_VOLTAGE * 100, BAT_MAX_VOLTAGE * 100, 0, 100);
  return constrain(ratio, 0, 100);
}