void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:Serial.write(1);
  static unsigned char i = 0;
  
  Serial.write('H');

  unsigned char rgb = i >> 4;

  unsigned int rBits = rgb;
  Serial.write(rBits);
  
  unsigned int gbBits = 0;
  gbBits = gbBits | (unsigned int)rgb;
  gbBits = gbBits | ((unsigned int)rgb << 4);
  
  Serial.write(gbBits);
  
  Serial.write(i);
  Serial.write(i);
  Serial.write(i);
  Serial.write(i);
  Serial.write(0x0A);
  delay(80);
  i++;
  if(i > 255)i = 0;
}
