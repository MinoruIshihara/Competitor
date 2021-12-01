#include <ZumoMotors.h>
#include <Pushbutton.h>

ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON);

void setup()
{
  Serial.begin(9600);
  button.waitForButton();
}

void loop()
{
  static int mode = 0;
  static int startTime = 0;

  switch(mode){
    case 0:
    backSeek();
    break;
    
    case 1:
    seekCup();
    break;
    
    case 2:
    runStraight();
    break;
  }
  
  backSeek();
}

void backSeek(){
  static int backSeekStartTime = millis();
  static int backSeekMode = 0;
  
  switch(backSeekMode){
    case 0:
    rightBack();
    break;
    
    case 1:
    leftBack();
    break;
  }
  if( millis() - backSeekStartTime > 1000 ){
    backSeekMode = (backSeekMode + 1)%2;
    backSeekStartTime = millis();
  }
}

void rightBack(){
    motors.setSpeeds(-200, -50);
}

void leftBack(){
    motors.setSpeeds(-50, -200);
}

seekCup(){
  
}
