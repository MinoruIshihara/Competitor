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
  
  static struct RGB_STRUCT rgb = {0, 0, 0};
  static double distance = 0;
  static double prevDistance = 0;
  static double radian = 0.0;

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
  }
  return mode; 
}

int faceCupLeft(double distance, double prevDistance, double radian){
  
  #ifdef DEBUG_SONOR
  Serial.print("deltaSita: ");
  Serial.print(distance - prevDistance);
  Serial.print(", ");
  #endif
  
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
    startTime = millis();
    cycleNum ++;
    mode = BACK;
  }
  return mode;  
}
