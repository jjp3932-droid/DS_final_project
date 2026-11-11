/* chips.c - 환승역 옆 호선 칩 구현
 * 담당: 정재부
 *
 * 숫자를 찍기 직전에만 배경색을 호선 색으로 바꾸고 바로 돌려놓으면
 * 작은 색 칩처럼 보임. 건희의 Station 구조체(lines[6], line_count) 기준.
 */
#include <stdio.h>
#include "chips.h"
#include "screen.h"

/* 호선별 칩 색 (앞자리 = 배경, 뒷자리 = 글자). 콘솔 16색 중 실제 노선색과 비슷한 것 */
static const int lineColor[MAX_LINE_NO + 1] = {
    COLOR_DEFAULT, /* 0: 안 씀 */
    0x1F,          /* 1호선 남색 바탕 + 흰 글자 */
    0xA0,          /* 2호선 연두 바탕 + 검은 글자 */
    0xE0,          /* 3호선 노랑(주황 대신) */
    0xB0,          /* 4호선 하늘 */
    0x5F,          /* 5호선 보라 */
    0x6F,          /* 6호선 갈색(어두운 노랑) */
    0x2F,          /* 7호선 올리브(어두운 초록) */
    0xD0,          /* 8호선 분홍 */
    0x6E           /* 9호선 금색 대신 */
};

int isTransfer(const Station* s) {
    return s != NULL && s->line_count >= 2;
}

int lineColorOf(int line) {
    if (line < 0 || line > MAX_LINE_NO) return 0x70;   /* 번호표 밖 호선은 흰 바탕 */
    return lineColor[line];
}

int drawLineChipsEx(const Station* s, int exclude1, int exclude2) {
    int width = 0;
    if (!isTransfer(s)) return 0;                       /* 환승역이 아니면 칩 없음 */

    for (int i = 0; i < s->line_count; i++) {
        int line = s->lines[i];
        if (line == exclude1 || line == exclude2) continue;   /* 지금 보이는 호선은 생략 */
        setColor(lineColorOf(line));
        width += printf("%d", line);
        setColor(COLOR_DEFAULT);
        width += printf(" ");
    }
    return width;
}

int drawLineChips(const Station* s, int currentLine) {
    return drawLineChipsEx(s, currentLine, -1);
}
