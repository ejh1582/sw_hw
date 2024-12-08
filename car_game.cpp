#include <iostream>
#include <ncurses.h>
#include <cstdlib>
#include <ctime>
#include <locale.h>
#include <unistd.h>  // usleep 사용을 위한 헤더

#define MAP_WIDTH 20
#define MAP_HEIGHT 10
#define OBSTACLE_COUNT 30

char map[MAP_HEIGHT][MAP_WIDTH];
int carX = 0, carY = 0;
int finishX = MAP_WIDTH - 1, finishY = MAP_HEIGHT - 1;
int moveCount = 0;
time_t startTime, endTime;

void initMap() {
    for (int i = 0; i < MAP_HEIGHT; ++i) {
        for (int j = 0; j < MAP_WIDTH; ++j) {
            map[i][j] = ' ';
        }
    }
    map[carY][carX] = 'C';  // 자동차
    map[finishY][finishX] = 'F';  // 피니시 라인

    srand(time(0));
    for (int i = 0; i < OBSTACLE_COUNT; ++i) {
        int x = rand() % MAP_WIDTH;
        int y = rand() % MAP_HEIGHT;
        if ((x == carX && y == carY) || (x == finishX && y == finishY)) {
            --i;
        } else {
            map[y][x] = '#';  // 장애물
        }
    }
}

void drawMap() {
    clear();
    for (int i = 0; i < MAP_HEIGHT; ++i) {
        for (int j = 0; j < MAP_WIDTH; ++j) {
            mvaddch(i, j, map[i][j]);
        }
    }
    mvprintw(MAP_HEIGHT, 0, "Moves: %d", moveCount);
    refresh();
}

void endGame(const char* message) {
    endTime = time(0);
    double elapsed = difftime(endTime, startTime);
    int score = 1000 - moveCount * 10;  // 이동 횟수에 따른 점수 계산

    // 점수가 0 이하로 떨어지지 않도록 처리
    if (score < 0) score = 0;

    mvprintw(MAP_HEIGHT + 1, 0, "%s", message);
    mvprintw(MAP_HEIGHT + 2, 0, "Time: %.2f seconds", elapsed);
    mvprintw(MAP_HEIGHT + 3, 0, "Score: %d", score);
    refresh();
    getch();
    endwin();
    exit(0);
}

void handleInput(int ch) {
    int newX = carX, newY = carY;
    switch (ch) {
        case 'w': newY--; break;
        case 's': newY++; break;
        case 'a': newX--; break;
        case 'd': newX++; break;
        case 'q': // 'q' 키로 게임 종료
            endGame("Game Over - quit game.");
            return;
    }

    // 맵 밖으로 나갈 수 없도록 처리
    if (newX < 0 || newX >= MAP_WIDTH || newY < 0 || newY >= MAP_HEIGHT) return;
    if (map[newY][newX] == '#') {
        endGame("Game Over - You hit an obstacle!");
    } else if (map[newY][newX] == 'F') {
        endGame("You Win!");
    } else {
        map[carY][carX] = ' ';
        carX = newX;
        carY = newY;
        map[carY][carX] = 'C';
        moveCount++;
    }
}

void showInstructions() {
    mvprintw(0, 0, "Welcome to the Car Game!");
    mvprintw(1, 0, "Use W, A, S, D to move the car (@).");
    mvprintw(2, 0, "Avoid obstacles (#) and reach the finish line (X).");
    mvprintw(3, 0, "Press 'q' to quit the game.");
    mvprintw(4, 0, "Press any key to start...");
    refresh();
    getch();
}

int main(void) {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);

    showInstructions();
    initMap();
    startTime = time(0);

    while (true) {
        drawMap();
        int ch = getch();
        handleInput(ch);
        usleep(150000);  // 애니메이션 효과를 위해 0.15초 대기 (더 긴 대기시간)
    }

    endwin();
    return 0;
}
