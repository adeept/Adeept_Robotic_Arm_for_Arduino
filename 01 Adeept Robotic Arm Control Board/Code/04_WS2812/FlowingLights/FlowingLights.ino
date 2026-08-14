/***********************************************************
Function:  Flowing Lights
Website:   www.adeept.com
***********************************************************/
#include <Adafruit_NeoPixel.h>

#define LED_PIN    11        
#define NUM_LEDS   2       
#define BRIGHTNESS 50      

Adafruit_NeoPixel pixels(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

const int base_colors[][3] = {
    {0, 255, 255},    
    {255, 0, 0},      
    {0, 255, 0},     
    {0, 0, 255},      
    {255, 255, 0},  
    {255, 0, 255},   
    {0, 128, 255}   
};

const int colorCount = sizeof(base_colors) / sizeof(base_colors[0]);

void setup() {
  pixels.begin();              
  pixels.setBrightness(BRIGHTNESS); 
  pixels.clear();             
  pixels.show();
}

void loop() {
  for (int i = 0; i < colorCount; i++) {
    setAllLEDColor(base_colors[i][0], base_colors[i][1], base_colors[i][2]);
    delay(300);
  }
}

void setAllLEDColor(uint8_t R, uint8_t G, uint8_t B) {
  for (int i = 0; i < NUM_LEDS; i++) {
    pixels.setPixelColor(i, pixels.Color(R, G, B));
  }
  pixels.show();
}