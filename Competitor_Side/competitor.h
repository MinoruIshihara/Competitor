#pragma once

#include <Wire.h>
#include <ZumoBuzzer.h>
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
#define FACE_NEXT 2
#define NEXT 3
#define NEXT_BACK 4
#define JUDGE 5
#define FACE_R 6
#define FACE_L 7
#define TAKE 8
#define BRING 9
#define PUSH 10
#define ORIGIN 11 
#define BACK_2 12
#define STOP 13

#define THRES_DISTANCE 40
#define RED_RGB 144, 16, 32

struct RGB_STRUCT {
  unsigned char r;
  unsigned char g;
  unsigned char b;
};

struct RGB_STRUCT initRGB;

bool serialEvent();
void sendRGB(struct RGB_STRUCT rgb);
void sendColor(struct RGB_STRUCT rgb);
void sendDistance(int distance);
void sendRadian(float radian);
void sendPower(int left, int right);
void sendMode(int mode);
void sendAll(RGB_STRUCT rgb, int distance, float radian, int left, int right, int mode);

int initColorSensor();
void calibrationRGB();
int identify_color(RGB_STRUCT rgb, int red, int green, int blue );

int initSonar();
float getDistance();

void initMagnetic();
float getRadian();
float getEndDirect(int n);
float getNextDirect(int n);
int getBackOrFront();

int backRun();
int back2();
int seekCup(float distance, float prevDistance, float angle);
int faceNext(float prevRadian, float radian);
int goNext();
int goNextBack();
int judge(float distance);
int faceCupR(float distance, float prevDistance, float radian);
int faceCupL(float distance, float prevDistance, float radian);
int takeCup(float distance);
int bringCup(float radian, RGB_STRUCT rgb);
int push();
int faceOrigin(float prevRadian, float radian);
int stopMotor();
