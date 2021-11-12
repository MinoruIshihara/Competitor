//ライブラリインクルード
#include <Wire.h>
#include <ZumoMotors.h>
#include <Pushbutton.h>
#include <LSM303.h>

struct RGB_STRUCT {
  unsigned char r;
  unsigned char g;
  unsigned char b;
};

ZumoMotors motors;//ロボット制御用ハンドラオブジェクト
Pushbutton button(ZUMO_BUTTON);//ボタンハンドラ
LSM303 compass;//

void setup() {
  // put your setup code here, to run once:
  initSonar();
  initColorSensor();
  Serial.begin(9600);
  Wire.begin();

}

void loop() {
  // put your main code here, to run repeatedly:
  sendDistance(getDistance());
}
