#include "Competitor.h"

ZumoBuzzer buzzer;
ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON);
LSM303 compass = LSM303();
Adafruit_TCS34725 tcs = Adafruit_TCS34725(
  TCS34725_INTEGRATIONTIME_2_4MS,
  TCS34725_GAIN_60X
);

int startTime = 0;
int currentTime = 0;
int prevTime = 0;
int deltaT = 0;

float sumE = 0.0;
int cycleNum = 0;
int facingCount = 0;
int rotateNum = 0;
int moveCount = 0;
float rotateEndDirect = - PI / 2;

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
  #ifdef SERIAL_TEST
  static int mode = STOP;
  #else
  static int mode = ORIGIN;
  #endif
    
  static struct RGB_STRUCT rgb = {0, 0, 0};
  static float distance = 0.0;
  static float prevDistance = 0.0;
  static float radian = 0.0;
  static float prevRadian = 0.0;
  
  prevTime = currentTime;
  currentTime = millis();
  deltaT = currentTime - prevTime;
  
  rgb = getRGB();
  prevDistance = distance;
  distance = getDistance();
  prevRadian = radian;
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
    mode = seekCup(distance, prevRadian, radian);
    break;
    
    case FACE_NEXT:
    mode = faceNext(prevRadian, radian);
    break;
    
    case NEXT:
    mode = goNext(rgb);
    break;

    case TURN_DOWN:
    mode = turnFaceDown(prevRadian, radian);
    break;
    
    case TURN_GO:
    mode = turnGoDown(radian);
    break;
            
    case JUDGE:
    mode = judge(distance);
    break;
    
    case FACE_R:
    mode = faceR(distance, prevDistance, radian);
    break;
    
    case FACE_L:
    mode = faceL(distance, prevDistance, radian);
    break;

    case CHECK:
    mode = moveCheck(prevDistance, distance);
    break;
    
    case TAKE:
    mode = takeCup(distance);
    break;
    
    case BRING:
    mode = bringCup(radian, rgb);
    break;
    
    case PUSH:
    mode = push();
    break;

    case ORIGIN:
    mode = faceOrigin(prevRadian, radian);
    break;
    
    case STOP:
    mode = stopMotor();
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
  if(currentTime - startTime > 300){
    startTime = currentTime;
    return SEEK;
  }
  return BACK;
}

int seekCup(float distance, float prevRadian, float radian){
  if(distance < 40 ){
    motors.setSpeeds(0, 0);
    delay(200);
    startTime = currentTime;
    return JUDGE;
  }
  if(currentTime - startTime > 1000 && 
      prevRadian < rotateEndDirect && 
      rotateEndDirect < radian){
    motors.setSpeeds(0, 0);
    playSound(1);
    delay(200);
    startTime = currentTime;
    return FACE_NEXT;
  }
  motors.setSpeeds(120, -120);
  
  return SEEK;
}

int faceNext(float prevRadian, float radian){
  if( faceDirect(radian, rotateEndDirect, deltaT) ){
    startTime = millis();
    rotateNum++;
    return NEXT;
  }
  return FACE_NEXT;
}

int goNext(RGB_STRUCT rgb){
  if( identify_color( rgb, BLACK_RGB ) ){
    playSound(1);
    rotateEndDirect = - rotateEndDirect;
    startTime = currentTime;
    return TURN_DOWN;
  }
  
  if (currentTime - startTime > 2000) {
    startTime = currentTime;
    return SEEK;
  }
  motors.setSpeeds( 150, 150);

  return NEXT;
}

int turnFaceDown(float prevRadian, float radian){
  if( faceDirect(radian, 0, deltaT) ){
    startTime = millis();
    rotateNum++;
    return TURN_GO;
  }
  return TURN_DOWN;
}

int turnGoDown(float radian){
  motors.setSpeeds(100, 100);
  if(currentTime - startTime > 2000){
    startTime = currentTime;
    return FACE_NEXT;
  }
  return TURN_GO;
}

int judge(float distance){
  facingCount = 0;
  if(distance < THRES_DISTANCE){
    startTime = currentTime;
    return FACE_L;
  }
  else{
    startTime = currentTime;
    return FACE_R;
  }
}

