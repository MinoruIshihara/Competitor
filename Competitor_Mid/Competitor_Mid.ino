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

    case WAVE_L:
    mode = waveL(distance, radian);
    break;

    case WAVE_R:
    mode = waveR(distance, radian);
    break;

    case TURN:
    mode = turn(prevRadian, radian);
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

int takeCup(float distance){
  int power = distance * 5 + 75;
  motors.setSpeeds(power, power);
  if(distance < 3.5){
    motors.setSpeeds(0, 0);
    delay(300);
    startTime = millis();
    return BRING;
  }
  return TAKE;
}

int bringCup(double radian, RGB_STRUCT rgb){
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
  
  if( identify_color( rgb, initRGB ) ){
    startTime = millis();
    return PUSH;
  }
  return BRING;
}

int push(){
  motors.setSpeeds(100, 100);
  if(millis() - startTime > 1000){
    startTime = millis();
    cycleNum ++;
    return BACK;
  }
  return PUSH;
}

int nextCup(double distance, double radian){
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
    return SEEK;
  }
  return NEXT;
}

int waveL(double distance, double angle){
  motors.setSpeeds(80, 150);

  if(millis() - startTime > 1500){
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

int waveR(double distance, double angle){
  motors.setSpeeds(150, 80);
  if(millis() - startTime > 1500){
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

int turn(float prevRadian, float radian){  
  if( faceDirect(radian, -colisionDirect, deltaT) ){
    startTime = currentTime;
    return WAVE_R;
  }
  return TURN;
}

int stopMotor(){
  motors.setSpeeds(0, 0);
  return STOP;
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
  if( u > PI ) u = PI;  // 飽和
  if( u < -PI ) u = -PI; // 飽和
  motors.setSpeeds(u * 50, -u * 50);
  return ( 0.08 < e && e < 0.08 );
}
