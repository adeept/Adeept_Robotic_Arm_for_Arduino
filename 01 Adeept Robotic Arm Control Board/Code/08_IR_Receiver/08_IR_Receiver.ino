/**********************************************************************
  Filename    : 08_IR_Receiver.ino
  Auther      : www.adeept.com
**********************************************************************/
#include <Wire.h>
#include <SSD1306AsciiWire.h>
#include <IRremote.hpp> 
SSD1306AsciiWire display;
#define RECV_PIN     2       
String command_ir = "";

void setup(){
  // Initialize OLED
  Wire.begin();
  Wire.setClock(400000L);
  display.begin(&Adafruit128x64, 0x3C);
  display.setFont(Adafruit5x7);
  display.clear();
  display.set2X();
  display.setCursor(0, 0); 
  display.print("Receive:");

  Serial.begin(115200);   
  IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK);       
  Serial.print("IRrecvDemo is now running and waiting for IR message on Pin ");
  Serial.println(RECV_PIN);   
}

void loop(){
  if (IrReceiver.decode()) {          
    unsigned long value = IrReceiver.decodedIRData.command;
    command_ir = switch_irr(value);
    Serial.println(command_ir);
    
    display.setCursor(0, 3); 
    display.print("       ");
    display.setCursor(0, 3); 
    display.print(command_ir);
    IrReceiver.resume();                        
  }
  delay(100);
}

String switch_irr(unsigned long irr_data){
  switch(irr_data){
    case 69: return "A";    
    case 70: return "B";    
    case 71: return "C";    
    case 68: return "D";    
    case 64: return "UP";   
    case 67: return "E";    
    case 7: return "LEFT"; 
    case 21: return "OK";   
    case 9: return "RIGHT";
    case 22: return "0";    
    case 25: return "DOWN"; 
    case 13: return "F";    
    case 12: return "1";    
    case 24: return "2";    
    case 94: return "3";    
    case 8: return "4";    
    case 28: return "5";    
    case 90: return "6";    
    case 66: return "7";    
    case 82: return "8";    
    case 74: return "9";    
    default: return "NONE";
  }
}
