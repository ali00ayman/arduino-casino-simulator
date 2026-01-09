#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h>

#define W 128
#define H 64
Adafruit_SSD1306 display(W, H, &Wire, -1);

// SHARED PINS
#define BUTTON 7
#define BUZZER 12
#define RED_PIN   4
#define GREEN_PIN 5
#define BLUE_PIN  3

enum Mode { MODE_MENU, MODE_ROULETTE, MODE_SLOT };
Mode mode = MODE_MENU;

bool pressed(){ return digitalRead(BUTTON) == LOW; }

void rgb(bool r,bool g,bool b){
  digitalWrite(RED_PIN,r);
  digitalWrite(GREEN_PIN,g);
  digitalWrite(BLUE_PIN,b);
}

/* =========================
   RETURN-TO-MENU PROMPT
   ========================= */
bool promptReturnToMenu(){
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  display.setCursor(8, 0);
  display.print("PLAYED 2 GAMES");

  display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

  display.setCursor(6, 22);
  display.print("Back to MAIN MENU?");

  display.setCursor(6, 38);
  display.print("Short Press: No");

  display.setCursor(6, 50);
  display.print("Long  Press: Yes");

  display.display();

  while(!pressed()) delay(5);

  unsigned long t0 = millis();
  while(pressed()){
    if(millis() - t0 >= 700){
      tone(BUZZER, 900, 80);
      while(pressed()) delay(5);
      return true;
    }
    delay(5);
  }

  tone(BUZZER, 1200, 80);
  return false;
}

/* =========================
   MENU  (✅ ONLY THIS CHANGED)
   ========================= */
void drawMenu(){
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  // ✅ no big border (prevents covering). only small decorations around title.
  display.setCursor(32, 0);
  display.print("CASINO MENU");
  display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
  display.fillCircle(8, 5, 2, SSD1306_WHITE);
  display.fillCircle(119, 5, 2, SSD1306_WHITE);

  display.setCursor(2, 22);
  display.print("Short press: ROULETTE");

  display.setCursor(2, 36);
  display.print("Long  press: SLOT");

  display.setCursor(2, 54);
  display.print("Pick a game...");

  display.display();
}

Mode menuPick(){
  drawMenu();
  while(!pressed()) delay(5);

  unsigned long t0 = millis();
  while(pressed()){
    if(millis() - t0 >= 700){
      tone(BUZZER, 900, 80);
      while(pressed()) delay(5);
      return MODE_SLOT;
    }
    delay(5);
  }

  tone(BUZZER, 1200, 80);
  return MODE_ROULETTE;
}

/* =========================
   ROULETTE GAME
   ========================= */

const int roulette_wheel[37] = {
  0, 32, 15, 19, 4, 21, 2, 25, 17, 34, 6, 27, 13, 36, 11, 30,
  8, 23, 10, 5, 24, 16, 33, 1, 20, 14, 31, 9, 22, 18, 29,
  7, 28, 12, 35, 3, 26
};

#define ROULETTE_WIN_MATCH_PERCENT 30

bool roulette_isRed(int n){
  switch(n){
    case 1: case 3: case 5: case 7: case 9:
    case 12: case 14: case 16: case 18:
    case 19: case 21: case 23: case 25: case 27:
    case 30: case 32: case 34: case 36:
      return true;
    default: return false;
  }
}
bool roulette_isBlack(int n){
  if(n==0) return false;
  return !roulette_isRed(n);
}

void roulette_spinTone(int step){
  int f = 2000 - step * 10;
  if(f < 550) f = 550;
  tone(BUZZER, f, 18);
}
void roulette_winSound(){
  tone(BUZZER,1200,120); delay(160);
  tone(BUZZER,1500,120); delay(160);
  tone(BUZZER,1800,180); delay(220);
  noTone(BUZZER);
}
void roulette_loseSound(){
  tone(BUZZER,440,180); delay(220);
  tone(BUZZER,392,180); delay(220);
  tone(BUZZER,330,240); delay(280);
  noTone(BUZZER);
}
void roulette_winFlashAll(){
  for(int k=0;k<6;k++){
    rgb(1,0,0); tone(BUZZER, 1600, 40); delay(80);
    rgb(0,1,0); tone(BUZZER, 1700, 40); delay(80);
    rgb(0,0,1); tone(BUZZER, 1800, 40); delay(80);
    rgb(1,1,1); tone(BUZZER, 1900, 40); delay(80);
    rgb(0,0,0); delay(80);
  }
  noTone(BUZZER);
}

