/* map_round.c - 4R 십자 노선도 빈칸 라운드 구현
 * 담당: 정재부
 *
 * 흐름: 환승역 고르기(그래프 데이터) → 칸 좌표 배열 만들기 → 빈칸 고르기
 *       → 12초 보여주기 → 빈칸으로 다시 그리기 → 20초 안에 입력 → 점수 반환
 *
 * 그리기·빈칸 만들기·입력이 모두 MapCell 배열 하나만 돌면 되도록 나눔.
 * 정답 비교·점수는 최현영의 score.h(isCorrect, calcScore)를 그대로 호출.
 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "map_round.h"
#include "station.h"
#include "chips.h"
#include "screen.h"
#include "sound.h"
#include "score.h"

#define CELL_W    23    /* 가로 칸 간격. 가장 긴 역 이름(동대문역사문화공원 = 18칸) + 여유 */
#define BOX_W     18    /* 빈칸 상자 [          ] 의 폭 */
#define MAP_X0     6    /* 가로줄 첫 칸의 x */
#define MAP_Y0    16    /* 가로줄의 y */
#define V_STEP     3    /* 세로줄 칸 사이 줄 수 */
#define STATUS_Y  (MAP_Y0 + 2 * V_STEP + 3)
#define GUIDE_Y   (STATUS_Y + 2)

/* 칸 결과 상태 */
#define MARK_NONE  -1   /* 아직 안 풂 */
#define MARK_WRONG  0
#define MARK_RIGHT  1

/* ---------- 1. 노선도 데이터 만들기 ---------- */

/* stationId가 line 노선 위에서 앞뒤 2칸씩 이웃을 가지면 arm[0..4]에 순서대로 채우고 1.
 * arm[2]가 stationId 자신. 순환선(첫 역 = 마지막 역)은 한 바퀴 이어서 계산 */
static int findArm(int stationId, int line, int arm[5]) {
    int segs[16];
    int nseg = get_segments_by_line(line, segs, 16);

    for (int s = 0; s < nseg; s++) {
        const Line* L = get_segment(segs[s]);
        int eff = L->count;
        int circular = (eff > 1 && L->stations[0] == L->stations[eff - 1]);
        if (circular) eff--;                       /* 마지막 칸은 첫 역의 반복이라 뺌 */

        for (int i = 0; i < eff; i++) {
            if (L->stations[i] != stationId) continue;
            if (circular && eff >= 5) {
                for (int k = -2; k <= 2; k++)
                    arm[k + 2] = L->stations[(i + k + eff) % eff];
                return 1;
            }
            if (!circular && i >= 2 && i <= eff - 3) {   /* 끝 역 근처는 제외 */
                for (int k = -2; k <= 2; k++)
                    arm[k + 2] = L->stations[i + k];
                return 1;
            }
        }
    }
    return 0;
}

/* 가로 5칸 + 세로 4칸, 9개 역이 모두 다른지 */
static int allDistinct(const int armA[5], const int armB[5]) {
    int ids[9] = { armA[0], armA[1], armA[2], armA[3], armA[4],
                   armB[0], armB[1], armB[3], armB[4] };
    for (int i = 0; i < 9; i++)
        for (int j = i + 1; j < 9; j++)
            if (ids[i] == ids[j]) return 0;
    return 1;
}

int buildMapCells(MapCell cells[], int* lineA, int* lineB) {
    int found = 0;
    int pickA[5] = { 0 }, pickB[5] = { 0 }, pickLA = 0, pickLB = 0;

    /* 환승역(호선 2개 이상)마다, 두 호선 모두 앞뒤 2칸이 있는 경우를 후보로 모음.
     * 후보를 배열에 저장하지 않고 "k번째 후보는 1/k 확률로 교체"해서 무작위로 하나 고름 */
    for (int id = 0; id < station_count; id++) {
        const Station* s = get_station(id);
        if (!isTransfer(s)) continue;

        for (int a = 0; a < s->line_count; a++) {
            for (int b = 0; b < s->line_count; b++) {
                int armA[5], armB[5];
                if (a == b) continue;
                if (!findArm(id, s->lines[a], armA)) continue;
                if (!findArm(id, s->lines[b], armB)) continue;
                if (!allDistinct(armA, armB)) continue;

                found++;
                if (rand() % found == 0) {
                    memcpy(pickA, armA, sizeof(armA));
                    memcpy(pickB, armB, sizeof(armB));
                    pickLA = s->lines[a];
                    pickLB = s->lines[b];
                }
            }
        }
    }
    if (found == 0) return 0;

    *lineA = pickLA;                               /* 가로 호선 */
    *lineB = pickLB;                               /* 세로 호선 */
    int cx = MAP_X0 + 2 * CELL_W;                  /* 가운데 환승역 x */

    /* 화면에서 읽는 순서(위 → 아래, 왼쪽 → 오른쪽)대로 저장 → 입력 순서도 이 순서 */
    int k = 0;
    for (int v = 0; v < 2; v++) {                  /* 세로 위 2칸 */
        cells[k].x = cx;
        cells[k].y = MAP_Y0 - (2 - v) * V_STEP;
        cells[k].id = pickB[v];
        k++;
    }
    for (int h = 0; h < 5; h++) {                  /* 가로 5칸 (h = 2가 환승역) */
        cells[k].x = MAP_X0 + h * CELL_W;
        cells[k].y = MAP_Y0;
        cells[k].id = pickA[h];
        k++;
    }
    for (int v = 3; v < 5; v++) {                  /* 세로 아래 2칸 */
        cells[k].x = cx;
        cells[k].y = MAP_Y0 + (v - 2) * V_STEP;
        cells[k].id = pickB[v];
        k++;
    }
    for (int i = 0; i < k; i++) {
        cells[i].isBlank = 0;
        cells[i].isCenter = (i == 4);              /* 가로 가운데 = 환승역 */
    }
    return k;
}

