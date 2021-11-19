const int CRB_REG_M_2_5GAUSS = 0x60;   // CRB_REG_M の値 ： 地磁気センサーのスケールを +/-2.5 ガウスに設定
const int CRA_REG_M_220HZ = 0x1C;   // CRA_REG_M の値 ： 地磁気センサのアップデートレートを 220 Hz に設定

void initMagnetic()
{
  compass.init();                 // LSM303 の初期化
  compass.enableDefault();        // 加速度センサ・地磁気センサ を利用可能にする
  compass.writeReg(LSM303::CRB_REG_M, CRB_REG_M_2_5GAUSS); // 地磁気センサーのスケールを +/-2.5 ガウスに設定
  compass.writeReg(LSM303::CRA_REG_M, CRA_REG_M_220HZ);    // 地磁気センサのアップデートレートを 220 Hz に設定
  compass.m_min.x = 30000;
  compass.m_min.y = 30000;
  compass.m_max.x = -30000;
  compass.m_max.y = -30000;
  delay(1000); // 良く分からないが必要
}

double getRadian(){
  compass.read();
  int mx = map(compass.m.x, -630, 3200, -1000, 1000);
  int my = map(compass.m.y, 4000, 8300, -1000, 1000);
  double radian = atan2((double)mx, (double)my);
  return radian;
}

void  calibrationMagnetic()
{
  int startTime = millis();
  //motors.setRightSpeed(200);
  //motors.setLeftSpeed(-200);
  
  while(millis() - startTime < 30000){
    compass.read();
  
    compass.m_min.x = min(compass.m_min.x, compass.m.x);
    compass.m_min.x = min(compass.m_min.y, compass.m.y);
  
    compass.m_max.x = max(compass.m_max.x, compass.m.x);
    compass.m_max.y = max(compass.m_max.y, compass.m.y);

    Serial.print("MX: ");
    Serial.println(compass.m.x);
    Serial.print("MY: ");
    Serial.println(compass.m.y);
  }
  //motors.setRightSpeed(0);
  //motors.setLeftSpeed(0);
}
