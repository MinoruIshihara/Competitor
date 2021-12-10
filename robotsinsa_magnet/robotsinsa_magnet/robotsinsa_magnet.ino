#include <Wire.h>
#include <ZumoMotors.h>
#include <Pushbutton.h>
#include <LSM303.h>

ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON);
LSM303 compass;

unsigned long timeInit_G, timeNow_G, timePrev_G; //  スタート時間，経過時間, 1回前
int motorR_G, motorL_G;  // 左右のZumoのモータに与える回転力
int mode_G = 0;
float mx = 0, my = 0, mz = 0;
float ax = 0, ay = 0, az = 0;
float heading_G = 0;
float thetaNext = 0; // 次に向かう方向
int count = 0; // 回転した回数

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  setupCompass();
  button.waitForButton();
  calibrationCompass();
  button.waitForButton();
  mode_G = 0;
  timeInit_G = millis();
  timePrev_G = 0;
}

float sum_e = 0;
float turnTo(float theta_r) {
  float u;
  float KP = 4.0;
  float TIinv = 2/1000.0;
  heading_G = atan2(my,mx) * 180 / M_PI;
  if (heading_G<0) heading_G += 360;
  float e = theta_r-heading_G;
  if (e<-180) e+=360;
  if (e>180)  e-=360;
  if (abs(e) > 45.0 ) { // |e|>45のときはP制御
    u = KP*e;           // P制御
  } else {              // |e|<=45 の時はPI制御
    sum_e += TIinv*e*(timeNow_G-timePrev_G);
    u = KP*(e+sum_e);   // PI 制御
  }
  if ( u> 180 ) u = 180;  // 飽和
  if ( u<-180 ) u = -180; // 飽和
  return u;
}

void loop()
{
  float speed0, diff;
  timeNow_G = millis() - timeInit_G; // 経過時間

  if (timeNow_G-timePrev_G<100) {
    return;
  }

  compass.read();
  compass.m_min.x = min(compass.m.x,compass.m_min.x);  compass.m_max.x = max(compass.m.x,compass.m_max.x);
  compass.m_min.y = min(compass.m.y,compass.m_min.y);  compass.m_max.y = max(compass.m.y,compass.m_max.y);
  compass.m_min.z = min(compass.m.z,compass.m_min.z);  compass.m_max.z = max(compass.m.z,compass.m_max.z);
  ax = compass.a.x/256; //map(compass.a.x,-32768,32767,-128,127);
  ay = compass.a.y/256; //map(compass.a.y,-32768,32767,-128,127);
  az = compass.a.z/256; //map(compass.a.z,-32768,32767,-128,127);
  mx = map(compass.m.x,compass.m_min.x,compass.m_max.x,-128,127);
  my = map(compass.m.y,compass.m_min.y,compass.m_max.y,-128,127);
  mz = map(compass.m.z,compass.m_min.z,compass.m_max.z,-128,127); 
  sendData(); // データ送信

  if ( button.isPressed() ) { // Zumo button が押されていればtrue, そうでなければ false
    mode_G = 0;
    delay(100);
    timeInit_G = millis();
    timePrev_G = 0;
  }

  switch (mode_G) {
    case 0:
      mode_G = 1;
      sum_e = 0.0;
      thetaNext = atan2(my,mx) * 180 / M_PI; // 現在向いている方角を弧度法で保存
      if (heading_G < 0)
        heading_G += 360; // 角度を0°~360°に収める
    case 1: // 30cm前進
      speed0 = 150.0;
      diff = turnTo(thetaNext); // 初めに向いていた方角に進むようにする
      if (waitfor(1450) == 1) { // 約30cm進んだら mode 2 へ
        mode_G = 2;
        thetaNext -= 90; // 向かう方向を90°回転
        if (thetaNext < 0)
          thetaNext += 360; // 角度を0°~360°に収める
      }
      break;
    case 2: // 90°回転
      speed0 = 0.0;
      diff = turnTo(thetaNext);
      if (abs(thetaNext-heading_G)<=5) { // 90°回転したら mode 1 へ
        mode_G = 1;
        count++;
      }
      if (count >= 4) { // 元の位置に戻ってきたら mode 3 へ
        mode_G = 3;
      }
      break;
    case 3: // 停止
      speed0 = 0.0;
      break;
    default:
      speed0 = 0;
      diff = 0;
      break;
  }
  motorL_G = speed0+diff;
  motorR_G = speed0-diff;
  motors.setSpeeds(motorL_G, motorR_G);
  timePrev_G=timeNow_G;
}

int waitfor( unsigned long period )
{
  static int flagStart = 0; // 0:待ち状態, 1:現在計測中
  static unsigned long startTime = 0;

  if ( flagStart == 0 ) {
    startTime = timeNow_G;
    flagStart = 1; // 現在計測中にしておく
  }

  if ( timeNow_G - startTime > period ) { // 計測開始からの経過時間が指定時間を超えた
    flagStart = 0;  // 待ち状態に戻しておく
    startTime = 0;  // なくてもよいが, 形式的に初期化
    return 1;
  }
  return 0;
}

void write1byteint(int x) {
  Serial.write(x+128);
}

void write2byte(int x) {
  Serial.write(x>>8);
  Serial.write(x&255);
}

void sendData()
{
    Serial.write('H');
    write1byteint((int)ax);
    write1byteint((int)ay);
    write1byteint((int)az);
    write1byteint((int)mx);
    write1byteint((int)my);
    write1byteint((int)mz);
    Serial.write(mode_G);
}