/* 환승역(가운데)을 뺀 칸 중 서로 다른 count개를 무작위로 빈칸으로 */
void pickBlanks(MapCell cells[], int n, int count) {
    int chosen = 0;
    if (count > n - 1) count = n - 1;
    while (chosen < count) {
        int i = rand() % n;
        if (cells[i].isCenter || cells[i].isBlank) continue;   /* 환승역은 힌트라 안 비움 */
        cells[i].isBlank = 1;
        chosen++;
    }
}

/* ---------- 2. 그리기 ---------- */

static int isVertical(int index) {                 /* 0,1,7,8번 칸이 세로줄 */
    return index < 2 || index > 6;
}

/* 칸이 화면에서 차지하는 폭 (빈칸 상자거나 이름) */
static int cellWidth(const MapCell* c, int hide, int mark) {
    if (hide && c->isBlank) return BOX_W;
    (void)mark;
    return (int)strlen(get_station(c->id)->name);  /* CP949: 한글 1글자 = 2바이트 = 2칸 */
}

/* 한 칸 그리기. hide = 1이면 빈칸은 상자로, mark는 그 빈칸의 채점 결과 */
static void drawCell(const MapCell* c, int index, int hide, int mark, int lineA, int lineB) {
    const Station* s = get_station(c->id);

    if (isTransfer(s)) {                           /* 환승역 표시 ◎ (이름 바로 왼쪽) */
        gotoxy(c->x - 2, c->y);
        printf("◎");
    }

    gotoxy(c->x, c->y);
    if (hide && c->isBlank) {
        if (mark == MARK_NONE) {                   /* 아직 안 푼 빈칸 */
            setColor(COLOR_BLANK);
            printf("[%*s]", BOX_W - 2, "");
        } else {                                   /* 푼 빈칸: 정답을 초록/빨강으로 */
            setColor(mark == MARK_RIGHT ? COLOR_CORRECT : COLOR_WRONG);
            printf("[%-*s]", BOX_W - 2, s->name);
        }
    } else {
        if (c->isCenter) setColor(COLOR_TITLE);    /* 가운데 환승역은 노란색 */
        printf("%s", s->name);
    }
    setColor(COLOR_DEFAULT);

    /* 환승 칩: 화면에 이미 보이는 두 호선은 빼고 출력 */
    if (isVertical(index))
        gotoxy(c->x + cellWidth(c, hide, mark) + 1, c->y);   /* 세로 칸: 이름 오른쪽 */
    else if (c->isCenter)
        gotoxy(c->x + 5, c->y + 1);                           /* 가운데: 세로 선 오른쪽 아래 */
    else
        gotoxy(c->x, c->y + 1);                               /* 가로 칸: 이름 아래 */
    drawLineChipsEx(s, lineA, lineB);
}

static void drawLegend(int lineA, int lineB) {
    setColor(COLOR_TITLE);
    gotoxy(MAP_X0, 1); printf("ROUND 4 · 노선도 빈칸 채우기");
    setColor(COLOR_DEFAULT);
    gotoxy(MAP_X0, 3); printf("가로 ");
    setColor(lineColorOf(lineA)); printf(" %d호선 ", lineA); setColor(COLOR_DEFAULT);
    printf("   세로 ");
    setColor(lineColorOf(lineB)); printf(" %d호선 ", lineB); setColor(COLOR_DEFAULT);
    printf("   ◎ = 환승역, 색 숫자 = 갈아탈 수 있는 다른 호선");
}

/* 노선도 전체 그리기. hide = 0이면 전부 보여주기, 1이면 빈칸은 상자(또는 채점 결과) */
static void drawMap(const MapCell c[], int n, int hide, const int mark[], int lineA, int lineB) {
    clearScreen();
    drawLegend(lineA, lineB);

    /* 세로 연결선 | : 위 끝 칸과 아래 끝 칸 사이, 역이 없는 줄에만 */
    int lineX = c[4].x + 2;
    for (int y = c[0].y + 1; y < c[n - 1].y; y++) {
        int isCellRow = 0;
        for (int i = 0; i < n; i++)
            if (isVertical(i) && c[i].y == y) isCellRow = 1;
        if (y == MAP_Y0 || isCellRow) continue;
        gotoxy(lineX, y);
        printf("|");
    }

    /* 가로 연결선 - : 앞 칸 이름이 끝난 뒤부터 다음 칸 ◎ 자리 전까지 */
    for (int i = 2; i < 6; i++) {
        int from = c[i].x + cellWidth(&c[i], hide, mark[i]) + 1;
        int to = c[i + 1].x - 3;
        gotoxy(from, MAP_Y0);
        for (int x = from; x <= to; x++) putchar('-');
    }

    for (int i = 0; i < n; i++)
        drawCell(&c[i], i, hide, mark[i], lineA, lineB);
}

