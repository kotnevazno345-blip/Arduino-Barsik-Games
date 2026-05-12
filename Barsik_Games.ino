#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- НАСТРОЙКИ ПИНОВ ---
#define BTN_PIN 2
#define BUZZER_PIN 8

enum State { MENU, PONG, SNAKE };
State gameState = MENU;
int menuSelection = 0;

// Переменные игр
int ballX = 64, ballY = 32, ballDX = 3, ballDY = 3;
int paddleY = 24;
int snakeX[30], snakeY[30], snakeLen = 3, snakeDir = 0;
int foodX, foodY;

void setup() {
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for(;;);
  }

  display.clearDisplay();
  randomSeed(analogRead(0));
}

void beep(int freq, int dur) {
  tone(BUZZER_PIN, freq, dur);
}

void loop() {
  if (gameState == MENU) {
    drawMenu();
  } else if (gameState == PONG) {
    playPong();
  } else if (gameState == SNAKE) {
    playSnake();
  }
}

void drawMenu() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10, 0);
  display.println(F("BARSIK"));

  display.setTextSize(1);
  display.setCursor(10, 30);
  if (menuSelection == 0) display.print(F("> "));
  display.println(F("Barsik Pong"));

  display.setCursor(10, 45);
  if (menuSelection == 1) display.print(F("> "));
  display.println(F("Snake Game"));
  display.display();

  if (digitalRead(BTN_PIN) == LOW) {
    unsigned long pressTime = millis();
    while(digitalRead(BTN_PIN) == LOW);

    if (millis() - pressTime > 600) {
      beep(1000, 100);
      gameState = (menuSelection == 0) ? PONG : SNAKE;
      resetGames();
    } else {
      menuSelection = (menuSelection + 1) % 2;
      beep(1500, 20);
    }
  }
}

void playPong() {
  display.clearDisplay();

  if (digitalRead(BTN_PIN) == LOW) {
    paddleY += 6;
    if (paddleY > 48) paddleY = 0;
    delay(30);
  }

  ballX += ballDX;
  ballY += ballDY;

  if (ballY <= 0 || ballY >= 60) {
    ballDY *= -1;
    beep(600, 10);
  }

  if (ballX >= 124) {
    ballDX *= -1;
    beep(600, 10);
  }

  if (ballX <= 10) {
    if (ballY >= paddleY && ballY <= paddleY + 16) {
      ballDX *= -1;
      ballDX++;
      beep(1200, 20);
    } else {
      gameOver();
    }
  }

  display.fillRect(4, paddleY, 3, 16, WHITE);
  display.fillRect(ballX, ballY, 3, 3, WHITE);
  display.display();
}

void playSnake() {
  display.clearDisplay();

  if (digitalRead(BTN_PIN) == LOW) {
    snakeDir = (snakeDir + 1) % 4;
    beep(1800, 10);
    delay(150);
  }

  for (int i = snakeLen - 1; i > 0; i--) {
    snakeX[i] = snakeX[i - 1];
    snakeY[i] = snakeY[i - 1];
  }

  if (snakeDir == 0) snakeX[0] += 4;
  else if (snakeDir == 1) snakeY[0] += 4;
  else if (snakeDir == 2) snakeX[0] -= 4;
  else if (snakeDir == 3) snakeY[0] -= 4;

  if (snakeX[0] < 0 || snakeX[0] >= 128 || snakeY[0] < 0 || snakeY[0] >= 64) {
    gameOver();
  }

  if (abs(snakeX[0] - foodX) < 4 && abs(snakeY[0] - foodY) < 4) {
    if (snakeLen < 30) snakeLen++;

    foodX = random(2, 30) * 4;
    foodY = random(2, 14) * 4;

    beep(2500, 40);
  }

  display.drawRect(foodX, foodY, 3, 3, WHITE);

  for (int i = 0; i < snakeLen; i++) {
    display.fillRect(snakeX[i], snakeY[i], 4, 4, WHITE);
  }

  display.display();
  delay(100 - snakeLen);
}

void resetGames() {
  ballX = 64;
  ballY = 32;
  ballDX = 3;
  ballDY = 3;

  snakeLen = 3;
  snakeX[0] = 64;
  snakeY[0] = 32;

  foodX = 40;
  foodY = 40;

  snakeDir = 0;
}

void gameOver() {
  beep(150, 600);

  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10, 25);
  display.print(F("GG WP!"));
  display.display();

  delay(1500);

  gameState = MENU;
}
