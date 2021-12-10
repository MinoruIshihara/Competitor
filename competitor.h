#pragma once

#include <Wire.h>
#include <ZumoMotors.h>
#include <Pushbutton.h>
#include <LSM303.h>
#include <Adafruit_TCS34725.h>

#define CALIBRATION
//#define DEBUG_ANGLE
//#define DEBUG_ACCEL
#define DEBUG_SONOR
#define DEBUG_MODE
//#define DEBUG_MOTOR

#define BACK 0
#define SEEK 1
#define FACE 2
#define TAKE 3
#define NEXT 4
#define STOP 5
#define BRING 6
struct ACCEL_STRUCT{
  double x;
  double y;
};

struct POSITION_STRUCT{
  double x;
  double y;
};

struct RGB_STRUCT {
  unsigned char r;
  unsigned char g;
  unsigned char b;
};

void initAcceleration();
struct POSITION_STRUCT getPos(struct POSITION_STRUCT pos, int deltaT);

int initColorSensor();
void calibrationRGB();

bool serialEvent();
void sendRGB(struct RGB_STRUCT rgb);
void sendColor(struct RGB_STRUCT rgb);
void sendDistance(int distance);
void sendRadian(double radian);
void sendPos(struct POSITION_STRUCT pos);
void sendAll(RGB_STRUCT rgb, int distance, double radian, struct POSITION_STRUCT mpos);

void initMagnetic();
double getRadian();

int seekCup(double distance, double prevDistance, double angle);
int takeCup(double distance);
int nextCup(double distance);
int stopMotor();
int backRun();
int faceCup(double distance, double prevDistance, double radian);
int bringCup(double radian);

int initSonar();
double getDistance();
