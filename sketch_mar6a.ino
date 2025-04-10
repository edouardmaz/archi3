int sizem = 63;
int bp1 = 2;
int bp2 = 3;

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

int a = 0;
int b = 16;
int score = 0;
int lastScore = -1; // Pour détecter les changements de score
int level = 1;
DFRobot_RGBMatrix matrix(A, B, C, D, E, CLK, LAT, OE, false, WIDTH, HIGH);

// Définition des couleurs
uint16_t red = matrix.Color333(7, 0, 0);
uint16_t blue = matrix.Color333(0, 0, 7);
uint16_t green = matrix.Color333(0, 7, 0);
uint16_t yellow = matrix.Color333(7, 7, 0);
uint16_t white = matrix.Color333(7, 7, 7);
uint16_t black = matrix.Color333(0, 0, 0);
uint16_t scoreColor = matrix.Color333(0, 7, 0); // Vert vif
uint16_t bgColor = matrix.Color333(1, 1, 1);    // Gris pour le fond

volatile long int temps1 = 0;
uint16_t game[10][20];

int pile1[] = {33, 3, 35, 5};
int pile2[] = {36, 3, 38, 5};
uint16_t c1 = red;
uint16_t c2 = blue;
bool fa = true;

void setup() {
  Serial.begin(9600);
  pinMode(bp1, INPUT_PULLUP);
  pinMode(bp2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(bp1), stepleft, FALLING);
  attachInterrupt(digitalPinToInterrupt(bp2), stepright, FALLING);
  matrix.begin();
  
  // Initialisation du tableau de jeu
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 20; j++) {
      game[i][j] = black;
    }
  }
  game[0][19] = red;

  // Initialisation de l'affichage
  matrix.fillScreen(matrix.Color333(0, 0, 0));
  drawBorders();
  drawDivider();
  displayScore();
  displayLevel();
  
  matrix.setTextColor(matrix.Color333(4, 0, 4));
  drawVirus();
  delay(500);

  generateViruses(30);
}

void drawBorders() {
  matrix.drawLine(0, 0, 63, 0, white);
  matrix.drawLine(0, 0, 0, 63, white);
  matrix.drawLine(0, 63, 63, 63, white);
  matrix.drawLine(63, 0, 63, 63, white);
}

void drawDivider() {
  int border = 31;
  for (int i = 0; i < 64; i++) {
    matrix.drawPixel(border, i, white); 
    matrix.drawPixel(border + 1, i, white); 
  }
}

void displayScore() {
  matrix.fillRect(1, 1, 30, 12, bgColor);
  matrix.setTextSize(1);
  matrix.setCursor(1, 1);
  matrix.setTextColor(white);
  matrix.println("Score:");
  matrix.setCursor(1, 10);
  matrix.setTextColor(scoreColor);
  matrix.println(score);
}

void displayLevel() {
  matrix.fillRect(1, 20, 30, 12, bgColor);
  matrix.setTextSize(1);
  matrix.setCursor(1, 20);
  matrix.setTextColor(white);
  matrix.println("Level:");
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
    matrix.setTextColor(scoreColor);
    matrix.println(score);
    delay(50);
    
    // Retour à l'affichage normal
    matrix.fillRect(1, 10, 30, 8, bgColor);
    matrix.setTextSize(1);
    matrix.setCursor(1, 10);
    matrix.setTextColor(scoreColor);
    matrix.println(score);
    
    lastScore = score;
    
    // Vérifier si le niveau doit augmenter
    if (score >= level * 1000) {
      level++;
      displayLevel();
    }
  }
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
      matrix.drawLine(x * 3 + 33, y * 3 + 3, x * 3 + 35, y * 3 + 5, game[x][y]);
      matrix.drawLine(x * 3 + 33, y * 3 + 5, x * 3 + 35, y * 3 + 3, game[x][y]);
      
      virusesGenerated++;
    }
  }
}

void drawVirus() {
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 20; j++) {
      if (game[i][j] != black) {
        matrix.drawLine(i * 3 + 33, j * 3 + 3, i * 3 + 35, j * 3 + 5, game[i][j]);
        matrix.drawLine(i * 3 + 33, j * 3 + 5, i * 3 + 35, j * 3 + 3, game[i][j]);
      }
    }
  }
}

