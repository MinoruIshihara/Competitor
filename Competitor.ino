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
  initSonar();
  initColorSensor();
  initMagnetic();
  initPos();
  Serial.begin(9600);
  Wire.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  sendDistance(getDistance());
  sendRadian(getRadian());
  setPos();
  sendPos(pos);
}
