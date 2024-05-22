
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
#define MAX_NODES 200

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
char** board;

struct Vec2 {
	size_t x;
	size_t y;
};

struct Snake {
	struct Vec2* nodes[MAX_NODES];
	enum Direction dir;
	struct Vec2* pos;
	size_t nodes_count;
	unsigned speed;
	char shape;
};

void render_board(unsigned width, unsigned height) {
	clear();
	char vert = '|';
	char hor = '-';
	for (size_t i = 0; i <= width; i++) {
		printf("%c", hor);
	}
	printf("\n");
	size_t j = 0;
	while (j < height) {
		for (size_t i = 0; i < width; i++) {
			(i == 0) ? printf("%c", vert) : printf(" ");
			if (i == width - 1) {
				printf("%c", vert);
				printf("\n");
			}
		}
		j++;
	}
	for (size_t i = 0; i <= width; i++) {
		printf("%c", hor);
	}
	printf("\n");
}

unsigned get_pos(size_t lim) { return rand() % lim; }

void render(unsigned width, unsigned height) {
	clear();
	hide_cursor();
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
	usleep(5400);
}

void move(struct Snake* snake, char c) {
	switch (c) {
	case 'w': {
		board[snake->pos->y][snake->pos->x] = 0;
		// snake->pos->y -= snake->speed;
		if (snake->dir != UP) {
			if (snake->dir == RIGHT) {
				for (size_t i = 1; i < snake->nodes_count; i++) {
					snake->nodes[i]->x += snake->speed;
					board[snake->nodes[i]->y][snake->nodes[i]->x] = SNAKE;
				}
			} else if (snake->dir == LEFT) {
				for (size_t i = 1; i < snake->nodes_count; i++) {
					snake->nodes[i]->x -= snake->speed;
					board[snake->nodes[i]->y][snake->nodes[i]->x] = SNAKE;
				}
			} else {
				for (size_t i = 1; i < snake->nodes_count; i++) {
					snake->nodes[i]->y -= snake->speed;
					board[snake->nodes[i]->y][snake->nodes[i]->x] = SNAKE;
				}
			}
		}
		board[snake->pos->y][snake->pos->x] = SNAKE;
		snake->dir = UP;
		break;
	}
	case 'd': {
		board[snake->pos->y][snake->pos->x] = 0;
		// snake->pos->x += snake->speed;
		if (snake->dir != RIGHT) {
			if (snake->dir == UP) {
				for (size_t i = 1; i < snake->nodes_count; i++) {
					snake->nodes[i]->y -= snake->speed;
					board[snake->nodes[i]->y][snake->nodes[i]->x] = SNAKE;
				}
			} else if (snake->dir == LEFT) {
				for (size_t i = 1; i < snake->nodes_count; i++) {
					snake->nodes[i]->x += snake->speed;
					board[snake->nodes[i]->y][snake->nodes[i]->x] = SNAKE;
				}
			} else {
				for (size_t i = 1; i < snake->nodes_count; i++) {
					snake->nodes[i]->y -= snake->speed;
					board[snake->nodes[i]->y][snake->nodes[i]->x] = SNAKE;
				}
			}
		}
		snake->dir = RIGHT;
		board[snake->pos->y][snake->pos->x] = SNAKE;
		break;
	}
	case 's': {
		board[snake->pos->y][snake->pos->x] = 0;
		// snake->pos->y += snake->speed;
		if (snake->dir != DOWN) {
			if (snake->dir == RIGHT) {
				for (size_t i = 1; i < snake->nodes_count; i++) {
					snake->nodes[i]->x += snake->speed;
					board[snake->nodes[i]->y][snake->nodes[i]->x] = SNAKE;
				}
			} else if (snake->dir == LEFT) {
				for (size_t i = 1; i < snake->nodes_count; i++) {
					snake->nodes[i]->x -= snake->speed;
					board[snake->nodes[i]->y][snake->nodes[i]->x] = SNAKE;
				}
			} else {
				for (size_t i = 1; i < snake->nodes_count; i++) {
					snake->nodes[i]->y += snake->speed;
					board[snake->nodes[i]->y][snake->nodes[i]->x] = SNAKE;
				}
			}
		}
		snake->dir = DOWN;
		board[snake->pos->y][snake->pos->x] = SNAKE;
		break;
	}
	case 'a': {
		board[snake->pos->y][snake->pos->x] = 0;
		// snake->pos->x -= snake->speed;
		if (snake->dir != LEFT) {
			if (snake->dir == RIGHT) {
				for (size_t i = 1; i < snake->nodes_count; i++) {
					snake->nodes[i]->x -= snake->speed;
					board[snake->nodes[i]->y][snake->nodes[i]->x] = SNAKE;
				}
			} else if (snake->dir == UP) {
				for (size_t i = 1; i < snake->nodes_count; i++) {
					snake->nodes[i]->y -= snake->speed;
					board[snake->nodes[i]->y][snake->nodes[i]->x] = SNAKE;
				}
			} else {
				for (size_t i = 1; i < snake->nodes_count; i++) {
					snake->nodes[i]->y += snake->speed;
					board[snake->nodes[i]->y][snake->nodes[i]->x] = SNAKE;
				}
			}
		}
		snake->dir = LEFT;
		board[snake->pos->y][snake->pos->x] = SNAKE;
		break;
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

void gen_food(struct Vec2* food_pos, unsigned width, unsigned height) {
	unsigned x = get_pos(width);
	unsigned y = get_pos(height);
	board[y][x] = FOOD;
	food_pos->x = x;
	food_pos->y = y;
}

bool is_eat(struct Snake* snake, struct Vec2* food_pos) {
	if (snake->pos->x == food_pos->x && snake->pos->y == food_pos->y) {
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
	ws = get_term_size();
	struct termios term;
	tcgetattr(STDIN_FILENO, &term);
	// Set new terminal attributes
	term.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echoing
	term.c_cc[VMIN] = 1;			  // Minimum number of characters to read
	term.c_cc[VTIME] = 0;			  // Timeout (in tenths of a second)
	tcsetattr(STDIN_FILENO, 0, &term);
}

int main() {
	term_init();
	srand(time(NULL));
	unsigned pad = 20;
	unsigned height = (ws.ws_row - 7);
	unsigned width = ws.ws_col - pad;
	board = (char**)malloc(sizeof(char*) * height);
	if (board == NULL) {
		perror("ERROR:");
		exit(1);
	}

	for (size_t i = 0; i < height; i++) {
		board[i] = malloc(sizeof(char) * width);
		if (board[i] == NULL) {
			perror("ERROR");
			exit(1);
		}
	}

	struct Vec2 pos = {
		.x = get_pos(width),
		.y = get_pos(height),
	};
	struct Snake snake = {
		.nodes = {0},
		.pos = &pos,
		.nodes_count = 1,
		.shape = SNAKE,
		.dir = get_dir(),
		.speed = 1,
	};
	/* snake.nodes = malloc(sizeof(struct Vec2) * MAX_NODES); */
	/* if (snake.nodes == NULL){ */
	/* 	perror("ERORR"); */
	/* 	exit(1); */
	/* } */
	board[pos.y][pos.x] = SNAKE;
	struct Vec2 food_pos = {0};
	gen_food(&food_pos, width, height);
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
		if (ret == -1) {
			usleep(50000);
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				if (is_eat(&snake, &food_pos)) {
					gen_food(&food_pos, width, height);
					board[food_pos.y][food_pos.x] = FOOD;
				}
				switch (snake.dir) {
				case UP: {
					board[snake.pos->y][snake.pos->x] = 0;
					snake.pos->y -= snake.speed;
					board[snake.pos->y][snake.pos->x] = SNAKE;
					break;
				}
				case RIGHT: {
					board[snake.pos->y][snake.pos->x] = 0;
					snake.pos->x += snake.speed;
					board[snake.pos->y][snake.pos->x] = SNAKE;
					break;
				}
				case DOWN: {
					board[snake.pos->y][snake.pos->x] = 0;
					snake.pos->y += snake.speed;
					board[snake.pos->y][snake.pos->x] = SNAKE;
					break;
				}
				case LEFT: {
					board[snake.pos->y][snake.pos->x] = 0;
					snake.pos->x -= snake.speed;
					board[snake.pos->y][snake.pos->x] = SNAKE;
					break;
				}
				}
			} else {
				perror("read");
				exit(EXIT_FAILURE);
			}
		} else if (ret == 1) {
			move(&snake, c);
			int is_eaten = 1;
			for (size_t i = 0; i < height; i++) {
				for (size_t j = 0; j < width; j++) {
					if (board[i][j] == FOOD) {
						is_eaten = 0;
						break;
					}
					if (is_eaten == 0) {
						break;
					}
				}
			}
			if (is_eaten) {
				struct Vec2 node_pos = {
					.x = food_pos.x,
					.y = food_pos.y,
				};
				board[node_pos.y][node_pos.x] = SNAKE;
				gen_food(&food_pos, width, height);
				board[food_pos.y][food_pos.x] = FOOD;
				struct Vec2* node = {0};
				node = &node_pos;
				snake.nodes[snake.nodes_count - 1] = node;
				snake.nodes_count++;
			}
		}
	}
	// tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);

	show_cursor();
	return 0;
}
