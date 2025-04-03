int sizem=63;
int bp1=2;
int bp2=3;


#include <DFRobot_RGBMatrix.h> // Hardware-specific library
#include <Wire.h>
#define OE    9
#define LAT   10
#define CLK   11
#define A     A0
#define B     A1
#define C     A2
#define D     A3
#define E     A4
#define WIDTH 64
#define HIGH  64
int a=0;
int b=16;
int score=0;
DFRobot_RGBMatrix matrix(A, B, C, D, E, CLK, LAT, OE, false, WIDTH, HIGH);
uint16_t red=matrix.Color333(7, 0, 0);
uint16_t blue=matrix.Color333(0, 0, 7);
uint16_t green=matrix.Color333(0, 7, 0);
uint16_t yellow=matrix.Color333(7, 7, 0);
uint16_t white=matrix.Color333(7, 7, 7);
uint16_t black=matrix.Color333(0, 0, 0);
//uint16_t colors[]={red,blue,yellow};
volatile long int temps1=0;
uint16_t game [10][20];

int pile1[]={33, 3, 35, 5};
int pile2[]={36, 3, 38, 5};
uint16_t c1=red;
uint16_t c2=blue;
bool fa=true;

void setup()
{
  Serial.begin(9600);
  pinMode(bp1,INPUT_PULLUP);
  pinMode(bp2,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(bp1), stepleft, FALLING);
  attachInterrupt(digitalPinToInterrupt(bp2), stepright, FALLING);
  matrix.begin();
  for(int i=0;i<10;i++){
    for (int j=0;j<20;j++){
      game[i][j]=black;
    }
  }
  game[0][19]=red;
  // fill the screen with 'black'
  matrix.fillScreen(matrix.Color333(0, 0, 0));
  matrix.drawLine(0, 0, 63, 0, white);
  matrix.drawLine(0, 0,0, 63, white);
  matrix.drawLine(0, 63, 63, 63, white);
  matrix.drawLine(63, 0, 63, 63, white);
  int border=31;
  for(int i=0;i<64;i++){
    matrix.drawPixel(border, i, matrix.Color333(7, 7, 7)); 
    matrix.drawPixel(border+1, i, matrix.Color333(7, 7, 7)); 
  }
  matrix.setTextSize(1);     // size 1 == 8 pixels high
  matrix.setCursor(1, 2);
  matrix.println("score");
   matrix.setCursor(1, 12);
  matrix.println(score);
  matrix.setTextColor(matrix.Color333(4, 0, 4));//
  drawVirus();
  delay(500);
  //test
  //drawPile(50,20,52,21,green);;
  //drawPile(53,20,55,21,red);;
  /*uint16_t c=red;
  for(int i=3;i<62;i=i+3){
    //drawPile(50,i,52,i+3,c);;
    if (c==green){
      c=red;
    }else{
      c=green;
    }
  }*/
}

int drawPile(int pile[],uint16_t c){
  int x1=pile[0];
  int y1=pile[1];
  int x2=pile[2];
  int y2=pile[3];
  for(int y=y1;y<y2+1;y++){
    matrix.drawLine(x1, y, x2, y, c);
  }
  return 1; //<-sert pour fun heal
}

uint16_t randomColors() {
  //return colors[random(3)],colors[random(3)];  //<- ça marche !?!?
}

uint16_t randomColor() {
//  return colors[random(3)];
}

void generateViruses(int n) {
  int i=0;
  while (i<=n) {
    int x = random(10);
    int y = random(10);
    if (game[x][y]==black) {
      game[random(10)][random(10)]=randomColor(); //10;10 ???
      i++;
    }
  }
}

void drawVirus(){
  for(int i=0;i<10;i++){
    for (int j=0;j<20;j++){
      if (game[i][j]!=black){
        matrix.drawLine(i*3+33, j*3+3, i*3+35, j*3+5, red);
        matrix.drawLine(i*3+33, j*3+5, i*3+35, j*3+3, red);
      }
    }
  }
}

void swapile(){
  int a=1;
}

