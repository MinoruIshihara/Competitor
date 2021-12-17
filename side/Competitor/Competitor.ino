#include "Competitor.h"

ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON);
LSM303 compass = LSM303();
Adafruit_TCS34725 tcs = Adafruit_TCS34725(
  TCS34725_INTEGRATIONTIME_2_4MS,
  TCS34725_GAIN_60X
);

int startTime = 0;
int cycleNum = 0;
int facingNum = 0;
int rotateNum = 0;

float thetaNext = 0.0;
float eSum = 0.0;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  initSonar();
  initColorSensor();
  initMagnetic();
  button.waitForButton();
  #ifdef DEBUG_MODE
  #else
  Serial.print("H0000000");
  Serial.write(0x0A);
  #endif  
  startTime = millis();
}

void loop() {
  static int mode = ROTATE_SEEK;
  
  static int prevTime = 0;
  static int deltaT = 0;
  
  static struct RGB_STRUCT rgb = {0, 0, 0};
  static double distance = 0;
  static double prevDistance = 0;
  static double radian = 0.0;

  deltaT = millis() - prevTime;
  prevTime = millis();
  
  rgb = getRGB();
  prevDistance = distance;
  distance = getDistance();
  radian = getRadian();
  
  #ifdef DEBUG_MODE
  Serial.print("mode: ");
  Serial.print(mode);
  Serial.print(", ");
  #endif

  switch(mode){
    case BACK:
    mode = backRun();
    break;
    
    case SEEK:
    mode = seekCup(distance, prevDistance, radian);
    break;
    
    case JUDGE:
    mode = judgeCup(distance);
    break;
    
    case FACE_RIGHT:
    mode = faceCupRight(distance, prevDistance, radian);
    break;
    
    case FACE_LEFT:
    mode = faceCupLeft(distance, prevDistance, radian);
    break;
    
    case TAKE:
    mode = takeCup(distance);
    break;

    case NEXT:
    mode = nextCup(distance, radian);
    break;

    case STOP:
    mode = stopMotor();
    break;

    case BRING:
    mode = bringCup(radian, rgb);
    break;

    case PUSH:
    mode = push();
    break;
    
    case ROTATE_SEEK:
    mode = rotateSeek(distance);
    break;
    
    case FACE_NEXT:
    mode = faceNext(radian, deltaT);
    break;
    
    case GO_NEXT_ROTATE:
    mode = goNextRotate(radian, deltaT);
    break;
  }

  #ifdef DEBUG_MOTOR
  Serial.print(", Right: ");
  Serial.print(motors.getRightSpeed());
  Serial.print(", Left: ");
  Serial.print(motors.getLeftSpeed());
  Serial.print(", ");
  #endif
  
  if(serialEvent()){
    sendAll(rgb, distance, radian, motors.getLeftSpeed(), motors.getRightSpeed(), mode);
  }
  #ifdef DEBUG_MODE
  Serial.println("");
  #endif
}

int backRun(){
  motors.setSpeeds(-400, -400);
  if(millis() - startTime > (2000 - (cycleNum / 2) * 500)){
    startTime = millis();
    return SEEK;
  }
  return BACK;
}

int seekCup(double distance, double prevDistance, double radian){
  int mode = SEEK;
  motors.setSpeeds(120, -120);
  if(millis() - startTime > 800 && 0 < radian && radian < 0.3 ){
    startTime = millis();
    mode = NEXT;
  }
  if(distance < 40 ){
    motors.setSpeeds(0, 0);
    delay(800);
    startTime = millis();
    mode = JUDGE;
  }
  return mode;
}

int judgeCup(double distance){
  int mode = JUDGE;
  facingNum = 0;
  if(distance > THRES_DISTANCE){
    mode = FACE_RIGHT;
  }
  else{
    mode = FACE_LEFT;
  }
  return mode;
}

int faceCupRight(double distance, double prevDistance, double radian){
  
  #ifdef DEBUG_SONOR
  Serial.print("deltaSita: ");
  Serial.print(distance - prevDistance);
  Serial.print(", ");
  #endif

  if(facingNum > 4)return NEXT;
  
  int mode = FACE_RIGHT;
  if(millis() - startTime < 1500){
    motors.setSpeeds(-90, 90);
    if(distance - prevDistance > 0 && distance < 40){
      startTime = millis();
      mode = TAKE;
    }
  }
  else{
    startTime = millis();
    mode = FACE_LEFT;
    facingNum ++;
  }
  return mode; 
}

