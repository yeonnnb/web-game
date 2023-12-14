#define _CRT_SECURE_NO_WARNINGS 

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>
#include <limits.h>
#include <conio.h>

#define   INF 1000000       // 거리의 초기값
#define   NUM_VERTICES 12   // 정점의 개수
#define GOLD 1000           //소지금


#define max1 30
#define box_size 25

char start = 'A', end;    // 플레이어가 선택한 현재 위치와 목적지에 대한 전역변수
char start_point;         // 현 위치
char end_point;           // 목적지 (보호구역)
int crow_distance;        // 까마귀의 최단 거리
int player_distance;      // 플레이어의 경로 선택 거리
int GOLD1 = GOLD;

int P_kan; //칸수비교용
int C_kan;

typedef struct Map
{
    struct Map* link;         // 연결
    int vertex;               // 정점
    int weight;               // 간선 가중치
}Map;

typedef struct {
    char name[20];
    int weight;
    int value;
    int gold;
} Item;

// 짐칸 구조체
typedef struct {
    Item items[7]; // 최대 7개의 물건을 담을 수 있는 짐칸
    int currentWeight;
} Cargo;

// 두 구조체를 포인터로 연결하는 초기화 함수
void initializeCargo(Cargo* Cargo, Item* items) {
    for (int i = 0; i < 7; i++) {
        // 각 물건의 초기화
        strcpy(Cargo->items[i].name, items[i].name);
        Cargo->items[i].weight = items[i].weight;
        Cargo->items[i].value = items[i].value;
        Cargo->items[i].gold = items[i].gold;
    }
    Cargo->currentWeight = 0;  // 현재 무게 초기화
}

// 사용자로부터 키보드 입력을 받아 숫자를 배열에 저장하는 함수
void readNumbersFromInput(int* intArr, int size, Item* items) {
    char input[100]; // 입력을 저장할 문자열
    printf("넣고 싶은 순위를 입력하세요 (스페이스바로 구분): ");
    fgets(input, sizeof(input), stdin);

    // 입력을 공백을 기준으로 나누어 int 배열에 저장
    int count = 0;
    char* token = strtok(input, " ");
    while (token != NULL && count < size) {
        intArr[count] = atoi(token);
        token = strtok(NULL, " ");
        count++;
    }
    /*
    for(int i = 0; i < size; i++)
    {
        printf("이름 : %s, 가치 : %d\n", items[i].name, intArr[i]);
    }
    printf("\n");
    */

}
// 두 구조체를 교환하는 함수
void swap(Item* a, Item* b) {
    Item temp = *a;
    *a = *b;
    *b = temp;
}

// 구조체 배열을 value 기준으로 오름차순으로 정렬하는 퀵정렬 함수
void quickSort(Item* items, int low, int high) {
    if (low < high) {
        // 파티션을 나누어서 정렬
        int pivot = items[high].value; // 피벗은 배열의 마지막 요소
        int i = low - 1;

        for (int j = low; j <= high - 1; j++) {
            if (items[j].value < pivot) {
                i++;
                swap(&items[i], &items[j]);
            }
        }

        swap(&items[i + 1], &items[high]);
        int partitionIndex = i + 1;

        // 파티션을 기준으로 좌우로 재귀 호출
        quickSort(items, low, partitionIndex - 1);
        quickSort(items, partitionIndex + 1, high);
    }
}

// 0-1 배낭 문제 해결 함수
int knapsack(Item* items, int n, int* cargoWeight, int* selectedItems) {
    int bag_size[8][101];
    int weight_size[8][101];
    for (int i = 0; i < 8; i++) {
        bag_size[i][0] = 0;
        weight_size[i][0] = 0;
    }

    for (int j = 0; j <= 100; j++) {
        bag_size[0][j] = 0;
        weight_size[0][j] = 0;
    }

    for (int i = 1; i < 8; i++) {
        for (int w = 1; w <= 100; w++) {
            //printf("%d %d\n", items[i - 1].weight, w);
            if (items[i - 1].weight > w) {
                bag_size[i][w] = bag_size[i - 1][w];
                weight_size[i][w] = weight_size[i - 1][w];
            }
            else {
                bag_size[i][w] = max(bag_size[i - 1][w], bag_size[i - 1][w - items[i - 1].weight] + items[i - 1].gold);
                if (bag_size[i - 1][w] > bag_size[i - 1][w - items[i - 1].weight] + items[i - 1].gold)
                {
                    weight_size[i][w] = weight_size[i - 1][w];
                }
                else
                {
                    weight_size[i][w] = weight_size[i - 1][w - items[i - 1].weight] + items[i - 1].weight;
                }
                //printf("아이템 이름 : %s, 아이템 골드 : %d\n", items[i-1].name, items[i-1].gold);
                //printf("%d %d %d\n\n", bag_size[i - 1][w], bag_size[i - 1][w - items[i - 1].weight] + items[i - 1].gold, bag_size[i][w]);
            }
        }
    }

    int result = bag_size[7][100];
    *cargoWeight = weight_size[7][100];

    Item temp[7];

    for (int i = 0; i < 7; i++)
    {
        temp[i] = items[i];
    }

    Item temp_item;

    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 6 - i; j++)
        {
            if (temp[j].weight > temp[j + 1].weight)
            {
                temp_item = temp[j];
                temp[j] = temp[j + 1];
                temp[j + 1] = temp_item;
            }
        }
    }
    for (int j = 0; j < 7; j++)
    {
        //printf("%s %d\n", temp[j].name, temp[j].weight);
    }

    int temp_weight = weight_size[7][100];
    for (int i = 6; i > 0; i--) {
        if (temp_weight >= temp[i].weight)
        {
            temp_weight -= temp[i].weight;
            for (int j = 0; j < 7; j++)
            {
                // printf("hi");
                if (!strcmp(items[j].name, temp[i].name))
                {
                    //printf("%s %s\n", items[j].name, temp[i].name);
                    selectedItems[j] = 1;
                }
            }
        }
    }

    return bag_size[7][100];
}

// 구조체 배열의 gold에 n*100을 넣어주고 n을 감소시키는 함수
void setGoldAndDecreaseN(Item* items, int n) {
    for (int i = 0; i < 7; i++, n--) {
        items[i].gold = n * 100;
    }
}

// 마차에 물건 실린 결과 출력 함수
void printResults_cargo(Cargo* Cargo, int* selectedItems, Item* items, int MaxValue) {
    //system("cls");
    printf("마차에 실린 물건을 알려드립니다.\n");
    printf("-----------------------------------------------\n");
    printf("\n");
    printf("\n");
    printf(" 마차 = [ ");

    // 마차에 실린 물건 출력
    for (int i = 0; i < 7; i++) {
        if (selectedItems[i] == 1) {
            printf("%s, ", Cargo->items[i].name);
        }
    }

    printf("] 가 순서대로 실렸습니다. ");
    printf("\n");
    printf("마차 현재 무게 : %d / 100(kg) ", Cargo->currentWeight);
    printf("\n");
    printf("짐칸에 실린 물건의 가치로 얻은 GOLD : %d G\n", MaxValue);
    printf("\n");
    printf("-------------------------------------------------\n");
}

