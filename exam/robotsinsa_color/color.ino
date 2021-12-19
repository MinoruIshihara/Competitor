#include <Adafruit_TCS34725.h> // カラーセンサライブラリのヘッダーファイル

unsigned int r_min, g_min, b_min; // このグローバル変数はこのファイル内のみで使用
unsigned int r_max, g_max, b_max;

void  CalibrationColorSensor()
{
  unsigned long timeInit;
  unsigned int r, g, b, clr;

  tcs.begin(); // カラーセンサーのsetup
  
  motors.setSpeeds(60, 60); 

  r_min = 30000; // 十分大きな数
  g_min = 30000;
  b_min = 30000;
  r_max = 0;
  g_max = 0;
  b_max = 0;

  timeInit = millis();

  while ( 1 ) {
    tcs.getRawData(&r, &g, &b, &clr); // rowdataの取得（ポインタ渡し）
  
    if ( g < r_min ) r_min = r;
    if ( g < g_min ) g_min = g;
    if ( b < b_min ) b_min = b;
    if ( r > r_max ) r_max = r;
    if ( g > g_max ) g_max = g;
    if ( b > b_max ) b_max = b;

    if ( millis() - timeInit > 2000 ) // 2000ms経過したら抜ける
      break;
  }
   motors.setSpeeds(0, 0); // 停止しておく
}


void getRGB( float& r0, float& g0, float& b0) {//（参照渡し）
  unsigned int r, g, b, clr;

  tcs.getRawData(&r, &g, &b, &clr); // rowdataの取得
 
  r0 = map(r, r_min, r_max, 0, 255);
  g0 = map(g, g_min, g_max, 0, 255);
  b0 = map(b, b_min, b_max, 0, 255);

  if ( r0 < 0.0 ) r0 = 0.0;
  if ( r0 > 255.0 ) r0 = 255.0;
  if ( g0 < 0.0 ) g0 = 0.0;
  if ( g0 > 255.0 ) g0 = 255.0;
  if ( b0 < 0.0 ) b0 = 0.0;
  if ( b0 > 255.0 ) b0 = 255.0;
}
