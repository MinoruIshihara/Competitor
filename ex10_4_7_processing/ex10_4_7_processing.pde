import processing.serial.*;

Serial port1;                                                       // 1台目のZumoのシリアル通信用

int red=240, green=255, blue=200;
int runningTime = 0;
int distance = 31;
int robotPosX = 0, robotPosY = 0;
float angle = 0;

void setup() {
  size(1200, 800);                                                  // 幅 1200px, 高さ 800px のウインドウを生成
  port1 = new Serial(this, "COM3", 9600);     // Serial クラスのインスタンスを生成
  port1.clear();
  port1.bufferUntil(0x0d);                                          // LF = 0x0d までバッファ

  background(0);                                                    // 背景色を黒に

  fill(60, 60, 60);
  rect(0, 0, width, height);         // 右上の領域を塗りつぶす
  
  fill(45, 45, 45);
  rect( 15,  15,  380, 585);        // 左下の領域を塗りつぶす
  rect( 15, 615,  380, 170);        // 左下の領域を塗りつぶす
  rect(410,  15,  380, 370);        // 左下の領域を塗りつぶす
  rect(805,  15,  380, 370);        // 左下の領域を塗りつぶす
  rect(410, 415,  380, 370);        // 左下の領域を塗りつぶす
}

void draw() {
  stroke(0);
  strokeWeight(1);
  fill(45, 45, 45);
  rect( 15,  15,  380, 585);        // 左下の領域を塗りつぶす
  rect( 15, 615,  380, 170);        // 左下の領域を塗りつぶす
  rect(410,  15,  380, 370);        // 左下の領域を塗りつぶす
  rect(410, 415,  380, 370);        // 左下の領域を塗りつぶす
  rect(805,  15,  380, 370);        // 左下の領域を塗りつぶす

  int textPosX =  15;
  int textPosY =  33;
  int DataPos  = 170;
  
  textSize(30);
  fill(240, 240, 240);
  
  String RGBText      = str(red) + ", " + str(green) +  ", " + str(blue);
  String colorText    = "Black";
  String TimeText     = str(runningTime);
  String DistanceText = str(distance);
  String PositionText = str(robotPosX) + ", " + str(robotPosY);
  String AngleText    = str(angle);
  
  text(     "RGB = ",  15 + textPosX,  15 + textPosY); text(     RGBText,  15 + textPosX + DataPos,  15 + textPosY);
  text(   "Color = ",  15 + textPosX, 300 + textPosY); text(   colorText,  15 + textPosX + DataPos, 300 + textPosY);
  text(     "Time: ",  15 + textPosX, 615 + textPosY); text(    TimeText,  15 + textPosX + DataPos, 615 + textPosY);
  text("Distance = ", 410 + textPosX,  15 + textPosY); text(DistanceText, 410 + textPosX + DataPos,  15 + textPosY);                
  text("Position = ", 410 + textPosX, 415 + textPosY); text(PositionText, 410 + textPosX + DataPos, 415 + textPosY);
  text(   "Angle = ", 805 + textPosX,  15 + textPosY); text(   AngleText, 805 + textPosX + DataPos,  15 + textPosY);
  
  int graphPosX = 68;
  int graphPosY = 270;
  int graphW = 80;
  int graphH = 180;
  
  fill( red, 0, 0); rect(graphPosX,                     graphPosY, graphW, -map(  red, 0, 255, 0, graphH));
  fill(0, green,0); rect(graphPosX + graphW + 15,       graphPosY, graphW, -map(green, 0, 255, 0, graphH));
  fill(0, 0, blue); rect(graphPosX + (graphW + 15) * 2, graphPosY, graphW, -map( blue, 0, 255, 0, graphH));
 
  int colorBoxX = 95;
  int colorBoxY = 365;
  int colorBoxSize = 200;

  fill(red, green, blue); rect(colorBoxX, colorBoxY, colorBoxSize,  colorBoxSize);
  
  int distanceBarX =  63;
  int distanceBarY = 300;
  int distanceBarW = 250;
  int distanceBarH =  30;
  int distanceObjX = distanceBarX + distanceBarW / 2;
  int distanceObjY = 200;
  int distanceObjSize = 30;

  fill(20, 210, 20); rect(410 + distanceBarX, distanceBarY , distanceBarW,  distanceBarH);
  fill(210, 20, 20); ellipse(410 + distanceObjX, distanceBarY - map(distance, 0, 32, 0, distanceObjY), distanceObjSize,  distanceObjSize);
  
  int xyGraphX = 90;
  int xyGraphY = 110;
  int xyGraphSize = 200;

  fill(200, 200, 200); rect(410 + xyGraphX, 415 + xyGraphY, xyGraphSize, xyGraphSize);
  stroke(20, 200, 20);
  line(410 + xyGraphX, 415 + xyGraphY + xyGraphSize / 2, 410 + xyGraphX + xyGraphSize, 415 + xyGraphY + xyGraphSize / 2);
  line(410 + xyGraphX + xyGraphSize / 2, 415 + xyGraphY, 410 + xyGraphX + xyGraphSize / 2, 415 + xyGraphY + xyGraphSize);

  strokeWeight(0);
  fill(210, 20, 20); ellipse(410 + xyGraphX + xyGraphSize / 2 + robotPosX, 415 + xyGraphY + xyGraphSize / 2 - robotPosY, 15,  15);
  
  int robotX = 125;
  int robotY = 150;
  int robotW =  110;
  int robotH = 150;
  translate( 805 + robotX + robotW / 2, 15 + robotY + robotH / 2);
  rotate(angle);
  fill(30, 200, 30); rect(-robotW / 2, -robotH / 2, robotW,  robotH);
}


void serialEvent(Serial p) {
  if (p.available() > 6) {
    red = p.read();
    int gb = p.read();
    green = gb >> 4;
    blue = gb & 15;
    distance = p.read();
    angle = p.read();
    robotPosX = p.read();
    robotPosY = p.read();
    if(p.read()!= 0x0A){
      println("Error : Bits are not correct");
      red = 0; green = 0; blue = 0;
      distance = 0; angle = 0; robotPosX = 0; robotPosY = 0;
    }
  }     
}