void cargoRound() {
    Item items[7] = {
        {"후추", 12, 0, 0},
        {"루비", 2, 0, 0},
        {"말린대추", 15, 0, 0},
        {"소금", 38, 0, 0},
        {"철광석", 90, 0, 0},
        {"다이아몬드", 1, 0, 0},
        {"화약", 33, 0, 0},
    };
    system("cls");

    int inputValues[7];
    Cargo playerCargo;
    initializeCargo(&playerCargo, items);
    //백팩 초기화 
    //system("cls");
    printf("여정을 떠나기 위해 짐칸에 물건을 실읍시다! \n");
    printf(" 물건의 가치에 따라 순위를 정하세요(위에서부터 아래로)\n");
    printf("-----------------------------------------------\n");
    //item 정보 출력
    printf("물건 정보:\n");
    for (int i = 0; i < 7; i++) {
        printf("%s\t무게: %d\t가치: %d\tGold: %d\n", items[i].name, items[i].weight, items[i].value, items[i].gold);
    }
    printf("-----------------------------------------------\n");

    //사용자로부터 입력 받아 숫자를 배열에 저장
    readNumbersFromInput(inputValues, 7, items);


    //숫자를 구조체 배열에 차례대로 할당
    for (int i = 0; i < 7; i++) {
        items[i].value = inputValues[i];
    }

    //value를 기준으로 퀵정렬 수행
    quickSort(items, 0, 6);

    /* for (int i = 0; i < 7; i++) {
        printf("이름 : %s, 가치 : %d\n", items[i].name, items[i].value);
    }
    */
    //구조체 배열의 gold에 n*100을 넣어주고, n 감소 = gold set
    int n = 7;
    setGoldAndDecreaseN(items, n);
    /*
    for (int i = 0; i < 7; i++) {
        printf("이름 : %s, 가치 : %d 골드 : %d\n", items[i].name, items[i].value, items[i].gold);
    }
    */
    //0-1 배낭, 선택된 물건들 출력
    int selectedItems[7] = { 0 };
    int cargoWeight = 0;
    int MaxValue = knapsack(items, n, &cargoWeight, selectedItems);

    playerCargo.currentWeight = cargoWeight;
    //결과 출력 
    printResults_cargo(&playerCargo, selectedItems, items, MaxValue);

    GOLD1 += MaxValue;
    printf("아무 키나 입력하세요.\n");
    _getch();

}

// 인접 리스트
Map* Graph_list[NUM_VERTICES];

void init_graph();
void insert_edge(int, int, int);
void shortest_path(int, int);

// 그래프 초기화 (시작점, 끝점, 가중치)
void init_graph()
{
    // A의 연결점 추가
    insert_edge(0, 1, 5);   // A-B
    insert_edge(0, 2, 8);   // A-C

    // B의 연결점 추가
    insert_edge(1, 3, 4);   // B-D
    insert_edge(1, 4, 18);   // B-E

    // C의 연결점 추가 
    insert_edge(2, 3, 9);   // C-D
    insert_edge(2, 5, 10);   // C-F

    // D의 연결점 추가
    insert_edge(3, 4, 8);   // D-E
    insert_edge(3, 5, 9);   // D-F

    // E의 연결점 추가
    insert_edge(4, 6, 11);   // E-G
    insert_edge(4, 7, 23);   // E-H

    // F의 연결점 추가
    insert_edge(5, 7, 11);   // F-H

    // G의 연결점 추가
    insert_edge(6, 7, 30);   // G-H
    insert_edge(6, 8, 18);   // G-I

    // H의 연결점 추가
    insert_edge(7, 8, 28);   // H-I
    insert_edge(7, 9, 17);   // H-J

    // I의 연결점 추가
    insert_edge(8, 9, 10);   // I-J
    insert_edge(8, 10, 7);   // I-K

    // J의 연결점 추가
    insert_edge(9, 10, 21);   // J-K

    // K의 연결점 추가
    insert_edge(10, 11, 15);   // K-L

}

// 지도 GUI 출력
void map_printing()
{
    system("cls");
    printf("< 올루키타 제국 지도 >\n");
    printf("\n");
    printf("                                                      ㅡㅡㅡㅡㅡㅡㅡ          ㅡㅡㅡㅡㅡ      ㅡㅡㅡㅡㅡ \n");
    printf("                                                     |             |          |        |     |         | \n");
    printf("                                                     |             |          |        |     |         | \n");
    printf("             ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ E ㅡㅡㅡㅡㅡ              G ㅡㅡㅡㅡ         |     |         K ㅡㅡㅡㅡ \n");
    printf("             |                           | |                       |                   |     |         |         | \n");
    printf("             |                           | |                       |                   |     |         |         | \n");
    printf("             |                           | |                       |                   |     |         |         | \n");
    printf("             |                       ㅡㅡ  |                       |                   I ㅡㅡ          |         | \n");
    printf("       ㅡㅡㅡ B ㅡㅡㅡ               |     |                       |                   |     |         |         | \n");
    printf("       |              |              |     |               ㅡㅡㅡㅡ                    |     |         |         | \n");
    printf("       |              |              |     |              |                            |     |         |         | \n");
    printf("       |              D ㅡㅡㅡㅡㅡㅡㅡ     |              |                            |     |         |         | \n");
    printf("       |              |                    |              |                            |     |         |         | \n");
    printf("  A ㅡㅡ              |                     ㅡㅡㅡㅡ       ㅡㅡ      ㅡㅡㅡㅡㅡㅡㅡㅡㅡ      |         |         | \n");
    printf("       |              |                            |           |     |                       |         |         L \n");
    printf("       |              |                            |           |     |                       |         | \n");
    printf("       |     ㅡㅡㅡㅡㅡㅡㅡㅡㅡ                    |           |     |                    ㅡㅡ         | \n");
    printf("       |     |                |                    |           |     |                    |            | \n");
    printf("       |     |                |                    ||ㅡㅡㅡㅡㅡ      |                    J ㅡㅡㅡㅡㅡㅡ \n");
    printf("       ㅡㅡㅡ C                F ㅡㅡㅡ            ||                |                    | \n");
    printf("             |                |       |            ||                |                    | \n");
    printf("             |                |       |            ||                |                    | \n");
    printf("             |                |       ㅡㅡㅡㅡㅡㅡ H ㅡㅡㅡㅡㅡㅡㅡㅡ                     | \n");
    printf("             ㅡㅡㅡㅡㅡㅡㅡㅡㅡ                    |                                      | \n");
    printf("                                                   ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ \n");
    printf("\n");



}

void insert_edge(int start, int end, int weight)
{
    Map* node;   // 연결을 위한 노드

    // 양방향으로 연결을 해줘야 한다.
    node = (Map*)malloc(sizeof(Map));
    node->vertex = end;
    node->weight = weight;
    node->link = Graph_list[start];
    Graph_list[start] = node;
    node->vertex = start;
    node->weight = weight;
    node->link = Graph_list[end];
    Graph_list[end] = node;
}

//다익스트라 알고리즘
void shortest_path(int start, int end)
{
    int distance[NUM_VERTICES];		// 거리
    int check[NUM_VERTICES];		// 이미 선택한 정점인지 여부
    Map* tmp;					    // 최근에 들어온 정점을 저장하기 위한 구조체 포인터
    int cycle, min, now, i;			// cycle : 작업 횟수 / min = 최단거리를 위한 비교값 / now : 최근에 들어온 정점

    // 초기화 과정
    for (i = 0; i < NUM_VERTICES; i++)
    {
        distance[i] = INF;		    // 거리를 무한으로 초기화
        check[i] = 0;				// 선택한 정점인지에 대한 여부
    }
    distance[start] = 0;			// 초기 start 설정 - 자신까지의 거리는 0
    cycle = 0;

    while (cycle < NUM_VERTICES - 1)		// 모든 정점에 대해 하기 위해서
    {
        min = INF;		// 최소값 무한으로 초기화
        for (i = 0; i < NUM_VERTICES; i++)
            if (distance[i] < min && !check[i])		// 선택하지 않은 정점 중 거리가 최소인 정점으로부터 시작
            {
                min = distance[i];
                now = i;	// 선택한 정점을 넣을 위치
            }
        check[now] = 1;			// 정점 선택하기
        tmp = Graph_list[now];	// 새로 선택한 정점의 구조체 포인터

        while (tmp != NULL)
        {
            if (!check[tmp->vertex])	// 선택하지 않은 정점이라면
            {
                //  수행한 거리 < 기존 최단거리 --> 간선 완화
                if (min + tmp->weight < distance[tmp->vertex])
                {
                    distance[tmp->vertex] = min + tmp->weight;	// 최단거리 갱신
                }
            }
            tmp = tmp->link;	// 다음 인접 정점, 간선 검사
        }
        cycle++;
        if (now == end) break;		// 현재 정점이 도착 정점과 같다면 종료
    }
    crow_distance = distance[end];
}

