#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define MAX_NODES 500
#define SPEED 1
#define FOOD 'O'
//#define SNAKE "\u25A0"
#define SNAKE "E"

#define clear() printf("\033c");
#define hide_cursor() printf("\33[?25l")
#define show_cursor() printf("\33[?25h")
#define MOVE_TO(y,x) printf("\033[%d;%dH",y,x)

enum Direction{
	UP,
	DOWN,
	RIGHT,
	LEFT,
};

struct winsize ws;
struct termios o_term;

struct Vec2 {
	unsigned int x;
	unsigned int y;
};

struct Vec2 food = {0};

struct Snake {
	struct Vec2 nodes[MAX_NODES];
	enum Direction dir;
	unsigned int count;
};

struct Snake snake = {0};

void term_init() {

	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1){
		perror("Error:");
		exit(1);
	};
	struct termios term;
	tcgetattr(STDIN_FILENO, &o_term);
	term = o_term;
	// Set new terminal attributes
	term.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echoing
	term.c_cc[VMIN] = 1;			  // Minimum number of characters to read
	term.c_cc[VTIME] = 1;			  // Timeout (in tenths of a second)
	tcsetattr(STDIN_FILENO, 0, &term);
	setvbuf(stdout, NULL, _IONBF, 0);
}

void term_reset() { tcsetattr(STDIN_FILENO, 0, &o_term),show_cursor(); }

void draw_text(unsigned y, unsigned x,char* s){
	MOVE_TO(y,x);
	printf("%s",s);

}

void draw_char(unsigned y, unsigned x, char c){
	MOVE_TO(y,x);
	putchar(c);
}

void render_arena(size_t width,size_t height){
	for (size_t i = 1 ;i <= height;++i) {
		for (size_t j = 1;j <= width;++j) {
			if (i == 1 || i == height  || j == 1 || j == width){
				putchar('#');
			}
			else {
				putchar(' ');
			}
		}
		putchar('\n');
	}
}

void move_snake(){
	struct Vec2 clear_node = snake.nodes[snake.count-1];
	draw_char(clear_node.y,clear_node.x,' '); //clear a character
	for (unsigned i = snake.count - 1;i > 0;--i){
		snake.nodes[i]	= snake.nodes[i - 1];
	}
	switch (snake.dir){
		case UP:
			snake.nodes[0].y--;
			break;
		case RIGHT:
			snake.nodes[0].x++;
			break;
		case LEFT:
			snake.nodes[0].x--;
			break;
		case DOWN:
			snake.nodes[0].y++;
			break;
	}
	draw_text(snake.nodes[0].y,snake.nodes[0].x,SNAKE);
}
void init_snake(unsigned count,enum Direction dir){
	snake.dir = dir;
	snake.count = count;
	int j = 4;
	for (unsigned i = 0; i < count; i++){
		snake.nodes[i].y = j++;
		snake.nodes[i].x = 10;
	}

	for (unsigned i = 0 ; i < snake.count;++i){
		draw_text(snake.nodes[i].y,snake.nodes[i].x,SNAKE);
	}

}

void add_node(){
	snake.nodes[snake.count].x = food.x ;
	snake.nodes[snake.count].y = food.y ;
	snake.count++;
}

int randint(int start,int end){
	int x = start + (int)((double)rand() / (RAND_MAX) * (end - start + 1));
	return x;
}

void gen_food(unsigned width,unsigned height){
	/*
	the food vector position must be in:
	width > x > 0
	height > y > 0
		*/
	srand(time(NULL));
	food.x = randint(2,width-2);
	food.y = randint(2,height-2) ;
	draw_char(food.y,food.x,FOOD);
}

int is_dead(unsigned width,unsigned height){
	if (snake.nodes[0].y <= 1 || snake.nodes[0].y >= height - 1 || snake.nodes[0].x <= 1
		|| snake.nodes[0].x >= width){
		return 1;
	}
//	for(unsigned i = 1;i < snake.count;++i){
//		if(snake.nodes[0].x == snake.nodes[i].x && snake.nodes[0].y == snake.nodes[i].y) return 1;
//	}
	return 0;
}

int is_eaten(){
	if (snake.nodes[0].x == food.x && snake.nodes[0].y == food.y)  return 1;
	return 0;
}

int main(void){
	term_init();
	clear();
	int height = ws.ws_row ;
	int width = ws.ws_col ;
	render_arena(width,height);
	init_snake(3,UP);
	gen_food(width,height);

	int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
	if (flags == -1) {
		perror("fcntl");
		exit(EXIT_FAILURE);
	}
	if (fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK) == -1) {
		perror("fcntl");
		exit(EXIT_FAILURE);
	}
	char c;
	hide_cursor();
	while(!is_dead(width,height)){
		usleep(50000);
		ssize_t ret = read(STDIN_FILENO, &c, 1);
		if (ret == -1) { // there is no input
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				move_snake();
				if (is_eaten()){
					add_node();
					//draw_char(food.y,food.x,' ');
					gen_food(width,height);
				}
			}else {
				perror("read");
				exit(EXIT_FAILURE);
			}
		} else if (ret) {
			switch (c) {
				case 'w':
					snake.dir = UP;
					break;
				case 's':
					snake.dir = DOWN;
					break;
				case 'd':
					snake.dir = RIGHT;
					break;
				case 'a':
					snake.dir = LEFT;
					break;
			}
			move_snake();
			if (is_eaten()){
				add_node();
				//draw_char(food.y,food.x,' ');
				gen_food(width,height);
			}
		}
	}
	term_reset();
	return EXIT_SUCCESS;
}
