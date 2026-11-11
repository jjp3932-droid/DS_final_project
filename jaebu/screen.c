/* screen.c - 화면 공통 함수 구현
 * 담당: 정재부
 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "screen.h"

HANDLE hOut = NULL;

void initScreen(void) {
    char cmd[64];
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    sprintf(cmd, "mode con cols=%d lines=%d", SCREEN_COLS, SCREEN_ROWS);
    system(cmd);                          /* 창 크기 고정 (4R 노선도가 잘리지 않게) */
    SetConsoleTitleA("역 이름 외우기");
}

void gotoxy(int x, int y) {
    COORD pos;
    pos.X = (SHORT)x;
    pos.Y = (SHORT)y;
    SetConsoleCursorPosition(hOut, pos);
}

void setColor(int attr) {
    SetConsoleTextAttribute(hOut, (WORD)attr);
}

void showCursor(int visible) {
    CONSOLE_CURSOR_INFO info;
    GetConsoleCursorInfo(hOut, &info);
    info.bVisible = visible ? TRUE : FALSE;
    SetConsoleCursorInfo(hOut, &info);
}

void clearScreen(void) {
    setColor(COLOR_DEFAULT);
    system("cls");
}

void clearLine(int y) {
    gotoxy(0, y);
    for (int i = 0; i < SCREEN_COLS - 1; i++) putchar(' ');
    gotoxy(0, y);
}

void waitKey(void) {
    while (_kbhit()) _getch();            /* 이미 눌려 있던 키는 버림 */
    _getch();
}

int showMenu(void) {
    clearScreen();
    showCursor(0);

    setColor(COLOR_TITLE);
    gotoxy(40, 6);  printf("==============================");
    gotoxy(40, 7);  printf("        역 이름 외우기        ");
    gotoxy(40, 8);  printf("==============================");

    setColor(COLOR_DEFAULT);
    gotoxy(48, 12); printf("1. 게임 시작");
    gotoxy(48, 14); printf("2. 랭킹 보기");
    gotoxy(48, 16); printf("3. 종료");

    setColor(COLOR_HINT);
    gotoxy(42, 20); printf("번호 키를 누르세요 (엔터 필요 없음)");
    setColor(COLOR_DEFAULT);

    while (1) {
        int c = _getch();
        if (c >= '1' && c <= '3') return c - '0';   /* 다른 키는 무시 */
    }
}
