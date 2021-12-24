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
#define DEBUG_MODE
//#define DEBUG_MOTOR
//#define SERIAL_TEST

#define ORIGIN 0
#define RUN 1
#define BACK 2
#define SEEK 3
#define JUDGE_R 4
#define JUDGE_L 5
#define FACE_R 6
#define FACE_L 7
#define TAKE 8
#define REVERT_FACE 9
#define REVERT_RUN 10
#define NEXT 11
#define STOP 12
#define BRING 13
#define PUSH 14
#define WAVE_L 15
#define WAVE_R 16

#define RUN_TIME 2000
#define MELODY_LENGTH 5
#define THRES_DISTANCE 40
#define RED_RGB {144, 16, 32}
#define BLUE_RGB {40, 80, 110}
#define BLACK_RGB {10, 10, 10}

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

int faceOrigin(float prevRadian, float radian);
int runFront(float distance, int runTime, int L, int R);
int seekCup(double distance, double prevRadian, double angle);
int takeCup(RGB_STRUCT rgb, float distance, float radian);
int nextCup(double distance, double radian);
int stopMotor();
int backRun();
int judgeL(double distance);
int judgeR(double distance);
int faceCupL(double distance, double prevDistance, double radian);
int faceCupR(double distance, double prevDistance, double radian);
int bringCup(double radian, RGB_STRUCT rgb);
int push();

int waveL(double distance, double angle);
int waveR(double distance, double angle);

bool faceDirect(float current, float obj, int deltaT, int offsetSpeed);

void playSound(int soundNum);
