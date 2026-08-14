/*************************************
Website:www.adeept.com
E-mail:support@adeept.com
*************************************/
#include <Wire.h>
#include <SSD1306AsciiWire.h>

#define OLED_I2C_ADDR   0x3C
SSD1306AsciiWire oled;
char oledBuf[10];
int val1=0;
int val2=0;
int val3=0;
int val4=0;
int val5=0;
int lastOledVal[5] = {-1, -1, -1, -1, -1};

void oledInit() {
  Wire.begin();
  oled.begin(&Adafruit128x64, OLED_I2C_ADDR);
  oled.setFont(Adafruit5x7);
  oled.clear();
}

void oledPrintNum(int x, int y, int num) {
  oled.set1X();
  oled.setCursor(x, y);
  oled.print("    ");     
  oled.setCursor(x, y);
  oled.print(itoa(num, oledBuf, 10));
}

void oledPrintStr(int x, int y, char text[]) {
  oled.set1X();
  oled.setCursor(x, y);
  oled.print(text);
}

void setup() {
  Serial.begin(115200);
  oledInit();
  oled.invertDisplay(true);
  delay(500);
  oled.invertDisplay(false);
  oled.clear();
}

void loop() {
  val1 = analogRead(0);  
  val2 = analogRead(1);  
  val3 = analogRead(2);  
  val4 = analogRead(3);
  val5 = analogRead(6);

  Serial.print("A0:");Serial.print(val1);
  Serial.print("   A1:");Serial.print(val2);
  Serial.print("   A2:");Serial.print(val3);
  Serial.print("   A3:");Serial.print(val4);
  Serial.print("   A6:");Serial.println(val5);

  if(val1 != lastOledVal[0]){
    oledPrintStr(0, 1, "A0:");
    oledPrintNum(25, 1, val1);
    lastOledVal[0] = val1;
  }
  if(val2 != lastOledVal[1]){
    oledPrintStr(64, 1, "A1:");
    oledPrintNum(88, 1, val2);
    lastOledVal[1] = val2;
  }
  if(val3 != lastOledVal[2]){
    oledPrintStr(0, 3, "A2:");
    oledPrintNum(25, 3, val3);
    lastOledVal[2] = val3;
  }
  if(val4 != lastOledVal[3]){
    oledPrintStr(64, 3, "A3:");
    oledPrintNum(88, 3, val4);
    lastOledVal[3] = val4;
  }
  if(val5 != lastOledVal[4]){
    oledPrintStr(0, 5, "A6:");
    oledPrintNum(25, 5, val5);
    lastOledVal[4] = val5;
  }
  delay(200);
}