static void drawStatus(const char* label, int sec, int total, int score) {
    clearLine(STATUS_Y);
    gotoxy(MAP_X0, STATUS_Y);
    printf("%s %2d초  [", label, sec);
    for (int i = 0; i < total; i++) putchar(i < sec ? '#' : '.');   /* 시간 막대 */
    printf("]   점수 %d", score);
}

/* ---------- 3. 라운드 진행 ---------- */

int playMapRound(void) {
    MapCell cells[MAP_CELLS];
    int mark[MAP_CELLS];
    int lineA = 0, lineB = 0;
    int n = buildMapCells(cells, &lineA, &lineB);

    if (n == 0) {                                  /* 데이터에 쓸 환승역이 없을 때 */
        clearScreen();
        gotoxy(MAP_X0, 5); printf("노선도를 만들 환승역이 없어 4R을 건너뜁니다.");
        waitKey();
        return 0;
    }
    for (int i = 0; i < n; i++) mark[i] = MARK_NONE;
    pickBlanks(cells, n, MAP_BLANKS);
    showCursor(0);

    /* 1) 외우기: 전부 보여주고 1초마다 시간 막대 줄이기 */
    drawMap(cells, n, 0, mark, lineA, lineB);
    setColor(COLOR_HINT);
    gotoxy(MAP_X0, GUIDE_Y); printf("노선도를 외우세요. 잠시 뒤 몇 칸이 빈칸으로 바뀝니다.");
    setColor(COLOR_DEFAULT);
    for (int t = MAP_MEMO_SEC; t > 0; t--) {
        drawStatus("외울 시간", t, MAP_MEMO_SEC, 0);
        Sleep(1000);
    }

    /* 2) 맞히기: 빈칸을 위 → 아래, 왼쪽 → 오른쪽 순서로 입력 */
    int score = 0;
    clock_t roundStart = clock();                  /* 암기 시간이 끝난 뒤부터 잼 */
    clock_t last = roundStart;

    for (int i = 0; i < n; i++) {
        if (!cells[i].isBlank) continue;
        const char* answer = get_station(cells[i].id)->name;
        int used = (int)((clock() - roundStart) / CLOCKS_PER_SEC);

        if (used >= MAP_ANSWER_SEC) {              /* 시간이 다 됐으면 남은 빈칸은 0점 */
            mark[i] = MARK_WRONG;
            continue;
        }

        drawMap(cells, n, 1, mark, lineA, lineB);
        drawStatus("남은 시간", MAP_ANSWER_SEC - used, MAP_ANSWER_SEC, score);
        setColor(COLOR_HINT);
        gotoxy(MAP_X0, GUIDE_Y); printf("하늘색 칸에 들어갈 역 이름을 입력하고 엔터");
        setColor(COLOR_BLANK);
        gotoxy(cells[i].x + 1, cells[i].y);       /* 빈칸 상자 안으로 커서 이동 */
        showCursor(1);

        char input[64];
        if (fgets(input, sizeof(input), stdin) == NULL) input[0] = '\0';
        showCursor(0);
        setColor(COLOR_DEFAULT);

        int sec = (int)((clock() - last) / CLOCKS_PER_SEC);   /* 이 칸에 걸린 시간 */
        last = clock();
        int late = (int)((last - roundStart) / CLOCKS_PER_SEC) > MAP_ANSWER_SEC;

        if (!late && isCorrect(input, answer)) {
            mark[i] = MARK_RIGHT;
            score += calcScore(sec, 1);           /* 4R은 2배 점수 */
            playSfx(SFX_CORRECT);
        } else {
            mark[i] = MARK_WRONG;                  /* 오답 또는 시간 초과 = 0점 */
            playSfx(SFX_WRONG);
        }
    }

    /* 3) 결과: 모든 빈칸의 정답을 색으로 보여주기 */
    drawMap(cells, n, 1, mark, lineA, lineB);
    clearLine(STATUS_Y);
    setColor(COLOR_TITLE);
    gotoxy(MAP_X0, STATUS_Y); printf("4R 점수: %d점", score);
    setColor(COLOR_HINT);
    gotoxy(MAP_X0, GUIDE_Y); printf("초록 = 정답, 빨강 = 오답·시간 초과. 아무 키나 누르면 다음으로");
    setColor(COLOR_DEFAULT);
    playSfx(SFX_CLEAR);
    waitKey();
    return score;
}
