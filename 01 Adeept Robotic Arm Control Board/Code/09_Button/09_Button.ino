const int buttonPin = 4;    // Button connected to pin D4
const int ledPin = 13;      // Default pin 13 for Uno

void setup() {
  Serial.begin(115200);                
  pinMode(buttonPin, INPUT_PULLUP); 
  pinMode(ledPin, OUTPUT);           
  Serial.println("Button test started, press or release the button on pin D4");
}

void loop() {
  int buttonState = digitalRead(buttonPin);
  // Internal pull-up mode: LOW when pressed, HIGH when released
  if (buttonState == LOW) {
    digitalWrite(ledPin, LOW);  // Turn on built-in LED when pressed
    Serial.println("Status: Pressed");
  } else {
    digitalWrite(ledPin, HIGH);   // Turn off built-in LED when released
    Serial.println("Status: Released");
  }
  delay(100); 
}