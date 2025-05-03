int sizem = 63;
int bp1 = 2;
int bp2 = 3;
int bp3=18;
int bp4=19;
//const int buzz=14;
int muco;

#include <DFRobot_RGBMatrix.h> // Hardware-specific library
#include <Wire.h>
//#include "MeAuriga.h"
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

int a = 0;
int b = 16;
int score = 0;
int play=1;
int lastScore = -1; // Pour détecter les changements de score
int level = 1;
DFRobot_RGBMatrix matrix(A, B, C, D, E, CLK, LAT, OE, false, WIDTH, HIGH);
bool start = false; // bool pour le menu pour commencer le jeu
//MeBuzzer buzzer;

// Définition des couleurs
uint16_t red = matrix.Color333(7, 0, 0);
uint16_t blue = matrix.Color333(0, 0, 7);
uint16_t green = matrix.Color333(0, 7, 0);
uint16_t yellow = matrix.Color333(7, 7, 0);
uint16_t white = matrix.Color333(7, 7, 7);
uint16_t black = matrix.Color333(0, 0, 0);
uint16_t bgColor = matrix.Color333(1, 1, 1);    // Gris pour le fond

volatile long int temps1 = 0;
uint16_t game[10][20];
int gamepile[10][20][2];
int pile1[4];
int pile2[4];
uint16_t c1;
uint16_t c2;
bool fa = false;
bool move=false;

void setup() {
  randomSeed(analogRead(0));
  Serial.begin(9600);
  Serial2.begin(9600);
  pinMode(bp1, INPUT_PULLUP);
  pinMode(bp2, INPUT_PULLUP);
  pinMode(bp3,INPUT_PULLUP);
  pinMode(bp4,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(bp1), stepleft, FALLING);
  attachInterrupt(digitalPinToInterrupt(bp2), stepright, FALLING);
  attachInterrupt(digitalPinToInterrupt(bp3), spin, FALLING);
  attachInterrupt(digitalPinToInterrupt(bp4), commencer, FALLING);
  matrix.begin();
  
  // Initialisation du tableau de jeu
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 20; j++) {
      game[i][j] = black;
      gamepile[i][j][0]=0;
      gamepile[i][j][1]=0;
    }
  }
}

void envoi(int n) {
  Serial2.print(n);
}

void drawBorders() {

  // Game Borders
  matrix.drawLine(0, 0, 63, 0, white);
  matrix.drawLine(0, 0, 0, 63, white);
  matrix.drawLine(0, 63, 63, 63, white);
  matrix.drawLine(63, 0, 63, 63, white);

  // Border underneath Score
  matrix.drawLine(1, 22, 30, 22, white);
  matrix.drawLine(1, 27, 30, 27, white);

  // Border underneath Level
  matrix.drawLine(1, 45, 30, 45, white);
  matrix.drawLine(1, 46, 30, 46, white);
}

void drawDivider() {
  for (int i = 0; i < 64; i++) {
    matrix.drawPixel(31, i, white); 
    matrix.drawPixel(31 + 1, i, white); 
  }
}

void displayScore() {
  matrix.fillRect(1, 1, 30, 12, bgColor);
  matrix.setTextSize(1);
  matrix.setCursor(1, 1);
  matrix.setTextColor(white);
  matrix.println("Score");
  matrix.setCursor(1, 10);
  matrix.setTextColor(green);
  matrix.println(score);
}

void displayLevel() {
  matrix.fillRect(1, 20, 30, 12, bgColor);
  matrix.setTextSize(1);
  matrix.setCursor(2, 27);
  matrix.setTextColor(white);
  matrix.println("Level");
  matrix.setCursor(1, 28);
  matrix.setTextColor(yellow);
  matrix.println(level);
}

void updateScoreDisplay() {
  if (score != lastScore) {
    // Effet d'animation pour les nouveaux points
    matrix.fillRect(1, 10, 30, 8, bgColor);
    matrix.setTextSize(1);
    matrix.setCursor(1, 10);
    matrix.setTextColor(green);
    matrix.println(score);
    //delay(50);
    
    // Retour à l'affichage normal
    matrix.fillRect(1, 10, 30, 8, bgColor);
    matrix.setTextSize(1);
    matrix.setCursor(1, 10);
    matrix.setTextColor(green);
    matrix.println(score);
    
    lastScore = score;
    
    // Vérifier si le niveau doit augmenter
    if (score >= level * 1000) {
      level++;
      displayLevel();
    }
  }
}

