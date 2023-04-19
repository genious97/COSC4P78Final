//Serial read // bluetooth module
char value;
int btTX = 1;
int btRX = 0;
//Motor control
int EN1 = 9; //pwm digital (9 OR 10)
int INA1 = 8; //digital
int INA2 = 7; //digital
int EN2 = 10; //pwm digital (9 OR 10)
int INB1 = 6; //digital
int INB2 = 5; //digital
//Vision Sensor
int analogIn1; //analog A0
int analogIn2; //analog A1
//Servos
int servo1; //analog A4
int servo2; //analog A5
//Encoders
int encoder1 = 2; //digital with interrupt (2 OR 3)
int encoder2 = 3; //digital with interrupt (2 OR 3)

volatile int leftCount = 0;
volatile int rightCount = 0;
volatile int distance = 0;

void setup() {
  pinMode(encoder1, INPUT);
  pinMode(encoder2, INPUT);
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
  digitalWrite(INA1, HIGH);
  digitalWrite(INB1, HIGH);
  digitalWrite(INA2, LOW);
  digitalWrite(INB2, LOW);
  analogWrite(EN1, 128);
  analogWrite(EN2, 128);
  Serial.println(String(leftCount) + " " + String(rightCount));
}

void voiceControl() {
  if(Serial.available() > 0) {
    char value = Serial.read();
    Serial.println(value);
    switch(value) {
      case 'f': //forward
        break;
      case 'b': //backward
        break;
      case 'l': //left
        break;
      case 'r': //right
        break;
      case 'c': //follow
        break;
      case 's': //stop
        break;
    }
  }
}

void leftEncoder() {
  leftCount++;
  distance++;
}

void rightEncoder() {
  rightCount++;
  distance++;
}

// void forward(int dist) {
//   digitalWrite(INA1, HIGH);
//   digitalWrite(INB1, HIGH);
//   digitalWrite(INA2, LOW);
//   digitalWrite(INB2, LOW);
//   if(leftCount > rightCount + 10) {
//     analogWrite(EN1, 128);
//     analogWrite(EN2, 192)
//   } else if(rightCount > leftCount + 10) {
//     analogWrite(EN1, 192);
//     analogWrite(EN2, 192);
//   }
  
// }