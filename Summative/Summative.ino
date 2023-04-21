#include <HUSKYLENS.h>
#include <SoftwareSerial.h>
#include <Servo.h>

//Serial read // bluetooth module
char value;
char state;
int btTX = 1;
int btRX = 0;
//Motor control
int EN1 = 5;       //pwm digital (9 OR 10)
int INA1 = 8;      //digital
int INA2 = 7;      //digital
int EN2 = 6;       //pwm digital (9 OR 10)
int INB1 = 11;     //digital
int INB2 = 4;      //digital
int speedL = 140;  //speed
int speedR = 112;  //speed
//Servos
Servo cameraControl;  //between 75 and 120
Servo claw;           //between 0 and 75
//bool clawOpen = false;
//bool camUp = false;
int servo1 = 9;   //camera
int servo2 = 10;  //claw
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
bool seenObj = false;
bool seenFace = false;
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
  while (!lens.begin(Wire)) {
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

  if(value != state){
    leftCount = 0;
    rightCount = 0;
  }

  // Run commands based on signal
  switch (value) {
    case 'F':  //forward
      forward();
      seenObj = false;
      seenFace = false;
      break;
    case 'B':  //backward
      backward();
      seenObj = false;
      seenFace = false;
      break;
    case 'L':  //leftTurn
      leftTurn();
      seenObj = false;
      seenFace = false;
      break;
    case 'R':  //rightTurn
      rightTurn();
      seenObj = false;
      seenFace = false;
      break;
    case 'P':  //leftPivot
      leftPivot();
      seenObj = false;
      seenFace = false;
      break;
    case 'Q':  //rightPivot
      rightPivot();
      seenObj = false;
      seenFace = false;
      break;
    case 'U':  //lookUp
      lookUp();
      seenObj = false;
      seenFace = false;
      break;
    case 'D':  //lookDown
      lookDown();
      seenObj = false;
      seenFace = false;
      break;
    case 'O':  //openGrip
      openGrip();
      seenObj = false;
      seenFace = false;
      break;
    case 'C':  //closeGrip
      closeGrip();
      seenObj = false;
      seenFace = false;
      break;
    case 'S':  //stop
      stop();
      seenObj = false;
      seenFace = false;
      break;
    case 'X':  // Track object
      lookDown();
      if (!lens.request()) {
        //Serial.println(F("Fail to request data from HUSKYLENS, recheck the connection!"));
      } else if (!lens.isLearned()) {
        //Serial.println(F("Nothing learned, press learn button on HUSKYLENS to learn one!"));
      } else if (!lens.available()) {
        if (seenObj) { leftPivot(); }
        //Serial.println(F("No block or arrow appears on the screen!"));
      } else {
        seenObj = true;
        openGrip();
        HUSKYLENSResult result = lens.read();
        if(result.yCenter > 160){
          lookDownPlus();
        }
        if (result.xCenter < leftBounds - 40) {
          leftTurn();
        } else if (result.xCenter > rightBounds + 40) {
          rightTurn();
        } else if (result.width < objectWidth) {
          forward();
        }

        if (digitalRead(IRSensor) == LOW) {
          closeGrip();
          stop();
          seenObj = false;
        }
        //printResult(result);
      }

      break;
    case 'Z':  // Track Face
      lookUp();
      if (!lens.request()) {
        //Serial.println(F("Fail to request data from HUSKYLENS, recheck the connection!"));
      } else if (!lens.isLearned()) {
        //Serial.println(F("Nothing learned, press learn button on HUSKYLENS to learn one!"));
      } else if (!lens.available()) {
        if (seenFace) { leftPivot(); }
        //Serial.println(F("No block or arrow appears on the screen!"));
      } else {
        seenFace = true;
        HUSKYLENSResult result = lens.read();

        if(result.yCenter > 160){
          lookDownPlus();
        }
        if (result.xCenter < leftBounds - 60) {
          leftTurn();
        } else if (result.xCenter > rightBounds + 60) {
          rightTurn();
        } else if (result.width < objectWidth) {
          forward();
        }
      }
      break;
  }
  state = value;
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
  if (leftCount > (rightCount * 1.20) + 10) {
    analogWrite(EN1, speedL);
    analogWrite(EN2, speedR * 1.1);
  } else if (rightCount > leftCount + 10) {
    analogWrite(EN1, speedL * 1.1);
    analogWrite(EN2, speedR);
  } else {
    analogWrite(EN1, speedL);
    analogWrite(EN2, speedR);
  }
}

void backward() {
  digitalWrite(INA1, LOW);
  digitalWrite(INB1, LOW);
  digitalWrite(INA2, HIGH);
  digitalWrite(INB2, HIGH);
  if (leftCount > (rightCount * 1.20) + 10) {
    analogWrite(EN1, speedL);
    analogWrite(EN2, speedR * 1.1);
  } else if (rightCount > leftCount + 10) {
    analogWrite(EN1, speedL * 1.1);
    analogWrite(EN2, speedR);
  } else {
    analogWrite(EN1, speedL);
    analogWrite(EN2, speedR);
  }
}

void leftTurn() {
  digitalWrite(INA1, HIGH);
  digitalWrite(INB1, HIGH);
  digitalWrite(INA2, LOW);
  digitalWrite(INB2, LOW);
  // if (leftCount > (rightCount / 1.5) + 200) {
  //   analogWrite(EN1, speedL * 0.5);
  //   analogWrite(EN2, speedR * 1.4);
  // } else if (leftCount < (rightCount / 1.5) - 20) {
  //   analogWrite(EN1, speedL * 1.4);
  //   analogWrite(EN2, speedR * 0.5);
  // } else {
    analogWrite(EN1, speedL * 0.6);
    analogWrite(EN2, speedR * 1.4);
  //}
}

void rightTurn() {
  digitalWrite(INA1, HIGH);
  digitalWrite(INB1, HIGH);
  digitalWrite(INA2, LOW);
  digitalWrite(INB2, LOW);
  // if (rightCount > (leftCount / 1.5) + 2) {
  //   analogWrite(EN1, speedL * 1.4);
  //   analogWrite(EN2, speedR * 0.5);
  // } else if (rightCount < (leftCount / 1.5) - 20) {
  //   analogWrite(EN1, speedL * 0.5);
  //   analogWrite(EN2, speedR * 1.4);
  // } else {
    analogWrite(EN1, speedL * 1.4);
    analogWrite(EN2, speedR * 0.6);
  //}
}

void leftPivot() {
  digitalWrite(INA1, LOW);
  digitalWrite(INA2, HIGH);
  digitalWrite(INB1, HIGH);
  digitalWrite(INB2, LOW);
  analogWrite(EN1, speedL);
  analogWrite(EN2, speedR);
}

void rightPivot() {
  digitalWrite(INA1, HIGH);
  digitalWrite(INA2, LOW);
  digitalWrite(INB1, LOW);
  digitalWrite(INB2, HIGH);
  analogWrite(EN1, speedL);
  analogWrite(EN2, speedR);
}

void stop() {
  digitalWrite(INA1, LOW);
  digitalWrite(INA2, LOW);
  digitalWrite(INB1, LOW);
  digitalWrite(INB2, LOW);
  analogWrite(EN1, 0);
  analogWrite(EN2, 0);
}

void lookUp() {
  cameraControl.write(120);
  //camUp = true;
}

void lookDown() {
  cameraControl.write(65);
  //camUp = false;
}

void lookDownPlus() {
  cameraControl.write(30);
  //camUp = false;
}

void openGrip() {
  claw.write(0);
  //clawOpen = true;
}

void closeGrip() {
  claw.write(60);
  //clawOpen = false;
}