void losescreen(){
  matrix.drawLine(0,0,63,63, red);
  matrix.drawLine(0, 63, 63, 0, red);
}
uint16_t randomColor() {
  switch (random(3)) {
    case 0: return red;
    case 1: return blue;
    case 2: return green;
    default: return red;
  }
}

void generateViruses(int n) {
  int virusesGenerated = 0;
  while (virusesGenerated < n) {
    int x = random(10);  // Position X aléatoire (0-9)
    int y = random(15) + 5; // Position Y aléatoire (5-19) pour éviter le haut de l'écran
    
    // Vérifier si la case est vide
    if (game[x][y] == black) {
      game[x][y] = randomColor(); // Assigner une couleur aléatoire
      
      // Dessiner le virus sur la matrice
      drawVirus(x,y);
      
      virusesGenerated++;
    }
  }
}

void drawVirus(int x, int y) {
  matrix.drawLine(x * 3 + 33, y * 3 + 3, x * 3 + 35, y * 3 + 5, game[x][y]);
  matrix.drawLine(x * 3 + 33, y * 3 + 5, x * 3 + 35, y * 3 + 3, game[x][y]);
}

void drawPile(int pile[], uint16_t c) {
  int x1 = pile[0];
  int y1 = pile[1];
  int x2 = pile[2];
  int y2 = pile[3];
  for (int y = y1; y < y2 + 1; y++) {
    matrix.drawLine(x1, y, x2, y, c);
  }
}

void commencer(){
  start=true;
}

void stepleft() {
  long int temps = micros();
  int x1 = pile1[0];
  int x2 = pile2[0];
  int y1 = pile1[1];
  int y2 = pile2[1];
  if (temps - temps1 > 100000 && x1 > 33 && x2 > 33 && game[(x1 - 33) / 3 - 1][(y1 - 3) / 3] == black && game[(x2 - 33) / 3 - 1][(y2 - 3) / 3] == black && fa && move) {
    drawPile(pile1, black);
    drawPile(pile2, black);
    pile1[0] = x1 - 3;
    pile1[2] = x1 - 1;
    pile2[0] = x2 - 3;
    pile2[2] = x2 - 1;
    fa = hit(pile1,pile2, c1, fa);
    fa = hit(pile2,pile1, c2, fa);
    fa = hit(pile1,pile2, c1, fa);
    drawPile(pile1, c1);
    drawPile(pile2, c2);
    temps1 = temps;
  }  
}

void stepright() {
  long int temps = micros();
  int x1 = pile1[2];
  int x2 = pile2[2];
  int y1 = pile1[1];
  int y2 = pile2[1];
  if (temps - temps1 > 100000 && x1 < 62 && x2 < 62 && game[(x1 - 33) / 3 + 1][(y1 - 3) / 3] == black && game[(x2 - 33) / 3 + 1][(y2 - 3) / 3] == black && fa && move) {
    drawPile(pile1, black);
    drawPile(pile2, black);
    pile1[0] = x1 + 1;
    pile1[2] = x1 + 3;
    pile2[0] = x2 + 1;
    pile2[2] = x2 + 3;
    fa = hit(pile1,pile2, c1, fa);
    fa = hit(pile2,pile1, c2, fa);
    fa = hit(pile1,pile2, c1, fa);
    drawPile(pile1, c1);
    drawPile(pile2, c2);
    temps1 = temps;
  }
}

void spin(){
  long int temps=micros();
  if (temps-temps1>100000 and fa){
    drawPile(pile1,black);
    drawPile(pile2,black);
    int x1=pile1[0]/3-11;
    int x2=pile2[0]/3-11;
    int y1=pile1[1]/3-1;
    int y2=pile2[1]/3-1;
    if(y1==y2 && y1>0){
      if(x2>x1 && game[x2][y1-1]==black){   //penser à ajouter fit
        pile1[0]=pile2[0];
        pile1[2]=pile2[2];
        pile1[1]=pile2[1]-3;
        pile1[3]=pile2[3]-3;
      }
      else if(game[x1][y1-1]==black){
        pile2[0]=pile1[0];
        pile2[2]=pile1[2];
        pile2[1]=pile1[1]-3;
        pile2[3]=pile1[3]-3;
      }
    }else if (x1==x2 && x1>0){ //x1==x2 nécessaire pour eviter bug du no pile land
      if(y2>y1 && game[x1-1][y2]==black){
        pile2[0]=pile2[0]-3;
        pile2[2]=pile2[2]-3;
        pile1[1]=pile2[1];
        pile1[3]=pile2[3];
      }else if (game[x1-1][y1]==black){
        pile1[0]=pile1[0]-3;
        pile1[2]=pile1[2]-3;
        pile2[1]=pile1[1];
        pile2[3]=pile1[3];
      }
    }
    fa = hit(pile1,pile2, c1, fa);
    fa = hit(pile2,pile1, c2, fa);
    fa = hit(pile1,pile2, c1, fa);
    drawPile(pile1,c1);
    drawPile(pile2,c2);
    temps1=temps;
  }
}

