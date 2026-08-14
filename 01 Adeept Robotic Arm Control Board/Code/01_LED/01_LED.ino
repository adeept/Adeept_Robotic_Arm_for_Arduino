# define PIN_LED  13  // Define LED pin as 13.

void setup()
{
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);
  delay(10);
}

void loop()
{
  digitalWrite(PIN_LED, LOW);
  Serial.println("LED on");
  delay(1000); 
  digitalWrite(PIN_LED, HIGH);   
  Serial.println("LED off");
  delay(1000); 
}
