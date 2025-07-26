#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED config
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Pins
const int buttonPins[] = { 3, 10, 2, 6 }; // Left, Right, Down, Select
const int buzzerPin = 21;
const int numButtons = sizeof(buttonPins) / sizeof(buttonPins[0]);

// Game state
char board[3][3];
int cursorX = 0;
int cursorY = 0;
bool isXTurn = true;
bool gameEnded = false;
char winner = ' '; // 'X', 'O', or 'D' for draw

void setup() {
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 failed");
    while (true);
  }
  display.setRotation(2);
  display.clearDisplay();
  display.display();

  for (int i = 0; i < numButtons; i++)
    pinMode(buttonPins[i], INPUT); // External pull-downs assumed

  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  resetBoard();
  drawBoard();
}

void resetBoard() {
  for (int y = 0; y < 3; y++)
    for (int x = 0; x < 3; x++)
      board[y][x] = ' ';
  cursorX = 0;
  cursorY = 0;
  isXTurn = true;
  gameEnded = false;
  winner = ' ';
}

void playTone() {
  tone(buzzerPin, 1000, 100);
}

void drawBoard() {
  display.clearDisplay();

  // Draw grid lines
  for (int i = 1; i < 3; i++) {
    display.drawLine(i * 42, 0, i * 42, 32, SSD1306_WHITE); // vertical
    display.drawLine(0, i * 10 + 1, 128, i * 10 + 1, SSD1306_WHITE); // horizontal
  }

  // Draw X/O
  for (int y = 0; y < 3; y++) {
    for (int x = 0; x < 3; x++) {
      int cx = x * 42 + 21;
      int cy = y * 10 + 5;
      if (board[y][x] == 'X') {
        display.drawLine(cx - 5, cy - 3, cx + 5, cy + 3, SSD1306_WHITE);
        display.drawLine(cx + 5, cy - 3, cx - 5, cy + 3, SSD1306_WHITE);
      } else if (board[y][x] == 'O') {
        display.drawCircle(cx, cy, 5, SSD1306_WHITE);
      }
    }
  }

  // Highlight cursor
  if (!gameEnded) {
    int x0 = cursorX * 42;
    int y0 = cursorY * 10 + 1;
    display.drawRect(x0, y0, 42, 10, SSD1306_WHITE);
  }

  display.display();
}

bool checkWin(char p) {
  for (int i = 0; i < 3; i++)
    if ((board[i][0] == p && board[i][1] == p && board[i][2] == p) ||
        (board[0][i] == p && board[1][i] == p && board[2][i] == p))
      return true;
  if ((board[0][0] == p && board[1][1] == p && board[2][2] == p) ||
      (board[0][2] == p && board[1][1] == p && board[2][0] == p))
    return true;
  return false;
}

bool checkDraw() {
  for (int y = 0; y < 3; y++)
    for (int x = 0; x < 3; x++)
      if (board[y][x] == ' ')
        return false;
  return true;
}

void showResult(char result) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 10);

  if (result == 'X' || result == 'O') {
    display.print(result);
    display.print(" WINS!");
  } else {
    display.print("DRAW!");
  }
  display.display();

  // Wait for button press to continue
  while (true) {
    for (int i = 0; i < numButtons; i++) {
      if (digitalRead(buttonPins[i]) == HIGH) {
        delay(300);  // debounce
        resetBoard();
        drawBoard();
        return;
      }
    }
  }
}

void loop() {
  static int lastButton = -1;

  for (int i = 0; i < numButtons; i++) {
    if (digitalRead(buttonPins[i]) == HIGH && lastButton != i) {
      lastButton = i;
      playTone();

      if (!gameEnded) {
        if (i == 0) cursorX = max(0, cursorX - 1);       // Left
        else if (i == 1) cursorX = min(2, cursorX + 1);  // Right
        else if (i == 2) cursorY = (cursorY + 1) % 3;    // Down
        else if (i == 3) {                               // Select
          if (board[cursorY][cursorX] == ' ') {
            board[cursorY][cursorX] = isXTurn ? 'X' : 'O';

            if (checkWin(board[cursorY][cursorX])) {
              winner = board[cursorY][cursorX];
              gameEnded = true;
              drawBoard();
              showResult(winner);
              return;
            } else if (checkDraw()) {
              winner = 'D';
              gameEnded = true;
              drawBoard();
              showResult(winner);
              return;
            }

            isXTurn = !isXTurn;
          }
        }
      }

      drawBoard();
      delay(200);
    }
  }

  // Reset lastButton
  bool anyPressed = false;
  for (int i = 0; i < numButtons; i++) {
    if (digitalRead(buttonPins[i]) == HIGH) {
      anyPressed = true;
      break;
    }
  }
  if (!anyPressed) {
    lastButton = -1;
  }
}