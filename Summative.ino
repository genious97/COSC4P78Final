#include <HUSKYLENS.h>
#include <SoftwareSerial.h>
#include <Servo.h>

//Serial read // bluetooth module
char value;
int btTX = 1;
int btRX = 0;
//Motor control
int EN1 = 9;    //pwm digital (9 OR 10)
int INA1 = 8;   //digital
int INA2 = 7;   //digital
int EN2 = 10;   //pwm digital (9 OR 10)
int INB1 = 11;  //digital
int INB2 = 4;   //digital
//Vision Sensor
int analogIn1;  //analog A0
int analogIn2;  //analog A1
//Servos
Servo cameraControl;  //between 75 and 120
Servo claw;           //between 0 and 75
int servo1 = 9;       //camera
int servo2 = 10;      //claw
//Encoders
int encoder1 = 2;  //digital with interrupt (2 OR 3)
int encoder2 = 3;  //digital with interrupt (2 OR 3)

volatile int leftCount = 0;
volatile int rightCount = 0;
volatile int distance = 0;

void setup() {
  pinMode(encoder1, INPUT);
  pinMode(encoder2, INPUT);
  cameraControl.attach(servo1);
  claw.attach(servo2);
  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT);
  pinMode(INA1, OUTPUT);
  pinMode(INA2, OUTPUT);
  pinMode(INB1, OUTPUT);
  pinMode(INB2, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(encoder1), leftEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoder2), rightEncoder, CHANGE);
  Serial.begin(9600);
}

void loop() {
  bluetooth();
}


void bluetooth() {
  if (Serial.available() > 0) {
    value = Serial.read();
    Serial.println(value);
  }
  // switch (value) {
  //   case 'F':  //forward
  //     forward();
  //     break;
  //   case 'B':  //backward
  //              //  backward();
  //     break;
  //   case 'l':  //left
  //              //  left();
  //     break;
  //   case 'r':  //right
  //              //  right();
  //     break;
  //   case 's':  //stop
  //     break;
  // }
}

void leftEncoder() {
  leftCount++;
  distance++;
}

void rightEncoder() {
  rightCount++;
  distance++;
}

void forward() {
  digitalWrite(INA1, HIGH);
  digitalWrite(INB1, HIGH);
  digitalWrite(INA2, LOW);
  digitalWrite(INB2, LOW);
  if (leftCount > rightCount + 10) {
    analogWrite(EN1, 128);
    analogWrite(EN2, 192);
  } else if (rightCount > leftCount + 10) {
    analogWrite(EN1, 192);
    analogWrite(EN2, 128);
    
  }
}