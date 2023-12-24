#define LED_pin 2
#define LED_error_pin 3
String msg = "";

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_pin, OUTPUT);
  pinMode(LED_error_pin, OUTPUT);
  Serial.begin(921600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    msg = Serial.readStringUntil('\n');
    //Serial.println(msg);
  }

  if (msg.equals("ON")) {
    digitalWrite(LED_pin, HIGH);
    Serial.println("LED IS ON");
    //Serial.println('\n');
  }

  else if (msg.equals("OFF")) {
    digitalWrite(LED_pin, LOW);
    Serial.println("LED IS OFF");
    //Serial.println('\n');
  }

  else {
    digitalWrite(LED_error_pin, HIGH);
    delay(100);
    digitalWrite(LED_error_pin, LOW);
  }

  msg = "";
}
