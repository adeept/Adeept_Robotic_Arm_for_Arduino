/***********************************************************
 File name: ProcessingArmRobot.pde
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
PImage armImg;
Button[] mouseButton = new Button[10];
Button[] keyButton = new Button[10];
controlP5.Button b1,b2;
Textlabel label1,label2,label3,label4,label5,label6,label7,label8;
Textlabel wifiStatusLabel; 
boolean b1Open, b2Open;

char[] keyCmd = {'q','w','e','r','t','y','u','i','o','p'};

String[] mouseName = {
  "Gripper+",
  "Gripper-",
  "Rotate+",
  "Rotate-",
  "Elbow+",
  "Elbow-",
  "Shoulder+",
  "Shoulder-",
  "Base+",
  "Base-"
};

float b1x=-300, b1y=190, b2x=-300, b2y=190;
float[][] mousePos = {
  {-200, 330},  // m1
  {1400, 330},  // m2
  {-200, 390},  // m3
  {1400, 390},  // m4
  {-200, 450},  // m5
  {1400, 450},  // m6
  {-200, 510},  // m7
  {1400, 510},  // m8
  {-200, 570},  // m9
  {1400, 570}   // m10
};

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

float label1x=-600, label1y=600, label2x=-600, label2y=600, label3x=-600, label3y=600, label4x=-600, label4y=380, label5x=-600, label5y=380, label6x=1300, label6y=380, label7x=1300, label7y=380, label8x=1300, label8y=380;

void sendCmd(char cmdChar){
  if(!wifiConnected || outStream == null){
    println("WiFi Disconnected, can't send cmd: " + cmdChar);
    return;
  }
  outStream.print(cmdChar);
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

void setup() {
  size(1500, 700);
  smooth();
  
  connectESP8266(); 
  
  cp5 = new ControlP5(this);
  armImg = loadImage("robot.jpg");
  
  PFont pfont = createFont("Arial", 20, true);
  b1 = cp5.addButton("b1").setSize(250, 100);
  b2 = cp5.addButton("b2").setSize(200, 100);
  b1.getCaptionLabel().setFont(pfont).setSize(50).toUpperCase(false).setText(" Keyboard");
  b2.getCaptionLabel().setFont(pfont).setSize(50).toUpperCase(false).setText(" Mouse");

  for(int i =0; i < 10; i++){
    mouseButton[i] = cp5.addButton("m" + i).setSize(90, 50);
    mouseButton[i].getCaptionLabel().setFont(pfont).setSize(20).toUpperCase(false).setText(mouseName[i]);
  } 
  
  for(int i =0; i < 10; i++){
    keyButton[i] = cp5.addButton(String.valueOf(keyCmd[i])).setSize(90, 90);
    keyButton[i].getCaptionLabel().setFont(pfont).setSize(50).toUpperCase(false).setText(String.valueOf(keyCmd[i]).toUpperCase());
  } 
 
  PFont pfont2 = createFont("Arial", 40, true);
  label1 = cp5.addTextlabel("label1").setText("Keyboard Activated").setColorValue(0xff888888).setFont(pfont2);
  label2 = cp5.addTextlabel("label2").setText("Mouse Activated").setColorValue(0xff888888).setFont(pfont2);
  
  label4 = cp5.addTextlabel("label4").setText("Gripper").setFont(pfont2);
  label5 = cp5.addTextlabel("label5").setText("Rotate").setFont(pfont2);
  label6 = cp5.addTextlabel("label6").setText("Elbow").setFont(pfont2);
  label7 = cp5.addTextlabel("label7").setText("Shoulder").setFont(pfont2);
  label8 = cp5.addTextlabel("label8").setText("Base").setFont(pfont2);
  
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
  
  image(armImg, 1170, 100, 300, 500);
  textSize(70);
  fill(0, 169, 169);
  text("Adeept Robotic Arm", 300, 130);
  textSize(32);
  text("www.adeept.com", 1200, 650);
  
  b1x = b1x+(250-b1x)*0.1; 
  b1.setPosition(b1x, b1y);
  b2x = b2x+(750-b2x)*0.1; 
  b2.setPosition(b2x, b2y);
  if (b2Open) {
    for(int i=0; i<10;i++){
      if(i % 2 == 0){
        mousePos[i][0] = mousePos[i][0] + (520 - mousePos[i][0])*0.1;
      } else {
        mousePos[i][0] = mousePos[i][0] + (650 - mousePos[i][0])*0.1;
      }
      mouseButton[i].setPosition(mousePos[i][0], mousePos[i][1]);
    }
  } else {
    for(int i=0; i<10;i++){
      if(i % 2 == 0){
        mousePos[i][0] = mousePos[i][0] + (-100 - mousePos[i][0])*0.1;
      } else {
        mousePos[i][0] = mousePos[i][0] + (1500 - mousePos[i][0])*0.1;
      }
      mouseButton[i].setPosition(mousePos[i][0], mousePos[i][1]);
    }
  }
  if (b1Open) {
    for(int i=0; i<10;i++){
      keyPos[i][0] = keyPos[i][0] + ((i+1)*100 - keyPos[i][0])*0.1;
      keyButton[i].setPosition(keyPos[i][0], keyPos[i][1]);
    }
    
    label1x = label1x+(100-label1x)*0.1; 
    label1.setPosition(label1x, label1y);
    label4x = label4x+(140-label4x)*0.1; 
    label4.setPosition(label4x, label4y);
    label5x = label5x+(340-label5x)*0.1; 
    label5.setPosition(label5x, label5y);
    label6x = label6x+(550-label6x)*0.1; 
    label6.setPosition(label6x, label6y);
    label7x = label7x+(730-label7x)*0.1; 
    label7.setPosition(label7x, label7y);
    label8x = label8x+(960-label8x)*0.1; 
    label8.setPosition(label8x, label8y);
  } else {
    for(int i=0; i<10;i++){
      if(i % 2 == 0){
        keyPos[i][0] = keyPos[i][0] + (-100 - keyPos[i][0])*0.1;
      } else {
        keyPos[i][0] = keyPos[i][0] + (1500 - keyPos[i][0])*0.1;
      }
      keyButton[i].setPosition(keyPos[i][0], keyPos[i][1]);
    }
    
    label1x = label1x+(-600-label1x)*0.1; 
    label1.setPosition(label1x, label1y);
    label4x = label4x+(-600-label4x)*0.1; 
    label4.setPosition(label4x, label4y);
    label5x = label5x+(-600-label5x)*0.1; 
    label5.setPosition(label5x, label5y);
    label6x = label6x+(1500-label6x)*0.1; 
    label6.setPosition(label6x, label6y);
    label7x = label7x+(1500-label7x)*0.1; 
    label7.setPosition(label7x, label7y);
    label8x = label8x+(1500-label8x)*0.1; 
    label8.setPosition(label8x, label8y);
  }
  
  if (b2Open) {
    label2x = label2x+(100-label2x)*0.1; 
    label2.setPosition(label2x, label2y);
  } else {
    label2x = label2x+(-600-label2x)*0.1; 
    label2.setPosition(label2x, label2y);
  } 

  if (b1.isPressed()) {
    b1Open = true;
    b2Open = false;
  }
  if (b2.isPressed()) {
    b1Open = false;
    b2Open = true;
  }

  buttonColor();
  mouseCheck();
}

void keyPressed() {
  if (b1Open) {
    if (key >= 'A' && key <= 'z') {
      sendCmd(key);
      println(key);
    }
  }
}

void mouseCheck() {
  if (b1Open) {
    for(int i=0;i<10;i++){
      if (keyButton[i].isPressed()) {
        sendCmd(keyCmd[i]);
        println(keyCmd[i]);
        delay(100);
      }
    }
  }
  if (b2Open) {
    for(int i=0;i<10;i++){
      if (mouseButton[i].isPressed()) {
        sendCmd(keyCmd[i]);
        println(mouseName[i]);
        delay(100);
      }
    }
  }
}

void buttonColor() {
  if (b1Open) {
    for(int i=0;i<10;i++){
      if(keyPressed && Character.toLowerCase(key) == keyCmd[i] ) {
        keyButton[i].setColorBackground(color(255, 0, 0));
      }else{
        keyButton[i].setColorBackground(color(2, 72, 90));
      }
    }  
  }
}
