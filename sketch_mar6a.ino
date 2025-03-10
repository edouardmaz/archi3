int sizem=63;



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
String game [10][20];

void setup()
{
  matrix.begin();
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
  delay(500);
  //test
  int pile[]={34, 25, 39, 26};
  int sta=33;
  drawPile(pile,green);;
  //drawPile(50,20,52,21,green);;
  //drawPile(53,20,55,21,red);;
  uint16_t c=red;
  for(int i=3;i<62;i=i+3){
    //drawPile(50,i,52,i+3,c);;
    if (c==green){
      c=red;
    }else{
      c=green;
    }
  }
}

void drawPile(int pile[],uint16_t c){
  int x1=pile[0];
  int y1=pile[1];
  int x2=pile[2];
  int y2=pile[3];
  for(int y=y1;y<y2+1;y++){
    matrix.drawLine(x1, y, x2, y, c);
  }
}

void fall(int x1,int y1,int x2,int y2,bool e){
  a=1;
}

void loop()
{  /*byte i;
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