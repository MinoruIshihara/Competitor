const int CRB_REG_M_2_5GAUSS = 0x60;   // CRB_REG_M の値 ： 地磁気センサーのスケールを +/-2.5 ガウスに設定
const int CRA_REG_M_220HZ = 0x1C;   // CRA_REG_M の値 ： 地磁気センサのアップデートレートを 220 Hz に設定
float angleOffset = 0;

void initMagnetic()
{
  compass.init();                 // LSM303 の初期化
  compass.enableDefault();        // 加速度センサ・地磁気センサ を利用可能にする
  compass.writeReg(LSM303::CRB_REG_M, CRB_REG_M_2_5GAUSS); // 地磁気センサーのスケールを +/-2.5 ガウスに設定
  compass.writeReg(LSM303::CRA_REG_M, CRA_REG_M_220HZ);    // 地磁気センサのアップデートレートを 220 Hz に設定
  delay(1000); // 良く分からないが必要
  #ifdef CALIBRATION
    calibrationMagnetic();
  #else
    compass.m_max.x = 520;
    compass.m_max.y = 2100;
    compass.m_min.x = -780;
    compass.m_min.y = 740;
  #endif
  button.waitForButton();
  angleOffset = getRadian();
}

float getRadian(){  

  compass.read();
  int mx = map(compass.m.x,compass.m_min.x,compass.m_max.x,-1024,1023);
  int my = map(compass.m.y,compass.m_min.y,compass.m_max.y,-1024,1023);
  float radian = -atan2((float)mx, (float)my) - angleOffset;

  if(radian < - PI){
    radian =  2 * PI + radian;
  }
  if(radian > PI){
    radian = radian - 2 * PI;
  }

  #ifdef DEBUG_ANGLE
  Serial.print("MX: ");
  Serial.print(mx);
  Serial.print(", MY: ");
  Serial.print(my);
  Serial.print(", Angle: ");
  Serial.print(radian);
  Serial.println(compass.m.y);
  #endif

  return radian;
}

void  calibrationMagnetic()
{
  int16_t mxMax = -32767;
  int16_t myMax = -32767;
  int16_t mzMax = -32767;
  
  int16_t mxMin = 32767;
  int16_t myMin = 32767;
  int16_t mzMin = 32767;
  
  int startTime = millis();
  motors.setRightSpeed(200);
  motors.setLeftSpeed(-200);
  
  while(millis() - startTime < 3000){
    compass.read();
  
    mxMin = min(mxMin, compass.m.x);
    myMin = min(myMin, compass.m.y);
    mzMin = min(mzMin, compass.m.z);
  
    mxMax = max(mxMax, compass.m.x);
    myMax = max(myMax, compass.m.y);
    mzMax = max(mzMax, compass.m.z);

  }

  #ifdef DEBUG
  Serial.print("MX: ");
  Serial.print(mxMax);
  Serial.print(", ");
  Serial.println(mxMin);

  Serial.print("MY: ");
  Serial.print(myMax);
  Serial.print(", ");
  Serial.println(myMin);
  #endif

  compass.m_max.x = mxMax;
  compass.m_max.y = myMax;
  compass.m_max.z = mzMax;
  compass.m_min.x = mxMin;
  compass.m_min.y = myMin;
  compass.m_min.z = mzMin;
  
  motors.setRightSpeed(0);
  motors.setLeftSpeed(0);
}