int faceL(float distance, float prevDistance, float radian){    
  if(facingCount > 4){
    startTime = currentTime;
    return SEEK;
  }
  
  if(distance - prevDistance > 0 && distance < 40){
    startTime = currentTime;
    moveCount = 0;
    return CHECK;
  }
  motors.setSpeeds(-90, 90);
  
  if(currentTime - startTime > 1500){
    startTime = currentTime;
    facingCount ++;
    return FACE_R;
  }
  
  return FACE_L; 
}

int faceR(float distance, float prevDistance, float radian){
  if(facingCount > 4){
    startTime = currentTime;
    return SEEK;
  }
  
  if(distance - prevDistance > 0 && distance < 40){
    startTime = currentTime;
    moveCount = 0;
    return CHECK;
  }
  motors.setSpeeds(90, -90);
  
  if(currentTime - startTime > 1500){
    startTime = currentTime;
    facingCount ++;
    return FACE_L;
  }
  
  return FACE_R; 
}

int moveCheck(float prevDistance, float distance){
  motors.setSpeeds(0, 0);
  if (prevDistance - distance < - 0.1 || 0.1 < prevDistance - distance){ //距離が変化しているなら
    moveCount++; //回数を加算
  }
  if (currentTime - startTime > 1000) { //関数一回目起動から1秒以内なら
    startTime = currentTime;
    return CHECK_COUNT < moveCount? SEEK : TAKE;
  }
  return CHECK;
}

int takeCup(float distance){
  int power = distance * 5 + 75;
  motors.setSpeeds(power, power);
  if(distance < 3.5){
    motors.setSpeeds(0, 0);
    delay(300);
    startTime = currentTime;
    return BRING;
  }
  return TAKE;
}

int bringCup(float radian, RGB_STRUCT rgb){
  if(-0.1 < radian && radian < 0.1){
    motors.setSpeeds(200, 200);
  }
  else if(-1.5 < radian && radian < 1.5){
    if(radian < 0)motors.setSpeeds( -(radian - 0.4) * 150 , (radian - 0.4) * 100 );
    else motors.setSpeeds( -(radian + 0.4) * 100 , (radian + 0.4) * 150 );
  }
  else{
    motors.setSpeeds( -radian * 60 + 40, radian * 60 + 40);
  }
  
  if( identify_color( rgb, initRGB ) ){
    startTime = currentTime;
    return PUSH;
  }
  return BRING;
}

int push(){
  if(currentTime - startTime < 1000){
    motors.setSpeeds(100, 100);
  }
  else if(currentTime - startTime < 2000){
    motors.setSpeeds(-100, -100);
  }
  else {
    startTime = currentTime;
    cycleNum ++;
    return ORIGIN;
  }
  return PUSH;
}

int stopMotor(){
  motors.setSpeeds(0, 0);
  return STOP;
}

int faceOrigin(float prevRadian, float radian){
  if( faceDirect(radian, 0, deltaT) ){
    startTime = currentTime;
    rotateNum = 0;
    rotateEndDirect = - PI / 2;
    return BACK;
  }
  return ORIGIN;
}

bool faceDirect(float current, float obj, int deltaT){
  float u;
  float KP = 4.0;
  float TIinv = 2/1000.0;
  float e = obj-current;
  if(e < -PI) e += 2 * PI;
  if(e > PI) e -= 2 * PI;
  if( -PI < e && e < PI / 2 ) {
    u = KP * e;           // P制御
  }
  else {              // |e|<=45 の時はPI制御
    sumE += e * TIinv * deltaT;
    u = KP * ( e + sumE);   // PI 制御
  }
  Serial.println(e);
  if(e > 0){
    u = KP * (e + 0.4);
  }
  else{
    u = KP * (e - 0.4);
  }
  if( u > PI ) u = PI;  // 飽和
  if( u < -PI ) u = -PI; // 飽和
  motors.setSpeeds(u * 50, -u * 50);
  return ( -0.07 < e && e < 0.07 );
}

bool faceDirectFast(float prev, float current, float obj){
  float u;
  float KP = 55;
  float e = obj-current;
  if(e < -PI) e += 2 * PI;
  if(e > PI) e -= 2 * PI;
  if(e > 0){
    if(prev > obj)return true;
    u = KP * (e + 0.3);
  }
  else{
    if(prev < obj)return true;
    u = KP * (e - 0.3);
  }
  motors.setSpeeds(u, -u);
  return false;
}
