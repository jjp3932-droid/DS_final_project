#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "station.h"

/* 실제 데이터 */
Station* stations = NULL;
Line* lines = NULL;
int station_count = 0;
int segment_count = 0;

/* ---------- 내부용 함수 (이 파일에서만 사용) ---------- */

/* malloc 실패 시 종료 */
static void* safe_malloc(size_t size) {
    void* p = malloc(size);
    if (p == NULL) {
        printf("메모리 할당 실패\n");
        exit(1);
    }
    return p;
}

/* 데이터 표에서 구간 하나의 역 개수 세기 */
static int segment_length(const RawSegment* raw) {
    int n = 0;
    while (n < MAX_NAMES_PER_SEGMENT && raw->names[n] != NULL)
        n++;
    return n;
}

/* 이름으로 ID 찾기, 없으면 새 역으로 등록 */
static int get_station_id(const char* name) {
    int id = find_station(name);
    if (id != -1) return id;

    Station* s = &stations[station_count];
    strncpy(s->name, name, sizeof(s->name) - 1);
    s->name[sizeof(s->name) - 1] = '\0';
    s->line_count = 0;
    s->head = NULL;
    return station_count++;
}

/* 역에 호선 추가 (이미 있으면 무시) */
static void add_line_to_station(int id, int line) {
    Station* s = &stations[id];
    for (int i = 0; i < s->line_count; i++)
        if (s->lines[i] == line) return;
    if (s->line_count < MAX_LINE_PER_STATION)
        s->lines[s->line_count++] = line;
}

/* 간선 하나 추가 (a -> b) */
static void add_edge(int a, int b, int line) {
    Edge* e = (Edge*)safe_malloc(sizeof(Edge));
    e->to = b;
    e->line = line;
    e->next = stations[a].head;
    stations[a].head = e;
}

/* ---------- 생성 / 해제 ---------- */

void init_subway(void) {
    /* 역 배열은 표에 나온 이름 수 합계만큼 넉넉히 (환승역 중복 포함) */
    int total = 0;
    for (int i = 0; i < RAW_COUNT; i++)
        total += segment_length(&RAW_DATA[i]);

    stations = (Station*)safe_malloc(sizeof(Station) * total);
    lines = (Line*)safe_malloc(sizeof(Line) * RAW_COUNT);
    station_count = 0;
    segment_count = 0;

    for (int i = 0; i < RAW_COUNT; i++) {
        const RawSegment* raw = &RAW_DATA[i];
        int n = segment_length(raw);

        Line* L = &lines[segment_count++];
        L->line = raw->line;
        strncpy(L->branch, raw->branch, sizeof(L->branch) - 1);
        L->branch[sizeof(L->branch) - 1] = '\0';
        L->stations = (int*)safe_malloc(sizeof(int) * n);
        L->count = 0;

        int prev = -1;  /* 구간이 바뀌면 앞 역 초기화 */
        for (int j = 0; j < n; j++) {
            int id = get_station_id(raw->names[j]);
            add_line_to_station(id, raw->line);

            if (prev != -1 && prev != id) {
                add_edge(prev, id, raw->line);
                add_edge(id, prev, raw->line);
            }
            L->stations[L->count++] = id;
            prev = id;
        }
    }
}

void free_subway(void) {
    /* 안쪽부터 해제: 간선 -> 구간별 역 목록 -> 배열 */
    for (int i = 0; i < station_count; i++) {
        Edge* e = stations[i].head;
        while (e != NULL) {
            Edge* next = e->next;
            free(e);
            e = next;
        }
    }
    for (int i = 0; i < segment_count; i++)
        free(lines[i].stations);

    free(stations);
    free(lines);
    stations = NULL;
    lines = NULL;
    station_count = 0;
    segment_count = 0;
}

/* ---------- 조회 ---------- */

int find_station(const char* name) {
    for (int i = 0; i < station_count; i++)
        if (strcmp(stations[i].name, name) == 0)
            return i;
    return -1;
}

const Station* get_station(int id) {
    if (id < 0 || id >= station_count) return NULL;
    return &stations[id];
}

int get_neighbors(int id, int* out, int* out_line, int max) {
    if (id < 0 || id >= station_count) return 0;
    int n = 0;
    for (Edge* e = stations[id].head; e != NULL && n < max; e = e->next) {
        out[n] = e->to;
        if (out_line != NULL) out_line[n] = e->line;
        n++;
    }
    return n;
}

int get_segments_by_line(int line, int* out, int max) {
    int n = 0;
    for (int i = 0; i < segment_count && n < max; i++)
        if (lines[i].line == line)
            out[n++] = i;
    return n;
}

const Line* get_segment(int seg) {
    if (seg < 0 || seg >= segment_count) return NULL;
    return &lines[seg];
}
