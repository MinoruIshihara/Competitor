void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:Serial.write(1);
  static unsigned char i = 0;
  
  unsigned char rgb = i >> 4;
  Serial.write('H');
  Serial.write(rgb);

  unsigned int rgbBits = 0;
  rgbBits = rgbBits | (unsigned int)rgb;
  rgbBits = rgbBits | ((unsigned int)rgb << 4);
  rgbBits = rgbBits | ((unsigned int)rgb << 8);
  
  Serial.write(rgbBits);
  
  Serial.write(i);
  Serial.write(i);
  Serial.write(i);
  Serial.write(i);
  Serial.write(0x0A);
  delay(80);
  i++;
  if(i > 255)i = 0;
}
