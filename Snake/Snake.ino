#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const int leftBtn = 3;
const int rightBtn = 2;
const int buzzerPin = 21;

#define TILE_SIZE 8
#define GRID_WIDTH 16
#define GRID_HEIGHT 4
#define MAX_SNAKE_LENGTH (GRID_WIDTH * GRID_HEIGHT)

#define EMPTY 0
#define FOOD 1
#define SNAKE 2

enum Dir { UP, RIGHT, DOWN, LEFT };
Dir direction = RIGHT;

int snakeX[MAX_SNAKE_LENGTH];
int snakeY[MAX_SNAKE_LENGTH];
int snakeLength = 3;

int foodX = 5;
int foodY = 1;

bool gameOver = false;

void setup() {
  Serial.begin(115200);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    while (true);
  }

  display.setRotation(2); // vertical screen
  display.clearDisplay();
  display.display();

  pinMode(leftBtn, INPUT);
  pinMode(rightBtn, INPUT);
  pinMode(buzzerPin, OUTPUT);

  resetGame();
}

void beep(int freq = 1000, int dur = 50) {
  tone(buzzerPin, freq, dur);
}

void resetGame() {
  direction = RIGHT;
  snakeLength = 3;
  for (int i = 0; i < snakeLength; i++) {
    snakeX[i] = 3 - i;
    snakeY[i] = 1;
  }
  placeFood();
  gameOver = false;
}

void placeFood() {
  while (true) {
    foodX = random(0, GRID_WIDTH);
    foodY = random(0, GRID_HEIGHT);

    bool onSnake = false;
    for (int i = 0; i < snakeLength; i++) {
      if (snakeX[i] == foodX && snakeY[i] == foodY) {
        onSnake = true;
        break;
      }
    }
    if (!onSnake) break;
  }
}

void drawGame() {
  display.clearDisplay();

  // Draw food
  display.fillCircle(foodX * TILE_SIZE + 4, foodY * TILE_SIZE + 4, 2, SSD1306_WHITE);

  // Draw thinner snake (8x4)
  for (int i = 0; i < snakeLength; i++) {
    display.fillRect(snakeX[i] * TILE_SIZE, snakeY[i] * TILE_SIZE + 2, TILE_SIZE, 4, SSD1306_WHITE);
  }

  if (gameOver) {
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(30, 10);
    display.print("GAME OVER");
  }

  display.display();
}

void rotateLeft() {
  direction = (Dir)((direction + 3) % 4);
}

void rotateRight() {
  direction = (Dir)((direction + 1) % 4);
}

bool isOpposite(Dir newDir) {
  return (direction == UP && newDir == DOWN) ||
         (direction == DOWN && newDir == UP) ||
         (direction == LEFT && newDir == RIGHT) ||
         (direction == RIGHT && newDir == LEFT);
}

void moveSnake() {
  for (int i = snakeLength - 1; i > 0; i--) {
    snakeX[i] = snakeX[i - 1];
    snakeY[i] = snakeY[i - 1];
  }

  switch (direction) {
    case UP:    snakeY[0]--; break;
    case DOWN:  snakeY[0]++; break;
    case LEFT:  snakeX[0]--; break;
    case RIGHT: snakeX[0]++; break;
  }

  // Wall collision
  if (snakeX[0] < 0 || snakeX[0] >= GRID_WIDTH ||
      snakeY[0] < 0 || snakeY[0] >= GRID_HEIGHT) {
    gameOver = true;
    beep(300, 200);
    return;
  }

  // Self collision
  for (int i = 1; i < snakeLength; i++) {
    if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
      gameOver = true;
      beep(300, 200);
      return;
    }
  }

  // Food eaten
  if (snakeX[0] == foodX && snakeY[0] == foodY) {
    if (snakeLength < MAX_SNAKE_LENGTH) {
      snakeX[snakeLength] = snakeX[snakeLength - 1];
      snakeY[snakeLength] = snakeY[snakeLength - 1];
      snakeLength++;
    }
    beep(1500, 50);
    placeFood();
  }
}

void showGameOver() {
  drawGame();
  while (true) {
    if (digitalRead(leftBtn) == HIGH || digitalRead(rightBtn) == HIGH) {
      delay(300);
      resetGame();
      return;
    }
  }
}

void loop() {
  static unsigned long lastMove = 0;
  static int lastLeft = LOW, lastRight = LOW;

  int left = digitalRead(leftBtn);
  int right = digitalRead(rightBtn);

  if (left == HIGH && lastLeft == LOW) {
    Dir newDir = (Dir)((direction + 3) % 4);
    if (!isOpposite(newDir)) rotateLeft();
  }

  if (right == HIGH && lastRight == LOW) {
    Dir newDir = (Dir)((direction + 1) % 4);
    if (!isOpposite(newDir)) rotateRight();
  }

  lastLeft = left;
  lastRight = right;

  if (gameOver) {
    showGameOver();
    return;
  }

  if (millis() - lastMove > 400) {  // slowed down from 250ms
    lastMove = millis();
    moveSnake();
    drawGame();
  }
}