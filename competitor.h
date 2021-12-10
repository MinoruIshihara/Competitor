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
#define JUDGE 2
#define FACE_RIGHT 3
#define FACE_LEFT 4
#define TAKE 5
#define NEXT 6
#define STOP 7
#define BRING 8
#define PUSH 9

#define THRES_DISTANCE 40

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
int faceCupRight(double distance, double prevDistance, double radian);
int faceCupLeft(double distance, double prevDistance, double radian);
int bringCup(double radian, RGB_STRUCT rgb);
int push();

int initSonar();
double getDistance();
