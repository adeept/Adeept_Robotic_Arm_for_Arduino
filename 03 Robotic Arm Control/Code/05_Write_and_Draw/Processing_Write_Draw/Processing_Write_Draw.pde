/***********************************************************
File name: ProcessingWritingAndDrawing.ino
Description: Control the robotic arm to write and draw through the upper computer interface written by Processing.
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
Textlabel wifiStatusLabel;
ControlP5 cp5;
PImage img;

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
  size(1200, 600);
  background(0,200,150);
  fill(255, 150, 50); 
  stroke(255, 150, 50);
  ellipse(600, 600, 1250, 1250);
  fill(224,237,26);
  stroke(224,237,26);
  ellipse(600, 600, 600,600);
  img = loadImage("LOGO1.png");  
  
  cp5 = new ControlP5(this);
  connectESP8266(); 
  wifiStatusLabel = cp5.addTextlabel("wifiStatus").setPosition(10, 15).setFont(createFont("Arial", 28));
}

void draw() {
  stroke(50,100,255);
  strokeWeight(3);
  image(img, 530, 450);

  if(wifiConnected){
    wifiStatusLabel.setText("WiFi Connected").setColorValue(0xff006400); 
  }else{
    wifiStatusLabel.setText("WiFi Disconnected").setColorValue(0xffff3333);
  }

  if (mousePressed) {
    boolean now = dist(mouseX, mouseY, 600, 600) <= 625 && dist(mouseX, mouseY, 600, 600) >= 300;
    boolean pre = dist(pmouseX, pmouseY, 600, 600) <= 625 && dist(pmouseX, pmouseY, 600, 600) >= 300;
    if (now && pre) {
      line(mouseX, mouseY, pmouseX, pmouseY);
      print("mouseX:"+ mouseX + "   mouseY:"+ mouseY);  
      float px = height - mouseY;   
      float py = mouseX - width/2.0;      
      float x = map(px, 0, height, 0, 210);
      float y = map(py, -width/2.0, width/2.0, -210, 210);
      float z= 100;
      println("  x:"+ x + "   y:"+ y);  
      sendXYZ(x,y,z);
    }
  }
}

void sendXYZ(float x,float y,float z){
    if(wifiConnected){
      outStream.print(String.format("%.2f,%.2f,%.2f\n", x,y,z));
      outStream.flush();
    }
}