int heal(int x,int y,uint16_t c,String dir,int nb){
  int r=0;
  int l=0;
  int d=0;
  int u=0;
  //Serial.println(x);
  //Serial.println(y);
  //Serial.println(dir);
  //Serial.println(game[x][y+1]==c);
  //Serial.println(game[x][y+1]==black);
  if ((dir=="n"|| dir=="r") && game[x+1][y]==c){
    //Serial.println("r");
    //Serial.println(nb);
    int h1=nb;
    h1=heal(x+1,y,c,"r",h1+1); //variable prend en compte pour cross
    if(h1>3 && nb==1){
      r=h1;
      cleanBoard(x,y,"r",r);
    }
    else if (h1!=0){
      return h1;
    }
  }
  if ((dir=="n"|| dir=="l") && game[x-1][y]==c){
    //Serial.println("l");
    //Serial.println(nb);
    int h2=nb;
    h2=heal(x-1,y,c,"l",h2+1);
    if(h2>3 && nb==1){
      l=h2;
      cleanBoard(x,y,"l",l);
    }
    else if (h2!=0){
      return h2;
    }
  }
  if ((dir=="n"|| dir=="d") && game[x][y+1]==c){
    int h3=nb;
    h3=heal(x,y+1,c,"d",h3+1);
    if(h3>3 && nb==1){
      d=h3;
      cleanBoard(x,y,"d",d);
    }
    else if (h3!=0){
      return h3;
    }
  }
  if ((dir=="n"|| dir=="u") && game[x][y-1]==c){
    //Serial.println("u");
    //Serial.println(nb);
    int h4=nb;
    h4=heal(x,y-1,c,"u",h4+1);
    if(h4>3 && nb==1){
      u=h4;
      cleanBoard(x,y,"u",u);
    }
    else if (h4!=0){
      return h4;
    }
  }else{
    if(nb>3){
      return nb;
    }else{
      return 0;
    }
  }
}

int cleanBoard(int x,int y,String dir,int nb){
  Serial.println(dir);
  Serial.println(nb);
  if(dir=="l"){
    for(int i=0;i<nb;i++){
      game[x-i][y]=black;
      int pileb[]={(x+1-i)*3,y*3,(x+1-i)*3,y*3+2};
      drawPile(pileb,black);
    }
  }
  if(dir=="r"){
    for(int i=0;i<nb;i++){
      game[x+i][y]=black;
      int pileb[]={(x+1+i),y*3,(x+1+i)*3+2,y*3+2};
      drawPile(pileb,black);
    }
  }
  if(dir=="u"){
    for(int i=0;i<nb;i++){
      game[x][y-i]=black;
      int pileb[]={x*3+3,(y-i)*3,x*3+5,(y-i)*3+2};
      drawPile(pileb,black);
    }
  }
  if(dir=="d"){
    for(int i=0;i<nb;i++){
      //Serial.println(i);
      Serial.println(x);
      Serial.println(y+i);
      game[x][y+i]=black;
      int pileb[]={x*3+33,(y+i)*3+3,x*3+35,(y+i)*3+5};
      drawPile(pileb,black);
    }
  }
  return 1; //<-sert pour fun heal
}

void stepleft(){
  long int temps=micros();
  drawPile(pile1,black);
  drawPile(pile2,black);
  int x1=pile1[0];
  int x2=pile2[0];
  int y1=pile1[1];
  int y2=pile2[1];
  if(x1>33 && x2>33 && game[(x1-33)/3-1][(y1-3)/3]==black && game[(x2-33)/3-1][(y2-3)/3]==black && fa){
    if (temps-temps1>100000){
      pile1[0]=x1-3;
      pile1[2]=x1-1;
      pile2[0]=x2-3;
      pile2[2]=x2-1;
      //fa=true;
    }
  }
  //Serial.println("leftttttt");
  //Serial.println(x1/3-11);
  fa=hit(pile1,c1,fa);
  fa=hit(pile2,c2,fa);
  fa=hit(pile1,c1,fa);
  drawPile(pile1,c1);
  drawPile(pile2,c2);
  temps1=temps;
}

