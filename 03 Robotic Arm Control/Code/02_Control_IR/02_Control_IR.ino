#include <Wire.h>
#include <SSD1306AsciiWire.h>
#include <Adafruit_NeoPixel.h>
#include <Servo.h>
#include <IRremote.hpp>
#include <EEPROM.h>
#define OLED_I2C_ADDR 0x3C
#define RECV_PIN 2
#define SERVO_TOTAL 6
#define PIN_BUZZER 3                
#define BUZZER_FREQUENCY 4000   
#define LED_PIN    11        
#define NUM_LEDS   2        
#define DEFAULT_BRIGHTNESS 50 
#define PIN_BATTERY A7

SSD1306AsciiWire oled;
Servo servo[SERVO_TOTAL];
const byte servoPins[SERVO_TOTAL]={10,9,8,7,6,5};
int target[SERVO_TOTAL],lastShown[SERVO_TOTAL];
int currentPos[SERVO_TOTAL] = {90,90,90,90,90,90};
//select servo num, default step is 5
int sel=0,stepSize=5;
unsigned long tServo,tOLED;
Adafruit_NeoPixel pixels(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
uint8_t wsMode=0;
uint32_t wsTimer=0;
const float ADC_REF_VOLTAGE    = 5.1;
const float VOLTAGE_DIV_RATIO  = 2.0;
const float BAT_MAX_VOLTAGE    = 8.4;
float batteryVoltage=0;
int batteryADC=0;
float lastBatteryVoltage=0;
unsigned long oledTimer=0;
unsigned long batteryTimer=0;

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

void saveEEP(){
  for(int i=0;i<SERVO_TOTAL;i++){
    EEPROM.update(i,target[i]);
  }
}

void loadEEP(){
  for(int i=0;i<SERVO_TOTAL;i++){
    int v=EEPROM.read(i); 
    if(v>180)v=90; 
    target[i]=v; 
    lastShown[i]=-1;
  }
}

void drawStatic(){
  oled.clear(); 
  oled.setFont(Adafruit5x7);
  oled.setCursor(15,0); 
  oled.print("IR Control");
  oled.setCursor(70,5); 
  oled.print("Step:"); 
  for(int i=0;i<SERVO_TOTAL;i++){
    oled.setCursor(0,i+2); 
    oled.print(" S");
    oled.print(6-i); 
    oled.print(":");
  }
}

void updateOLED(){
  if(millis()-tOLED<1000)return; 
  tOLED=millis();
  oled.setCursor(0,2+sel); 
  oled.print("#");
  for(int i=0;i<SERVO_TOTAL;i++){
    if(i!=sel){
      oled.setCursor(0,i+2); 
      oled.print(" ");
    }
    if(lastShown[i]!=target[i]){
        oled.setCursor(24,i+2); 
        oled.print("    ");
        oled.setCursor(24,i+2); 
        oled.print(target[i]);
        lastShown[i]=target[i];
    }
  }
  oled.setCursor(100,5); 
  oled.print("  "); 
  oled.setCursor(100,5);
  oled.print(stepSize); 

  if(lastBatteryVoltage != batteryVoltage){
    oled.setCursor(96,0);
    oled.print("   ");
    oled.setCursor(96,0);
    oled.print(batteryVoltage,2);
    oled.print("V");
    lastBatteryVoltage = batteryVoltage;
  }
}

void updateServo(){
  if(millis()-tServo<5)return; 
  tServo=millis();
  for(int i=0;i<SERVO_TOTAL;i++){
    if(currentPos[i]<target[i]) currentPos[i]++;
    else if(currentPos[i]>target[i]) currentPos[i]--;
    servo[i].write(currentPos[i]);
  }
}

void initWS2812(){
  pixels.begin();        
  pixels.clear();          
  pixels.show();
}

void setAllLED(uint8_t R, uint8_t G, uint8_t B, uint8_t brightness) {
  uint8_t r = map(brightness, 0, 255, 0, R);
  uint8_t g = map(brightness, 0, 255, 0, G);
  uint8_t b = map(brightness, 0, 255, 0, B);
  for (int i = 0; i < NUM_LEDS; i++) {
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }
  pixels.show();
}

void updateWS2812(){
  if(millis()-wsTimer<500) return;
  wsTimer=millis();
  switch(wsMode){
    case 0:
        setAllLED(0,0,0,DEFAULT_BRIGHTNESS);
        break;
    case 1:
        setAllLED(255,0,0,DEFAULT_BRIGHTNESS);
        break;
    case 2:
        setAllLED(0,255,0,DEFAULT_BRIGHTNESS);
        break;
    case 3:
        setAllLED(0,0,255,DEFAULT_BRIGHTNESS);
        break;
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

void updateBattery(){
  if (millis() - batteryTimer < 1000)return;
  batteryTimer = millis();
  batteryADC = Get_Battery_Voltage_ADC();
  batteryVoltage = ((float)batteryADC / 1023.0) * ADC_REF_VOLTAGE * VOLTAGE_DIV_RATIO;
  batteryVoltage = constrain(batteryVoltage, 0, BAT_MAX_VOLTAGE);
}

void setup(){
  Wire.begin(); 
  oled.begin(&Adafruit128x64,OLED_I2C_ADDR);
  for(int i=0;i<SERVO_TOTAL;i++){
    servo[i].attach(servoPins[i]);
  }
  loadEEP(); 
  initWS2812();
  drawStatic(); 
  IrReceiver.begin(RECV_PIN,ENABLE_LED_FEEDBACK);
  Buzzer_Setup();
  Buzzer_Alert(2,1);
}

void loop(){
  if(IrReceiver.decode()){
    auto c=IrReceiver.decodedIRData.command;
    switch(c){
      case 12:
        sel=5;
        Buzzer_Alert(1,1);
        break;
      case 24:
        sel=4;
        Buzzer_Alert(1,1);
        break;
      case 94:
        sel=3;
        Buzzer_Alert(1,1);
        break;
      case 8:
        sel=2;
        Buzzer_Alert(1,1);
        break;
      case 28: 
        sel=1;
        Buzzer_Alert(1,1);
        break;
      case 90:
        sel=0;
        Buzzer_Alert(1,1);
        break;
      case 64:
        target[sel]=min(180,target[sel]+stepSize);
        break;
      case 25:
        if(sel == 4 || sel == 5){
          target[sel]=max(80,target[sel]-stepSize);
        } else {
          target[sel]=max(0,target[sel]-stepSize);
        }
        break;
      case 21:
        target[sel]=90;
        Buzzer_Alert(1,1);
        break;
      case 22:
        for(int i=0;i<SERVO_TOTAL;i++){
          target[i]=90;
        }
        Buzzer_Alert(1,1);
        break;
      case 67:
        saveEEP();
        Buzzer_Alert(1,1);
        break;
      case 69:
        stepSize=1;
        Buzzer_Alert(1,1);
        break;
      case 70:
        stepSize=3;
        Buzzer_Alert(1,1);
        break;
      case 71:
        stepSize=5;
        Buzzer_Alert(1,1);
        break;
      case 68:
        stepSize=10;
        Buzzer_Alert(1,1);
        break;
      case 66:
        wsMode=1;
        Buzzer_Alert(1,1);
        break;
      case 82:
        wsMode=2;
        Buzzer_Alert(1,1);
        break;
      case 74:
        wsMode=3;
        Buzzer_Alert(1,1);
        break; 
      case 13:
        wsMode=0;
        Buzzer_Alert(1,1);
        break;       
    }
    IrReceiver.resume();
  }
  updateServo();
  updateOLED();
  updateBattery();
  updateWS2812();
}
