#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <string.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Controls
const int leftBtn = 3;
const int rightBtn = 2;
const int buzzerPin = 21;

#define TILE_SIZE 8
#define MAZE_WIDTH 16
#define MAZE_HEIGHT 4

// Tile types
#define WALL 0
#define DOT  1
#define EMPTY 2

// Pac-Man direction
enum Dir { UP, RIGHT, DOWN, LEFT };
Dir direction = RIGHT;

int pacX = 1;
int pacY = 1;

// Minimal maze (renamed from `map` to `maze`)
byte maze[MAZE_HEIGHT][MAZE_WIDTH] = {
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
  {0,1,0,0,1,0,0,0,0,0,1,0,0,1,1,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

void setup() {
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 failed");
    while (true);
  }

  display.setRotation(2);  // Vertical screen
  display.clearDisplay();
  display.display();

  pinMode(leftBtn, INPUT);
  pinMode(rightBtn, INPUT);
  pinMode(buzzerPin, OUTPUT);
}

void beep(int f = 1000, int d = 50) {
  tone(buzzerPin, f, d);
}

void drawMaze() {
  display.clearDisplay();
  for (int y = 0; y < MAZE_HEIGHT; y++) {
    for (int x = 0; x < MAZE_WIDTH; x++) {
      int px = x * TILE_SIZE;
      int py = y * TILE_SIZE;
      if (x == pacX && y == pacY) {
        display.fillCircle(px + 4, py + 4, 3, SSD1306_WHITE); // Pac-Man
      } else if (maze[y][x] == WALL) {
        display.fillRect(px, py, TILE_SIZE, TILE_SIZE, SSD1306_WHITE);
      } else if (maze[y][x] == DOT) {
        display.fillCircle(px + 4, py + 4, 1, SSD1306_WHITE);
      }
    }
  }
  display.display();
}

bool allDotsCollected() {
  for (int y = 0; y < MAZE_HEIGHT; y++)
    for (int x = 0; x < MAZE_WIDTH; x++)
      if (maze[y][x] == DOT) return false;
  return true;
}

void rotateLeft() {
  direction = (Dir)((direction + 3) % 4);
}

void rotateRight() {
  direction = (Dir)((direction + 1) % 4);
}

void movePacman() {
  int dx = 0, dy = 0;
  switch (direction) {
    case UP: dy = -1; break;
    case DOWN: dy = 1; break;
    case LEFT: dx = -1; break;
    case RIGHT: dx = 1; break;
  }

  int newX = pacX + dx;
  int newY = pacY + dy;

  if (newX >= 0 && newX < MAZE_WIDTH &&
      newY >= 0 && newY < MAZE_HEIGHT &&
      maze[newY][newX] != WALL) {
    pacX = newX;
    pacY = newY;
    if (maze[pacY][pacX] == DOT) {
      maze[pacY][pacX] = EMPTY;
      beep(1200, 30);
    }
  }
}

void showWinScreen() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(30, 10);
  display.print("YOU WIN!");
  display.display();

  beep(1500, 200);
  delay(1000);

  // Wait for button to restart
  while (true) {
    if (digitalRead(leftBtn) == HIGH || digitalRead(rightBtn) == HIGH) {
      // Reset maze
      byte newMaze[MAZE_HEIGHT][MAZE_WIDTH] = {
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
        {0,1,0,0,1,0,0,0,0,0,1,0,0,1,1,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
      };
      memcpy(maze, newMaze, sizeof(maze));
      pacX = 1;
      pacY = 1;
      direction = RIGHT;
      return;
    }
  }
}

void loop() {
  static unsigned long lastMove = 0;
  static int lastLeft = LOW, lastRight = LOW;

  int left = digitalRead(leftBtn);
  int right = digitalRead(rightBtn);

  if (left == HIGH && lastLeft == LOW)
    rotateLeft();
  if (right == HIGH && lastRight == LOW)
    rotateRight();

  lastLeft = left;
  lastRight = right;

  if (millis() - lastMove > 200) {
    lastMove = millis();
    movePacman();
    drawMaze();

    if (allDotsCollected()) {
      showWinScreen();
    }
  }
}