void stepright(){ //ajouter collision horizontal
  long int temps=micros();
  drawPile(pile1,black);
  drawPile(pile2,black);
  int x1=pile1[2];
  int x2=pile2[2];
  int y1=pile1[1];
  int y2=pile2[1];
  if(x1<62 && x2<62 && game[(x1-33)/3+1][(y1-3)/3]==black && game[(x2-33)/3+1][(y2-3)/3]==black && fa){
    if (temps-temps1>100000){
      pile1[0]=x1+1;
      pile1[2]=x1+3;
      pile2[0]=x2+1;
      pile2[2]=x2+3;
      //fa=true;
    }
  }
  fa=hit(pile1,c1,fa);
  fa=hit(pile2,c2,fa);
  fa=hit(pile1,c1,fa);
  drawPile(pile1,c1);
  drawPile(pile2,c2);
  temps1=temps;
}

int fall(int py){
  return py+3;
}

bool hit(int pile[],uint16_t c,bool fa){  //rajouter heal
  int x1=pile[0];
  int y1=pile[1];
  int x2=pile[2];
  int y2=pile[3];
  Serial.println((x2-33)/3);
  Serial.println("vibe check");
  Serial.println(game[0][int(y2/3)+1]!=black);
  Serial.println(game[int((x2-33)/3)][int(y2/3)+1]!=black);
  if ((int(y2/3)+1)>19 || game[int((x2-33)/3)][int(y2/3)+1]!=black || fa==false){
    game[int((x2-33)/3)][int((y2)/3)]=c;
    fa=false;
  }
  return fa;
}

void loop()
{  
  int x=pile1[0]/3-11;
  int y=pile1[1]/3-1;
  Serial.println("start loop");
  Serial.println(pile1[0]/3-11);
  //int pile1[]={34, 1, 36, 3};
  //int pile2[]={37, 1, 39, 3};
  /*drawPile(pile1,c1);
  drawPile(pile2,c2);*/
  //delay(500);
  if(fa){
    drawPile(pile1,black);
    drawPile(pile2,black);
    Serial.println("f*ck you disco");
    Serial.println(pile1[0]/3-11);
    fa=hit(pile1,c1,fa);
    fa=hit(pile2,c2,fa);
    fa=hit(pile1,c1,fa);
    pile1[1]=fall(pile1[1]);
    pile1[3]=fall(pile1[3]);
    pile2[1]=fall(pile2[1]);
    pile2[3]=fall(pile2[3]);
    drawPile(pile1,c1);
    drawPile(pile2,c2);
    //Serial.println(y);
    //Serial.println(int(pile1[1]/3));
    
  }else{
    heal(x,y,c1,"n",1);
    x=pile2[0]/3-11;
    y=pile1[1]/3-1;
    heal(x,y,c2,"n",1);
    pile1[1]=3;
    pile1[3]=5;
    pile2[1]=3;
    pile2[3]=5;
    drawPile(pile1,c1);
    drawPile(pile2,c2);
    fa=true;
    //delay(10000);
  }
  Serial.println(pile1[0]/3-11);
  Serial.println("now delay");
  delay(500);
  Serial.println("end loop");
  Serial.println(pile1[0]/3-11);
  /*byte i;
  if(a<=-64)
{a=64;
  }
  if(b<=-48)
{b=80;
  }
  matrix.fillScreen(0);//clear the screen
  matrix.setTextSize(2);//Set as 16*16 pixel
  matrix.setCursor(15, a);    // start at top left, with 8 pixel of spacing
  matrix.setTextColor(matrix.Color333(4, 0, 4));//
  matrix.println("DFR");//
  matrix.setTextSize(1);//Set as 8*8 pixel
  matrix.setCursor(12, b);
  matrix.println("DFROBOT");
  matrix.setTextSize(1);
  matrix.print("Let's joinhands witheach otherto create a future");
  delay(600);
  a=a-6;
  b=b-6;*/
  //matrix.fillScreen(matrix.Color333(5, 5, 0));
}