int drawPile(int pile[], uint16_t c) {
  int x1 = pile[0];
  int y1 = pile[1];
  int x2 = pile[2];
  int y2 = pile[3];
  for (int y = y1; y < y2 + 1; y++) {
    matrix.drawLine(x1, y, x2, y, c);
  }
  return 1;
}

void stepleft() {
  long int temps = micros();
  drawPile(pile1, black);
  drawPile(pile2, black);
  int x1 = pile1[0];
  int x2 = pile2[0];
  int y1 = pile1[1];
  int y2 = pile2[1];
  
  if (x1 > 33 && x2 > 33 && game[(x1 - 33) / 3 - 1][(y1 - 3) / 3] == black && 
      game[(x2 - 33) / 3 - 1][(y2 - 3) / 3] == black && fa) {
    if (temps - temps1 > 100000) {
      pile1[0] = x1 - 3;
      pile1[2] = x1 - 1;
      pile2[0] = x2 - 3;
      pile2[2] = x2 - 1;
    }
  }
  
  fa = hit(pile1, c1, fa);
  fa = hit(pile2, c2, fa);
  drawPile(pile1, c1);
  drawPile(pile2, c2);
  temps1 = temps;
}

void stepright() {
  long int temps = micros();
  drawPile(pile1, black);
  drawPile(pile2, black);
  int x1 = pile1[2];
  int x2 = pile2[2];
  int y1 = pile1[1];
  int y2 = pile2[1];
  
  if (x1 < 62 && x2 < 62 && game[(x1 - 33) / 3 + 1][(y1 - 3) / 3] == black && 
      game[(x2 - 33) / 3 + 1][(y2 - 3) / 3] == black && fa) {
    if (temps - temps1 > 100000) {
      pile1[0] = x1 + 1;
      pile1[2] = x1 + 3;
      pile2[0] = x2 + 1;
      pile2[2] = x2 + 3;
    }
  }
  
  fa = hit(pile1, c1, fa);
  fa = hit(pile2, c2, fa);
  drawPile(pile1, c1);
  drawPile(pile2, c2);
  temps1 = temps;
}

int fall(int py) {
  return py + 3;
}

bool hit(int pile[], uint16_t c, bool fa) {
  int x1 = pile[0];
  int y1 = pile[1];
  int x2 = pile[2];
  int y2 = pile[3];
  
  if ((int(y2 / 3) + 1) > 19 || game[int((x2 - 33) / 3)][int(y2 / 3) + 1] != black || fa == false) {
    game[int((x2 - 33) / 3)][int((y2) / 3)] = c;
    score += 10; // Augmentation du score quand un bloc se pose
    updateScoreDisplay();
    fa = false;
  }
  return fa;
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

void loop() {
  if (fa) {
    drawPile(pile1, black);
    drawPile(pile2, black);
    
    fa = hit(pile1, c1, fa);
    fa = hit(pile2, c2, fa);
    
    pile1[1] = fall(pile1[1]);
    pile1[3] = fall(pile1[3]);
    pile2[1] = fall(pile2[1]);
    pile2[3] = fall(pile2[3]);
    
    drawPile(pile1, c1);
    drawPile(pile2, c2);
  } else {
    int x = pile1[0] / 3 - 11;
    int y = pile1[1] / 3 - 1;
    heal(x, y, c1, "n", 1);
    
    x = pile2[0] / 3 - 11;
    y = pile2[1] / 3 - 1;
    heal(x, y, c2, "n", 1);
    
    // Réinitialiser les piles pour la chute suivante
    pile1[0] = 33; pile1[1] = 3; pile1[2] = 35; pile1[3] = 5;
    pile2[0] = 36; pile2[1] = 3; pile2[2] = 38; pile2[3] = 5;
    
    // Changer les couleurs pour le prochain bloc
    c1 = randomColor();
    c2 = randomColor();
    
    drawPile(pile1, c1);
    drawPile(pile2, c2);
    fa = true;
  }
  
  updateScoreDisplay();
  delay(500 - (level * 50)); // Le jeu s'accélère avec le niveau
}

// Les fonctions heal(), cleanBoard() et comboEffect() restent identiques à votre version originale
// Elles ont été conservées mais non modifiées pour garder la logique du jeu