void tofall(){
  long int temps=micros();
  if (temps-temps1>100000 && move){
    drawPile(pile1,black);
    drawPile(pile2,black);
    while (fa){
      pile1[1]=pile1[1]+3;
      pile2[1]=pile2[1]+3;
      pile1[3]=pile1[3]+3;
      pile2[3]=pile2[3]+3;
      fa = hit(pile1,pile2, c1, fa);
      fa = hit(pile2,pile1, c2, fa);
      fa = hit(pile1,pile2, c1, fa);
    }
  drawPile(pile1,c1);
  drawPile(pile2,c2);
  temps1=temps;
  }
}

void topile(int pile[],uint16_t c){
  int x=pile[0]/3-11;
  int y=pile[1]/3-1;
  //Serial.print(pile[1]);
  game[x][y]=black;
  bool tp=true;
  drawPile(pile,black);
  while (tp){
    if (game[pile[0]/3-11][(pile[1]/3-1)+1]!=black || pile[1]==63){
      tp=false;
    }else{
    pile[1]=pile[1]+3;
    pile[3]=pile[3]+3;
    }
  }
  game[x][y]=c;
  drawPile(pile,c);
}

int fall(int py) {
  return py + 3;
}

bool hit(int pilea[],int pileb[], uint16_t c, bool fa) {
  int x1=pilea[0]/3-11;
  int y1=pilea[1]/3-1;
  int x2=pilea[2]/3-11;
  int y2=pilea[3]/3-1;
  
  if (y2+1>19 || game[x2][y2+1]!=black || fa==false){
    game[x2][y2]=c;
    gamepile[x2][y2][0]=pileb[0];
    gamepile[x2][y2][1]=pileb[1];
    score += 10; // Augmentation du score quand un bloc se pose
    updateScoreDisplay();
    fa = false;
  }
  return fa;
}

void cleanBoard(int x,int y,char dir,int nb){
  int pile[4];
  //Serial.print("do you?");
  if(dir=='r'){
    for(int i=0;i<nb;i++){
      game[x+i][y]=black;
      if (gamepile[x+i][y][0]!=0){
        pile[0]=gamepile[x+i][y][0];;
        pile[1]=gamepile[x+i][y][1];;
        pile[2]=pile[0]+3;;
        pile[3]=pile[1]+3;;
        uint16_t pc=game[pile[0]/3-11][pile1[1]/3-1];
        //topile(pile,pc);
      }
      int pileb[]={(x+i)*3+33,y*3+3,(x+i)*3+35,y*3+5};
      drawPile(pileb,black);
    }
  }
  else{
    for(int i=0;i<nb;i++){
      game[x][y-i]=black;
      if (gamepile[x][y-i][0]!=0){
        pile[0]=gamepile[x][y-i][0];;
        pile[1]=gamepile[x][y-i][1];;
        Serial.println(pile[0]);
        Serial.print(pile[1]);
        pile[2]=pile[0]+2;;
        pile[3]=pile[1]+2;;
        uint16_t pc=game[pile[0]/3-11][pile1[1]/3-1];
        //topile(pile,pc);
      }
      int pileb[]={x*3+33,(y-i)*3+3,x*3+35,(y-i)*3+5};
      drawPile(pileb,black);
    }
  }
  envoi(nb-3);
}

int heal(int x,int y,uint16_t c,char dir,int nb){
  //Serial.println(dir);
  int r=0;
  int l=0;
  int d=0;
  int u=0;
  if ((dir=='n'|| dir=='r') && game[x+1][y]==c){
    int h1=nb;
    h1=heal(x+1,y,c,'r',h1+1);
    if(h1>3 && nb==1){
      r=h1;
      cleanBoard(x,y,'r',r);
    }
    else if (h1!=0){
      return h1;
    }
  }
  if ((dir=='n'|| dir=='u') && game[x][y-1]==c){
    int h3=nb;
    h3=heal(x,y-1,c,'u',h3+1);
    if(h3>3 && nb==1){
      u=h3;
      cleanBoard(x,y,'u',u);
    }
    else if (h3!=0){
      return h3;
    }
  }else{
    if(nb>3){
      return nb;
    }else{
      return 0;
    }
  }
}

