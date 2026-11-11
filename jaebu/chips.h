/* chips.h - 환승역 옆 호선 칩
 * 담당: 정재부
 */
#ifndef CHIPS_H
#define CHIPS_H

#include "station.h"

#define MAX_LINE_NO 9          /* 칩 색을 정해 둔 최대 호선 번호 (호선 번호표 확정 후 늘리기) */

int  isTransfer(const Station* s);                   /* 호선이 2개 이상이면 1 */
int  lineColorOf(int line);                          /* 호선 → 칩 색 */

/* 지금 커서 위치에, currentLine을 뺀 나머지 호선 번호를 색 칩으로 출력.
 * 출력한 칸 수를 반환 (칩이 없으면 0). 현영의 순서 입력 화면에서도 이걸 부르면 됨 */
int  drawLineChips(const Station* s, int currentLine);

/* 두 호선을 빼고 출력 (4R처럼 화면에 두 호선이 같이 나올 때) */
int  drawLineChipsEx(const Station* s, int exclude1, int exclude2);

#endif
