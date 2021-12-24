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
int waveCount = 0;
float colisionDirect = 0.0;
float waveDirect = PI / 2;
float revertDirect = 0;
int originalMode = SEEK;

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
  static int mode = RUN;
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
    case ORIGIN:
    mode = faceOrigin(prevRadian, radian);
    break;

    case RUN:
    mode = runFront(distance);
    break;
    
    case BACK:
    mode = backRun();
    break;
    
    case SEEK:
    mode = seekCup(distance, prevRadian, radian);
    break;

    case WAVE_L:
    mode = waveL(distance, radian, rgb);
    break;

    case WAVE_R:
    mode = waveR(distance, radian, rgb);
    break;

    case JUDGE_L:
    mode = judgeL(distance);
    break;
    
    case JUDGE_R:
    mode = judgeR(distance);
    break;

    case FACE_R:
    mode = faceR(distance, prevDistance, radian);
    break;
    
    case FACE_L:
    mode = faceL(distance, prevDistance, radian);
    break;
    
    case TAKE:
    mode = takeCup(rgb, distance, radian);
    break;

    case REVERT_FACE:
    mode = revertFace(radian);
    break;
    
    case REVERT_RUN:
    mode = revertRun();
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

//======================FACE TO ORIGINAL DIRECT======================

int faceOrigin(float prevRadian, float radian){
  float direct = cycleNum == 0? -PI : 0;
  if( faceDirect(radian, direct, deltaT, 0) ){
    startTime = currentTime;
    return cycleNum == 0? RUN : BACK;
  }
  return ORIGIN;
}

//====================RUN FORWARD=====================

int runFront(float distance){  
  motors.setSpeeds(400, 400);
  if(millis() - startTime > RUN_TIME){
    startTime = millis();
    return SEEK;
  }
  return RUN;
}

//====================RUN BACKWARD=====================

int backRun(){
  motors.setSpeeds(-400, -400);
  if(millis() - startTime > (2000 - (cycleNum / 2) * 500)){
    startTime = millis();
    return SEEK;
  }
  return BACK;
}

//==============ROTATE AND SEARCH A CUP===============

int seekCup(double distance, double prevRadian, double radian){  
  if(distance < 40 ){
    motors.setSpeeds(0, 0);
    delay(200);
    startTime = millis();
    return JUDGE_R;
  }
  if(millis() - startTime > 2000 && prevRadian < 1.5 && 1.5 < radian){
    motors.setSpeeds(0, 0);
    delay(200);
    startTime = millis();
    return WAVE_R;
  }
  motors.setSpeeds(120, -120);
  
  return SEEK;
}

//=========THE CUP IS IN FRONT OF THE ROBOT OR NOT=========

int judgeL(double distance){
  facingCount = 0;
  if(distance < THRES_DISTANCE){
    startTime = millis();
    return FACE_L;
  }
  else{
    startTime = millis();
    return FACE_R;
  }
}

//=========THE CUP IS IN FRONT OF THE ROBOT OR NOT=========

int judgeR(double distance){
  facingCount = 0;
  if(distance < THRES_DISTANCE){
    startTime = millis();
    return FACE_R;
  }
  else{
    startTime = millis();
    return FACE_L;
  }
}

//=================TURN LEFT AND SEARCH CUP=================

int faceL(double distance, double prevDistance, double radian){    
  if(facingCount > 4){
    startTime = millis();
    return SEEK;
  }
  
  if(distance - prevDistance > 0 && distance < 40){
    startTime = millis();
    return TAKE;
  }
  motors.setSpeeds(-90, 90);
  
  if(millis() - startTime > 1500){
    startTime = millis();
    facingCount ++;
    return FACE_R;
  }
  
  return FACE_L; 
}

//=================TURN RIGHT AND SEARCH CUP=================

