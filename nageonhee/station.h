#ifndef STATION_H
#define STATION_H

#define MAX_LINE_PER_STATION 6     /* 한 역이 가질 수 있는 최대 호선 수 */
#define MAX_NAMES_PER_SEGMENT 60   /* 데이터 표에서 한 구간의 최대 역 수 */

/* 간선: 역과 역 사이 연결 하나 */
typedef struct Edge {
    int to;              // 연결된 역 ID
    int line;            // 몇 호선으로 이어졌는지
    struct Edge* next;   // 같은 역의 다음 간선
} Edge;

/* 역: 그래프의 정점 */
typedef struct {
    char name[32];       // 역 이름
    int lines[6];        // 이 역이 속한 호선들
    int line_count;      // 속한 호선 개수
    Edge* head;          // 인접한 역 목록 (연결 리스트)
} Station;

/* 구간: 한 호선의 한 방면 */
typedef struct {
    int line;            // 호선 번호 (1, 2, ...)
    char branch[32];     // 방면 이름 ("인천 방면")
    int* stations;       // 역 ID들, 순서대로 (동적 할당)
    int count;           // 이 구간의 역 개수
} Line;

/* 원본 데이터 표 (subway_data.c에 있음) */
typedef struct {
    int line;
    const char* branch;
    const char* names[60];   // 역 이름들, 끝은 NULL
} RawSegment;

extern const RawSegment RAW_DATA[];
extern const int RAW_COUNT;

/* 실제 데이터 (station.c에 있음) */
extern Station* stations;
extern Line* lines;
extern int station_count;
extern int segment_count;

/* 다른 파일에서 쓰는 함수 */
void init_subway(void); // 동적 할당 및 생성자 함수
void free_subway(void); // 동적 할당 free
int find_station(const char* name); // 지하철역의 이름으로 역 ID 찾기
const Station* get_station(int id); // 역 ID로 역 정보(인접한 역 포함) 가져오기
int get_neighbors(int id, int* out, int* out_line, int max); // 인접 역 ID와 연결 호선을 채우고 개수 반환
int get_segments_by_line(int line, int* out, int max); // 해당 호선의 구간 번호들을 out에 채우고 개수 반환
const Line* get_segment(int seg); // 구간 번호로 구간 정보 가져오기

#endif