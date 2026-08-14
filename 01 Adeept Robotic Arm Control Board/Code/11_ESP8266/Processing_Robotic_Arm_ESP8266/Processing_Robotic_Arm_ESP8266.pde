/***********************************************************
 File name: ProcessingRoboticArmESP8266.pde
 Website: www.adeept.com
 E-mail: support@adeept.com
 ***********************************************************/
import controlP5.*;
import java.net.*;
import java.io.*;

final String ESP_IP  = "192.168.4.1"; 
final int ESP_PORT  = 4000;         
Socket espSocket;
PrintWriter outStream;
BufferedReader inStream;
boolean wifiConnected = false;

ControlP5 cp5;
Textfield input;
Button[] keyButton = new Button[10];
Textlabel wifiStatusLabel; 

char[] keyCmd = {'q','w','e','r','t','y','u','i','o','p'};
float[][] keyPos = {
  {-200, 430},  // k1
  {1400, 430},  // k2
  {-200, 430},  // k3
  {1400, 430},  // k4
  {-200, 430},  // k5
  {1400, 430},  // k6
  {-200, 430},  // k7
  {1400, 430},  // k8
  {-200, 430},  // k9
  {1400, 430}   // k10
};

void sendCmd(char cmdChar) {
  sendCmd(String.valueOf(cmdChar));
}

void sendCmd(String data) {
  if (!wifiConnected || outStream == null) {
    println("WiFi Disconnected, can't send: " + data);
    return;
  }
  outStream.print(data);
  outStream.flush();
}

void connectESP8266() {
  closeWiFiSocket(); 
  try {
    espSocket = new Socket(ESP_IP, ESP_PORT);
    outStream = new PrintWriter(espSocket.getOutputStream(), true);
    inStream  = new BufferedReader(new InputStreamReader(espSocket.getInputStream()));
    wifiConnected = true;
    println("WiFi Connected -> " + ESP_IP + ":" + ESP_PORT);
  } catch (Exception e) {
    wifiConnected = false;
    println("ESP Connect Failed, check IP/ESP WiFi");
  }
}

void closeWiFiSocket(){
  try{
    if(inStream != null) inStream.close();
    if(outStream != null) outStream.close();
    if(espSocket != null) espSocket.close();
  }catch(Exception e){
      println("Stream close failed");
  }
}

void receiveData(){
  try{
    String data="";
    while(inStream.ready()){
        data+=(char)inStream.read();
    }
    println(data);
  }catch(Exception e){
    println(e);
  }

}

void setup() {
  size(1200, 700);
  smooth();
  connectESP8266(); 
  PFont pfont = createFont("Arial", 20, true);
  cp5 = new ControlP5(this);
  cp5.addTextlabel("tip")
     .setText("Enter text and press Enter to send:")
     .setPosition(180,250)
     .setFont(pfont);
  input = cp5.addTextfield("input")
             .setPosition(500, 240)
             .setSize(200, 40)
             .setFont(pfont)
             .setAutoClear(false)
             .setText("Adeept");
  input.getCaptionLabel().setVisible(false);
  
  for(int i =0; i < 10; i++){
    keyButton[i] = cp5.addButton(String.valueOf(keyCmd[i])).setSize(90, 90);
    keyButton[i].getCaptionLabel().setFont(pfont).setSize(50).toUpperCase(false).setText(String.valueOf(keyCmd[i]).toUpperCase());
  } 
  wifiStatusLabel = cp5.addTextlabel("wifiStatus").setPosition(10, 15).setFont(createFont("Arial", 28));
}

void stop(){
  closeWiFiSocket();
  super.stop() ;
}

void draw() {
  background(0, 0, 0);
  if(wifiConnected){
    wifiStatusLabel.setText("WiFi Connected: " + ESP_IP + ":" + ESP_PORT).setColorValue(0xff00ff00); 
  }else{
    wifiStatusLabel.setText("WiFi Disconnected").setColorValue(0xffff3333);
  }
  
  textSize(70);
  fill(0, 169, 169);
  text("Adeept ESP8266 Test", 300, 130);
  textSize(32);
  text("www.adeept.com", 500, 650);
  
  for(int i=0; i<10;i++){
    keyPos[i][0] = keyPos[i][0] + ((i+1)*100 - keyPos[i][0])*0.1;
    keyButton[i].setPosition(keyPos[i][0], keyPos[i][1]);
  }
  receiveData();
  buttonColor();
  mouseCheck();
}

void keyPressed() {
  if (key == ENTER || key == RETURN) {
    sendCmd(input.getText());
    println(input.getText());
  } else {
      sendCmd(key);
      println(key);
  }
}

void mouseCheck() {
  for(int i=0;i<10;i++){
    if (keyButton[i].isPressed()) {
      sendCmd(keyCmd[i]);
      println(keyCmd[i]);
      delay(100);
    }
  }
}

void buttonColor() {
  for(int i=0;i<10;i++){
    if(keyPressed && Character.toLowerCase(key) == keyCmd[i] ) {
      keyButton[i].setColorBackground(color(255, 0, 0));
    }else{
      keyButton[i].setColorBackground(color(2, 72, 90));
    }
  } 
}
