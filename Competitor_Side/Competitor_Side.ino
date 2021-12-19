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
int cycleNum = 0;
int facingCount = 0;
int rotateNum = 0;
float nextDirect = 0.0;
float rotateEndDirect = 0.0;

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
  static int mode = BACK;
  
  static int prevTime = 0;
  static int deltaT = 0;
  
  static struct RGB_STRUCT rgb = {0, 0, 0};
  static float distance = 0.0;
  static float prevDistance = 0.0;
  static float radian = 0.0;
  static float prevRadian = 0.0;
  
  deltaT = millis() - prevTime;
  prevTime = millis();
  
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
    
    case BACK_2:
    mode = back2();
    break;

    case SEEK:
    mode = seekCup(distance, prevRadian, radian);
    break;
    
    case FACE_NEXT:
    mode = faceNext(prevRadian, radian);
    break;
    
    case NEXT:
    mode = goNext();
    break;
    
    case NEXT_BACK:
    mode = goNextBack();
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
  if(millis() - startTime > 2000){
    startTime = millis();
    return SEEK;
  }
  return BACK;
}

int back2(){
  motors.setSpeeds(-200, -200);
  if(millis() - startTime > 1000){
    startTime = millis();
    return SEEK;
  }
  return BACK_2;
}

int seekCup(float distance, float prevRadian, float radian){
  if(distance < 40 ){
    motors.setSpeeds(0, 0);
    delay(200);
    startTime = millis();
    return JUDGE;
  }
  if(millis() - startTime > 2000 && 
      prevRadian < rotateEndDirect && 
      rotateEndDirect < radian){
    motors.setSpeeds(0, 0);
    delay(200);
    startTime = millis();
    return FACE_NEXT;
  }
  motors.setSpeeds(120, -120);
  
  return SEEK;
}

int faceNext(float prevRadian, float radian){
  float e = rotateEndDirect - nextDirect;
  motors.setSpeeds(- e * 55, e * 55);
  if( -0.08 < e && e < 0.08 ){
    rotateNum++;
    rotateEndDirect = getEndDirect(rotateNum);
    nextDirect = getNextDirect(rotateNum);
    return getBackOrFront();
  }
  return FACE_NEXT;
}

int goNext(){
  if (millis() - startTime > 1500) {
    startTime = millis();
    return SEEK;
  }
  motors.setSpeeds( 150, 150);

  return NEXT;
}

int goNextBack(){
  if (millis() - startTime > 1500) {
    startTime = millis();
    return SEEK;
  }
  motors.setSpeeds( -150, -150);

  return NEXT_BACK;
}

int judge(float distance){
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

int faceL(float distance, float prevDistance, float radian){    
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

int faceR(float distance, float prevDistance, float radian){
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

int bringCup(float radian, RGB_STRUCT rgb){
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
  if(millis() - startTime < 1000){
    motors.setSpeeds(100, 100);
  }
  else if(millis() - startTime < 2000){
    motors.setSpeeds(-100, -100);
  }
  else {
    startTime = millis();
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
  motors.setSpeeds(-radian * 55, radian * 55);
  if( -0.08 < radian && radian < 0.08 ){
    rotateNum = 0;
    rotateEndDirect = getEndDirect(rotateNum);
    nextDirect = getNextDirect(rotateNum);
    if(cycleNum < 2)return BACK;
    return BACK_2;
  }
  return ORIGIN;
}
