#include <Wire.h>
#include <ZumoMotors.h>
#include <Pushbutton.h>
#include <LSM303.h>
#include <Adafruit_TCS34725.h>

struct ACCEL_STRUCT{
  double x;
  double y;
};

struct POSITION_STRUCT{
  unsigned long prevTime;
  double x;
  double y;
};

struct RGB_STRUCT {
  unsigned char r;
  unsigned char g;
  unsigned char b;
};

void initPos();
void setPos();

int initColorSensor();
void calibrationRGB();

void sendRGB(struct RGB_STRUCT rgb);
void sendColor(struct RGB_STRUCT rgb);
void sendDistance(int distance);
void sendRadian(double radian);
void sendPos(struct POSITION_STRUCT pos);

void initMagnetic();
double getRadian();

int initSonar();
double getDistance();
