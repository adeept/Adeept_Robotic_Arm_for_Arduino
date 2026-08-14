/***********************************************************
 Website:     www.adeept.com
 E-mail:      support@adeept.com
***********************************************************/
#include <EEPROM.h>
#include <Wire.h>
#include <SSD1306AsciiWire.h>

#define EEPROM_ADDR  5
#define TARGET_DATA  2
int readData;
#define OLED_ADDR 0x3C
SSD1306AsciiWire display;

void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.println("====================");
  Serial.println("EEPROM Test Start");
  Serial.println("====================");

  Wire.begin();
  display.begin(&Adafruit128x64, OLED_ADDR);
  display.setFont(Adafruit5x7);
  display.clear();

  display.set2X();
  display.setCursor(10,0);
  display.print("EEPROM");
  display.set1X();
  display.setCursor(0,3);
  display.print("Testing...");
  delay(1000);

  readData = EEPROM.read(EEPROM_ADDR);
  Serial.print("Read Address: ");
  Serial.println(EEPROM_ADDR);
  Serial.print("Read Data: ");
  Serial.println(readData);
  if(readData == TARGET_DATA) {
    Serial.println("EEPROM Read Success");
    display.clear();
    display.setCursor(0,0);
    display.print("EEPROM OK");
    display.setCursor(0,2);
    display.print("ADDR:");
    display.print(EEPROM_ADDR);
    display.setCursor(0,3);
    display.print("DATA:");
    display.print(readData);
  } else {
    Serial.println("EEPROM Data Error");
    Serial.println("Writing new data...");
    display.clear();
    display.setCursor(0,0);
    display.print("EEPROM ERROR");
    display.setCursor(0,2);
    display.print("Writing...");
    EEPROM.update(EEPROM_ADDR, TARGET_DATA);
    delay(500);

    readData = EEPROM.read(EEPROM_ADDR);
    Serial.print("New Data:");
    Serial.println(readData);
    display.clear();
    if(readData == TARGET_DATA) {
      Serial.println("Write Success");
      display.setCursor(0,0);
      display.print("WRITE OK");
    } else {
      Serial.println("Write Failed");
      display.setCursor(0,0);
      display.print("WRITE FAIL");
    }
    display.setCursor(0,2);
    display.print("ADDR:");
    display.print(EEPROM_ADDR);
    display.setCursor(0,3);
    display.print("DATA:");
    display.print(readData);
  }
  Serial.println("====================");
  Serial.println("EEPROM Test End");
}

void loop() {

}