/* sound.c - 효과음과 배경음 구현
 * 담당: 정재부
 *
 * 효과음은 Beep, 배경음은 PlaySound로 나눔.
 * PlaySound는 한 번에 한 소리만 나서, 효과음까지 PlaySound로 내면 BGM이 끊김.
 */
#include <windows.h>
#include <mmsystem.h>
#include "sound.h"

#ifdef _MSC_VER
#pragma comment(lib, "winmm.lib")      /* Visual Studio에서 PlaySound 링크 */
#endif

void playSfx(int type) {
    /* Beep은 소리가 끝날 때까지 멈추므로 짧게 (입력 시간에 영향 최소화) */
    if (type == SFX_CORRECT)    { Beep(880, 80);  Beep(1320, 100); }
    else if (type == SFX_WRONG) { Beep(220, 180); }
    else                        { Beep(660, 90);  Beep(880, 90);  Beep(1100, 160); }
}

void playBgm(const char* wavPath) {
    PlaySoundA(wavPath, NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
}

void stopBgm(void) {
    PlaySoundA(NULL, NULL, 0);
}
