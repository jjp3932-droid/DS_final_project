/* ranking.h - 랭킹 (점수 순으로 정렬된 연결 리스트 + 파일 저장)
 * 담당: 정재부
 */
#ifndef RANKING_H
#define RANKING_H

#define RANKING_FILE "ranking.txt"
#define NAME_MAX_BYTES 20            /* 이름 최대 19바이트 = 한글 9자 */

typedef struct Record {
    char name[NAME_MAX_BYTES];       /* 플레이어 이름 */
    int  score;                      /* 총점 */
    int  totalSec;                   /* 총 소요 시간(초), 동점일 때 비교 */
    struct Record* next;             /* 다음 순위 노드 */
} Record;

/* 새 기록을 순위에 맞는 자리에 끼워 넣음. 반드시 head = insertRecord(head, ...); 로 받기 */
Record* insertRecord(Record* head, const char* name, int score, int totalSec);

Record* loadRanking(const char* path);              /* 파일 → 리스트 (파일 없으면 NULL) */
void    saveRanking(Record* head, const char* path); /* 리스트 → 파일 */
void    showRanking(Record* head);                   /* 1~10등 화면 출력 */
void    freeRanking(Record* head);                   /* 프로그램 끝날 때 한 번 */

/* 게임이 끝났을 때 한 번 호출: 이름 입력 → 삽입 → 파일 저장. 바뀐 head 반환 */
Record* registerScore(Record* head, int score, int totalSec);

#endif
