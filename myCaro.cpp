#include <iostream>
#include <windows.h>

using namespace std;

/* ---------------------------------- CONSTANTS & VARIABLES ---------------------------------- */

const int width = 43, height = 21;
const int X0 = 12, Y0 = 4;

const int MAX = 64;
const int MICRO_CYCLE_LENGTH = 100;

const int BLUE = 1;
const int RED = 4;
const int WHITE = 7;
const int GREEN = 2;
const int YELLOW = 6;

const int topLeft = 201, topRight = 187;
const int bottomLeft = 200, bottomRight = 188;
const int horizontal = 205, vertical = 186;

const int LEFT = 75;
const int RIGHT = 77;
const int UP = 72;
const int DOWN = 80;
const int ENTER = 13;
const int SPACE = 32;

HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

int caro[MAX][MAX];
int turn = 1;
int countTurn = 0;
int moveX = 21, moveY = 10;
// moveX = 2n+1, moveY = n, n is in range [0; 20]

/* -------------------------------- CONSOLE & SYSTEM SETTING -------------------------------- */

// Set window size
void setWindowSize(SHORT width, SHORT height) {
    SMALL_RECT WindowSize;
    WindowSize.Top = 0;
    WindowSize.Left = 0;
    WindowSize.Right = width;
    WindowSize.Bottom = height;
    SetConsoleWindowInfo(handle, 1, &WindowSize);
}

// Delay one second
void oneMicroSecond() {
    for (int i = 0; i < MICRO_CYCLE_LENGTH; i++) {}
}

void oneMilliSecond() {
    for (int i = 0; i < 1000; i++) {
        oneMicroSecond();
    }
}

void delay(int ms) {
    while (ms--) {
        oneMilliSecond();
    }
}

// Set cursor position
void setCursor(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(handle, coord);
}

void moveTo(int x, int y) {
    setCursor(X0 + x, Y0 + y);
}

// Hide/show cursor
void setCursorVisible(bool show) {
    CONSOLE_CURSOR_INFO info;
    GetConsoleCursorInfo(handle, &info);
    info.bVisible = show;
    SetConsoleCursorInfo(handle, &info);
}

// Set color
void setColor(int color) {
    SetConsoleTextAttribute(handle, color);
}

// Clear text
void clearText(int rowIndex) {
    moveTo(0, rowIndex);
    for (int i = 0; i < MAX; i++) {
        cout << ' ';
    }
}

// Print text
void printText(int x, int y, const char* text, int color = WHITE) {
    setColor(color);
    moveTo(x, y);
    cout << text;
}

// Count the number of play turn
void showTurns() {
    printText(46, 0, "\tPlay : ", GREEN);
    setColor(YELLOW);
    cout << countTurn;
    printText(46, 2, "\tTurn : ", GREEN);
    if (countTurn % 2 == 0) {
        setColor(BLUE);
        cout << 'x';
    }
    else {
        setColor(RED);
        cout << 'o';
    }
}

// Show play guide
void showPlayGuide() {
    int guideX = 52, guideY = 10;
    printText(guideX, guideY, "\t<< PLAY GUIDE >>");

    printText(guideX, guideY + 2, "LEFT KEY\t:\tMove Left");
    printText(guideX, guideY + 4, "RIGHT KEY\t:\tMove Right");
    printText(guideX, guideY + 6, "UP KEY\t\t:\tMove Up");
    printText(guideX, guideY + 8, "DOWN KEY\t:\tMove Down");
    printText(guideX, guideY + 10, "SPACE/ENTER\t:\tSet Caro Location");
}

// Print double-sided rectangular frame
void printRow(int rowIndex, int left, int mid, int right) {
    moveTo(-1, rowIndex);
    cout << (char)left;
    for (int x = 0; x < width; x++) {
        cout << (char)mid;
    }
    cout << (char)right << endl;
}

// Print play area
void printPlayArea() {
    system("cls");
    printText(0, -2, "\tGAME CARO");
    printRow(-1, topLeft, horizontal, topRight);
    for (int y = 0; y < height; y++) {
        printRow(y, vertical, ' ', vertical);
    }
    printRow(height, bottomLeft, horizontal, bottomRight);
    showPlayGuide();
}

/* ------------------------------------- METHOD SETTING ------------------------------------- */

// Create play mouse's array
void playArray() {
    for (int x = 1; x < MAX; x += 2) {
        for (int y = 0; y < MAX; y++) {
            caro[x][y] = 0;
        }
    }
}

// Set play mouse's position
void setMouse(int dx, int dy, char before, char after) {
    moveX += dx; moveY += dy;
    moveTo(moveX - 1, moveY);
    setColor(WHITE);
    cout << before;
    moveTo(moveX + 1, moveY);
    setColor(WHITE);
    cout << after;
}

// Set begin play mouse's position
void beginMousePosition() {
    setMouse(0, 0, '[', ']');
    // Test top left corner         setMouse(0, 0, '[', ']');
    // Test top right corner        setMouse(40, 0, '[', ']');
    // Test bottom left corner      setMouse(0, 20, '[', ']');
    // Test bottom right corner     setMouse(40, 20, '[', ']');
}

// Clear before play mouse's position
void clearMouse() {
    setMouse(0, 0, ' ', ' ');
}

