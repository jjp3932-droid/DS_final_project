#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "queue1.h"
#include "score.h"
#include "round.h"

static const char* FAKE[] = {
    "길음", "성신여대입구", "한성대입구", "혜화", "동대문",
    "동대문역사문화공원", "충무로", "명동", "회현", "서울역",
    "숙대입구", "삼각지", "신용산"
};
const char* nameOf(int id) { return FAKE[id]; }

// ===== 남은 시간 표시용 스레드 (함수 밖, 파일 위쪽에) =====
static volatile int timerRunning = 0;
static clock_t timerStart;
static int timerLimit;

static DWORD WINAPI countdown(LPVOID arg) {
    char title[64];
    while (timerRunning) {
        int left = timerLimit - (int)((clock() - timerStart) / CLOCKS_PER_SEC);
        if (left < 0) left = 0;
        sprintf(title, "남은 시간: %d초", left);
        SetConsoleTitleA(title);
        Sleep(200);
    }
    SetConsoleTitleA("역 이름 외우기");
    return 0;
}

int playOrderRound(int count, int memSec, int inputSec) {
    int question[13];
    for (int i = 0; i < count; i++) question[i] = i;

    Queue q;
    initQueue(&q);

    system("cls");
    printf("순서대로 외우세요! (%d초)\n\n", memSec);
    for (int i = 0; i < count; i++) {
        printf("%s", nameOf(question[i]));
        if (i < count - 1) printf(" → ");
        enqueue(&q, question[i]);
    }
    printf("\n\n");

    // 암기 시간 카운트다운
    for (int t = memSec; t > 0; t--) {
        printf("\r남은 암기 시간: %2d초", t);
        Sleep(1000);
    }
    system("cls");

    int score = 0;
    int order = 1;
    char input[64];
    clock_t roundStart = clock();
    clock_t last = roundStart;

    // 타이머 시작
    timerStart = roundStart;
    timerLimit = inputSec;
    timerRunning = 1;
    HANDLE hTimer = CreateThread(NULL, 0, countdown, NULL, 0, NULL);

    while (!isEmpty(&q)) {
        int answer = dequeue(&q);

        if ((clock() - roundStart) / CLOCKS_PER_SEC >= inputSec) {
            printf("%d번째 역: 시간 초과 (정답: %s)\n", order++, nameOf(answer));
            continue;
        }

        printf("%d번째 역: ", order++);
        fgets(input, sizeof(input), stdin);

        int sec = (int)((clock() - last) / CLOCKS_PER_SEC);
        last = clock();

        if (isCorrect(input, nameOf(answer))) {
            int s = calcScore(sec, 0);
            printf(" o 정답! +%d점 (%d초)\n", s, sec);
            score += s;
        }
        else {
            printf(" x 오답 (정답: %s)\n", nameOf(answer));
        }
    }

    // 타이머 끝
    timerRunning = 0;
    WaitForSingleObject(hTimer, INFINITE);
    CloseHandle(hTimer);

    return score;
}
