/***********************************************************
Description: Potentiometer controls servo angle with filter & stable OLED
Website: www.adeept.com
E-mail: support@adeept.com
***********************************************************/
#include <Wire.h>
#include <SSD1306AsciiWire.h>
#include <Servo.h>

#define OLED_I2C_ADDR   0x3C
#define FILTER_WINDOW   5      
#define SERVO_TOTAL     6      
#define ADC_CH_TOTAL    10     
#define PIN_BATTERY     A7
#define PIN_BUZZER 3                
#define BUZZER_FREQUENCY 4000   

#define POT5_MIN_ANGLE  80
#define POT5_MAX_ANGLE  180
// The two parameters POT5_MIN_ANGLE and POT5_MAX_ANGLE are used to adjust the movement range of the gripper. The installation neutral angle is 90 degrees, and a 10-degree margin is reserved here to prevent the gripper from failing to close completely.
const int servoPins[SERVO_TOTAL] = {10, 9, 8, 7, 6, 5};
const int adcPins[ADC_CH_TOTAL] = {A0, A1, A2, A3, A6};

const float ADC_REF_VOLTAGE    = 5.1;
const float VOLTAGE_DIV_RATIO  = 2.0;
const float BAT_MIN_VOLTAGE    = 6.0;
const float BAT_MAX_VOLTAGE    = 8.4;
float batteryVoltage = 0.0;
unsigned long oledTime;

SSD1306AsciiWire oled;

Servo servos[SERVO_TOTAL];
int adcFilterBuf[ADC_CH_TOTAL][FILTER_WINDOW];
int adcFilterIndex[ADC_CH_TOTAL] = {0};
long adcFilterSum[ADC_CH_TOTAL] = {0};
int servoTarget[SERVO_TOTAL];
int lastOledVal[ADC_CH_TOTAL] = {-1, -1, -1, -1, -1};

void adcFilterInit() {
  for (int ch = 0; ch < ADC_CH_TOTAL; ch++) {
    adcFilterSum[ch] = 512L * FILTER_WINDOW;
    for (int i = 0; i < FILTER_WINDOW; i++) {
      adcFilterBuf[ch][i] = 512;
    }
  }
}

int adcFilter(int channel, int rawVal) {
  adcFilterSum[channel] -= adcFilterBuf[channel][adcFilterIndex[channel]];
  adcFilterBuf[channel][adcFilterIndex[channel]] = rawVal;
  adcFilterSum[channel] += rawVal;
  adcFilterIndex[channel] = (adcFilterIndex[channel] + 1) % FILTER_WINDOW;
  return (int)(adcFilterSum[channel] / FILTER_WINDOW);
}

void oledInit() {
  Wire.begin();
  oled.begin(&Adafruit128x64, OLED_I2C_ADDR);
  oled.setFont(Adafruit5x7);
  oled.clear();
   
  oledPrintStr(5,0,"Battery Percent:");
  oledPrintStr(0, 2, "A0:");
  oledPrintStr(65, 2, "A1:");
  oledPrintStr(0, 4, "A2:");
  oledPrintStr(65, 4, "A3:");
  oledPrintStr(0, 6, "A6:");
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

void servoInit() {
  for (int i = 0; i < SERVO_TOTAL; i++) {
    servos[i].attach(servoPins[i]);
  }
}

void setup() {
  adcFilterInit();
  servoInit();
  oledInit();
  Buzzer_Setup();
  Buzzer_Alert(2,1);
}

void loop() {
  int filteredAdc[ADC_CH_TOTAL];
  for (int ch = 0; ch < ADC_CH_TOTAL; ch++) {
    int raw = analogRead(adcPins[ch]);
    filteredAdc[ch] = adcFilter(ch, raw);
  }

  for (int i = 0; i < 4; i++) {
    servoTarget[i] = map(filteredAdc[i], 0, 1023, 0, 180);
  }
  // Potentiometer A6: It limits the angle control range of the gripper and synchronously controls Servos 5 and 6 to prevent the servos from being damaged due to stall.
  int angle5 = map(filteredAdc[4], 0, 1023, POT5_MIN_ANGLE, POT5_MAX_ANGLE);
  servoTarget[4] = angle5;
  servoTarget[5] = angle5;

  for (int i = 0; i < SERVO_TOTAL; i++) {
    servos[i].write(servoTarget[i]);
    delay(5);
  }

  OLED_display();
}

void OLED_display(){
  if(millis()-oledTime<1000) return;
  oledTime=millis();
  if (servoTarget[0] != lastOledVal[0]) {
    oledPrintNum(20, 2, servoTarget[0]);
    lastOledVal[0] = servoTarget[0];
  }
  if (servoTarget[1] != lastOledVal[1]) {
    oledPrintNum(85, 2, servoTarget[1]);
    lastOledVal[1] = servoTarget[1];
  }
  if (servoTarget[2] != lastOledVal[2]) {
    oledPrintNum(20, 4, servoTarget[2]);
    lastOledVal[2] = servoTarget[2];
  }
  if (servoTarget[3] != lastOledVal[3]) {
    oledPrintNum(85, 4, servoTarget[3]);
    lastOledVal[3] = servoTarget[3];
  }
  if (servoTarget[4] != lastOledVal[4]) {
    oledPrintNum(20, 6, servoTarget[4]);
    lastOledVal[4] = servoTarget[4];
  }

  int batRatio = Get_Battery_Ratio();
  oledPrintNum(100, 0, batRatio);
  oledPrintStr(115, 0, "%");
}


//////////////////////Buzzer drive area///////////////////////////////////
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