void roulette_drawPalmTree(int x, int y){
  for(int i=0;i<6;i++) display.drawPixel(x, y+i, SSD1306_WHITE);
  display.drawLine(x, y, x-4, y-2, SSD1306_WHITE);
  display.drawLine(x, y, x-3, y,   SSD1306_WHITE);
  display.drawLine(x, y, x+4, y-2, SSD1306_WHITE);
  display.drawLine(x, y, x+3, y,   SSD1306_WHITE);
  display.drawLine(x, y, x,   y-4, SSD1306_WHITE);
}

void roulette_drawBetScreen(){
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  display.setCursor(26, 0);
  display.print("VEGAS ROULETTE");
  display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

  display.setCursor(10, 20);
  display.print("Short press: RED");

  display.setCursor(10, 34);
  display.print("Long press : BLACK");

  roulette_drawPalmTree(6, 52);
  roulette_drawPalmTree(121, 52);

  display.display();
}

bool roulette_chooseBetRed(){
  roulette_drawBetScreen();

  while(!pressed()) delay(5);
  unsigned long t0 = millis();

  while(pressed()){
    if(millis() - t0 >= 700){
      tone(BUZZER, 900, 80);
      return false;
    }
    delay(5);
  }

  tone(BUZZER, 1200, 80);
  return true;
}

void roulette_drawWheel(float ballAng, int resultNum, int resultIndex, const char* status, bool betRed){
  display.clearDisplay();

  const int cx = 38, cy = 32;
  const int rOuter = 30, rInner = 22, rBall = 26;

  display.drawCircle(cx, cy, rOuter, SSD1306_WHITE);
  display.drawCircle(cx, cy, rInner, SSD1306_WHITE);

  for(int i=0;i<37;i++){
    float ang = (2.0f * 3.1415926f * i) / 37.0f - 3.1415926f/2.0f;
    int x1 = cx + (int)round(cos(ang) * rInner);
    int y1 = cy + (int)round(sin(ang) * rInner);
    int x2 = cx + (int)round(cos(ang) * rOuter);
    int y2 = cy + (int)round(sin(ang) * rOuter);
    display.drawLine(x1,y1,x2,y2,SSD1306_WHITE);
  }

  if(resultIndex >= 0){
    float ang = (2.0f * 3.1415926f * resultIndex) / 37.0f - 3.1415926f/2.0f;
    int x1 = cx + (int)round(cos(ang) * (rInner-1));
    int y1 = cy + (int)round(sin(ang) * (rInner-1));
    int x2 = cx + (int)round(cos(ang) * (rOuter+1));
    int y2 = cy + (int)round(sin(ang) * (rOuter+1));
    display.drawLine(x1,y1,x2,y2,SSD1306_WHITE);
    display.drawLine(x1+1,y1,x2+1,y2,SSD1306_WHITE);
  }

  int bx = cx + (int)round(cos(ballAng) * rBall);
  int by = cy + (int)round(sin(ballAng) * rBall);
  display.fillCircle(bx, by, 2, SSD1306_WHITE);

  if(resultNum >= 0){
    display.setTextSize(2);
    if(resultNum < 10) display.setCursor(cx - 6, cy - 8);
    else               display.setCursor(cx - 12, cy - 8);
    display.print(resultNum);
  }

  display.setTextSize(1);
  display.setCursor(78, 0);
  display.print("ROULETTE");

  display.setCursor(78, 12);
  display.print("Bet: ");
  display.print(betRed ? "R" : "B");

  display.setCursor(78, 24);
  display.print("Color: ");
  if(resultNum < 0) display.print("-");
  else if(resultNum == 0) display.print("G");
  else if(roulette_isRed(resultNum)) display.print("R");
  else display.print("B");

  display.setCursor(78, 56);
  display.print(status);

  display.display();
}

int rouletteRounds = 0;