void player_choose() {

    char cross_point;         // 지나가는 정점
    int weight = 0;           // 정점의 가중치를 더한 값

    start_point = start;
    printf("현재 위치는 %c 입니다\n\n", start_point);
    getchar();

    if (start_point == 'A') printf("어느 보호구역을 목적지로 선정하겠습니까? \n보호구역은 'E', 'J', 'L'로 총 3구역이 있습니다.\n");
    else if (start_point == 'E') printf("어느 보호구역을 목적지로 선정하겠습니까? \n보호구역은 'J', 'L'로 총 2구역이 있습니다.\n");
    else printf("어느 보호구역을 목적지로 선정하겠습니까? \n보호구역은 'L'로 총 1구역이 있습니다.\n");

    while (1) {
        printf("목적지 : ");
        scanf("%c", &end_point);
        end = end_point;
        getchar();

        if ((end_point == 'E') || (end_point == 'J') || (end_point == 'L'))
        {
            printf("\n현재 위치 %c에서 목적지 %c로 출발하겠습니다.\n", start_point, end_point);
            break;
        }
        else {
            printf("\n다시 선정해주세요\n");
        }
    }

    while (1) {
        if (start_point == end_point) {
            printf("출발지에서 %c까지...총 %dkm를 왔습니다.\n\n", end_point, weight);
            player_distance = weight;
            break;
        }
        else {
            printf("\n현재 위치는 %c입니다\n", start_point);
            printf("갈림길이 나왔습니다! 어디로 가시겠습니까? : ");
            scanf(" %c", &cross_point);


            if (start_point == 'A') {   //A에서 출발
                if (cross_point == 'B') {
                    weight = weight + 5;
                    start_point = cross_point;
                    getchar();
                    printf("%c로 이동 중...\n", cross_point);
                }
                else if (cross_point == 'C') {
                    weight = weight + 8;
                    start_point = cross_point;
                    getchar();
                    printf("%c로 이동 중...\n", cross_point);
                }
                else {
                    printf("\n갈 수 없는 곳 입니다. 다시 선택해주세요.\n");
                    getchar();
                }

            }
            else if (start_point == 'B') {  //B에서 출발
                if (cross_point == 'A') {
                    weight = weight + 5;
                    start_point = cross_point;
                    getchar();
                    printf("%c로 이동 중...\n", cross_point);
                }
                else if (cross_point == 'D') {
                    weight = weight + 4;
                    start_point = cross_point;
                    getchar();
                    printf("%c로 이동 중...\n", cross_point);
                }
                else if (cross_point == 'E') {
                    weight = weight + 18;
                    start_point = cross_point;
                    getchar();
                    printf("%c로 이동 중...\n", cross_point);
                }
                else {
                    printf("갈 수 없는 곳 입니다. 다시 선택해주세요.");
                }
            }
            else if (start_point == 'C') {   //C에서 출발
                if (cross_point == 'A') {
                    weight = weight + 8;
                    start_point = cross_point;
                    getchar();
                    printf("%c로 이동 중...\n", cross_point);
                }
                else if (cross_point == 'D') {
                    weight = weight + 9;
                    start_point = cross_point;
                    getchar();
                    printf("%c로 이동 중...\n", cross_point);
                }
                else if (cross_point == 'F') {
                    weight = weight + 10;
                    start_point = cross_point;
                    getchar();
                    printf("%c로 이동 중...\n", cross_point);
                }
                else {
                    printf("갈 수 없는 곳 입니다. 다시 선택해주세요.");
                }

            }
            else if (start_point == 'D') {    //D에서 출발
                if (cross_point == 'B') {
                    weight = weight + 4;
                    start_point = cross_point;
                    getchar();
                    printf("%c로 이동 중...\n", cross_point);
                }
                else if (cross_point == 'C') {
                    weight = weight + 9;
                    start_point = cross_point;
                    getchar();
                    printf("%c로 이동 중...\n", cross_point);
                }
                else if (cross_point == 'E') {
                    weight = weight + 8
                        ;
                    start_point = cross_point;
                    getchar();
                    printf("%c로 이동 중...\n", cross_point);
                }
                else if (cross_point == 'F') {
                    weight = weight + 9;
                    start_point = cross_point;
                    getchar();
                    printf("%c로 이동 중...\n", cross_point);
                }
                else {
                    printf("갈 수 없는 곳 입니다. 다시 선택해주세요.");
                }

            }
            else if (start_point == 'E') {    //E에서 출발
                if (cross_point == 'B') {
                    weight = weight + 18;
                    start_point = cross_point;
                    getchar();
                    printf("%c로 이동 중...\n", cross_point);
                }
                else if (cross_point == 'D') {
                    weight = weight + 8;
                    start_point = cross_point;
                    getchar();
                    printf("%c로 이동 중...\n", cross_point);
                }
                else if (cross_point == 'G') {
                    weight = weight + 11;
                    start_point = cross_point;
                    getchar();
                    printf("%c로 이동 중...\n", cross_point);
                }
                else if (cross_point == 'H') {
                    weight = weight + 23;
                    start_point = cross_point;
                    getchar();
                    printf("%c로 이동 중...\n", cross_point);
                }
                else {
                    printf("갈 수 없는 곳 입니다. 다시 선택해주세요.");
                }

            }
            else if (start_point == 'F') {    //F에서 출발
                if (cross_point == 'C') {
                    weight = weight + 10;
                    start_point = cross_point;
                    getchar();
                    printf("%c로 이동 중...\n", cross_point);
                }
                else if (cross_point == 'D') {
                    weight = weight + 9;
                    start_point = cross_point;
                    getchar();
                    printf("%c로 이동 중...\n", cross_point);
                }
                else if (cross_point == 'H') {
                    weight = weight + 11;
                    start_point = cross_point;
                    getchar();
                    printf("%c로 이동 중...\n", cross_point);
                }
                else {
                    printf("갈 수 없는 곳 입니다. 다시 선택해주세요.");
                }

            }
            else if (start_point == 'G') {    //G에서 출발
                if (cross_point == 'E') {
                    weight = weight + 11;
                    start_point = cross_point;
                    getchar();
                    printf("%c로 이동 중...\n", cross_point);
                }
                else if (cross_point == 'H') {
                    weight = weight + 30;
                    start_point = cross_point;
                    getchar();
                    printf("%c로 이동 중...\n", cross_point);
                }
                else if (cross_point == 'I') {
                    weight = weight + 18;
                    start_point = cross_point;
                    getchar();
                    printf("%c로 이동 중...\n", cross_point);
                }
                else {
                    printf("갈 수 없는 곳 입니다. 다시 선택해주세요.");
                }

            }
            else if (start_point == 'H') {    //H에서 출발
                if (cross_point == 'E') {
                    weight = weight + 23;
                    start_point = cross_point;
                    getchar();
                    printf("%c로 이동 중...\n", cross_point);
                }
                else if (cross_point == 'F') {
                    weight = weight + 11;
                    start_point = cross_point;
                    getchar();
                    printf("%c로 이동 중...\n", cross_point);
                }
                else if (cross_point == 'G') {
                    weight = weight + 30;
                    start_point = cross_point;
                    getchar();
                    printf("%c로 이동 중...\n", cross_point);
                }
                else if (cross_point == 'I') {
                    weight = weight + 28;
                    start_point = cross_point;
                    getchar();
                    printf("%c로 이동 중...\n", cross_point);
                }
                else if (cross_point == 'J') {
                    weight = weight + 17;
                    start_point = cross_point;
                    getchar();
                    printf("%c로 이동 중...\n", cross_point);
                }
                else {
                    printf("갈 수 없는 곳 입니다. 다시 선택해주세요.");
                }

            }
            else if (start_point == 'I') {    //I에서 출발
                if (cross_point == 'G') {
                    weight = weight + 18;
                    start_point = cross_point;
                    getchar();
                    printf("%c로 이동 중...\n", cross_point);
                }
                else if (cross_point == 'H') {
                    weight = weight + 28;
                    start_point = cross_point;
                    getchar();
                    printf("%c로 이동 중...\n", cross_point);
                }
                else if (cross_point == 'J') {
                    weight = weight + 10;
                    start_point = cross_point;
                    getchar();
                    printf("%c로 이동 중...\n", cross_point);
                }
                else if (cross_point == 'K') {
                    weight = weight + 7;
                    start_point = cross_point;
                    getchar();
                    printf("%c로 이동 중...\n", cross_point);
                }
                else {
                    printf("갈 수 없는 곳 입니다. 다시 선택해주세요.");
                }

            }
            else if (start_point == 'J') {    //J에서 출발
                if (cross_point == 'H') {
                    weight = weight + 17;
                    start_point = cross_point;
                    getchar();
                    printf("%c로 이동 중...\n", cross_point);
                }
                else if (cross_point == 'I') {
                    weight = weight + 10;
                    start_point = cross_point;
                    getchar();
                    printf("%c로 이동 중...\n", cross_point);
                }
                else if (cross_point == 'K') {
                    weight = weight + 21;
                    start_point = cross_point;
                    getchar();
                    printf("%c로 이동 중...\n", cross_point);
                }
                else {
                    printf("갈 수 없는 곳 입니다. 다시 선택해주세요.");
                }

            }
            else if (start_point == 'K') {     //K에서 출발
                if (cross_point == 'I') {
                    weight = weight + 7;
                    start_point = cross_point;
                    getchar();
                    printf("%c로 이동 중...\n", cross_point);
                }
                else if (cross_point == 'J') {
                    weight = weight + 21;
                    start_point = cross_point;
                    getchar();
                    printf("%c로 이동 중...\n", cross_point);
                }
                else if (cross_point == 'L') {
                    weight = weight + 8;
                    start_point = cross_point;
                    getchar();
                    printf("%c로 이동 중...\n", cross_point);
                }
                else {
                    printf("갈 수 없는 곳 입니다. 다시 선택해주세요.");
                }

            }
            else if (cross_point == 'L') {
                start_point = cross_point;
            }
        }

    }

}

