# station.c

지하철 노선과 역 데이터를 메모리에 구축하고 조회하는 구현 파일입니다.

### 내부용 함수 (`static`)
```c
static void* safe_malloc(size_t size);
```
메모리를 할당하고, 실패할 경우 에러 메시지를 출력한 뒤 프로그램을 종료합니다.

```c
static int segment_length(const RawSegment* raw);
```
원본 데이터 표(`RawSegment`)의 한 구간에 포함된 역의 개수를 셉니다.

```c
static int get_station_id(const char* name);
```
역 이름으로 기존 역 ID를 찾고, 없으면 `stations` 배열에 새 역으로 등록한 뒤 새 ID를 반환합니다.

```c
static void add_line_to_station(int id, int line);
```
해당 역 정보에 호선 번호를 추가합니다. 이미 등록된 호선이면 무시합니다.

```c
static void add_edge(int a, int b, int line);
```
`a` 역의 인접 리스트에 `b` 역으로 향하는 간선(`Edge`)을 연결합니다.

---

### 초기화 및 해제
```c
void init_subway(void);
```
`RAW_DATA`를 순회하며 역(`stations`)과 구간(`lines`) 배열을 동적 할당하고, 인접한 역끼리 양방향 간선으로 연결합니다.

```c
void free_subway(void);
```
간선 연결 리스트, 구간별 역 목록, 역 및 구간 배열 순으로 동적 할당된 모든 메모리를 해제합니다.

---

### 역 조회
```c
int find_station(const char* name);
```
역 이름과 일치하는 역의 ID를 찾아 반환하며, 없으면 `-1`을 반환합니다.

```c
const Station* get_station(int id);
```
역 ID에 해당하는 `Station` 구조체 포인터를 반환하며, 범위를 벗어나면 `NULL`을 반환합니다.

```c
int get_neighbors(int id, int* out, int* out_line, int max);
```
해당 역의 인접 역 ID를 `out` 배열에, 연결 호선을 `out_line` 배열에 최대 `max`개까지 채우고 그 개수를 반환합니다.

---

### 구간 조회
```c
int get_segments_by_line(int line, int* out, int max);
```
특정 호선(`line`)에 속하는 구간 번호들을 `out` 배열에 최대 `max`개까지 채우고 그 개수를 반환합니다.

```c
const Line* get_segment(int seg);
```
구간 번호에 해당하는 `Line` 구조체 포인터를 반환하며, 범위를 벗어나면 `NULL`을 반환합니다.