void roulette_modeLoop(){
  rouletteRounds = 0;

  while(mode == MODE_ROULETTE){
    bool betRed = roulette_chooseBetRed();

    bool wantMatch = (random(100) < ROULETTE_WIN_MATCH_PERCENT);
    int winIndex;

    while(true){
      winIndex = random(37);
      int n = roulette_wheel[winIndex];
      bool matches = (n != 0) && (betRed ? roulette_isRed(n) : roulette_isBlack(n));
      if((wantMatch && matches) || (!wantMatch && !matches)) break;
    }

    int winNum = roulette_wheel[winIndex];

    float ballAng = (float)random(628) / 100.0f;
    int dir = (random(2) == 0) ? 1 : -1;
    float speed = 0.45f * dir;
    int dly = 8;

    for(int i=0;i<140;i++){
      ballAng += speed;
      if(ballAng > 6.28318f) ballAng -= 6.28318f;
      if(ballAng < 0.0f)     ballAng += 6.28318f;

      if(i > 45){
        speed *= 0.986f;
        if(fabs(speed) < 0.03f) speed = 0.03f * dir;
        dly = min(dly + 1, 40);
      }

      roulette_drawWheel(ballAng, -1, -1, (i < 70) ? "Spinning..." : "Slowing...", betRed);
      roulette_spinTone(i);
      delay(dly);
    }
    noTone(BUZZER);

    float targetAng = (2.0f * 3.1415926f * winIndex) / 37.0f - 3.1415926f/2.0f;
    ballAng = targetAng;

    roulette_drawWheel(ballAng, winNum, winIndex, "RESULT!", betRed);
    delay(250);

    bool win = (winNum != 0) && (betRed ? roulette_isRed(winNum) : roulette_isBlack(winNum));

    if(win){
      roulette_drawWheel(ballAng, winNum, winIndex, "YOU WIN!", betRed);
      roulette_winFlashAll();
      roulette_winSound();
    } else {
      roulette_drawWheel(ballAng, winNum, winIndex, "YOU LOSE", betRed);
      rgb(1,0,0);
      roulette_loseSound();
      rgb(0,0,0);
    }

    rouletteRounds++;

    if(rouletteRounds >= 2){
      bool goMenu = promptReturnToMenu();
      if(goMenu){
        mode = MODE_MENU;
        break;
      } else {
        rouletteRounds = 0;
      }
    }

    delay(300);
  }
}

/* =========================
   SLOT MACHINE
   ========================= */

const char slot_syms[] = {'7','A','*','$','#','@'};
const int  SLOT_N = sizeof(slot_syms) / sizeof(slot_syms[0]);

#define SLOT_WIN_BIAS_PERCENT 7
#define SLOT_PITY_AFTER 4

int slot_loseStreak = 0;
int slotRounds = 0;

void slot_tickSound(){ tone(BUZZER, 2200, 8); }

void slot_reelStopDing(int which) {
  if (which == 1) { tone(BUZZER, 1200, 120); delay(140); }
  if (which == 2) { tone(BUZZER, 1350, 120); delay(140); }
  if (which == 3) { tone(BUZZER, 1600, 200); delay(230); }
  noTone(BUZZER);
}

void slot_loseSound() {
  tone(BUZZER, 440, 180); delay(220);
  tone(BUZZER, 392, 180); delay(220);
  tone(BUZZER, 330, 240); delay(280);
  noTone(BUZZER);
}

void slot_winCelebrate() {
  for (int k = 0; k < 3; k++) {
    rgb(1,0,0); tone(BUZZER, 1400, 40); delay(70);
    rgb(1,1,0); tone(BUZZER, 1500, 40); delay(70);
    rgb(0,1,0); tone(BUZZER, 1600, 40); delay(70);
    rgb(0,1,1); tone(BUZZER, 1700, 40); delay(70);
    rgb(0,0,1); tone(BUZZER, 1800, 40); delay(70);
    rgb(1,0,1); tone(BUZZER, 1900, 40); delay(70);
    rgb(1,1,1); tone(BUZZER, 2000, 40); delay(70);
    rgb(0,0,0); delay(70);
  }
  noTone(BUZZER);
}

