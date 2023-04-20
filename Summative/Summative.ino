#include <HUSKYLENS.h>
#include <SoftwareSerial.h>
#include <Servo.h>

//Serial read // bluetooth module
char value;
char state;
int btTX = 1;
int btRX = 0;
//Motor control
int EN1 = 5;    //pwm digital (9 OR 10)
int INA1 = 8;   //digital
int INA2 = 7;   //digital
int EN2 = 6;   //pwm digital (9 OR 10)
int INB1 = 11;  //digital
int INB2 = 4;   //digital
//Servos
Servo cameraControl;  //between 75 and 120
Servo claw;           //between 0 and 75
//bool clawOpen = false;
//bool camUp = false;
int servo1 = 9;       //camera
int servo2 = 10;      //claw
//Encoders
int encoder1 = 2;  //digital with interrupt (2 OR 3)
int encoder2 = 3;  //digital with interrupt (2 OR 3)
//Husky object
int leftBounds = 140;
int rightBounds = 180;
int distComp = 6;
int turnComp = 13;
int objectWidth = 75;
int objectMax = 120;
int faceWidth = 20;
HUSKYLENS lens;
int ID1 = 1;
//IRsensor
int IRSensor = 13;

void printResult(HUSKYLENSResult result);

volatile int leftCount = 0;
volatile int rightCount = 0;
volatile int distance = 0;

SoftwareSerial BTSerial(1, 0);

void setup() {
  // Huskylens setup
  //Serial.begin(115200);
  Wire.begin();
  while(!lens.begin(Wire)){
    Serial.println(F("Failed to begin"));
    delay(100);
  }
  lens.writeAlgorithm(ALGORITHM_OBJECT_TRACKING);

  // Other setup
  pinMode(encoder1, INPUT);
  pinMode(encoder2, INPUT);
  pinMode(IRSensor, INPUT);
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
  // Get the current bluetooth command
  bluetooth();

  // Run commands based on signal
  switch (value) {
    case 'F':  //forward
      forward();
      break;
    case 'B':  //backward
      backward();
      break;
    case 'L':  //leftTurn
      leftTurn();
      break;
    case 'R':  //rightTurn
      rightTurn();
      break;
    case 'P': //leftPivot
      leftPivot();
      break;
    case 'Q': //rightPivot
      rightPivot();
      break;
    case 'U': //lookUp
      lookUp();
      break;
    case 'D':  //lookDown
      lookDown();
      break;
    case 'O':  //openGrip
      openGrip();
      break;
    case 'C': //closeGrip
      closeGrip();
      break;
    case 'S':  //stop
      stop();
      break;
    case 'X': // Track object
      lookDown();
      if (!lens.request()) {
        //Serial.println(F("Fail to request data from HUSKYLENS, recheck the connection!"));
      } else if (!lens.isLearned()) {
        //Serial.println(F("Nothing learned, press learn button on HUSKYLENS to learn one!"));
      } else if (!lens.available()) {
        stop();
        delay(500);
        leftPivot();
        //Serial.println(F("No block or arrow appears on the screen!"));
      } else {
        HUSKYLENSResult result = lens.read();
        if (result.width < objectWidth) {
          forward();
        } else if (result.xCenter < leftBounds) {
          rightTurn();
        } else if (result.xCenter > rightBounds) {
          leftTurn();
        }

      if (digitalRead(IRSensor) == LOW){
        closeGrip();
        leftPivot();
        delay(2000);
        stop();
      }
        //printResult(result);
      }

      break;
    case 'Z': // Track Face
      lookUp();
      if (!lens.request()) {
          //Serial.println(F("Fail to request data from HUSKYLENS, recheck the connection!"));
        } else if (!lens.isLearned()) {
          //Serial.println(F("Nothing learned, press learn button on HUSKYLENS to learn one!"));
        } else if (!lens.available()) {
          stop();
          delay(500);
        leftPivot();
          //Serial.println(F("No block or arrow appears on the screen!"));
        } else {
          HUSKYLENSResult result = lens.read();
          if (result.width < faceWidth) {
            forward();
          } else if (result.xCenter < leftBounds - 40) {
            rightTurn();
          } else if (result.xCenter > rightBounds + 40) {
            leftTurn();
          }
        }
        break;
  }
}