// Limit play mouse's moving position
void limitMouse() {
    if (moveX == 43) {
        clearMouse();
        setMouse(-42, 0, '[', ']');
    }
    if (moveX == -1) {
        clearMouse();
        setMouse(42, 0, '[', ']');
    }
    if (moveY == 21) {
        setMouse(0, 0, horizontal, horizontal);
        setMouse(0, -21, '[', ']');
    }
    if (moveY == -1) {
        setMouse(0, 0, horizontal, horizontal);
        setMouse(0, 21, '[', ']');
    }
}

// Move play mouse
void moveMouse() {
    if (GetAsyncKeyState(VK_LEFT)) {
        clearMouse();
        setMouse(-2, 0, '[', ']');
    }
    if (GetAsyncKeyState(VK_RIGHT)) {
        clearMouse();
        setMouse(2, 0, '[', ']');
    }
    if (GetAsyncKeyState(VK_UP)) {
        clearMouse();
        setMouse(0, -1, '[', ']');
    }
    if (GetAsyncKeyState(VK_DOWN)) {
        clearMouse();
        setMouse(0, 1, '[', ']');
    }
    limitMouse();
    delay(1000);
}

// Setup caro checkers
void setCaro() {
    if (caro[moveX][moveY] == 0) {
        if (turn == 1) {
            moveTo(moveX, moveY);
            setColor(BLUE);
            cout << 'x';
            countTurn++;
            caro[moveX][moveY] = 1;
            turn = 1 - turn;
        }
        else {
            moveTo(moveX, moveY);
            setColor(RED);
            cout << 'o';
            countTurn++;
            caro[moveX][moveY] = 2;
            turn = 1 - turn;
        }
    }
}

// Set caro checkers' position
void showCaro() {
    if (GetAsyncKeyState(VK_RETURN) || GetAsyncKeyState(VK_SPACE)) {
        setCaro();
    }
}

// How to win?
bool gameRule() {
    for (int i = 0; i < 17; i++) {
        if (caro[moveX][i] != 0 && caro[moveX][i] == caro[moveX][i + 1] && caro[moveX][i] == caro[moveX][i + 2] && caro[moveX][i] == caro[moveX][i + 3] && caro[moveX][i] == caro[moveX][i + 4]) {
            return true;
        }
        else if (caro[2 * i + 1][moveY] != 0 && caro[2 * i + 1][moveY] == caro[2 * i + 3][moveY] && caro[2 * i + 1][moveY] == caro[2 * i + 5][moveY] && caro[2 * i + 1][moveY] == caro[2 * i + 7][moveY] && caro[2 * i + 1][moveY] == caro[2 * i + 9][moveY]) {
            return true;
        }
        else {
            int dx = 0, dy = ((moveX - 1) / 2) - moveY;
            if (2 * (dy + i) + 1 >= 1 && 2 * (dy + i) + 9 <= 41)
            if (caro[2 * (dy + i) + 1][i + dx] != 0 && caro[2 * (dy + i) + 1][i + dx] == caro[2 * (dy + i) + 3][i + 1 + dx] && caro[2 * (dy + i) + 1][i + dx] == caro[2 * (dy + i) + 5][i + 2 + dx] && caro[2 * (dy + i) + 1][i + dx] == caro[2 * (dy + i) + 7][i + 3 + dx] && caro[2 * (dy + i) + 1][i + dx] == caro[2 * (dy + i) + 9][i + 4 + dx]) {
                return true;
            }
        }
        int dx = 0, dy = ((moveX - 1) / 2) + moveY;
        if (caro[2 * (dy - i) - 1][i + dx] != 0 && caro[2 * (dy - i) - 1][i + dx] == caro[2 * (dy - i) - 3][i + 1 + dx] && caro[2 * (dy - i) - 1][i + dx] == caro[2 * (dy - i) - 5][i + 2 + dx] && caro[2 * (dy - i) - 1][i + dx] == caro[2 * (dy - i) - 7][i + 3 + dx] && caro[2 * (dy - i) - 1][i + dx] == caro[2 * (dy - i) - 9][i + 4 + dx]) {
            return true;
        }
    }
    return false;
}

// Show winner's name
void showWinner() {
    if (gameRule()) {
        if (countTurn % 2 == 1) {
            printText(46, 4, "\tTHE WINNER IS ", YELLOW);
            setColor(BLUE);
            cout << "'X'";
        }
        if (countTurn % 2 == 0) {
            printText(46, 4, "\tTHE WINNER IS ", YELLOW);
            setColor(RED);
            cout << "'O'";
        }
    }
}

// Stop game
void endGame() {
    if (gameRule()) {
        printText(46, 6, "\tWAIT 10 SECONDS TO RESTART...");
        if (GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState(VK_UP) || GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState(VK_RETURN) || GetAsyncKeyState(VK_SPACE)) {
            delay(40000);
            moveX = 21, moveY = 10;
            printPlayArea();
            beginMousePosition();
            playArray();
            turn = 1;
            countTurn = 0;
        }
    }
}

// Play Caro
void playCaro() {
    moveMouse();
    showCaro();
    showTurns();
    showWinner();
    endGame();
}

/* ---------------------------------------- GAME MAIN ---------------------------------------- */

int main() {
    printPlayArea();
    beginMousePosition();
    setCursorVisible(false);
    while (true) {
        playCaro();
    }
    system("pause");
    return 0;
}