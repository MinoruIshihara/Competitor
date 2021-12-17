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
#define JUDGE_RIGHT 2
#define JUDGE_LEFT 3
#define FACE_RIGHT 4
#define FACE_LEFT 5
#define TAKE 6
#define NEXT 7
#define STOP 8
#define BRING 9
#define PUSH 10

#define WAVE_SEEK_LEFT 11
#define WAVE_SEEK_RIGHT 12

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

int seekCup(double distance, double angle);
int takeCup(double distance);
int nextCup(double distance, double radian);
int stopMotor();
int backRun();
int faceCupRight(double distance, double prevDistance, double radian);
int faceCupLeft(double distance, double prevDistance, double radian);
int bringCup(double radian, RGB_STRUCT rgb);
int push();

int waveSeek(double distance, double angle);
