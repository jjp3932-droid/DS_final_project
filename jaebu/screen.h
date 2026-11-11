/* screen.h - 화면 공통 함수 (팀 전체가 같이 씀)
 * 담당: 정재부
 * 커서 이동, 글자색, 화면 지우기, 메인 메뉴
 */
#ifndef SCREEN_H
#define SCREEN_H

#include <windows.h>

/* 팀 공통 색 상수 (앞자리 = 배경색, 뒷자리 = 글자색) */
#define COLOR_DEFAULT  0x07   /* 회색 글자 (기본) */
#define COLOR_TITLE    0x0E   /* 노란 글자: 제목 */
#define COLOR_CORRECT  0x0A   /* 초록 글자: 정답 */
#define COLOR_WRONG    0x0C   /* 빨간 글자: 오답 */
#define COLOR_HINT     0x08   /* 어두운 회색: 안내문 */
#define COLOR_BLANK    0x0B   /* 하늘색 글자: 빈칸 */

/* 콘솔 창 크기 (팀 공통) */
#define SCREEN_COLS 120
#define SCREEN_ROWS 40

extern HANDLE hOut;              /* 콘솔 출력 핸들, initScreen()에서 한 번 설정 */

void initScreen(void);           /* main 맨 처음에 한 번: 핸들, 창 크기, 제목 */
void gotoxy(int x, int y);       /* 커서를 (x, y)로 이동. x = 가로 칸, y = 세로 줄 */
void setColor(int attr);         /* 이후 출력할 글자 색 바꾸기 */
void showCursor(int visible);    /* 1 = 커서 보이기(입력받을 때), 0 = 숨기기 */
void clearScreen(void);          /* 화면 전체 지우기 */
void clearLine(int y);           /* y번째 줄만 지우기 */
void waitKey(void);              /* 아무 키나 누를 때까지 기다리기 */
int  showMenu(void);             /* 메인 메뉴 출력, 1·2·3 중 선택한 번호 반환 */

#endif
