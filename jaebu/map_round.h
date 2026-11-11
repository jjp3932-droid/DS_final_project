/* map_round.h - 4R 십자 노선도 빈칸 라운드
 * 담당: 정재부
 */
#ifndef MAP_ROUND_H
#define MAP_ROUND_H

#define MAP_CELLS       9    /* 가로 5칸(환승역 포함) + 세로 위 2칸 + 아래 2칸 */
#define MAP_BLANKS      3    /* 빈칸 개수 (팀에서 확정 후 수정: 보너스 2 기준과 연결됨) */
#define MAP_MEMO_SEC   12    /* 외울 시간 */
#define MAP_ANSWER_SEC 20    /* 답할 시간 */

typedef struct {
    int x, y;        /* 화면 좌표 */
    int id;          /* 역 ID (건희 get_station(id)로 이름·호선 조회) */
    int isBlank;     /* 빈칸이면 1 */
    int isCenter;    /* 두 호선이 만나는 가운데 환승역이면 1 */
} MapCell;

/* 4R 한 판 실행 → 얻은 점수 반환 (최현영의 playGame에서 호출) */
int playMapRound(void);

/* 아래는 테스트·디버깅용으로 공개 */
int buildMapCells(MapCell cells[], int* lineA, int* lineB);   /* 성공하면 칸 수(9), 실패 0 */
void pickBlanks(MapCell cells[], int n, int count);

#endif
