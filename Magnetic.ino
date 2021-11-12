#include <LSM303.h>               // 加速度，地磁気センサ用ライブラリの読み込み

#define CRB_REG_M_2_5GAUSS 0x60   // CRB_REG_M の値 ： 地磁気センサーのスケールを +/-2.5 ガウスに設定
#define CRA_REG_M_220HZ    0x1C   // CRA_REG_M の値 ： 地磁気センサのアップデートレートを 220 Hz に設定

LSM303 compass;

void initMagnetic()
{  
  compass.init();                 // LSM303 の初期化
  compass.enableDefault();        // 加速度センサ・地磁気センサ を利用可能にする
  compass.writeReg(LSM303::CRB_REG_M, CRB_REG_M_2_5GAUSS); // 地磁気センサーのスケールを +/-2.5 ガウスに設定
  compass.writeReg(LSM303::CRA_REG_M, CRA_REG_M_220HZ);    // 地磁気センサのアップデートレートを 220 Hz に設定
}

double calcRadian(int mx, int my){
  return atan2((double)my, (double)mx);
}

int mapRadian(double radian){
  return (int)map(radian, -PI, PI, 0, 255);
}
