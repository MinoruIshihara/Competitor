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

#define BACK 0
#define SEEK 1
#define FACE_NEXT 2
#define NEXT 3
#define TURN_DOWN 4
#define TURN_GO 5
#define JUDGE 7
#define FACE_R 8
#define FACE_L 9
#define CHECK 10
#define TAKE 11
#define BRING 12
#define PUSH 13
#define ORIGIN 14 
#define STOP 15

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
int seekCup(float distance, float prevDistance, float angle);
int faceNext(float prevRadian, float radian);
int goNext(RGB_STRUCT rgb);
int turnFaceDown(float prevRadian, float radian);
int turnGoDown(float radian);
int judge(float distance);
int faceCupR(float distance, float prevDistance, float radian);
int faceCupL(float distance, float prevDistance, float radian);
int moveCheck(float prevDistance, float dintance);
int takeCup(float distance);
int bringCup(float radian, RGB_STRUCT rgb);
int push();
int faceOrigin(float prevRadian, float radian);
int stopMotor();

bool faceDirect(float current, float obj, int deltaT);
bool faceDirectFast(float prev, float current, float obj);

void playSound(int soundNum);
