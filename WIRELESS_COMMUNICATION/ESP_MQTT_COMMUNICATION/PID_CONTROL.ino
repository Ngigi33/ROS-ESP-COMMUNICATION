//#define en 5
//#define in1 18
//#define in2 19
//
//// encoder pulses per revolution
//#define EN_Rev 360
//// pin connected to encoder
//#define EN_IN 25
////initializing encoder pulse count
//volatile long encoderValue = 0;
//
////measurement interval in milliseconds
//int interval = 1000;
//
////milliseconds counter during the interval
//long previousMillis = 0;
//long currentMillis = 0;
//
////RPM measurement variable
//int rpm = 0;
//float rpm_oriji;
//
//int dt = 1;
//
//double error, previous_error, output, integral = 0;
//double setspeed, currentspeed;
//double kp, ki, kd;
//
////SoftwareSerial mySerial(2)
//// speed needed in rpm
////The speed in rpm should be between o and 210
//double speed_rpm = 170;
//void setup() {
//  //PID constants
//  kp = 0.48;
//  ki = 0.48;
//  kd = 0.12;
//  Serial.begin(9600);
//  pinMode(en, OUTPUT);
//  pinMode(in1, OUTPUT);
//  pinMode(in2, OUTPUT);
//
//  pinMode(EN_IN, INPUT_PULLUP);
//
//  //Attaching interrupt. updateEncoder increments encoder value
//  attachInterrupt(digitalPinToInterrupt(EN_IN), updateEncoder, RISING);
//
//  Serial.print("code_start");
//
//  //initializing the timer
//  previousMillis = millis();
//}
//
//void loop() {
//  setspeed = map(speed_rpm, 0, 210, 0, 255);
//
//  //to update milliseconds every second
//  currentMillis = millis();
//  if (currentMillis - previousMillis > interval) {
//    previousMillis = currentMillis;
//
//    //calculating RPM. encoderValue is pulses/sec EN_REV is pulses/rev
//    rpm = (float)(encoderValue * 60 / EN_Rev);
//    rpm_oriji = rpm / 3;
//    currentspeed = map(rpm_oriji, 0, 210, 0, 255);
//    error = setspeed - currentspeed;
//    output = pid(error);
//    digitalWrite(in1, HIGH);
//    digitalWrite(in2, LOW);
//    analogWrite(en, output);
//
//    // Setspeed VS currentspeed in pwm
//    // Serial.print(setspeed);
//    // Serial.print(" , ");
//    // Serial.println(currentspeed);
//
//    // // Setspeed VS currentspeed in rpm
//    Serial.print(speed_rpm);
//    Serial.print(" , ");
//    Serial.println(rpm_oriji);
//
//    //Resetting the encoderValue
//    encoderValue = 0;
//  }
//}
//double pid(double error) {
//  double proportional = error;
//  integral += error * dt;
//  double derivative = (error - previous_error) / dt;
//  previous_error = error;
//  double output = (kp * proportional) + (ki * integral) + (kd * derivative);
//  return output;
//}
//
//void updateEncoder() {
//  //incrementing encoder value
//  encoderValue++;
//}