void slot_drawUI(const char* status) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("SLOT MACHINE");
  display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

  int top = 14, bw = 36, bh = 34, gap = 8;
  int x1 = 4, x2 = x1 + bw + gap, x3 = x2 + bw + gap;
  display.drawRoundRect(x1, top, bw, bh, 4, SSD1306_WHITE);
  display.drawRoundRect(x2, top, bw, bh, 4, SSD1306_WHITE);
  display.drawRoundRect(x3, top, bw, bh, 4, SSD1306_WHITE);

  display.setCursor(0,56);
  display.print(status);
}

void slot_drawReels(char a, char b, char c, const char* status) {
  slot_drawUI(status);

  int top = 14, bw = 36, gap = 8;
  int x1 = 4, x2 = x1 + bw + gap, x3 = x2 + bw + gap;

  display.setTextSize(3);
  display.setCursor(x1 + 10, top + 6); display.print(a);
  display.setCursor(x2 + 10, top + 6); display.print(b);
  display.setCursor(x3 + 10, top + 6); display.print(c);

  display.display();
}

void slot_modeLoop(){
  slotRounds = 0;
  slot_drawReels('7','7','7',"Press to SPIN");

  while(mode == MODE_SLOT){
    while(!pressed()) delay(5);
    delay(30);

    int fa, fb, fc;
    bool forceWin = (slot_loseStreak >= SLOT_PITY_AFTER) || (random(100) < SLOT_WIN_BIAS_PERCENT);

    if (forceWin) {
      fa = random(SLOT_N);
      fb = fa;
      fc = fa;
    } else {
      fa = random(SLOT_N);
      fb = random(SLOT_N);
      fc = random(SLOT_N);
      if (fa == fb && fb == fc) fc = (fc + 1) % SLOT_N;
    }

    int a=0, b=0, c=0;
    int d = 28;
    bool lockA=false, lockB=false, lockC=false;

    for (int i = 0; i < 95; i++) {
      if (!lockA) a = (a + 1) % SLOT_N;
      if (!lockB && (i % 2 == 0)) b = (b + 1) % SLOT_N;
      if (!lockC && (i % 3 == 0)) c = (c + 1) % SLOT_N;

      if (i > 55) d += 3;

      if (i == 65 && !lockA) { a = fa; lockA = true; slot_reelStopDing(1); }
      if (i == 78 && !lockB) { b = fb; lockB = true; slot_reelStopDing(2); }
      if (i == 90 && !lockC) { c = fc; lockC = true; slot_reelStopDing(3); }

      slot_drawReels(slot_syms[a], slot_syms[b], slot_syms[c], (i < 60) ? "Spinning..." : "Stopping...");
      slot_tickSound();
      delay(d);

      if (lockA && lockB && lockC) break;
    }

    bool win = (a == b && b == c);

    if (win) {
      slot_loseStreak = 0;
      slot_drawReels(slot_syms[a], slot_syms[b], slot_syms[c], "JACKPOT!");
      slot_winCelebrate();
    } else {
      slot_loseStreak++;
      slot_drawReels(slot_syms[a], slot_syms[b], slot_syms[c], "LOSE");
      rgb(1,0,0);
      slot_loseSound();
      rgb(0,0,0);
    }

    slotRounds++;

    if(slotRounds >= 2){
      bool goMenu = promptReturnToMenu();
      if(goMenu){
        mode = MODE_MENU;
        break;
      } else {
        slotRounds = 0;
        slot_drawReels(slot_syms[a], slot_syms[b], slot_syms[c], "Press to SPIN");
      }
    }

    while(pressed()) delay(5);
    delay(150);
    slot_drawReels(slot_syms[a], slot_syms[b], slot_syms[c], "Press to SPIN");
  }
}

/* =========================
   SETUP / MAIN LOOP
   ========================= */
void setup(){
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  rgb(0,0,0);

  randomSeed((unsigned long)micros() ^ (unsigned long)analogRead(A0) ^ (unsigned long)analogRead(A1));

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)){
      while(1){ tone(BUZZER,200,200); delay(400); }
    }
  }
  display.setTextWrap(false);
}

void loop(){
  if(mode == MODE_MENU){
    mode = menuPick();
  }

  if(mode == MODE_ROULETTE){
    roulette_modeLoop();
  } else if(mode == MODE_SLOT){
    slot_modeLoop();
  }
}

