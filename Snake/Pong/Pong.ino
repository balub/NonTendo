#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED config
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Pins
const int buttonUp = 3;
const int buttonDown = 2;
const int buzzerPin = 21;

// Game constants
const int paddleHeight = 8;
const int paddleWidth = 2;
const int playerX = 2;
const int enemyX = SCREEN_WIDTH - 4;

// Game state
int playerY = SCREEN_HEIGHT / 2 - paddleHeight / 2;
int enemyY = SCREEN_HEIGHT / 2 - paddleHeight / 2;

int ballX = SCREEN_WIDTH / 2;
int ballY = SCREEN_HEIGHT / 2;
int ballSpeedX = -1;
int ballSpeedY = 1;

int playerScore = 0;
int enemyScore = 0;
bool gameOver = false;

void setup() {
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 failed");
    while (true);
  }

  display.setRotation(2);
  display.clearDisplay();
  display.display();

  pinMode(buttonUp, INPUT);
  pinMode(buttonDown, INPUT);
  pinMode(buzzerPin, OUTPUT);
}

void beep() {
  tone(buzzerPin, 1000, 50);
}

void resetBall() {
  ballX = SCREEN_WIDTH / 2;
  ballY = SCREEN_HEIGHT / 2;
  ballSpeedX = ballSpeedX > 0 ? -1 : 1; // Alternate serve
  ballSpeedY = random(0, 2) == 0 ? 1 : -1;
}

void drawGame() {
  display.clearDisplay();

  // Draw player paddle
  display.fillRect(playerX, playerY, paddleWidth, paddleHeight, SSD1306_WHITE);

  // Draw enemy paddle
  display.fillRect(enemyX, enemyY, paddleWidth, paddleHeight, SSD1306_WHITE);

  // Draw ball
  display.fillRect(ballX, ballY, 2, 2, SSD1306_WHITE);

  // Draw scores
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(40, 0);
  display.print(playerScore);
  display.print(" - ");
  display.print(enemyScore);

  display.display();
}

void moveEnemy() {
  // Simple AI to follow the ball
  if (ballY < enemyY + paddleHeight / 2 && enemyY > 0)
    enemyY--;
  else if (ballY > enemyY + paddleHeight / 2 && enemyY + paddleHeight < SCREEN_HEIGHT)
    enemyY++;
}

void showGameOver() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(25, 10);
  display.print("GAME OVER");
  display.display();

  // Wait for button press to restart
  while (true) {
    if (digitalRead(buttonUp) == HIGH || digitalRead(buttonDown) == HIGH) {
      playerScore = 0;
      enemyScore = 0;
      playerY = SCREEN_HEIGHT / 2 - paddleHeight / 2;
      enemyY = SCREEN_HEIGHT / 2 - paddleHeight / 2;
      resetBall();
      gameOver = false;
      delay(300);
      return;
    }
  }
}

void loop() {
  if (gameOver) {
    showGameOver();
  }

  // Player control
  if (digitalRead(buttonUp) == HIGH) {
    playerY = max(0, playerY - 1);
  } else if (digitalRead(buttonDown) == HIGH) {
    playerY = min(SCREEN_HEIGHT - paddleHeight, playerY + 1);
  }

  moveEnemy();

  // Move ball
  ballX += ballSpeedX;
  ballY += ballSpeedY;

  // Bounce off top/bottom
  if (ballY <= 0 || ballY >= SCREEN_HEIGHT - 1) {
    ballSpeedY = -ballSpeedY;
    beep();
  }

  // Collision with player
  if (ballX <= playerX + paddleWidth &&
      ballY >= playerY &&
      ballY <= playerY + paddleHeight) {
    ballSpeedX = abs(ballSpeedX);
    beep();
  }

  // Collision with enemy
  if (ballX >= enemyX - 1 &&
      ballY >= enemyY &&
      ballY <= enemyY + paddleHeight) {
    ballSpeedX = -abs(ballSpeedX);
    beep();
  }

  // Missed ball
  if (ballX <= 0) {
    enemyScore++;
    if (enemyScore >= 3) gameOver = true;
    resetBall();
    delay(500);
  } else if (ballX >= SCREEN_WIDTH) {
    playerScore++;
    if (playerScore >= 3) gameOver = true;
    resetBall();
    delay(500);
  }

  drawGame();
  delay(10);
}