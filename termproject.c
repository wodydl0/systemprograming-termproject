#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
int size;
int **puzzle;
int key_count= 0;
int key;
int success_key;

void make_outline() {
    for (int i = 0; i < COLS; i++) {
        mvprintw(0, i, "-");
        mvprintw(LINES - 1, i, "-");
    }

    for (int i = 1; i < LINES - 1; i++) {
        mvprintw(i, 0, "|");
        mvprintw(i, COLS - 1, "|");
    }
}
void main_page() {
	clear();
    make_outline();

    char* menu[] = {
        "1. game start",
        "2. menual",
        "3. exit"
    };
    int start_row = 0;
    int start_col = 0;
    start_row = (LINES - 1 - sizeof(menu) / sizeof(menu[0])) / 2;
    for (int i = 0; i < sizeof(menu) / sizeof(menu[0]); ++i) {
        start_col = (COLS - strlen(menu[i])) / 2;
        mvprintw(start_row + i, start_col, menu[i]);
    }
    mvprintw(7,COLS/2-10,"Sliding puzzle game");
    refresh();
}

void init_puzzle(int size) {
    puzzle = (int**)malloc(size * sizeof(int*));
    for (int i = 0; i < size; ++i) {
        puzzle[i] = (int*)malloc(size * sizeof(int));
    }
    // 랜덤 시드 설정
    srand(time(NULL));

    // 1에서 size * size - 1까지의 숫자를 배열에 할당
    int* numbers = (int*)malloc((size * size - 1) * sizeof(int));
    for (int i = 0; i < size * size - 1; ++i) {
        numbers[i] = i + 1;
    }

    // Fisher-Yates 셔플 알고리즘을 사용하여 배열 섞기
    for (int i = size * size - 2; i > 0; --i) {
        int j = rand() % (i + 1);
        // 숫자 교환
        int temp = numbers[i];
        numbers[i] = numbers[j];
        numbers[j] = temp;
    }

    // 배열의 값들을 퍼즐에 할당
    int count = 0;
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            puzzle[i][j] = numbers[count++];
        }
    }
    puzzle[size - 1][size - 1] = 0;
    free(numbers);
}

void manual_page() {
    clear();
    make_outline();

    mvprintw(3, COLS - 25, "Press 'b' to go back");
    mvprintw(LINES / 2 - 7, COLS / 2 - 12, "Sliding Puzzle Game Manual");
    mvprintw(LINES / 2 - 2, COLS / 2 - 19, "2. Use arrow keys to move empty block(0)");
    mvprintw(LINES / 2 - 4, COLS / 2 - 19, "1. Select puzzle size(n)");
    mvprintw(LINES / 2, COLS / 2 - 19, "3. You need to arrange the puzzle in ascending order");
    mvprintw(LINES / 2 + 2, COLS / 2 - 19, "4. The empty block should be located at the last");
    mvprintw(LINES / 2 + 4, COLS / 2 - 19, "5. By doing so, you win! Good luck.");

    refresh();
}

void draw_size_input_page() {
    echo();
    clear();
    make_outline();
    mvprintw(LINES/2-1, (COLS-9)/2, "Enter puzzle size : ");
    refresh();
    scanw("%d", &size);
    init_puzzle(size);
    noecho();
}


void draw_puzzle() {
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            mvprintw(i * 2, j * 5, "-----");
            mvprintw(i * 2 + 1, j * 5, "|%3d|", puzzle[i][j]);
            if (i == size - 1) mvprintw(i * 2 + 2, j * 5, "-----");
        }
    }
    //key_count 값 구석에 출력
    mvprintw(LINES - 3, COLS - 20, "Key Count: %d", key_count);
    mvprintw(3, COLS - 20, "Press 'q' to quit");
    refresh();
}

