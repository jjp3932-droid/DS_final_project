/* test_main.c - [재부 혼자 테스트용] 메뉴 → 4R → 이름 입력 → 랭킹
 * 합칠 때는 이 파일을 프로젝트에서 빼고 현영의 최종 main을 사용
 *
 * 함께 필요한 파일: screen, ranking, chips, map_round, sound (재부)
 *                   station, station_data (건희), score (test_only 가짜 또는 현영 진짜)
 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "screen.h"
#include "ranking.h"
#include "map_round.h"
#include "station.h"
#include "sound.h"

int main(void) {
    srand((unsigned)time(NULL));     /* 매번 다른 문제가 나오게 한 번만 */
    initScreen();
    init_subway();                   /* 건희: 역 데이터·그래프 만들기 */

    Record* ranking = loadRanking(RANKING_FILE);
    /* playBgm("bgm.wav"); */        /* wav 파일을 프로젝트 폴더에 넣은 뒤 주석 해제 */

    while (1) {
        int menu = showMenu();
        if (menu == 1) {
            clock_t start = clock();
            int score = playMapRound();                         /* 지금은 4R만 테스트 */
            int totalSec = (int)((clock() - start) / CLOCKS_PER_SEC);
            ranking = registerScore(ranking, score, totalSec);  /* head가 바뀔 수 있어 다시 받음 */
        } else if (menu == 2) {
            showRanking(ranking);
        } else {
            break;
        }
    }

    stopBgm();
    freeRanking(ranking);
    free_subway();
    clearScreen();
    return 0;
}
