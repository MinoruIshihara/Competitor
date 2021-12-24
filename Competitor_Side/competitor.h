#pragma once

#include <Wire.h>
#include <ZumoBuzzer.h>
#include <ZumoMotors.h>
#include <Pushbutton.h>
#include <LSM303.h>
#include <Adafruit_TCS34725.h>

#define CALIBRATION
//#define DEBUG_ANGLE
//#define DEBUG_ACCEL
//#define DEBUG_SONOR
#define DEBUG_MODE
//#define DEBUG_MOTOR
//#define SERIAL_TEST

#define ORIGIN 0
#define RUN 1
#define BACK 2
#define SEEK 3
#define FACE_NEXT 4
#define NEXT 5
#define TURN_DOWN 6
#define TURN_GO 7
#define JUDGE 8
#define FACE_R 9
#define FACE_L 10
#define CHECK 11
#define TAKE 12
#define BRING 13
#define PUSH 14
#define STOP 15
#define REVERT_FACE 16
#define REVERT_RUN 17

#define RUN_TIME 1500
#define THRES_DISTANCE 40
#define CHECK_COUNT 3
#define RED_RGB {144, 16, 32}
#define BLACK_RGB {0, 0, 0}

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
int runFront(float distance, int runTime, int L, int R);
int seekCup(float distance, float prevDistance, float angle);
int faceNext(float prevRadian, float radian);
int goNext(RGB_STRUCT rgb);
int turnFaceDown(float prevRadian, float radian);
int turnGoDown(float radian);
int judge(float distance);
int faceCupR(float distance, float prevDistance, float radian);
int faceCupL(float distance, float prevDistance, float radian);
int moveCheck(float prevDistance, float dintance);
int takeCup(RGB_STRUCT rgb, float distance, float radian);
int bringCup(float radian, RGB_STRUCT rgb);
int push();
int faceOrigin(float prevRadian, float radian);
int stopMotor();

bool faceDirect(float current, float obj, int deltaT,int offsetSpeed);
bool faceDirectFast(float prev, float current, float obj);

void playSound(int soundNum);