int faceR(double distance, double prevDistance, double radian){
  if(facingCount > 4){
    startTime = millis();
    return SEEK;
  }
  
  if(distance - prevDistance > 0 && distance < 40){
    startTime = millis();
    return TAKE;
  }
  motors.setSpeeds(90, -90);
  
  if(millis() - startTime > 1500){
    startTime = millis();
    facingCount ++;
    return FACE_L;
  }
  
  return FACE_R; 
}

//===========================TAKE CUP===========================

int takeCup(RGB_STRUCT rgb, float distance, float radian){
  if( identify_color( rgb, BLACK_RGB ) ){
    playSound(1);
    startTime = currentTime;
    revertDirect = radian < 0 ? PI / 2 : - PI / 2;
    originalMode = SEEK;
    return REVERT_FACE;
  }
  
  int power = distance * 5 + 75;
  motors.setSpeeds(power, power);
  if(distance < 4){
    motors.setSpeeds(0, 0);
    delay(300);
    startTime = millis();
    playSound(1);
    return BRING;
  }
  return TAKE;
}

//=====================BRING CUP TO THE BASE=====================

int bringCup(double radian, RGB_STRUCT rgb){
  if( identify_color( rgb, BLACK_RGB ) ){
    playSound(1);
    startTime = currentTime;
    revertDirect = radian < 0 ? PI / 2 : - PI / 2;
    originalMode = BRING;
    return REVERT_FACE;
  }
  
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
    startTime = millis();
    return PUSH;
  }
  return BRING;
}

//===================REVERT IF ON THE BLCK LINE===================

int revertFace(float radian){
  if( faceDirect(radian, revertDirect, deltaT, 0) ){
    startTime = millis();
    return REVERT_RUN;
  }
  return REVERT_FACE;
}

int revertRun(){
  motors.setSpeeds(200, 200);
  if(currentTime - startTime > 1500)return originalMode;  
  return REVERT_RUN;
}

//======================PUSH AND PUT IN THE CUP======================

int push(){
  if(currentTime - startTime < 500){
    motors.setSpeeds(100, 100);
  }
  else if(currentTime - startTime < 1000){
    motors.setSpeeds(-100, -100);
  }
  else {
    startTime = currentTime;
    cycleNum ++;
    return ORIGIN;
  }
  return PUSH;
}

//==================WAVING RUN (LEFT)==================

int waveL(double distance, double angle, struct RGB_STRUCT rgb){
  if( identify_color( rgb, BLACK_RGB ) ){
    waveDirect = - waveDirect;
    revertDirect = waveDirect;
    originalMode = WAVE_L;
    return REVERT_FACE;
  }
  
  if( faceDirect(angle, waveDirect * 0.5, deltaT, 150) ){
    waveCount ++;
    startTime = millis();
    return WAVE_R;
  }
  
  if(distance < 40 ){
    motors.setSpeeds(0, 0);
    delay(800);
    startTime = millis();
    return JUDGE_L;
  }
  return WAVE_L;
}

//==================WAVING RUN (RIGHT)==================

int waveR(double distance, double angle, struct RGB_STRUCT rgb){
  if( identify_color( rgb, BLACK_RGB ) ){
    waveDirect = - waveDirect;
    revertDirect = waveDirect;
    originalMode = WAVE_R;
    return REVERT_FACE;
  }
  
  if( faceDirect(angle, waveDirect * 1.5, deltaT, 150) ){
    waveCount ++;
    startTime = millis();
    return WAVE_L;
  }

  if(distance < 40 ){
    motors.setSpeeds(0, 0);
    delay(800);
    startTime = millis();
    return JUDGE_R;
  }
  return WAVE_R;
}

//======================JUST STOP======================

int stopMotor(){
  motors.setSpeeds(0, 0);
  return STOP;
}

//=================FACE SPECIFIC DIRECT=================

bool faceDirect(float current, float obj, int deltaT,int offsetSpeed){
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
  motors.setSpeeds(u * 50 + offsetSpeed, -u * 50 + offsetSpeed);
  return ( -0.07 < e && e < 0.07 );
}
