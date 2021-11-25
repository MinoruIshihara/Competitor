#include "Competitor.h"

ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON);
LSM303 compass = LSM303();
Adafruit_TCS34725 tcs = Adafruit_TCS34725(
  TCS34725_INTEGRATIONTIME_2_4MS,
  TCS34725_GAIN_60X
);

struct POSITION_STRUCT pos;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin();
  initSonar();
  initColorSensor();
  initMagnetic();
  initPos();

  //calibrationRGB()
  calibrationMagnetic();
}

void loop() {
  setPos();
  
  if(Serial.available() > 0){
    if(Serial.read() == 'H'){
      Serial.write('H');
      sendRGB(getRGB());  
      sendDistance(getDistance());
      sendRadian(getRadian());
      sendPos(getPos());  
      Serial.write(0x0A);
    }
  }  
}
