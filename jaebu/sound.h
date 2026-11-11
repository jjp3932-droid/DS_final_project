/* sound.h - 효과음(Beep)과 배경음(PlaySound)
 * 담당: 정재부
 */
#ifndef SOUND_H
#define SOUND_H

#define SFX_CORRECT 0     /* 정답 */
#define SFX_WRONG   1     /* 오답 */
#define SFX_CLEAR   2     /* 라운드 클리어 */

void playSfx(int type);                 /* 예: playSfx(SFX_CORRECT); */
void playBgm(const char* wavPath);      /* wav 파일 반복 재생 (실행 파일·프로젝트 폴더에 둠) */
void stopBgm(void);

#endif