void find_map() {
    // 그래프 생성
    init_graph();

    // 지도 출력
    map_printing();

    player_choose();

    shortest_path(start - 'A', end - 'A'); // 다익스트라 알고리즘 실행
    start = end_point;

    if (crow_distance < player_distance)
    {
        printf("\n까마귀에게 잡혀 소지금 500G를 강탈당하였습니다.\n");
        GOLD1 = GOLD1 - 500;
    }
    else
    {
        printf("\n까마귀를 따돌리고 무사히 보호구역에 도착했습니다.\n");
    }
}

enum ColorType { skyblue = 11, LIGHTGRAY = 7}COLOR;

void insert_value(int bingong[5][5], int ib[], int count);

typedef struct block { //블럭 구조체  
    int num; //블럭 칸 수 
    int index[5]; //블럭 모양  
}block;

block all_block[max1] = { //전체 블록 모양 저장 
         {1, {1}}, //index 0 
         {2, {1,2}}, {2, {1,11}}, //index 1~2
         {3, {1,2,3}}, {3, {1,2,11}}, {3, {2,11,12}}, {3, {1,11,21}}, {3,{1,2,12}}, //index 3~7
         {4, {1,2,11,12}}, {4, {1,11,12,21}}, {4, {1,2,3,4}}, {4, {1,11,12,22}}, {4, {1,2,3,12}}, //index 8~12
         {5, {1,2,3,4,5}}, {5, {1,11,21,31,41}}, {5, {1,2,3,12,13}}, {5, {1,11,12,21,22}}, {5, {1,2,3,12,22}} //index 13~17
};

void textcolor(int colorNum) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorNum);
}

