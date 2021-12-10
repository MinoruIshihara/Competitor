#include <Wire.h>
#include <ZumoMotors.h>        //モータのライブラリ
#include <Adafruit_TCS34725.h> //カラーセンサのライブラリ
#include <Pushbutton.h>

Pushbutton button(ZUMO_BUTTON);

#define WHITE     0 //白
#define BLACK     1 //黒
#define RED       2 //赤
#define BLUE      3 //青
#define OTHER     4 //その他
#define INIT     10 //初期状態
#define FORWARD  11 //直進
#define ROTATE   12 //回転
#define STOP     13 //停止
#define FORCOLOR 14 //直進(青線,赤線上)

//モータのglobal変数
ZumoMotors motors;
int motorL, motorR;

//カラーセンサのglobal変数
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_60X);
float red_G, green_G, blue_G; // カラーセンサで読み取ったRGB値（0-255）

//状態遷移のglobal変数
int mode;                 //ロボットの状態（初期状態／直進／回転／停止）
unsigned long start_time; //時間計測の開始時間

void task() {
  switch (mode) {
    case INIT: //初期状態
      mode = FORWARD;
      break;
    
    case FORWARD: //直進
      motorL = motorR = 150;
      if ( identify_color( 0, 0, 0 ) ) { // 黒色なら
        mode = ROTATE;
        start_time = millis();
      } else if ( identify_color( 170, 80, 50 ) || identify_color( 40, 80, 110 ) ) { // 赤色もしくは青色なら
        mode = STOP;
        start_time = millis();
      }
      break;
      
    case ROTATE: //回転
      if (millis() - start_time < 300) { //0.3秒後退
        motorL = motorR = -150;
      } else if (millis() - start_time < 1300) { //1秒回転
        motorL = -150;
        motorR = 150;
      } else {
        mode = FORWARD;
      }
      if ( identify_color( 170, 80, 50 ) || identify_color( 40, 80, 110 ) ) { // 赤色もしくは青色なら
        mode = STOP;
        start_time = millis();
      }
      break;
      
    case STOP: //停止
      motorL = motorR = 0;
      if (millis() - start_time > 1000) { //1秒経過したら
        mode = FORCOLOR;
      }
      break;

    case FORCOLOR: //直進(青線赤線上)
      motorL = motorR = 150;
      if ( identify_color( 255, 255, 255 ) ) { // 白色なら
        mode = FORWARD;
      } else if ( identify_color( 0, 0, 0 ) ) { // 黒色なら
        mode = ROTATE;
        start_time = millis();
      }
      break;
  }
}

void setup() {
  Serial.begin(9600);
  tcs.begin();
  Wire.begin();

  button.waitForButton(); // Zumo buttonが押されるまで待機
  CalibrationColorSensor(); // カラーセンサーのsetupとキャリブレーション

  button.waitForButton(); // Zumo buttonが押されるまで待機
  mode = INIT;
  motorL = motorR = 0;
}

void loop() {
  //モータを駆動
  motors.setSpeeds(motorL, motorR);

  getRGB(red_G, green_G, blue_G); // カラーセンサでRGB値を取得(0-255)

  task(); //zumoロボットのモード変化
}

int identify_color( int red, int green, int blue )
{
  float d2;
  float d2_max = 30; // パラメーター（適宜調整）

  d2 = pow(red - red_G, 2) + pow(green - green_G, 2) + pow(blue - blue_G, 2);
  if ( d2 < d2_max * d2_max )
    return 1;
  else
    return 0;
}
