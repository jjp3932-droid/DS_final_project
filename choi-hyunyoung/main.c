#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "round.h"

int main(void) {
	int score = playOrderRound(3, 6, 20);
	printf("\n이번 라운드 점수: %d점\n", score);
	return 0;
}