int faceCupLeft(double distance, double prevDistance, double radian){
  
  #ifdef DEBUG_SONOR
  Serial.print("deltaSita: ");
  Serial.print(distance - prevDistance);
  Serial.print(", ");
  #endif
  
  if(facingNum > 4)return NEXT;

  int mode = FACE_LEFT;
  if(millis() - startTime < 1500){
    motors.setSpeeds(100, -100);
    if(distance - prevDistance > 0 && distance < 40){
      startTime = millis();
      mode = TAKE;
    }
  }
  else{
    startTime = millis();
    mode = FACE_RIGHT;
    facingNum ++;
  }
  return mode; 
}

int takeCup(double distance){
  int mode = TAKE;
  motors.setSpeeds(100, 100);
  if(distance < 3.5){
    startTime = millis();
    mode = BRING;
    motors.setSpeeds(0, 0);
    delay(1000);
  }
  return mode;
}

int nextCup(double distance, double radian){  
  int mode = NEXT;
  if(abs(radian) < 0.1){
    motors.setSpeeds(200, 200);    
  }
  else if(abs(radian) > 1.5){
    motors.setSpeeds(-radian * 60 , radian * 60 );
  }
  else{
    motors.setSpeeds( -radian * 160 + 80, radian * 160 + 80);
  }
  if(millis() - startTime > 3000){
    startTime = millis();
    mode = SEEK;
  }
  return mode;
}

int stopMotor(){
  int mode = STOP;
  motors.setSpeeds(0, 0);
  return mode;
}

int bringCup(double radian, RGB_STRUCT rgb){
  int mode = BRING;
  if(abs(radian) < 0.1){
    motors.setSpeeds(200, 200);    
  }
  else if(abs(radian) > 1.5){
    motors.setSpeeds( -radian * 60 + 40, radian * 60 + 40);
  }
  else{
    if(radian < 0)motors.setSpeeds( -(radian - 0.4) * 150 , (radian - 0.4) * 100 );
    else motors.setSpeeds( -(radian + 0.4) * 100 , (radian + 0.4) * 150 );
  }
  #ifdef TEAM_RED
  if( identify_color( rgb, RED_RGB ) ){
    mode = PUSH;
  }
  #else
  if( identify_color( rgb, 40, 80, 110 ) ){
    mode = PUSH;
  }
  #endif
  return mode;
}

int push(){
  int mode = PUSH;
  motors.setSpeeds(100, 100);
  if(millis() - startTime > 1000){
    motors.setSpeeds(-100, -100);
  }
  else if(millis() - startTime > 2000){    
    startTime = millis();
    cycleNum ++;
    mode = ROTATE_SEEK;
  }
  return mode;
}

int rotateSeek(double distance){
  int mode = ROTATE_SEEK;
  if(distance < 40 ){
    motors.setSpeeds(0, 0);
    delay(800);
    startTime = millis();
    mode = JUDGE;
  }
  if (millis() - startTime < 3000) {
    motors.setSpeeds(150, -150);
  }
  else {
    mode = FACE_NEXT;
    rotateNum++;
    double thetaNext =  - PI / 3 + 2 * PI / 3 * (rotateNum % 2);
    if (thetaNext < -PI) thetaNext += 2 * PI; // 角度を0°~360°に収める
    if (thetaNext > PI) thetaNext -= PI;
  }
  return mode;
}

int faceNext(double radian, int deltaT){
  if (abs(thetaNext - radian) <= PI / 36 ) {
    return GO_NEXT_ROTATE;
  }
  
  float u = faceTo(thetaNext, radian, deltaT);
  
  motors.setSpeeds(u * 60, - u * 60);
  
  return FACE_NEXT;
}

int goNextRotate(double radian, int deltaT){
  int mode = GO_NEXT_ROTATE;
  float u = faceTo(thetaNext, radian, deltaT);
  motors.setSpeeds(u * 55 + 150, - u * 55 + 150);
  
  if (millis() - startTime > 1500) {
    mode = ROTATE_SEEK;
    startTime = millis();
  }

  return mode;
}

float faceTo(float nextRadian, double radian , int deltaT){
  float e = nextRadian - radian;
  float u;
  float KP = 4.0;
  float TIinv = 2/1000.0;
  
  if (e < -PI ) e += 2 * PI;
  if (e > PI )  e -= 2 * PI;
  if (abs(e) > 45.0 ) { // |e|>45のときはP制御
    u = KP*e;           // P制御
  } else {              // |e|<=45 の時はPI制御
    eSum += TIinv * e * (deltaT);
    u = KP * ( e + eSum );   // PI 制御
  }
  if ( u > PI ) u = PI;  // 飽和
  if ( u < -PI ) u = -PI; // 飽和

  return u;
}
