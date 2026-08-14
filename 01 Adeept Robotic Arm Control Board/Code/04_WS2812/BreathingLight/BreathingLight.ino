/***********************************************************
Function:  Breathing Light
Website: www.adeept.com
***********************************************************/
#include <Adafruit_NeoPixel.h>

#define LED_PIN    11        
#define NUM_LEDS   2          
#define DEFAULT_BRIGHTNESS 50  

Adafruit_NeoPixel pixels(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  pixels.begin();         
  pixels.clear();      
  pixels.show();
}

void loop() {
  breatheColor(0, 0, 255); 
}


void breatheColor(uint8_t R, uint8_t G, uint8_t B) {
  for (int i = 0; i <= 255; i++) {
    setAllLED(R, G, B, i);
    delay(8);
  }
  for (int i = 255; i >= 0; i--) {
    setAllLED(R, G, B, i);
    delay(8);
  }
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