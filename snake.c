
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#define FOOD '*'
#define SPAWN_SIZE 2
#define SNAKE '@'
#define MAX_NODES 500

#define clear() printf("\033c");
#define hide_cursor() printf("\033[?25l");
#define show_cursor() printf("\033[?25h");

enum Direction {
	UP,
	DOWN,
	RIGHT,
	LEFT,
};

struct winsize ws;
struct termios o_term;
char** board;

struct Vec2 {
	size_t x;
	size_t y;
};

struct Snake {
	struct Vec2* head;
	enum Direction dir;
	size_t size;
	unsigned speed;
};

unsigned getrand(size_t lim) { return rand() % lim; }

void render(unsigned width, unsigned height) {
	clear();
	for (size_t i = 0; i < height; i++) {
		for (size_t j = 0; j < width; j++) {
			switch (board[i][j]) {
			case SNAKE: {
				printf("%c", SNAKE);
				break;
			}
			case FOOD: {
				printf("%c", FOOD);
				break;
			}
			default:
				printf(".");
				break;
			}
		}
		printf("\n");
	}
}

void move(struct Snake* snake, char c) {
	switch (c) {
	case 'w': {
		switch (snake->dir) {
		case LEFT:
			board[snake->head->y][snake->head->x + snake->size] = 0;
			break;
		case RIGHT:
			board[snake->head->y][snake->head->x - snake->size] = 0;
			break;
		case DOWN:
			board[snake->head->y - snake->size][snake->head->x] = 0;
			break;
		case UP:
			board[snake->head->y + snake->size][snake->head->x] = 0;
			break;
		}
		snake->head->y -= snake->speed;
		board[snake->head->y][snake->head->x] = SNAKE;
		snake->dir = UP;
	}
	case 'd': {
		switch (snake->dir) {
		case LEFT:
			board[snake->head->y][snake->head->x + snake->size] = 0;
			break;
		case RIGHT:
			board[snake->head->y][snake->head->x - snake->size] = 0;
			break;
		case DOWN:
			board[snake->head->y - snake->size][snake->head->x] = 0;
			break;
		case UP:
			board[snake->head->y + snake->size][snake->head->x] = 0;
			break;
		}
		snake->head->x += snake->speed;
		board[snake->head->y][snake->head->x] = SNAKE;
		snake->dir = RIGHT;
	}
	case 's': {
		switch (snake->dir) {
		case LEFT:
			board[snake->head->y][snake->head->x + snake->size] = 0;
			break;
		case RIGHT:
			board[snake->head->y][snake->head->x - snake->size] = 0;
			break;
		case DOWN:
			board[snake->head->y - snake->size][snake->head->x] = 0;
			break;
		case UP:
			board[snake->head->y + snake->size][snake->head->x] = 0;
			break;
		}
		snake->head->y += snake->speed;
		board[snake->head->y][snake->head->x] = SNAKE;
		snake->dir = DOWN;
	}
	case 'a': {
		switch (snake->dir) {
		case LEFT:
			board[snake->head->y][snake->head->x + snake->size] = 0;
			break;
		case RIGHT:
			board[snake->head->y][snake->head->x - snake->size] = 0;
			break;
		case DOWN:
			board[snake->head->y - snake->size][snake->head->x] = 0;
			break;
		case UP:
			board[snake->head->y + snake->size][snake->head->x] = 0;
			break;
		}
		snake->head->x -= snake->speed;
		board[snake->head->y][snake->head->x] = SNAKE;
		snake->dir = LEFT;
	}
	}
}

struct winsize get_term_size() {
	struct winsize ws;
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1) {
		perror("ERROR:");
		exit(1);
	}
	return ws;
}

void gen_food(struct Vec2* food_head, unsigned width, unsigned height) {
	unsigned x = getrand(width);
	unsigned y = getrand(height);
	board[y][x] = FOOD;
	food_head->x = x;
	food_head->y = y;
}

bool is_eat(struct Snake* snake, struct Vec2* food_head) {
	if (snake->head->x == food_head->x && snake->head->y == food_head->y) {
		return true;
	}
	return false;
}

enum Direction get_dir() {
	unsigned int index = rand() % 4;
	switch (index) {
	case 0: {
		return UP;
		break;
	}
	case 1: {
		return LEFT;
		break;
	}
	case 2: {
		return RIGHT;
	}
	case 3: {
		return DOWN;
		break;
	}
	}
	return LEFT;
}

void term_init() {
	struct termios term;
	tcgetattr(STDIN_FILENO, &o_term);
	term = o_term;
	// Set new terminal attributes
	term.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echoing
	term.c_cc[VMIN] = 1;			  // Minimum number of characters to read
	term.c_cc[VTIME] = 1;			  // Timeout (in tenths of a second)
	tcsetattr(STDIN_FILENO, 0, &term);
}

void term_reset() { tcsetattr(STDIN_FILENO, 0, &o_term); }

int main() {
	term_init();
	hide_cursor();
	ws = get_term_size();
	srand(time(NULL)); // initializing the random seed
	// unsigned pad = 20;
	unsigned height = ws.ws_row - 7;
	unsigned width = ws.ws_col - 10;
	board = (char**)malloc(sizeof(char*) * height);
	if (board == NULL) {
		perror("ERROR:");
		exit(1);
	}

	// allocating memory for the board;
	for (size_t i = 0; i < height; i++) {
		board[i] = malloc(sizeof(char) * width);
		if (board[i] == NULL) {
			perror("ERROR");
			exit(1);
		}
	}

	// get the random starting headition
	struct Vec2 head = {
		.x = getrand(width),
		.y = getrand(height),
	};
	board[head.y][head.x] = SNAKE;

	// initializing the sanke
	struct Snake snake = {
		.head = &head,
		.size = 0,
		.dir = get_dir(),
		.speed = 1,
	};
	// generating the food headition randomly
	struct Vec2 food_head = {0};
	gen_food(&food_head, width, height);
	// non-blocking I/O
	int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
	if (flags == -1) {
		perror("fcntl");
		exit(EXIT_FAILURE);
	}
	if (fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK) == -1) {
		perror("fcntl");
		exit(EXIT_FAILURE);
	}

	while (1) {
		render(width, height);
		char c;
		ssize_t ret = read(STDIN_FILENO, &c, 1);
		if (ret == -1) { // there is no input
			usleep(50000);
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				switch (snake.dir) {
				case UP: {
					board[(snake.head->y) + snake.size][snake.head->x] = 0;
					snake.head->y -= snake.speed;
					board[snake.head->y][snake.head->x] = SNAKE;
					break;
				}
				case RIGHT: {
					board[snake.head->y][(snake.head->x) - snake.size] = 0;
					snake.head->x += snake.speed;
					board[snake.head->y][snake.head->x] = SNAKE;
				}
				case DOWN: {
					board[( snake.head->y ) - snake.size][snake.head->x] = 0;
					snake.head->y += snake.speed;
					board[snake.head->y][snake.head->x] = SNAKE;
				}
				case LEFT: {
					board[snake.head->y][(snake.head->x) + snake.size] = 0;
					snake.head->x -= snake.speed;
					board[snake.head->y][snake.head->x] = SNAKE;
				}
				}
			} else {
				perror("read");
				exit(EXIT_FAILURE);
			}
		} else if (ret == 1) {
			move(&snake, c);
		}
	}
	show_cursor();
	term_reset();
	return 0;
}
