#include <ZumoMotors.h>        //モータのライブラリ
#include <Adafruit_TCS34725.h> //カラーセンサのライブラリ
#include <Pushbutton.h>

Pushbutton button(ZUMO_BUTTON);

#define WHITE    0 //白
#define BLACK    1 //黒
#define RED      2 //赤
#define BLUE     3 //青
#define OTHER    4 //その他
#define INIT    10 //初期状態
#define FORWARD 11 //直進
#define ROTATE  12 //回転
#define STOP    13 //停止
#define SEARCH  14 //探索
#define FIND    15 //発見
#define PUSH    16 //押出
#define ROT    17 //回転押出


//150,125,100 or 150,150,150



//個体差によって変える変数

int mo_for = 150;   //通常時の前進速度
int mo_rot = 125;   //回転速度
int mo_rev = 100;   //角度調整の際の速度(回転速度よりも小さめに設定)

unsigned long t_for = 2000;   //前進する時間(ms)
unsigned long t_rot = 4000;   //回転する時間(ms)




//モータのglobal変数
ZumoMotors motors;
int motorL, motorR;

//超音波センサのglobal変数
const int trig = 2; //Tringピン
const int echo = 4; //Echoピン
int dist;           //距離(cm)
int rist;           //距離記録用の変数


//状態遷移のglobal変数
int mode;                 //ロボットの状態（初期状態／直進／回転／停止）
int mode2;                 //ロボットの状態2（探索／発見／押し出し）
unsigned long start_time; //時間計測の開始時間

int distance() {
  float interval; //Echoのパルス幅(μs)
  int dst;                //距離(mm)
  
  //10μsのパルスを超音波センサのTrigピンに出力
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  //5767μs(100cm)、超音波が反射してこなければタイムアウトしてエラー値0を返す
  //Echo信号がHIGHである時間(μs)を計測
  interval = pulseIn(echo, HIGH, 5767);
  
  dst = interval * 340. / 2. * 1e-3 ; //距離(mm)に変換
  if (dst == 0) {dst = 1000;} //エラー値0（超音波が反射してこない）は100cmを検出したことにする
  delay(60); //trigがHIGHになる間隔を60ms以上空ける（超音波センサの仕様）

  return dst;
}


void task() {
  switch (mode) {
    case INIT: //初期状態
      button.waitForButton(); // Zumo buttonが押されるまで待機
      mode = ROTATE;
      start_time = millis();
      break;
    case FORWARD: //直進
      motorL = motorR = mo_for;
  
      if(mode2 == SEARCH){ //ロボットがカップを探している状態なら
      if(millis() - start_time > t_for){
        mode = ROTATE;
        start_time = millis();
        break;
      }
      //距離が30cm未満なら発見状態に
      if (dist < 400) {
        mode2 = PUSH; //mode2を押し出し状態
        start_time = millis();
        }
        break;
      }
      
      if(mode2 == PUSH){ //ロボットが押し出し状態なら
        
        if(millis() - start_time > 3500){ //3.5秒間キャッチできなかったらROTATEに
        mode = ROTATE;
        start_time = millis();
        break;
      }
        if (dist < 50) {
        mode = STOP; //停止
        break;
        }
      }
      break;
      
    case ROTATE: //回転
      if (dist < 400) {
        mode = ROT; //modeを調整に
        rist = dist;  //測定した距離を記録
        start_time = millis();
        break;
        }
        
      if (millis() - start_time < t_rot) { //回転
        motorL = -mo_rot; motorR = mo_rot;
      } else {
        mode = FORWARD;
        mode2 = SEARCH;
        start_time = millis();
      }
      break;
      
    case ROT: //微調整
    motorL = 0; motorR = 0;
      if(millis() - start_time < 500){  //0.5秒停止
        break;
        }
      if(millis() - start_time > 4000){  //4秒何もなければROTATEに
        mode = ROTATE;
        start_time = millis();
        break;
        }
        
      if(dist < rist+3){  //測定した値より現在の距離が小さくなれば
          mode = FORWARD;
          mode2 = PUSH;
          start_time = millis();
          break;
        }
      else{
          motorL = mo_rev; motorR = -mo_rev;  //調整のための逆回転
          break;
        }
      
     break;
      
    case STOP: //停止
      motorL = motorR = 0;
      if (dist > 40) {
        mode = FORWARD; //modeを前進状態
        mode2 = SEARCH; //mode2を探索状態
        start_time = millis();
        }
      break;
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(trig, OUTPUT); //trigを出力ポートに設定
  pinMode(echo, INPUT);  //echoを入力ポートに設定
  mode = INIT;
  mode2 = SEARCH;
  motorL = motorR = 0;
}

void loop() {
  //モータを駆動
  motors.setSpeeds(motorL, motorR);

  //距離を計測
  dist = distance();
  Serial.print(mode);
  Serial.print(",");
  Serial.print(mode2);
  Serial.print(",");
  Serial.println(dist);


  task(); //演習10.2.2の主な処理をする関数
}
