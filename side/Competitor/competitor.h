#pragma once

#include <Wire.h>
#include <ZumoMotors.h>
#include <Pushbutton.h>
#include <LSM303.h>
#include <Adafruit_TCS34725.h>

#define TEAM_RED
#define CALIBRATION
//#define DEBUG_ANGLE
//#define DEBUG_ACCEL
//#define DEBUG_SONOR
//#define DEBUG_MODE
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

#define ROTATE_SEEK 10
#define FACE_NEXT 11
#define GO_NEXT_ROTATE 12

#define THRES_DISTANCE 40
#define RED_RGB 144, 16, 32

struct RGB_STRUCT {
  unsigned char r;
  unsigned char g;
  unsigned char b;
};

bool serialEvent();
void sendRGB(struct RGB_STRUCT rgb);
void sendColor(struct RGB_STRUCT rgb);
void sendDistance(int distance);
void sendRadian(double radian);
void sendPower(int left, int right);
void sendMode(int mode);
void sendAll(RGB_STRUCT rgb, int distance, double radian, int left, int right, int mode);

int initColorSensor();
void calibrationRGB();
int identify_color(RGB_STRUCT rgb, int red, int green, int blue );

int initSonar();
double getDistance();

void initMagnetic();
double getRadian();

float faceTo(float nextRadian, double radian , int deltaT);

int seekCup(double distance, double prevDistance, double angle);
int takeCup(double distance);
int nextCup(double distance, double radian);
int stopMotor();
int backRun();
int faceCupRight(double distance, double prevDistance, double radian);
int faceCupLeft(double distance, double prevDistance, double radian);
int bringCup(double radian, RGB_STRUCT rgb);
int push();

int rotateSeek(double distance);
int faceNext(double radian, int deltaT);
int goNextRotate(double radian, int deltaT);
