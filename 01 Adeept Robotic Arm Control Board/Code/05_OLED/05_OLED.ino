#include <Wire.h>
#include <SSD1306AsciiWire.h>

#define SCREEN_W    128
#define SCREEN_H    64
#define OLED_ADDR   0x3C

SSD1306AsciiWire display;

void setup() {
  Serial.begin(115200);
  
  Wire.begin();
  display.begin(&Adafruit128x64, OLED_ADDR);
  display.setFont(Adafruit5x7);
  display.clear();

  display.set2X();
  display.setCursor(20, 2);
  display.print("Adeept");
}

void loop() {

}