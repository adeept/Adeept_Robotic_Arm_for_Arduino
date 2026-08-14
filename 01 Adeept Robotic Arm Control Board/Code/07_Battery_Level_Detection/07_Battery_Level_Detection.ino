#include <Wire.h>
#include <SSD1306AsciiWire.h>

#define OLED_I2C_ADDR   0x3C
SSD1306AsciiWire display;

#define PIN_BATTERY            A7
const float ADC_REF_VOLTAGE    = 5.1;
const float VOLTAGE_DIV_RATIO  = 2.0;
const float BAT_MIN_VOLTAGE    = 6.0;
const float BAT_MAX_VOLTAGE    = 8.4;
float batteryVoltage = 0.0;
int lastBatADC     = 0;
float lastBatVolt  = 0.0;
int lastBatLevel   = 0;

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

void DrawBatteryUI(int adc, float volt, int level){
  display.clear();
  display.set1X();

  display.setCursor(10, 0);
  display.print(F("Battery Monitor"));

  display.setCursor(10, 3);
  display.print(F("Bat ADC: "));
  display.print(adc);

  display.setCursor(10, 4);
  display.print(F("Bat Voltage: "));
  display.print(volt, 2);
  display.print(F("V"));

  display.setCursor(10, 5);
  display.print(F("Bat Level: "));
  display.print(level);
  display.print(F("%"));
}

void setup(){
  Serial.begin(9600);
  pinMode(PIN_BATTERY, INPUT);
  Wire.begin();
  display.begin(&Adafruit128x64, OLED_I2C_ADDR);
  display.setFont(Adafruit5x7);
  display.clear();

  int initADC = Get_Battery_Voltage_ADC();
  float initVolt = Get_Battery_Voltage();
  int initLevel = Get_Battery_Ratio();
  DrawBatteryUI(initADC, initVolt, initLevel);

  lastBatADC = initADC;
  lastBatVolt = initVolt;
  lastBatLevel = initLevel;
}

void loop(){
  int curBatADC = Get_Battery_Voltage_ADC();
  float curBatVolt = Get_Battery_Voltage();
  int curBatLevel = Get_Battery_Ratio();
  if ((curBatADC != lastBatADC) || (abs(curBatVolt - lastBatVolt) > 0.01f) || (curBatLevel != lastBatLevel)){
    DrawBatteryUI(curBatADC, curBatVolt, curBatLevel);
    lastBatADC = curBatADC;
    lastBatVolt = curBatVolt;
    lastBatLevel = curBatLevel;
  }
  delay(1000);
}