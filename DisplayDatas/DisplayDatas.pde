import processing.serial.*;

Serial port0, port1, port2;
int PORT_NUM = 3;

RGB receivedRGB[] = new RGB[3];
int runningTime = 0;
float distance[] = {31.0, 31.0, 31.0};
int powerL[] = {400, -400, -400}, powerR[] = {400, 400, -400};
float angle[] = {0, -PI, PI};
int mode[] = {0, 1, 2};

int prevRecvTime = -10;
double recvps = 0.0;
int dataCount = 0;

int prevDrawTime = -10;
int fCount = 0;
int fps = 0;

class RGB{
  public int r;
  public int g;
  public int b;
  
  public RGB(int r, int g, int b){
    this.r = r;
    this.g = g;
    this.b = g;
  }
}

void setup() {
  size(1900, 960);
  /*
  port0 = new Serial(this, "COM4", 9600);
  port0.clear();
  port0.bufferUntil(0x0a);
  port1 = new Serial(this, "COM8", 9600);
  port1.clear();
  port1.bufferUntil(0x0a);
  port2 = new Serial(this, "COM9", 9600);
  port2.clear();
  port2.bufferUntil(0x0a);
  */
  for(int i = 0; i < 3; i++){
    receivedRGB[i]= new RGB(240, 240, 180);
  }

  background(0);

  fill(60, 60, 60);
  rect(0, 0, width, height);
  
  /*fill(45, 45, 45);
  rect( 15,  15,  380, 585);
  rect( 15, 615,  380, 170);
  rect(410,  15,  380, 370);
  rect(805,  15,  380, 370);
  rect(410, 415,  380, 370);*/
  
  delay(2000);
  //port1.write('H');
}

void draw() {
  int sqareSize = 300;
  int gapSize = 15;
  int textPosX =  15;
  int textPosY =  33;
  int DataPos  = 135;
  
  fill(60, 60, 60);
  rect(0, 0, width, height);
  textSize(25);
  
  translate(gapSize , gapSize);
  
  for(int portIndex = 0; portIndex < PORT_NUM; portIndex++){

    String RGBText      = str(receivedRGB[portIndex].r) + ", " + str(receivedRGB[portIndex].g) +  ", " + str(receivedRGB[portIndex].b);
    String colorText    = "Black";
    String DistanceText = str(distance[portIndex]);
    String PowerText = str(powerR[portIndex]) + ", " + str(powerL[portIndex]);
    String AngleText    = str(angle[portIndex]);
    String ModeText = str(mode[portIndex]);
    
    stroke(0);
    strokeWeight(1);
    
    if(portIndex > 0)translate(0, sqareSize + gapSize);
    
    //-------------------------RGB-------------------------
    fill(45, 45, 45);
    rect( 0, 0, sqareSize, sqareSize);        // 左下の領域を塗りつぶす
    
    fill(240, 240, 240);
    text( "RGB = ", textPosX, textPosY);
    text( RGBText, textPosX + 100, textPosY);

    int graphPosX = 30;
    int graphPosY = 200;
    int graphW = 40;
    int graphH = 120;
    fill( receivedRGB[portIndex].r, 0, 0); rect(graphPosX                         , graphPosY, graphW, -map( receivedRGB[portIndex].r, 0, 255, 0, graphH));
    fill( 0, receivedRGB[portIndex].g, 0); rect(graphPosX + graphW + gapSize      , graphPosY, graphW, -map( receivedRGB[portIndex].g, 0, 255, 0, graphH));
    fill( 0, 0, receivedRGB[portIndex].b); rect(graphPosX + (graphW + gapSize) * 2, graphPosY, graphW, -map( receivedRGB[portIndex].b, 0, 255, 0, graphH));
   
    int colorBoxX = 200;
    int colorBoxY = 200;
    int colorBoxSize = 80;
    fill( receivedRGB[portIndex].r, receivedRGB[portIndex].g, receivedRGB[portIndex].b); rect(colorBoxX, colorBoxY, colorBoxSize,  colorBoxSize);

    translate(sqareSize + gapSize, 0);

    //-------------------------Distance-------------------------
    fill(45, 45, 45);
    rect( 0, 0, sqareSize, sqareSize);        // 左下の領域を塗りつぶす
    
    fill(240, 240, 240);
    text( "Distance = ", textPosX, textPosY);
    text( DistanceText, textPosX + DataPos, textPosY);                

    int distanceBarX =   0;
    int distanceBarY = 250;
    int distanceBarW = 300;
    int distanceBarH =   4;
    int distanceObjX = distanceBarX + distanceBarW / 2;
    int distanceObjY = 160;
    int distanceObjSize = 30;
  
    fill(20, 210, 20); rect(distanceBarX, distanceBarY , distanceBarW,  distanceBarH);
    fill(210, 20, 20); ellipse(distanceObjX, distanceBarY - map(distance[portIndex], 0, 32, 0, distanceObjY), distanceObjSize,  distanceObjSize);

    translate(sqareSize + gapSize, 0);

    //-------------------------Angle-------------------------
    fill(45, 45, 45);
    rect( 0, 0, sqareSize, sqareSize);        // 左下の領域を塗りつぶす
    
    fill(240, 240, 240);
    text( "Angle = ", textPosX,  textPosY);
    text( AngleText, textPosX + DataPos, textPosY);

    int robotX = 95;
    int robotY = 120;
    int robotW =  110;
    int robotH = 130;
    translate( robotX + robotW / 2, robotY + robotH / 2 );
    rotate(angle[portIndex]);
    fill(30, 200, 30);
    rect(-robotW / 2, -robotH / 2     , robotW    ,  robotH );
    fill(20, 20, 20);
    rect(-robotW /  3, -robotH / 2 - 15, robotW / 4,  robotH / 5);
    rect( robotW / 12, -robotH / 2 - 15, robotW / 4,  robotH / 5);
    rotate(-angle[portIndex]);
    translate( -( robotX + robotW / 2 ), -( robotY + robotH / 2 ));

    translate(sqareSize + gapSize, 0);

    //-------------------------power-------------------------
    fill(45, 45, 45);
    rect( 0, 0, sqareSize, sqareSize);

    fill(240, 240, 240);
    text("Power = ", textPosX, textPosY);
    text(PowerText, textPosX + DataPos, textPosY);

    int xyGraphX = 55;
    int xyGraphY = 65;
    int xyGraphSize = 200;
  
    fill(200, 200, 200); rect(xyGraphX, xyGraphY, xyGraphSize, xyGraphSize);
    stroke(20, 200, 20);
    line(xyGraphX, xyGraphY + xyGraphSize / 2, xyGraphX + xyGraphSize, xyGraphY + xyGraphSize / 2);
    line(xyGraphX + xyGraphSize / 2, xyGraphY, xyGraphX + xyGraphSize / 2, xyGraphY + xyGraphSize);
  
    strokeWeight(3);
    int verticalPower = powerR[portIndex] + powerL[portIndex];
    int rotatePower = powerR[portIndex] - powerL[portIndex];
    stroke(200, 20, 20);
    line(xyGraphX + xyGraphSize / 2 , xyGraphY + xyGraphSize / 2, xyGraphX + xyGraphSize / 2 , xyGraphY + xyGraphSize / 2 - verticalPower / 8);
    stroke(20, 20, 200);
    line(xyGraphX + xyGraphSize / 2 , xyGraphY + xyGraphSize / 2, xyGraphX + xyGraphSize / 2 + rotatePower / 8, xyGraphY + xyGraphSize / 2);

    translate(sqareSize + gapSize, 0);

    //-------------------------mode-------------------------
    strokeWeight(1);
    stroke(0);
    fill(45, 45, 45);
    rect( 0, 0, sqareSize, sqareSize);
    
    fill(240, 240, 240);
    text("Mode = ", textPosX, textPosY);
    text(ModeText, textPosX + DataPos, textPosY);
    
    translate(-(sqareSize + gapSize) * 4, 0);
                      
  }
  
  translate(0, -(sqareSize + gapSize) * 2);
  translate((sqareSize + gapSize) * 5, 0);

  //-------------------------Time/Rate-------------------------
  String TimeText     = str(runningTime);
  String fpsText      = str((float)fps);
  String dpsText      = str((float)recvps);
  
  
  fill(45, 45, 45);
  rect( 0, 0, sqareSize, sqareSize);        // 左下の領域を塗りつぶす
  fill(240, 240, 240);
  
  text( "Time: ",  textPosX, textPosY);
  text( TimeText,  textPosX + DataPos, textPosY);
  
  text( "FPS: ",  textPosX, textPosY + 40);
  text( fpsText,  textPosX + DataPos, textPosY + 40);

  text( "COM/s: ",  textPosX, textPosY + 80);
  text( dpsText,  textPosX + DataPos, textPosY + 80);      

  fCount ++;
  if(millis() - prevDrawTime > 1000){
    fps = fCount;
    fCount = 0;
    prevDrawTime = millis();
  }
}

