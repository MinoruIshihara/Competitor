#include "Competitor.h"

ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON);
LSM303 compass = LSM303();
Adafruit_TCS34725 tcs = Adafruit_TCS34725(
  TCS34725_INTEGRATIONTIME_2_4MS,
  TCS34725_GAIN_60X
);

int startTime = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  initSonar();
  initColorSensor();
  initMagnetic();
  button.waitForButton();
  //Serial.print("H000000");
  //Serial.write(0x0A);
  initAcceleration();
  
  startTime = millis();
}

void loop() {
  static int prevTime = 0;
  static int currentTime = 0;
  static int deltaT = 0;
  
  static int mode = BACK;
  
  static struct RGB_STRUCT rgb = {0, 0, 0};
  static double distance = 0;
  static double prevDistance = 0;
  static double radian = 0.0;
  static struct POSITION_STRUCT pos = {0.0, 0.0};

  prevTime = currentTime;
  currentTime = millis();
  deltaT = currentTime - prevTime;

  rgb = getRGB();
  prevDistance = distance;
  distance = getDistance();
  radian = getRadian();
  pos = getPos(pos, deltaT);
  
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
    
    case FACE:
    mode = faceCup(distance, prevDistance, radian);
    break;
    
    case TAKE:
    mode = takeCup(distance);
    break;

    case NEXT:
    mode = nextCup(distance);
    break;

    case STOP:
    mode = stopMotor();
    break;

    case BRING:
    mode = bringCup(radian);
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
    sendAll(rgb, distance, radian, pos);
  }
  Serial.println("");
}

int backRun(){
  motors.setSpeeds(-400, -400);  
  if(millis() - startTime > 1000){
    startTime = millis();
    return SEEK;
  }
  return BACK;
}

int seekCup(double distance, double prevDistance, double radian){
  int mode = SEEK;
  if(millis() - startTime > 2000){
    motors.setSpeeds(-120, 120);
  }
  else {
    motors.setSpeeds(120, -120);
  }
  if(millis() - startTime > 100000){
    startTime = millis();
    mode = STOP;
  }
  if(distance < 40 /*&& prevDistance < 40*/ ){
    motors.setSpeeds(0, 0);
    delay(800);
    startTime = millis();
    mode = FACE;
  }
  return mode;
}

int faceCup(double distance, double prevDistance, double radian){
  Serial.print("deltaSita: ");
  Serial.print(distance - prevDistance);
  Serial.print(", ");
  int mode = FACE;
  motors.setSpeeds(-100, 100);
  if(millis() - startTime > 1000){
    motors.setSpeeds(100, -100);
    if(distance - prevDistance > 0 && distance - prevDistance < 1 && distance < 40){
      startTime = millis();
      mode = TAKE;
    }
  }
  return mode; 
}

int takeCup(double distance){
  int mode = TAKE;
  motors.setSpeeds(120, 120);
  if(distance < 5){
    startTime = millis();
    mode = BRING;
  }
  return mode;
}

int nextCup(double distance){  
  int mode = NEXT;
  motors.setSpeeds(100, 100);
  if(millis() - startTime > 100){
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

int bringCup(double radian){
  int mode = BRING;
  /*if(abs(radian) < 0.1){
    motors.setSpeeds(300, 300);
  }
  else */if(radian < 0){
    motors.setSpeeds(-radian * 100 + 120 , radian * 100 + 80);
  }
  else{
    motors.setSpeeds(-radian * 100 + 80 , radian * 100 + 120);
  }
  
  return mode;
}
