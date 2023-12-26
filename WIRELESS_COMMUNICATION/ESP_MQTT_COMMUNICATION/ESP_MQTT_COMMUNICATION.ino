#include "Secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"

#define AWS_IOT_PUBLISH_TOPIC   "esp32/sub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/pub"
#define RGB_B 13
#define RGB_G 32
#define RGB_R 0
#define ENCODER 39
#define ENCODER_IN 36
#define IN1 27
#define IN2 26
#define ENA 33

// encoder pulses per revolution
#define EN_Rev 360

volatile long encoderValue = 0;

//measurement interval in milliseconds
int interval = 1000;

//milliseconds counter during the interval
long previousMillis = 0;
long currentMillis = 0;

//RPM measurement variable
int rpm = 0;
float rpm_oriji;

int dt = 1;

//Used with the PID control
double error, previous_error, output, integral = 0;
double setspeed, currentspeed;
double kp, ki, kd;
double speed_rpm = 170;


//Used with the data from the cloud 
String h, dir, previousDir, newSpeed;
int spd; //Speed variable
int i=0;

WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);

/**************************************************/
//Setting up WiFi and AWS IoT Core cloud mqtt
void connectAWS()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    for (int i = 0 ; i < 4 ; i++)
     {
        digitalWrite(RGB_B, HIGH);
        delay(500);
        digitalWrite(RGB_B, LOW);
        delay(500);
        
      }
    Serial.println("Connected to WiFi");
  }

  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.setServer(AWS_IOT_ENDPOINT, 8883);

  // Create a message handler
  client.setCallback(messageHandler);

  Serial.println("Connecting to AWS IOT");

  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(100);
  }
  for (int i = 0 ; i < 4 ; i++)
    {
      digitalWrite(RGB_B, HIGH);
      digitalWrite(RGB_R, HIGH);
      delay(300);
      digitalWrite(RGB_B, LOW);
      digitalWrite(RGB_R, HIGH);
      delay(300);
      
    }

  if (!client.connected())
  {
    Serial.println("AWS IoT Timeout!");
    return;
  }
    
  

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
  Serial.println("AWS IoT Connected!");
}

/*****************************************/
//Publishing the data to the IoT core cloud
void publishMessage()
{
  StaticJsonDocument<200> doc;
  doc["current speed: "] = h;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client

  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

/*****************************************/
//Message handler for the topic
void messageHandler(char* topic, byte* payload, unsigned int length)
{
  Serial.print("incoming: ");
  Serial.println(topic);

  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const char* message = doc["message"];
  String msg = message;
  Serial.println(message);
  newSpeed = msg;
}

/*****************************************/
//Main initialization for the program
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  connectAWS();
  pinMode(RGB_R, OUTPUT);
  pinMode(RGB_B, OUTPUT);
  pinMode(RGB_G, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENCODER, OUTPUT);
  pinMode(ENCODER_IN, INPUT_PULLUP);
  
  //PID controller constants
  kp = 0.50;
  ki = 0.48;
  kd = 0.10;
  
  //Attaching interrupt and updateEncoder increments encoder value
  attachInterrupt(digitalPinToInterrupt(ENCODER_IN), updateEncoder, RISING);

  //Initializing the timer
  previousMillis = millis();

}

/*****************************************/
//Looping function for the program
void loop() {
  client.loop();
  speed_rpm = directionAndSpeed(newSpeed);
  setspeed = map(speed_rpm, 0, 210, 0, 255);

  //to update milliseconds every second
  currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;

    //calculating RPM. encoderValue is pulses/sec EN_REV is pulses/rev
    rpm = (float)(encoderValue * 60 / EN_Rev);
    rpm_oriji = rpm / 3;
    currentspeed = map(rpm_oriji, 0, 210, 0, 255);
    error = setspeed - currentspeed;
    output = pid(error);
    analogWrite(ENA, output);

    // Setspeed VS currentspeed in rpm
    Serial.print(speed_rpm);
    Serial.print(" , ");
    Serial.println(rpm_oriji);
    

    //Resetting the encoderValue
    encoderValue = 0;
  }
  h = rpm_oriji;
  publishMessage();
  delay(1000);
}

/*****************************************/
//Changing the directionality and speed of the motor
double directionAndSpeed(String myData)
{
  //Splitting the string to direction and speed
  dir = myData.substring(0, myData.indexOf(':'));
  Serial.print("New direction:");
  Serial.println(dir);
  
  //Assigning the second part to speed variable
  String speedStr = myData.substring(myData.indexOf(':')+1);
  spd = speedStr.toInt();

  //Determining the directionality of the motor
  if(dir.equals("CW") && dir != previousDir)
  {
    //Setting the motor speed to 0
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, 0);
    delay(2000);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(RGB_B, LOW);
    digitalWrite(RGB_G, HIGH);
  }
  else if(dir.equals("CCW") && dir != previousDir)
  {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, 0);
    delay(2000);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(RGB_G, LOW);
    digitalWrite(RGB_B, HIGH);
  }

  previousDir = dir;
  //Determining the speed of motor
  return spd;
}

/*****************************************/
//Returns the value of speed needed to control the motor
double pid(double error) {
  double proportional = error;
  integral += error * dt;
  double derivative = (error - previous_error) / dt;
  previous_error = error;
  double output = (kp * proportional) + (ki * integral) + (kd * derivative);
  return output;
}

/*****************************************/
//Updating encoder reading 
void updateEncoder() {
  //incrementing encoder value
  encoderValue++;
}
