재부 담당 코드 (화면·메뉴, 랭킹, 환승 칩, 4R 노선도, 사운드)
==========================================================

[파일]
screen.h / screen.c        gotoxy, setColor, 색 상수, showMenu  -> 팀원에게 가장 먼저 공유
ranking.h / ranking.c      랭킹 연결 리스트, ranking.txt 저장/불러오기, 이름 입력(registerScore)
chips.h / chips.c          환승역 옆 호선 색 칩 (drawLineChips) -> 현영 순서 입력 화면에서도 사용
map_round.h / map_round.c  4R 십자 노선도 빈칸 라운드 (playMapRound -> 점수 반환)
sound.h / sound.c          효과음 playSfx(SFX_CORRECT 등), 배경음 playBgm/stopBgm

test_only/ 폴더 (혼자 테스트할 때만, 합칠 때는 빼기)
  test_main.c              메뉴 -> 4R -> 이름 입력 -> 랭킹 순서로 실행
  score.h / score.c        현영 score 파일 대신 쓰는 가짜 (합칠 때 현영 진짜로 교체)

[Visual Studio에서 혼자 테스트하기]
1. Finals 프로젝트 폴더(Finals.vcxproj 있는 곳)에 아래 파일을 모두 복사
   - 이 폴더의 .c .h 10개
   - test_only 폴더의 3개 (test_main.c, score.h, score.c)
   - 기말프로젝트\station 폴더의 station.h, station.c, station_data.c (main.c는 빼기)
2. 솔루션 탐색기에서 소스 파일 -> 추가 -> 기존 항목으로 .c 전부, 헤더 파일 -> 기존 항목으로 .h 전부 추가
3. main 함수가 있는 다른 파일(Finals.c 등)은 "프로젝트에서 제외"
4. Ctrl+Shift+B 빌드, Ctrl+F5 실행

[합칠 때]
- test_only 폴더 3개 파일은 빼고, 현영의 진짜 score.h/score.c와 최종 main을 사용
- 현영 score.h의 함수 모양이 아래와 같은지 확인 (다르면 map_round.c 두 줄만 고치면 됨)
    int isCorrect(const char* input, const char* answer);
    int calcScore(int sec, int isDouble);

[모든 파일은 CP949(ANSI)로 저장되어 있음. 다른 이름으로 저장할 때 UTF-8로 바꾸지 말 것]