void mapping(){
  for(int portIndex = 0; portIndex < PORT_NUM; portIndex++){
    receivedRGB[portIndex].r = receivedRGB[portIndex].r << 4;
    receivedRGB[portIndex].g =  receivedRGB[portIndex].g << 4;
    receivedRGB[portIndex].b = receivedRGB[portIndex].b << 4;
    distance[portIndex] = map(distance[portIndex], 0, 255, 0.0, 32.0);
    angle[portIndex] = map(angle[portIndex], 0, 255, -PI, PI);
    powerL[portIndex] = (int)map(powerL[portIndex], 0, 255, -400, 400);
    powerR[portIndex] = (int)map(powerR[portIndex], 0, 255, -400, 400);
  }
}

void readBits(Serial p){
  int portIndex = 0;
  if(p == port1)portIndex = 1;
  if(p == port2)portIndex = 2;
  
  receivedRGB[portIndex].r = p.read();
  int gb = p.read();
  receivedRGB[portIndex].g = gb >> 4;
  receivedRGB[portIndex].b = gb & 0xF;
  distance[portIndex] = p.read();
  angle[portIndex] = p.read();
  powerL[portIndex] = p.read();
  powerR[portIndex] = p.read();
  mode[portIndex] = p.read();
  if(p.read() != 0x0A){
    println("Error : Bits are not correct");
    receivedRGB[portIndex].r = 0; receivedRGB[portIndex].g = 0; receivedRGB[portIndex].b = 0;
    distance[portIndex] = 0; angle[portIndex] = 0; powerL[portIndex] = 0; powerR[portIndex] = 0;
  }
  else{
    println("Receive successed");
    
    dataCount ++;
    if(millis() - prevRecvTime > 1000){
      recvps = dataCount;
      dataCount = 0;
      prevRecvTime = millis();        
    }
    mapping();
    
    p.write('H');
    
  }
}

void serialEvent(Serial p) {
  if (p.available() > 8) {
    if(p.read() == 'H'){
      readBits(p);
    }
  }
}