int checkborderx(int x,int y,uint16_t c){
  int nux=0;
  while (game[x-nux-1][y]==c){
    nux++;
  }
  return nux;
}
int checkbordery(int x,int y,uint16_t c){
  int nuy=0;
  while (game[x][y+nuy+1]==c){
    nuy++;
  }
  return nuy;
}

/*void treugame(){  // ajoute l'affichage du modèle
  for(int i=0;i<20;i++){
    for (int j=0;j<10;j++){
      Serial.print("[");
      if (game[j][i]==black){
        Serial.print(" ");
      }else{
        Serial.print("X");
      }
      Serial.print("]");
    }
    Serial.println();
  }
}*/

void loop() {
  while (!start) {
    // Game start!
    // Main Menu
    // Borders
    matrix.drawLine(0, 0, 63, 0, white);
    matrix.drawLine(0, 0, 0, 63, white);
    matrix.drawLine(0, 63, 63, 63, white);
    matrix.drawLine(63, 0, 63, 63, white);

    // Dr Mario Text
    matrix.setTextSize(1);
    matrix.setCursor(10,10);
    matrix.setTextColor(yellow);
    matrix.println("Dr.Mario");

    // Start button
    matrix.setTextSize(1);
    matrix.setCursor(17,40);
    matrix.setTextColor(white);
    matrix.println("Start");
    if (start) {
       pile1[0] = 33; pile1[1] = 3; pile1[2] = 35; pile1[3] = 5;
      pile2[0] = 36; pile2[1] = 3; pile2[2] = 38; pile2[3] = 5;
      c1 = randomColor();
      c2 = randomColor();
      drawPile(pile1, c1);
      drawPile(pile2, c2);
      attachInterrupt(digitalPinToInterrupt(bp4), tofall, FALLING);
      //buzzer.tone(500,10);
      // Initialisation de l'affichage
      matrix.fillScreen(matrix.Color333(0, 0, 0));
      drawBorders();
      drawDivider();
      displayScore();
      displayLevel();
      
      matrix.setTextColor(matrix.Color333(4, 0, 4));
      delay(500);

      generateViruses(10);
    }
  }
  if (Serial2.available() > 0) {
    muco = Serial2.read()-48;
    generateViruses(muco);
  }
  int x=pile1[0]/3-11;
  int y=pile1[1]/3-1;
  if (fa) {
    move=false;
    drawPile(pile1, black);
    drawPile(pile2, black);
    
    pile1[1] = fall(pile1[1]);
    pile1[3] = fall(pile1[3]);
    pile2[1] = fall(pile2[1]);
    pile2[3] = fall(pile2[3]);

    drawPile(pile1, c1);
    drawPile(pile2, c2);
    
    fa = hit(pile1,pile2, c1, fa);
    fa = hit(pile2,pile1, c2, fa);
    fa = hit(pile1,pile2, c1, fa); //<- nécéssaire pour heal()
    move=true;
    
  } else if (play==1) {
    int nux=x-checkborderx(x,y,c1);
    int nuy=y+checkbordery(x,y,c1);
    if (x!=nux){
      heal(nux, y, c1, 'n', 1);
    }
    heal(x, nuy, c1, 'n', 1);
    
    x = pile2[0] / 3 - 11;
    y = pile2[1] / 3 - 1;
    nux=x-checkborderx(x,y,c2);
    nuy=y+checkbordery(x,y,c2);
    if (x!=nux){
      heal(nux, y, c2, 'n', 1);
    }
    heal(x, nuy, c2, 'n', 1);

    
    // Réinitialiser les piles pour la chute suivante
    pile1[0] = 33; pile1[1] = 3; pile1[2] = 35; pile1[3] = 5;
    pile2[0] = 36; pile2[1] = 3; pile2[2] = 38; pile2[3] = 5;
    
    // Changer les couleurs pour le prochain bloc
    c1 = randomColor();
    c2 = randomColor();
    
    drawPile(pile1, c1);
    drawPile(pile2, c2);
    if(game[0][1]!=black || game[1][1]!=black){
      losescreen();
      play=0;
    }else{
      fa = true;
    }
  }
  //treugame();
  updateScoreDisplay();
  delay(2000 - (level * 50)); // Le jeu s'accélère avec le niveau
}

// Les fonctions heal(), cleanBoard() et comboEffect() restent identiques à votre version originale
// Elles ont été conservées mais no non modifiées pour garder la logique du jeu