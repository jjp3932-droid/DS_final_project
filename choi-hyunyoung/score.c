#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include "score.h"

static void normalize(const char* src, char* out) {
	int j = 0;
	for (int i = 0; src[i] != '\0' && j < 63; i++) {
		if (src[i] != ' ' && src[i] != '\n' && src[i] != '\r')
			out[j++] = src[i];
	}
	out[j] = '\0';

	int len = (int)strlen(out);
	if (len >= 2 && strcmp(out + len - 2, "¿ª") == 0)
		out[len - 2] = '\0';

}
int isCorrect(const char* input, const char* answer) {
	char a[64], b[64];
	normalize(input, a);
	normalize(answer, b);
	return strcmp(a, b) == 0;
}
int calcScore(int elapsedSec, int isDouble) {
	int s = 100 - 5 * elapsedSec;
	if (s < 60) s = 60;
	return isDouble ? s * 2 : s;
}