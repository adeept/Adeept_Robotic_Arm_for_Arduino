#include <Wire.h>
#include <SSD1306Ascii.h>
#include <SSD1306AsciiWire.h>

#define OLED_I2C_ADDR 0x3C
SSD1306AsciiWire oled;
char oledBuf[10];
unsigned long tOLED;
String command = "";
String lastCommand = "";

void setup(){
  Serial.begin(115200);

  Serial.println("AT+CWMODE=3\r\n");
  delay(300);     
  Serial.println("AT+CWSAP=\"Adeept_ADA031\",\"\",8,0\r\n");   
  delay(300);    
  Serial.println("AT+RST\r\n");    
  delay(1000);    
  Serial.println("AT+CIPMUX=1\r\n");
  delay(300);
  Serial.println("AT+CIPSERVER=1,4000\r\n");
  delay(300);
  Serial.println("AT+CIPSTO=7000\r\n");
  delay(300);

  oledInit();
  oledUpdate();   
}

void loop(){
  while(Serial.available()){
    command = Serial.readStringUntil('\n');
  }

  oledUpdate();   
  delay(10);
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
  oled.print("ESP8266 Receive:");
  oled.setCursor(10,5);
  oled.print(command);
}


void oledUpdate(){
  if(millis()-tOLED<100)return; 
  tOLED=millis();
  if(lastCommand != command){
    oled.setCursor(10,5);
    oled.print("                     ");
    oled.setCursor(10,5);
    oled.print(command);
    lastCommand = command;
  }
}