void gotoxy(int x, int y) {
    COORD pos = { x,y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void choose_block(block* choosed_block) {
    int random;
    srand(time(NULL)); // 매번 다른 시드값 생성

    choosed_block[0] = all_block[0];

    random = rand() % 2; // 난수 생성
    if (random == 0) choosed_block[1] = all_block[1];
    else choosed_block[1] = all_block[2];

    random = rand() % 5; // 난수 생성

    if (random == 0) { //random 값에 따라 블럭 뽑기 
        choosed_block[2] = all_block[3];
        choosed_block[3] = all_block[4];
        choosed_block[4] = all_block[8];
        choosed_block[5] = all_block[9];
        choosed_block[6] = all_block[13];
        choosed_block[7] = all_block[14];
    }
    else if (random == 1) {
        choosed_block[2] = all_block[4];
        choosed_block[3] = all_block[5];
        choosed_block[4] = all_block[9];
        choosed_block[5] = all_block[10];
        choosed_block[6] = all_block[14];
        choosed_block[7] = all_block[15];
    }
    else if (random == 2) {
        choosed_block[2] = all_block[5];
        choosed_block[3] = all_block[6];
        choosed_block[4] = all_block[10];
        choosed_block[5] = all_block[11];
        choosed_block[6] = all_block[15];
        choosed_block[7] = all_block[16];
    }
    else if (random == 3) {
        choosed_block[2] = all_block[6];
        choosed_block[3] = all_block[7];
        choosed_block[4] = all_block[11];
        choosed_block[5] = all_block[12];
        choosed_block[6] = all_block[16];
        choosed_block[7] = all_block[17];
    }
    else {
        choosed_block[2] = all_block[3];
        choosed_block[3] = all_block[7];
        choosed_block[4] = all_block[8];
        choosed_block[5] = all_block[12];
        choosed_block[6] = all_block[13];
        choosed_block[7] = all_block[17];
    }
}

void print_block(block* choosed_block, int* selected_block) {
    for (int i = 0; i < 2; i++) {
        for (int k = 0; k < 4; k++) {
            if (selected_block[i * 4 + k + 1] == 1) continue;
            for (int j = 0; j < choosed_block[i * 4 + k].num; j++) {
                int a = choosed_block[i * 4 + k].index[j] % 5;
                int b = choosed_block[i * 4 + k].index[j] / 5 / 2;
                if (a == 0) a = 5;

                gotoxy(13 + (k * 13) + ((a - 1) * 2), 10 + (i * 8) + b);
                printf("■");
            }
        }
    }
}

void print_box(int* b) { //b는 1차원 배열 
    for (int i = 0; i < 5; i++) {
        gotoxy(30, 30 + (i * 1));
        for (int k = 1; k <= 5; k++) {
            if (b[(i * 5) + k] == 1) printf("■");
            else printf("□");
        }
    }
}

void UIprint(int* b, block* choosed_block, int* selected_block) {
    system("cls");
    print_block(choosed_block, selected_block); //주어진 블록들  출력 
    print_box(b); //현재 박스 상태 출력  
    Sleep(5);
}

int compare(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

void insert_value(int bingong[5][5], int ib[], int count) {
    int x1, y1;
    for (int a = 0; a < count; a++) {
        y1 = ib[a] / 10;
        x1 = ib[a] % 10 - 1;
        bingong[y1][x1] = 1;
    }
}

void check_space(int bingong[5][5], int ib[], int count) {
    int x, y;
    for (int i = 0; i < count; i++) {
        y = ib[i] / 10;
        x = ib[i] % 10 - 1;

        if (bingong[y][x] == 1) return;
    }
    insert_value(bingong, ib, count);
}

void block_case1(int count, block* choosed_block, int ib[], int j_ib[], int bingong[5][5], int del[]) {
    check_space(bingong, ib, count);
    del[0] = 1;
}

void block_case2(int count, block* choosed_block, int ib[], int j_ib[], int bingong[5][5], int del[]) {
    for (int j = 0; j < 2; j++) { //2칸짜리 입력한거 맞는지 보는중.
        if (j_ib[j] != choosed_block[1].index[j]) {
            break;
        }
        else if (j_ib[j] == choosed_block[1].index[j]) {
            //이미 그 자리에 블럭이 있는지 확인
            check_space(bingong, ib, count);
            del[1] = 1;
            //빈자리면 블럭넣기.

        }
    }
}

void block_case3(int count, block* choosed_block, int ib[], int j_ib[], int bingong[5][5], int del[]) {

    switch (1) {
    case 1:

        printf("\n[[이번답: %d, %d, %d\n", choosed_block[2].index[0], choosed_block[2].index[1], choosed_block[2].index[2]);

        if (choosed_block[2].index[0] - choosed_block[2].index[1] == j_ib[0] - j_ib[1]) {
            if (choosed_block[2].index[1] - choosed_block[2].index[2] == j_ib[1] - j_ib[2]) {
                check_space(bingong, ib, count);
                del[2] = 1;
                break;
            }
        }
        else {
            printf("블럭 다름");
        }

    case 2:

        if (choosed_block[3].index[0] - choosed_block[3].index[1] == j_ib[0] - j_ib[1]) {
            if (choosed_block[3].index[1] - choosed_block[3].index[2] == j_ib[1] - j_ib[2]) {
                printf("블럭 같음");
                check_space(bingong, ib, count);
                del[3] = 1;
                break;
            }
        }
        else {
            printf("블럭 다름");
        }

    default:
        printf("오답");
    }

}

void block_case4(int count, block* choosed_block, int ib[], int j_ib[], int bingong[5][5], int del[]) {
    switch (1) {
    case 1:


        if (choosed_block[4].index[0] - choosed_block[4].index[1] == j_ib[0] - j_ib[1]) {
            if (choosed_block[4].index[1] - choosed_block[4].index[2] == j_ib[1] - j_ib[2]) {
                if (choosed_block[4].index[2] - choosed_block[4].index[3] == j_ib[2] - j_ib[3]) {
                    printf("블럭 같음");
                    check_space(bingong, ib, count);
                    del[4] = 1;
                    break;
                }
            }
        }
        else {
            printf("블럭 다름");
        }

    case 2:


        if (choosed_block[5].index[0] - choosed_block[5].index[1] == j_ib[0] - j_ib[1]) {
            if (choosed_block[5].index[1] - choosed_block[5].index[2] == j_ib[1] - j_ib[2]) {
                if (choosed_block[5].index[2] - choosed_block[5].index[3] == j_ib[2] - j_ib[3]) {
                    printf("블럭 같음");
                    check_space(bingong, ib, count);
                    del[5] = 1;
                    break;
                }
            }
        }
        else {
            printf("블럭 다름");
        }

    default:
        printf("오답");
    }
}

void block_case5(int count, block* choosed_block, int ib[], int j_ib[], int bingong[5][5], int del[]) {
    switch (1) {
    case 1:


        if (choosed_block[6].index[0] - choosed_block[6].index[1] == j_ib[0] - j_ib[1]) {
            if (choosed_block[6].index[1] - choosed_block[6].index[2] == j_ib[1] - j_ib[2]) {
                if (choosed_block[6].index[2] - choosed_block[6].index[3] == j_ib[2] - j_ib[3]) {
                    if (choosed_block[6].index[3] - choosed_block[6].index[4] == j_ib[3] - j_ib[4]) {
                        printf("블럭 같음");
                        check_space(bingong, ib, count);
                        del[6] = 1;
                        break;
                    }
                }
            }
        }
        else {
            printf("블럭 다름");
        }

    case 2:


        if (choosed_block[7].index[0] - choosed_block[7].index[1] == j_ib[0] - j_ib[1]) {
            if (choosed_block[7].index[1] - choosed_block[7].index[2] == j_ib[1] - j_ib[2]) {
                if (choosed_block[7].index[2] - choosed_block[7].index[3] == j_ib[2] - j_ib[3]) {
                    if (choosed_block[7].index[3] - choosed_block[7].index[4] == j_ib[3] - j_ib[4]) {
                        printf("블럭 같음");
                        check_space(bingong, ib, count);
                        del[7] = 1;
                        break;
                    }
                }
            }
        }
        else {
            printf("블럭 다름");
        }

    default:
        printf("오답");
    }
}

void box_print(int bingong[5][5]) {
    gotoxy(30, 29);
    printf(".1.2.3.4.5");
    gotoxy(28, 30);
    printf("0.");
    gotoxy(28, 31);
    printf("1.");
    gotoxy(28, 32);
    printf("2.");
    gotoxy(28, 33);
    printf("3.");
    gotoxy(28, 34);
    printf("4.");
    P_kan = 0;
    for (int i = 0; i < 5; i++) {
        gotoxy(30, 30 + (i * 1));
        for (int j = 0; j < 5; j++) {
            if (bingong[i][j] == 1) {
                printf("■");
                P_kan++;
            }
            else printf("□");
        }
    }


    gotoxy(25, 27);
    printf("채운 칸/전체 칸  :  %d/25", P_kan);
}

void asdf(block* choosed_block, int* selected_block) {
    gotoxy(30, 29);
    printf(".1.2.3.4.5");
    gotoxy(28, 30);
    printf("0.");
    gotoxy(28, 31);
    printf("1.");
    gotoxy(28, 32);
    printf("2.");
    gotoxy(28, 33);
    printf("3.");
    gotoxy(28, 34);
    printf("4.");
    int bingong[5][5] = { 0 };
    int save_bingong[5][5] = { 0 };
    int count;
    int im;
    int del[8] = { 0 };

    int ib[5];
    int j_ib[5];



    while (1) { //플레이어 계속 입력받기~
        memcpy(save_bingong, bingong, sizeof(bingong));



        for (int i = 0; i < 5; i++) {//입력부

            gotoxy(3, 25);
            printf("99는 현재 블럭입력, 1000은 게임 마침, 111은 공간 초기화입니다.\n");
            gotoxy(30, 35);
            printf("입력 : ");
            scanf("%d", &ib[i]);
            if (ib[i] == 99) {//999라 치면 입력종료
                for (int j = i + 1; j < 5; j++) {
                    ib[j] = 99;
                }
                break;
            }
            else if (ib[i] == 1000) {
                return;
            }
            else if (ib[i] == 111) {
                for (int g = 0; g < 8; g++) {
                    del[g] = 0;
                }
                for (int h = 0; h < 5; h++) {
                    for (int e = 0; e < 5; e++) {
                        bingong[h][e] = 0;
                    }
                }
                break;
            }
        }

        //이 아래는 플레이어가 입력한 모양의 블럭이 있는지 봄.
        qsort(ib, 5, sizeof(int), compare);

        //해당모양 블럭이 있는지 판별은: 입력값 정렬. 모든 입력값으로부터 최솟값을 빼기. 값들의 차이값이 selec된 블럭 좌표값들의 차이값과 같으면 맞게 입력했다 판별.
        printf("정렬: ");
        for (int i = 0; i < 5; i++) {
            im = ib[i] - ib[0];
            j_ib[i] = im + 1;
            printf("ib: %d |", ib[i]);
            printf("j: %d |", j_ib[i]);
        }
        count = 0;
        for (int i = 0; i < 5; i++) {
            if (ib[i] < 50 && -50 < ib[i]) {
                count++;
            }
        }
        printf("%d", count);

        switch (count) {
        case 1:
            block_case1(count, choosed_block, ib, j_ib, bingong, del);
            break;
        case 2:
            block_case2(count, choosed_block, ib, j_ib, bingong, del);
            break;
        case 3:
            block_case3(count, choosed_block, ib, j_ib, bingong, del);
            break;
        case 4:
            block_case4(count, choosed_block, ib, j_ib, bingong, del);
            break;
        case 5:
            block_case5(count, choosed_block, ib, j_ib, bingong, del);
            break;
        default:
            printf("d");
        }

        //대충 여기다가 입력받기 전에 bingong이랑 스위치 지난 후에 빈공이랑 비교떠서 if !=이면 print_box돌려도 될듯
        if (save_bingong != bingong) {
            system("cls"); //초기화
            print_block(choosed_block, selected_block); //선별블럭
            for (int c = 0; c < 8; c++) {//쓴 블럭 가리기
                if (del[c] == 1) {
                    switch (c) {
                    case 0:
                        gotoxy(13, 10);
                        printf("   ");
                        //printf("%d %d %d %d %d %d %d %d", del[0], del[1], del[2], del[3], del[4], del[5], del[6], del[7]);
                        break;
                    case 1:
                        gotoxy(26, 10);
                        printf("    ");
                        gotoxy(26, 11);
                        printf("    ");
                        //printf("%d %d %d %d %d %d %d %d", del[0], del[1], del[2], del[3], del[4], del[5], del[6], del[7]);
                        break;
                    case 2:
                        gotoxy(39, 10);
                        printf("      ");
                        gotoxy(39, 11);
                        printf("      ");
                        gotoxy(39, 12);
                        printf("      ");
                        //printf("%d %d %d %d %d %d %d %d", del[0], del[1], del[2], del[3], del[4], del[5], del[6], del[7]);
                        break;
                    case 3:
                        gotoxy(52, 10);
                        printf("       ");
                        gotoxy(52, 11);
                        printf("       ");
                        gotoxy(52, 12);
                        printf("       ");
                        //printf("%d %d %d %d %d %d %d %d", del[0], del[1], del[2], del[3], del[4], del[5], del[6], del[7]);
                        break;
                    case 4:
                        gotoxy(13, 18);
                        printf("        ");
                        gotoxy(13, 19);
                        printf("        ");
                        gotoxy(13, 20);
                        printf("        ");
                        gotoxy(13, 21);
                        printf("        ");
                        //printf("%d %d %d %d %d %d %d %d", del[0], del[1], del[2], del[3], del[4], del[5], del[6], del[7]);
                        break;

                    case 5:
                        gotoxy(26, 18);
                        printf("          ");
                        gotoxy(26, 19);
                        printf("        ");
                        gotoxy(26, 20);
                        printf("        ");
                        gotoxy(26, 21);
                        printf("        ");
                        //printf("%d %d %d %d %d %d %d %d", del[0], del[1], del[2], del[3], del[4], del[5], del[6], del[7]);
                        break;

                    case 6:
                        gotoxy(39, 18);
                        printf("         ");
                        gotoxy(39, 19);
                        printf("       ");
                        gotoxy(39, 20);
                        printf("       ");
                        gotoxy(39, 21);
                        printf("       ");
                        gotoxy(39, 22);
                        printf("       ");
                        //printf("%d %d %d %d %d %d %d %d", del[0], del[1], del[2], del[3], del[4], del[5], del[6], del[7]);
                        break;
                    case 7:
                        gotoxy(52, 18);
                        printf("         ");
                        gotoxy(52, 19);
                        printf("       ");
                        gotoxy(52, 20);
                        printf("       ");
                        gotoxy(52, 21);
                        printf("       ");
                        gotoxy(52, 22);
                        printf("       ");
                        //printf("%d %d %d %d %d %d %d %d", del[0], del[1], del[2], del[3], del[4], del[5], del[6], del[7]);
                        break;
                    default:
                        break;
                    }
                }
            }
            box_print(bingong);
        }
    }
}

void view(block* choosed_block) {
    int box[10][max1][max1];
    int selected_block[10][max1][max1];
    // int i, k, j;
    int r1 = 0, r2 = 0;

    UIprint(box[r1][r2], choosed_block, selected_block[r1][r2]);

    asdf(choosed_block, selected_block[r1][r2]); //문제나면 이 r1 r2다..
}

void copy(int* a, int* b) {
    for (int i = 0; i < max1; i++) {
        a[i] = b[i];
    }
}

//index는 블럭을 놓을 기준 번호 
int can_i_put_this(int* box, block* b, int index) {
    int i, k;
    int temp[5];

    //블럭을 놓는 기준인 index에 따라 블럭 놓을 자리를 정함 
    if (1 <= index && index <= 5) i = index - b->index[0];
    else if (6 <= index && index <= 10) i = (index + 5) - b->index[0];
    else if (11 <= index && index <= 15) i = (index + 10) - b->index[0];
    else if (16 <= index && index <= 20) i = (index + 15) - b->index[0];
    else i = (index + 20) - b->index[0];

    for (k = 0; k < b->num; k++) temp[k] = b->index[k] + i;

    for (k = 0; k < b->num; k++) {
        //블럭을 놓았을 때 박스 밖으로 삐져나오면 
        if (!((1 <= temp[k] && temp[k] <= 5) || (11 <= temp[k] && temp[k] <= 15) || (21 <= temp[k] && temp[k] <= 25) || (31 <= temp[k] && temp[k] <= 35) || (41 <= temp[k] && temp[k] <= 45))) {
            return 0;
        }
        //이미 놓여져 있던 블럭이랑 겹침 
        else if (box[temp[k] - (temp[k] / 10) * 5] == 1) return 0;
    }

    return 1;
}

void put_block(int* box, block* b, int index) {
    int i, k;
    int temp[5];

    //블럭을 놓는 기준인 index에 따라 블럭 놓을 자리를 정함 
    if (1 <= index && index <= 5) i = index - b->index[0];
    else if (6 <= index && index <= 10) i = (index + 5) - b->index[0];
    else if (11 <= index && index <= 15) i = (index + 10) - b->index[0];
    else if (16 <= index && index <= 20) i = (index + 15) - b->index[0];
    else i = (index + 20) - b->index[0];

    for (k = 0; k < b->num; k++) temp[k] = (b->index[k] + i) - ((b->index[k] + i) / 10) * 5;

    for (k = 0; k < b->num; k++) {
        box[temp[k]] = 1;
    }
}

void Knapsack_block_game(block* choosed_block) {
    int box[10][max1][max1]; //박스 index에 블럭을 놓은 상태면 1, 안 놓은 상태면 -1 저장 
    int selected_block[10][max1][max1]; //각각 블럭을 박스에 놓은 상태면 1, 안 놓은 상태면 0 저장, index 0에는 넣은 블록의 총 칸 수 저장 
    int i, k, j;

    //배낭(박스) -1(박스 칸이 채워지지 않은 상태)로 초기화 
    for (i = 0; i < 10; i++) {
        for (k = 0; k < max1; k++) {
            for (j = 0; j < max1; j++) {
                box[i][k][j] = -1;
                selected_block[i][k][j] = 0;
            }
        }
    }

    for (i = 1; i <= 8; i++) {
        for (k = 1; k <= box_size; k++) {
            if (can_i_put_this(box[i - 1][k - 1], &choosed_block[i - 1], k) == 0) { //블럭 넣을 공간 없음 
                copy(box[i][k], box[i - 1][k - 1]);
                copy(selected_block[i][k], selected_block[i - 1][k - 1]);
            }
            else {
                //블럭 넣기 전의 블록 총 칸 수가 더 많음 
                if (selected_block[i - 1][k][0] > selected_block[i - 1][k - 1][0] + choosed_block[i - 1].num) {
                    copy(box[i][k], box[i - 1][k]);
                    copy(selected_block[i][k], selected_block[i - 1][k]);
                }
                //블럭 넣은 후의 블록 총 칸 수가 더 많음 
                else {
                    copy(box[i][k], box[i - 1][k - 1]);
                    put_block(box[i][k], &choosed_block[i - 1], k);
                    copy(selected_block[i][k], selected_block[i - 1][k - 1]);
                    selected_block[i][k][0] += choosed_block[i - 1].num;
                    selected_block[i][k][i] = 1;
                }
            }
            UIprint(box[i][k], choosed_block, selected_block[i][k]);
        }
    }

    int result = 0;
    int r1, r2;

    for (i = 1; i <= 8; i++) {
        for (k = 1; k <= box_size; k++) {
            if (result < selected_block[i][k][0]) {
                result = selected_block[i][k][0];
                r1 = i;
                r2 = k;
            }
        }
    }

    //가장 많은 칸을 담아둔 박스로 모두 초기화 
    for (i = 0; i < 10; i++) {
        for (k = 0; k < max1; k++) {
            copy(box[i][k], box[r1][r2]);
            copy(selected_block[i][k], selected_block[r1][r2]);
        }
    }

    for (i = 1; i <= 8; i++) {
        for (k = 1; k <= box_size; k++) {
            if (selected_block[i][k][i] == 1) continue;
            if (can_i_put_this(box[i - 1][k - 1], &choosed_block[i - 1], k) == 0) { //블럭 넣을 공간 없음 
                copy(box[i][k], box[i - 1][k - 1]);
                copy(selected_block[i][k], selected_block[i - 1][k - 1]);
            }
            else {
                //블럭 넣기 전의 블록 총 칸 수가 더 많음 
                if (selected_block[i - 1][k][0] > selected_block[i - 1][k - 1][0] + choosed_block[i - 1].num) {
                    copy(box[i][k], box[i - 1][k]);
                    copy(selected_block[i][k], selected_block[i - 1][k]);
                }
                //블럭 넣은 후의 블록 총 칸 수가 더 많음 
                else {
                    copy(box[i][k], box[i - 1][k - 1]);
                    put_block(box[i][k], &choosed_block[i - 1], k);
                    copy(selected_block[i][k], selected_block[i - 1][k - 1]);
                    selected_block[i][k][0] += choosed_block[i - 1].num;
                    selected_block[i][k][i] = 1;
                }
            }
            UIprint(box[i][k], choosed_block, selected_block[i][k]);
        }
    }

    for (i = 1; i <= 8; i++) {
        for (k = 1; k <= box_size; k++) {
            if (result < selected_block[i][k][0]) {
                result = selected_block[i][k][0];
                r1 = i;
                r2 = k;
            }
        }
    }

    //가장 많은 칸을 담아둔 박스로 모두 초기화 
    for (i = 0; i < 10; i++) {
        for (k = 0; k < max1; k++) {
            copy(box[i][k], box[r1][r2]);
            copy(selected_block[i][k], selected_block[r1][r2]);
        }
    }

    for (i = 1; i <= 8; i++) {
        for (k = 1; k <= box_size; k++) {
            if (selected_block[i][k][i] == 1) continue;
            if (can_i_put_this(box[i - 1][k - 1], &choosed_block[i - 1], k) == 0) { //블럭 넣을 공간 없음 
                copy(box[i][k], box[i - 1][k - 1]);
                copy(selected_block[i][k], selected_block[i - 1][k - 1]);
            }
            else {
                //블럭 넣기 전의 블록 총 칸 수가 더 많음 
                if (selected_block[i - 1][k][0] > selected_block[i - 1][k - 1][0] + choosed_block[i - 1].num) {
                    copy(box[i][k], box[i - 1][k]);
                    copy(selected_block[i][k], selected_block[i - 1][k]);
                }
                //블럭 넣은 후의 블록 총 칸 수가 더 많음 
                else {
                    copy(box[i][k], box[i - 1][k - 1]);
                    put_block(box[i][k], &choosed_block[i - 1], k);
                    copy(selected_block[i][k], selected_block[i - 1][k - 1]);
                    selected_block[i][k][0] += choosed_block[i - 1].num;
                    selected_block[i][k][i] = 1;
                }
            }
        }
        UIprint(box[i][k], choosed_block, selected_block[i][k]);
    }

    for (i = 1; i <= 8; i++) {
        for (k = 1; k <= box_size; k++) {
            if (result < selected_block[i][k][0]) {
                result = selected_block[i][k][0];
                r1 = i;
                r2 = k;
            }
        }
    }

    UIprint(box[r1][r2], choosed_block, selected_block[r1][r2]); //최종 결과 출력 
    gotoxy(40, 40);
    printf("%d 칸", selected_block[r1][r2][0]);
    C_kan = selected_block[r1][r2][0];


}


void sangdae_script() {
    system("cls");
    printf("                                                                                    BK   Bs   \n");
    printf("          di                                 2P       ...:..         ..             .B. iB     \n");
    printf("          .BBv                            .gM.  .2dEEqIJLvJIQ7       :BQ              EBi       \n");
    printf("           B DD                          PB: Zi .r.           g.       2B             .B        \n");
    printf("           B  Q5                        BS  BL                7B        BU            :B        \n");
    printf("           :Q.iB                       iB  .B                  B         B            MB         \n");
    printf("             XB                        iB   B                 7Q         Bi          BriB        \n");
    printf("              B:                       B:   B.            ..  gU .B      7B         :B  Bi       \n");
    printf("              :B                      BQi   gj.7YJ    .EdI1:  LB  1B. .XP2B.        iB  Bi          \n");
    printf("               B:                       X   7R ::.Dr           Bu .BgUBr  .B         BLrQ           \n");
    printf("              5BBKr                    iB   .B  BB:Q     BBr    B B. .    Mq          Bd            \n");
    printf("              B   QZ                   B7rQr B  S2 B     :7     :BS      qQ           B             \n");
    printf("              KQ   B                   rj::JRB    :B  Ur        .B      BB            B.            \n");
    printf("               BK UB                       .BB    B.  .B7        B.  Y7ZBJ            B             \n");
    printf("                rBr                       dBi1E   U                 rQ: B            PBq1           \n");
    printf("                 Pj                     JBB.  QY    .:::::rg.       QI JP           iB  Br          \n");
    printf("                  B                   .QBY     B5 BB7ir:i1Bg      rBuB Pu           Id  B:          \n");
    printf("                 uBBP:                BBJ       ZQ 717i72I.      MB  Qj B           7R sB           \n");
    printf("                 B   Q:               .QBPP:     iB   :.      :bB7    DKIQY          BKQ            \n");
    printf("                 B   KD                 :.         Ev      iSQq:       .iLBB:        RB             \n");
    printf("                 :B  B:                         ...rBBKqbddj:            YMJYP7.     JX             \n");
    printf("                  :PBP                     LE5EBjjrjB                iIMK:    vBZ    UZ            \n");
    printf("                     B                    BK  Br   .B           .i2dDIi      PS vB7  SX            \n");
    printf("                     iB                  Bi  B2     rMq5uu1SXbdESs:         :B    Br.Br            \n");
    printf("                     2BB                SR  QP         .:::..               B:     BB B            \n");
    printf("                    Q1 rQ               B: iB                              .B      iE ZU           \n");
    printf("                    SQ  gu              B  B:                              Sg      5u Iq           \n");
    printf("                     QK Xj             LQ  B                               Br    QLBI dr            \n");
    printf("                      .Dg.gPg          Q: 7B                               B:   B7 BB Bvi:          \n");
    printf("                    :271BB  PB        .B  7B                               B:   BUr5UL:..vQ1        \n");
    printf("                    B.   iP  B2       Bv   B                               uB   Bj:   .:r  Br       \n");
    printf("                   iQ1:PuL7dq B:     :B    Bi                               Br .Qi.J12vr:  KM       \n");
    printf("                   Q:      qi  Q    .B     iB                                Q:K2          B.      \n");
    printf("------------------------------------------------------------------------------------------------------------\n");
    printf("          처음뵙겠어요.\n");
    printf("          용케도 결승까지 올라오셨네요.\n");
    printf("          하지만 여기까지에요.\n");
    printf("          나, 사슬낫의 제니가 상대니까.\n");
    _getch();


}

void test() {
    textcolor(skyblue);

    block choosed_block[8];
    choose_block(choosed_block);

    view(choosed_block);//플레이어

    textcolor(LIGHTGRAY);
    sangdae_script();

    textcolor(skyblue); 
    Knapsack_block_game(choosed_block);//상대
    _getch();
    textcolor(LIGHTGRAY);
}

void find_map_script() {
    system("cls");
    printf("이젠 떠나야할 시간 입니다. \n");
    printf("방문하고 싶은 보호구역을 선택하세요. \n");
    printf("현재 위치에서 보호구역까지 갈 수 있는 경로는 여러가지입니다. \n");
    printf("단, 현재 위치부터 보호구역까지 최대한 빨리, 최단 경로로 이동해야 합니다. \n");
    printf("까마귀 도적단의 습격을 받을지도 모르니까요. \n");
    printf("그들은 재빠르고 영악합니다. 이미 제국 내의 모든 경로에 대해 잘 알고 있으니  \n");
    printf("조금이라도 길을 돌아가게 되면 도적단에게 골드를 강탈당하게 될겁니다. \n");
    printf("당신의 선택에 따라 무사히 보호구역까지 도착할 수도, 도적단을 맞닥뜨릴 수도 있습니다. \n");
    printf("그럼...무운을 빕니다! \n");
    printf("ps.입력값은 알파벳 대문자로 입력하세요 \n");
}

void start_script() {
    printf("상단 대표: 어서와, [플레이어 이름]. 들어와서 앉아. 너에게 이야기가 있어.\n");
    _getch();
    printf("플레이어: 무슨 일이에요?\n");
    _getch();
    printf("상단 대표: 최근에 우리 상단이 큰 문제에 휘말렸어. 까마귀들이 무역길을 터주고 상인들의 물자를 약탈하고 있어. 이렇게 계속되면 우리 상단은 큰 타격을 입을 거야.\n");
    _getch();
    printf("플레이어: 그래서 어떻게 도와드릴 수 있나요?\n");
    _getch();
    printf("상단 대표: 발레노스 제국으로 물자를 운반해야 해. 하지만 길에는 까마귀들이 기다리고 있을 거야. 이 임무를 성공적으로 완료하면, 네게는 훌륭한 보상이 기다린다. 네 자유를 존중하지만, 이번에는 모든 손을 모아 우리를 도와줬으면 하는 마음이야.\n");
    _getch();
    printf("플레이어: 이해했습니다. 어떻게 출발하면 좋을까요?\n");
    _getch();
    printf("상단 대표: 네가 무사히 발레노스 제국에 도착하면, 그 수익금의 절반을 네게 돌려줄게. 이건 네 미래에 큰 도움이 될 거야. 정말로 감사하다, [플레이어 이름]. 이 세계가 더 어두워지기 전에 물자를 안전하게 운반해다오.\n");
    _getch();
    printf("플레이어: 알겠습니다. 최선을 다하겠습니다.\n");
    _getch();
}

void good_script() {
    printf("상단에서 주어진 강수를 통해 얻은 수익금의 절반을 받았습니다. 이제 여러분은 부유한 삶을 시작할 수 있습니다.\n");
    printf("발레노스 제국에서의 성공은 여러 길을 여러분에게 제시했습니다.\n");
    printf("여러분의 훌륭한 상인 기질로 발레노스 제국의 상위 계층에 오르는 것 또한 가능할 것입니다.\n");
    printf("또는 더 큰 무역 여정을 꿈꾸어 새로운 대륙으로 나아가 볼 수도 있습니다.\n");
    printf("여러분이 선택하는 길은 자유롭습니다. 새로운 모험을 기대하세요!\n");
}

void soso_script() {
    printf("상단에서 주어진 강수를 통해 수익금을 얻었지만, 아쉽게도 그 양은 크지 않았습니다.\n");
    printf("그래도 여러분은 무역 여정을 무사히 마치고 발레노스 제국에 도착했습니다.\n");
    printf("다음에는 더 나은 결과를 얻기 위해 더 많은 경험과 노력을 기울여 보세요.\n");
    printf("새로운 도전을 향해 나아가기를 기대합니다!\n");
}

void notbad_script() {
    printf("상단에서 주어진 강수를 통해 수익금을 얻었고, 여러분은 현재 상당한 소지금을 가지고 있습니다.\n");
    printf("이번 무역에서 얻은 수익을 기반으로 더 많은 투자와 사업을 고려해보세요.\n");
    printf("여러분의 능력과 재무 상태에 따라 더 큰 성장을 이룰 수 있을 것입니다.\n");
    printf("발레노스 제국에서는 더 큰 기회와 도전이 여러분을 기다리고 있습니다.\n");
}

void bad_script() {
    printf("상단에서 주어진 강수를 통해 수익금을 얻었지만, 까마귀 도적들에게 모든 소지금을 약탈당했습니다.\n");
    printf("현재 소지금은 0원이며, 무역 여정은 큰 실패로 끝났습니다.\n");
    printf("그러나 여전히 플레이어는 발레노스 제국에 도착했습니다.\n");
    printf("당신은 다시 일어나 새로운 시작을 할 수 있습니다. 소지금은 떨어졌지만, 기회는 여전히 기다리고 있습니다.\n");
    printf("무엇이든 도전해보고자 하는 용기가 있다면, 새로운 여정에 나서보세요.\n");
}

void gamehamsu() {

    start_script();
    while (end != 'L') {
        cargoRound();
        find_map_script();
        _getch();
        find_map();
        _getch();
        test();
        system("cls");
        if (C_kan > P_kan) {
            printf("크윽...졌다...\n");
            _getch();
        }
        else {
            printf("아싸~ 이겼다~!\n700G 상당의 상품을 얻었다~\n");

            GOLD1 = GOLD1 + 700;
            _getch();
        }
        system("cls");
    }
    if (GOLD1 > 3000) {
        system("cls");
        good_script();
    }
    else if (GOLD1 > 2000) {
        system("cls");
        notbad_script();
    }
    else if (GOLD1 > 1000) {
        system("cls");
        soso_script();
    }
    else {
        system("cls");
        bad_script();
    }
    _getch();

}

void title() {
    printf("\n\n\n\n\n\n");
    printf("                               u                             S                                                 \n");
    printf("                               Q2                           QX                              vB                   \n");
    printf("                   YZqPqKKQLXSS1BqXXSuQIqKPqZr            YBBsPbZbZdEP5qB                   vB                   \n");
    printf("                   ..... .B1         BQ  . ..           1BJ            BP                   7Q                   \n");
    printf("                           B        :B               7QQv            :Bi                    IX                   \n");
    printf("                     .rrrr.BBir7r7ijBr:rrri        DBS.  :QP:      .BB                      BB                   \n");
    printf("                     B1irri.:.rrrr7:.:irriQB               :dRr  :QB:                       BB                   \n");
    printf("                     B       B    :Z.     :B                  QBRE.                        52 B                  \n");
    printf("                     Q     gB:      ZQi   rB               .vI7.                           B  BY                 \n");
    printf("                     B  :QB:          gB. rB          :75BBBQ5rv155KXqSSPi                Qr   B.                \n");
    printf("                     B iQ:  vvYsjYYvv  .B iB      KERDPr:B    . ....... bB               B2    .B.               \n");
    printf("                     B     BK:iiii::QQ    rB      i.     B              rB              QU      .B:              \n");
    printf("                     B     B.       vD    rB             B              vQ            vBi         BX             \n");
    printf("                     B     BY  . .  MB    rB             B              iB          .BB            bB:           \n");
    printf("                     B.    rPIXXqSK2d:    rB             B.:i:iiiiiiiii.EB        .MBi              .BQ.         \n");
    printf("                     B.                   5B             B7JuUu1u1j11UJvBB      iBBr                  :BRr       \n");
    printf("                     M                sMZdB.             1              .v     7B:                      .MY      \n");
    printf("\n\n\n");
    printf("                                               >시작을 위해 버-튼을 누르시오.<");
    _getch();
    system("cls");

}

int main() {
    title();
    gamehamsu();
}
