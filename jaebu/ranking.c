/* ranking.c - 랭킹 연결 리스트 구현
 * 담당: 정재부
 *
 * 핵심: 새 기록을 넣을 때 자기 자리를 찾아 끼워 넣으므로
 *       리스트는 항상 1등부터 순서대로 정렬된 상태 (삽입 O(n), 정렬 함수 필요 없음)
 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ranking.h"
#include "screen.h"

/* a가 b보다 순위가 높으면 1: 점수가 높을수록, 동점이면 시간이 짧을수록 위 */
static int isBetter(const Record* a, const Record* b) {
    if (a->score != b->score) return a->score > b->score;
    return a->totalSec < b->totalSec;
}

Record* insertRecord(Record* head, const char* name, int score, int totalSec) {
    Record* node = (Record*)malloc(sizeof(Record));
    if (node == NULL) return head;                  /* 메모리 부족이면 그냥 넘어감 */
    strncpy(node->name, name, NAME_MAX_BYTES - 1);
    node->name[NAME_MAX_BYTES - 1] = '\0';
    node->score = score;
    node->totalSec = totalSec;
    node->next = NULL;

    /* 1) 맨 앞에 들어가는 경우 (빈 리스트 포함) → head가 바뀜 */
    if (head == NULL || isBetter(node, head)) {
        node->next = head;
        return node;
    }

    /* 2) 나보다 순위가 높거나 같은 노드들을 지나가서, 그 뒤에 끼움
     *    (동점·동시간이면 먼저 들어온 기록이 위에 남음) */
    Record* cur = head;
    while (cur->next != NULL && !isBetter(node, cur->next))
        cur = cur->next;
    node->next = cur->next;
    cur->next = node;
    return head;
}

Record* loadRanking(const char* path) {
    Record* head = NULL;
    FILE* fp = fopen(path, "r");
    if (fp == NULL) return NULL;                    /* 처음 실행이면 파일이 없음 */

    char name[NAME_MAX_BYTES];
    int score, sec;
    while (fscanf(fp, "%19s %d %d", name, &score, &sec) == 3)
        head = insertRecord(head, name, score, sec);
    fclose(fp);
    return head;
}

void saveRanking(Record* head, const char* path) {
    FILE* fp = fopen(path, "w");
    if (fp == NULL) return;
    for (Record* p = head; p != NULL; p = p->next)
        fprintf(fp, "%s %d %d\n", p->name, p->score, p->totalSec);
    fclose(fp);
}

void showRanking(Record* head) {
    clearScreen();
    showCursor(0);
    setColor(COLOR_TITLE);
    gotoxy(40, 3); printf("======== 랭킹 TOP 10 ========");
    setColor(COLOR_HINT);
    gotoxy(40, 5); printf("순위  이름                점수    시간");
    setColor(COLOR_DEFAULT);

    int rank = 1;
    for (Record* p = head; p != NULL && rank <= 10; p = p->next, rank++) {
        if (rank == 1) setColor(COLOR_TITLE);       /* 1등은 노란색 */
        gotoxy(40, 6 + rank);
        printf("%2d등  %-18s %6d점 %4d초", rank, p->name, p->score, p->totalSec);
        setColor(COLOR_DEFAULT);
    }
    if (head == NULL) {
        gotoxy(40, 8); printf("아직 기록이 없습니다.");
    }

    setColor(COLOR_HINT);
    gotoxy(40, 19); printf("아무 키나 누르면 메뉴로 돌아갑니다");
    setColor(COLOR_DEFAULT);
    waitKey();
}

void freeRanking(Record* head) {
    while (head != NULL) {
        Record* next = head->next;                  /* free 전에 다음 노드 저장 */
        free(head);
        head = next;
    }
}

/* 이름 정리: 줄바꿈 제거, 공백은 '_'로 (파일의 %s가 공백에서 끊기므로),
 * 한글이 반으로 잘리지 않게 19바이트 안에서 자름 (CP949 한글 = 2바이트) */
static void cleanName(const char* in, char* out) {
    int i = 0, j = 0;
    while (in[i] != '\0' && in[i] != '\n' && in[i] != '\r') {
        int step = ((unsigned char)in[i] >= 0x80) ? 2 : 1;   /* 한글이면 2바이트 */
        if (j + step > NAME_MAX_BYTES - 1) break;
        if (step == 2 && in[i + 1] == '\0') break;
        for (int k = 0; k < step; k++) {
            char c = in[i + k];
            out[j++] = (c == ' ') ? '_' : c;
        }
        i += step;
    }
    out[j] = '\0';
    if (j == 0) strcpy(out, "익명");
}

Record* registerScore(Record* head, int score, int totalSec) {
    char input[64], name[NAME_MAX_BYTES];

    clearScreen();
    setColor(COLOR_TITLE);
    gotoxy(40, 8);  printf("게임 종료!");
    setColor(COLOR_DEFAULT);
    gotoxy(40, 10); printf("총점: %d점   총 소요 시간: %d초", score, totalSec);
    gotoxy(40, 12); printf("이름을 입력하세요 (한글 9자까지): ");
    showCursor(1);

    if (fgets(input, sizeof(input), stdin) == NULL) input[0] = '\0';
    cleanName(input, name);

    head = insertRecord(head, name, score, totalSec);
    saveRanking(head, RANKING_FILE);

    showCursor(0);
    gotoxy(40, 14); printf("%s 님의 기록이 저장되었습니다.", name);
    gotoxy(40, 16); printf("아무 키나 누르면 랭킹을 봅니다");
    waitKey();
    showRanking(head);
    return head;
}