// void printResult(HUSKYLENSResult result)                        //Display the results on the serial monitor
// {
//     if (result.command == COMMAND_RETURN_BLOCK)
//     {
//         Serial.println(String()+F("Block:xCenter=")+result.xCenter+F(",yCenter=")+result.yCenter+F(",width=")+result.width+F(",height=")+result.height+F(",ID=")+result.ID);
//     }
//     else if (result.command == COMMAND_RETURN_ARROW)
//     {
//         Serial.println(String()+F("Arrow:xOrigin=")+result.xOrigin+F(",yOrigin=")+result.yOrigin+F(",xTarget=")+result.xTarget+F(",yTarget=")+result.yTarget+F(",ID=")+result.ID);
//     }
//     else
//     {
//         Serial.println("Object unknown!");
//     }
// }

void bluetooth() {
  if (Serial.available() > 0) {
    value = Serial.read();
    Serial.println(value);
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

// Helper Functions ----------------------------------------
void forward() {
  digitalWrite(INA1, HIGH);
  digitalWrite(INB1, HIGH);
  digitalWrite(INA2, LOW);
  digitalWrite(INB2, LOW);
  if(leftCount > (rightCount * 1.23) + 10) {
    analogWrite(EN1, 158);
    analogWrite(EN2, 192);
  } 
  else if(rightCount > leftCount + 10) {
    analogWrite(EN1, 192);
    analogWrite(EN2, 128);
  } 
  else{
    analogWrite(EN1, 158);
    analogWrite(EN2, 128);
  }

}

void backward() {
  digitalWrite(INA1, LOW);
  digitalWrite(INB1, LOW);
  digitalWrite(INA2, HIGH);
  digitalWrite(INB2, HIGH);
  if(leftCount > (rightCount * 1.23) + 10) {
    analogWrite(EN1, 158);
    analogWrite(EN2, 192);
  } 
  else if(rightCount > leftCount + 10) {
    analogWrite(EN1, 192);
    analogWrite(EN2, 128);
  } 
  else{
    analogWrite(EN1, 158);
    analogWrite(EN2, 128);
  }
}

void leftTurn(){
  digitalWrite(INA1, HIGH);
  digitalWrite(INB1, HIGH);
  digitalWrite(INA2, LOW);
  digitalWrite(INB2, LOW);
  if(leftCount > (rightCount / 1.5) + 10){
    analogWrite(EN1, 98);
    analogWrite(EN2, 256);
  }
  else if(leftCount < (rightCount / 1.5) - 10){
    analogWrite(EN1, 158);
    analogWrite(EN2, 256);
  }
  else{
    analogWrite(EN1, 128);
    analogWrite(EN2, 256);
  }
}

void rightTurn(){
  digitalWrite(INA1, HIGH);
  digitalWrite(INB1, HIGH);
  digitalWrite(INA2, LOW);
  digitalWrite(INB2, LOW);
  if(rightCount > (leftCount / 1.5) + 10){
    analogWrite(EN1, 256);
    analogWrite(EN2, 98);
  }
  else if(rightCount < (leftCount / 1.5) - 10){
    analogWrite(EN1, 256);
    analogWrite(EN2, 158);
  }
  else{
    analogWrite(EN1, 256);
    analogWrite(EN2, 128);
  }
}

void leftPivot(){
  digitalWrite(INA1, LOW);
  digitalWrite(INA2, HIGH);
  digitalWrite(INB1, HIGH);
  digitalWrite(INB2, LOW);
  analogWrite(EN1, 128);
  analogWrite(EN2, 98);
}

void rightPivot(){
  digitalWrite(INA1, HIGH);
  digitalWrite(INA2, LOW);
  digitalWrite(INB1, LOW);
  digitalWrite(INB2, HIGH);
  analogWrite(EN1, 128);
  analogWrite(EN2, 98);
}

void stop(){
  digitalWrite(INA1, LOW);
  digitalWrite(INA2, LOW);
  digitalWrite(INB1, LOW);
  digitalWrite(INB2, LOW);
  analogWrite(EN1, 0);
  analogWrite(EN2, 0);
}

void lookUp(){
  cameraControl.write(120);
  //camUp = true;
}

void lookDown(){
  cameraControl.write(65);
  //camUp = false;
}

void openGrip(){
  claw.write(0);
  //clawOpen = true;
}

void closeGrip(){
  claw.write(75);
  //clawOpen = false;
}