void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void move_empty(int key) {
    int empty_i, empty_j;

    // 빈 칸의 위치 찾기
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if (puzzle[i][j] == 0) {
                empty_i = i;
                empty_j = j;
            }
        }
    }

    // 사용자 입력에 따라 빈 칸 이동
    switch (key) {
    case KEY_UP:
        if (empty_i < size - 1) {
            key_count++;
            swap(&puzzle[empty_i][empty_j], &puzzle[empty_i + 1][empty_j]);
        }
        break;
    case KEY_DOWN:
        if (empty_i > 0) {
            key_count++;
            swap(&puzzle[empty_i][empty_j], &puzzle[empty_i - 1][empty_j]);
        }
        break;
    case KEY_LEFT:
        if (empty_j < size- 1) {
            key_count++;
            swap(&puzzle[empty_i][empty_j], &puzzle[empty_i][empty_j + 1]);
        }
        break;
    case KEY_RIGHT:
        if (empty_j > 0) {
            key_count++;
            swap(&puzzle[empty_i][empty_j], &puzzle[empty_i][empty_j - 1]);
        }
        break;
    }
}

int success() {
    int number = 1;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (puzzle[i][j] != number++) {
                if (i == size - 1 && j == size - 1 && puzzle[i][j] == 0) return 0;
                return 1;
            }
        }
    }
    return 0;
}

void success_page() {
    clear();
    make_outline();
    mvprintw(LINES / 2 - 5, COLS / 2 - 6, "Success!");
    mvprintw(LINES / 2 - 4, COLS / 2 - 7, "Record : %d", key_count);
    mvprintw(LINES / 2, COLS / 2 - 9, "Press r to restart.");
    mvprintw(LINES / 2 + 1, COLS / 2 - 9, "Press h to go Home.");
    mvprintw(LINES / 2 + 2, COLS / 2 - 9, "Press e to exit game.");
    refresh();
}
void fail_page() {
    clear();
    make_outline();
    mvprintw(LINES / 2 - 5, COLS / 2 - 6, "Fail!");
    mvprintw(LINES / 2 - 4, COLS / 2 - 7, "Record : %d", key_count);
    mvprintw(LINES / 2, COLS / 2 - 9, "Press r to restart.");
    mvprintw(LINES / 2 + 1, COLS / 2 - 9, "Press h to go Home.");
    mvprintw(LINES / 2 + 2, COLS / 2 - 9, "Press e to exit game.");
    refresh();
}
void free_puzzle() {
    if (puzzle != NULL) {
        for (int i = 0; i < size; ++i) {
            free(puzzle[i]);
        }
        free(puzzle);
    }
}

void play_game() {
    draw_size_input_page();
    clear();
    while (success())
    {
        draw_puzzle();
        key = getch();
        move_empty(key);
        if (key == 'q') break;
    }
    sleep(1);
    if (key == 'q') fail_page();
    else success_page();
    key_count = 0;
}

int main() {
    initscr();
    curs_set(0);
    keypad(stdscr, TRUE);
    noecho();

    int menu_select;
    int status;
    while (1) {
        main_page();
        menu_select = getch();
        if (menu_select == '1') {
            pid_t pid = fork(); 
            if (pid == 0) {
                play_game();
                while (1) {
                    success_key = getch();
                    if (success_key == 'r') {
                        clear();
                        make_outline();
                        mvprintw(LINES / 2, COLS / 2 - 5, "Restarting...");
                        refresh();
                        sleep(2); // 잠시 메시지 표시 후 재시작
                        // 기존 퍼즐 해제
                        free_puzzle();
                        play_game();
                        continue;  // 다시 while 루프로 진입
                    }
                    else if (success_key == 'h') {
                        clear();
                        make_outline();
                        mvprintw(LINES / 2, COLS / 2 - 5, "Going Home...");
                        refresh();
                        sleep(2); // 잠시 메시지 표시 후 Home으로 돌아가기
			            break;
                    }
                    else if (success_key == 'e') {
                        clear();
                        make_outline();
                        mvprintw(LINES / 2, COLS / 2 - 5, "Exiting Game...");
                        refresh();
                        sleep(2);
			            free_puzzle();
			            kill(getppid(), SIGINT);
			            exit(0);
		            }
                }
                exit(0);
            }
            else if (pid > 0) {
                // 부모 프로세스: 메인 페이지 계속 진행
		waitpid(pid,&status,0);

		continue;
            }
            else {
                // fork 실패
                printw("Error creating child process\n");
                refresh();
            }
        }
        else if (menu_select == '2') {
		manual_page();
		while ( (key = getch()) != 'b') ;

		continue;
	}
        else if (menu_select == '3') {
            free_puzzle();
            break; // 프로그램 종료
        }
    }

    endwin();
    return 0;
}