/***************************************************
*
* For sizemxsizem RGB LED matrix.
*
* @author lg.gang
* @version  V1.0
* @date  2016-10-28
*
* GNU Lesser General Public License.
* See <http://www.gnu.org/licenses/> for details.
* All above must be included in any redistribution
* ****************************************************/
/*
#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library

// If your sizemxsizem matrix has the SINGLE HEADER input,
// use this pinout:
#define CLK 8  // MUST be on PORTB! (Use pin 11 on Mega)
#define OE  9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3
#define E     A4
#define WIDTH sizem
#define HIGH  sizem
// If your matrix has the DOUBLE HEADER input, use:
//#define CLK 8  // MUST be on PORTB! (Use pin 11 on Mega)
//#define LAT 9
//#define OE  10
//#define A   A3
//#define B   A2
//#define C   A1
//#define D   A0
//RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false,WIDTH, HIGH);
#include <DFRobot_RGBMatrix.h> // Hardware-specific library
#include <Wire.h>
#define OE    9
#define LAT   10
#define CLK   11
#define A     A0
#define B     A1
#define C     A2
#define D     A3
#define E     A4
#define WIDTH sizem
#define HIGH  sizem
int a=0;
int b=16;
DFRobot_RGBMatrix matrix(A, B, C, D, E, CLK, LAT, OE, false, WIDTH, HIGH);


void setup() {

  matrix.begin();

  // whew!
}

void loop() {
  // fix the screen with red
  matrix.fillRect(0, 0, sizem, sizem, matrix.Color333(7, 0, 0));
  delay(500);
  // fix the screen with green
  matrix.fillRect(0, 0, sizem, sizem, matrix.Color333(0, 7, 0));
  delay(500);
  // fix the screen with blue
  matrix.fillRect(0, 0, sizem, sizem, matrix.Color333(0, 0, 7));
  delay(500);
  // fix the screen with yellow
  matrix.fillRect(0, 0, sizem, sizem, matrix.Color333(7, 7, 0));
  delay(500);
  // fix the screen with purple
  matrix.fillRect(0, 0, sizem, sizem, matrix.Color333(7, 0, 7));
  delay(500);
  // fix the screen with cyan
  matrix.fillRect(0, 0, sizem, sizem, matrix.Color333(0,7, 7));
  delay(500);
  // fix the screen with white
  matrix.fillRect(0, 0, sizem, sizem, matrix.Color333(3, 3, 3));
  delay(500);

   // fill the screen with 'black'
  matrix.fillScreen(matrix.Color333(0, 0, 0));
  // draw a pixel in solid white
  matrix.drawPixel(0, 0, matrix.Color333(7, 7, 7));
  delay(500);

  // fix the screen with green
  matrix.fillRect(0, 0, sizem, sizem, matrix.Color333(0, 7, 0));
  delay(500);

  // draw a box in yellow
  matrix.drawRect(0, 0, sizem, sizem, matrix.Color333(7, 7, 0));
  delay(500);

  // draw an 'X' in red
  matrix.drawLine(0, 0, 63, 63, matrix.Color333(7, 0, 0));
  matrix.drawLine(63, 0, 0, 63, matrix.Color333(7, 0, 0));
  delay(500);

  // draw a blue circle
  matrix.drawCircle(10, 10, 10, matrix.Color333(0, 0, 7));
  delay(500);

  // fill a violet circle
  matrix.fillCircle(21, 21, 10, matrix.Color333(7, 0, 7));
  delay(500);

  // fill the screen with 'black'
  matrix.fillScreen(matrix.Color333(0, 0, 0));

  // draw some text!
  matrix.setCursor(1, 0);    // start at top left, with one pixel of spacing
  matrix.setTextSize(1);     // size 1 == 8 pixels high
  matrix.setTextWrap(false); // Don't wrap at end of line - will do ourselves

  matrix.setTextColor(matrix.Color333(0,7,0));
  matrix.println("green");
  matrix.setTextColor(matrix.Color333(0,7,7));
  matrix.println("robot");

  // print each letter with a rainbow color
  matrix.setTextColor(matrix.Color333(7,0,0));
  matrix.print('3');
  matrix.setTextColor(matrix.Color333(7,4,0));
  matrix.print('2');
  matrix.setTextColor(matrix.Color333(7,7,0));
  matrix.print('x');
  matrix.setTextColor(matrix.Color333(4,7,0));
  matrix.print('3');
  matrix.setTextColor(matrix.Color333(0,7,0));
  matrix.println('2');

  matrix.setTextColor(matrix.Color333(0,7,7));
  matrix.print('*');
  matrix.setTextColor(matrix.Color333(0,4,7));
  matrix.print('R');
  matrix.setTextColor(matrix.Color333(0,0,7));
  matrix.print('G');
  matrix.setTextColor(matrix.Color333(4,0,7));
  matrix.print('B');
  matrix.setTextColor(matrix.Color333(7,0,4));
  matrix.print('*');
  delay(5000);
}
*/