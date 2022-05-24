#include <stdio.h>
#include <stdlib.h>

#pragma output CLIB_MALLOC_HEAP_SIZE = 0
#pragma output CLIB_STDIO_HEAP_SIZE = 0
#pragma output CLIB_OPT_PRINTF = 0x1;

#define CYCLES_PER_TICK 750
#define SCORE_PER_APPLE 10
#define CYCLES_PER_APPLE 2
#define COLOR_BG 40
#define COLOR_TEXT 37
#define COLOR_WALL 44
#define COLOR_SNAKE 45
#define COLOR_APPLE 42
#define COLOR_END 31
#define GRID_WIDTH 40
#define GRID_HEIGHT 24
#define CELL_EMPTY ' '
#define CELL_WALL 'W'
#define CELL_APPLE 'A'
#define DIR_UP 'U'
#define DIR_DOWN 'D'
#define DIR_LEFT 'L'
#define DIR_RIGHT 'R'
#define START_KEY ' '
#define QUIT_KEY 'x'
#define STATE_START 0
#define STATE_RUN 1
#define STATE_PAUSE 2
#define STATE_END 3

extern unsigned char getchar_noblock(void);
extern void quit(void);

void ansi_cls(void) {
  fputs("\x1B[2J", stdout);
}

void ansi_hide_cursor(void) {
  fputs("\x1B[?25l", stdout);
}

void ansi_show_cursor(void) {
  fputs("\x1B[?25h", stdout);
}

void ansi_move_cursor(unsigned int row, unsigned int col) {
  printf("\x1B[%d;%dH", row, col);
}

void ansi_color(unsigned int col) {
  printf("\x1B[%dm", col);
}

void ansi_reset(void) {
  fputs("\x1B[0m", stdout);
}

typedef struct {
  unsigned int x;
  unsigned int y;
} cell;

typedef struct {
  unsigned int x;
  unsigned int y;
  unsigned char direction;
} snake_cell;

snake_cell snake_head;
snake_cell snake_tail;
cell apple;

unsigned char grid[GRID_WIDTH * GRID_HEIGHT];

unsigned int state = STATE_START;

unsigned int score = 0;
unsigned int cycles_per_tick = CYCLES_PER_TICK;

unsigned char direction = DIR_UP;

unsigned int temp_x;
unsigned int temp_y;

unsigned int cell_index(unsigned int x, unsigned int y) {
  return y * GRID_WIDTH + x;
}

unsigned char get_cell(unsigned int x, unsigned int y) {
  return grid[cell_index(x, y)];
}

void set_cell(unsigned int x, unsigned int y, unsigned char val) {
  grid[cell_index(x, y)] = val;
}

void draw_cell(unsigned int x, unsigned int y, unsigned int col) {
  ansi_move_cursor(y + 1, 2 * x + 1);
  ansi_color(col);
  fputs("  ", stdout);
}

void init_grid(void) {
  for ( temp_y = 0; temp_y < GRID_HEIGHT; temp_y++ ) {
    for ( temp_x = 0; temp_x < GRID_WIDTH; temp_x++ ) {
      if ( temp_y == 0 || temp_y == GRID_HEIGHT - 1 || temp_x == 0 || temp_x == GRID_WIDTH - 1 ) {
        set_cell(temp_x, temp_y, CELL_WALL);
      } else {
        set_cell(temp_x, temp_y, CELL_EMPTY);
      }
    }
  }
}

void init_snake(void) {
  snake_head.x = GRID_WIDTH / 2;
  snake_head.y = GRID_HEIGHT / 2;
  snake_head.direction = DIR_UP;

  snake_tail.x = snake_head.x;
  snake_tail.y = snake_head.y + 2;
  snake_tail.direction = snake_head.direction;

  set_cell(snake_head.x, snake_head.y, snake_head.direction);
  set_cell(snake_head.x, snake_head.y + 1, snake_head.direction);
  set_cell(snake_head.x, snake_head.y + 2, snake_head.direction);
}

void init_apple(void) {
  apple.x = 0;
  apple.y = 0;
}

void set_direction(char key) {
  if ( direction != snake_head.direction ) return;

  switch ( key ) {
    case 'w':
    case 'W':
      if ( direction != DIR_DOWN ) direction = DIR_UP;
      break;

    case 'a':
    case 'A':
      if ( direction != DIR_RIGHT ) direction = DIR_LEFT;
      break;

    case 's':
    case 'S':
      if ( direction != DIR_UP ) direction = DIR_DOWN;
      break;

    case 'd':
    case 'D':
      if ( direction != DIR_LEFT ) direction = DIR_RIGHT;
      break;
  }
}

void new_apple(void) {
  while ( get_cell(apple.x, apple.y) != CELL_EMPTY ) {
    apple.x = rand() % GRID_WIDTH;
    apple.y = rand() % GRID_HEIGHT;
  }

  set_cell(apple.x, apple.y, CELL_APPLE);
}

void update_snake(void) {
  snake_head.direction = direction;

  set_cell(snake_head.x, snake_head.y, snake_head.direction);

  switch ( snake_head.direction ) {
    case DIR_UP:    snake_head.y--; break;
    case DIR_DOWN:  snake_head.y++; break;
    case DIR_LEFT:  snake_head.x--; break;
    case DIR_RIGHT: snake_head.x++; break;
  }

  if ( apple.x == snake_head.x && apple.y == snake_head.y ) return;

  snake_tail.direction = get_cell(snake_tail.x, snake_tail.y);

  set_cell(snake_tail.x, snake_tail.y, CELL_EMPTY);

  switch ( snake_tail.direction ) {
    case DIR_UP:    snake_tail.y--; break;
    case DIR_DOWN:  snake_tail.y++; break;
    case DIR_LEFT:  snake_tail.x--; break;
    case DIR_RIGHT: snake_tail.x++; break;
  }
}

void end(void) {
  state = STATE_END;
  ansi_cls();
}

void update_score(void) {
  unsigned char val = get_cell(snake_head.x, snake_head.y);

  if ( val != CELL_APPLE && val != CELL_EMPTY ) end();

  if ( apple.x != snake_head.x || apple.y != snake_head.y ) return;

  new_apple();

  score += SCORE_PER_APPLE;
  cycles_per_tick -= CYCLES_PER_APPLE;
}

void draw_start(void) {
  ansi_color(31);
  ansi_move_cursor(GRID_HEIGHT / 2 - 6, GRID_WIDTH - 12);
  puts("####  #   #  ####  #  #");
  ansi_color(32);
  ansi_move_cursor(GRID_HEIGHT / 2 - 5, GRID_WIDTH - 12);
  puts("#     ##  #  #     # #");
  ansi_color(33);
  ansi_move_cursor(GRID_HEIGHT / 2 - 4, GRID_WIDTH - 12);
  puts("####  # # #  ###   ##");
  ansi_color(35);
  ansi_move_cursor(GRID_HEIGHT / 2 - 3, GRID_WIDTH - 12);
  puts("   #  #  ##  #     # #");
  ansi_color(36);
  ansi_move_cursor(GRID_HEIGHT / 2 - 2, GRID_WIDTH - 12);
  puts("####  #   #  ####  #  #");

  ansi_color(COLOR_TEXT);

  ansi_move_cursor(GRID_HEIGHT / 2, GRID_WIDTH - 13);
  puts("Created by Grant Colegate");

  ansi_move_cursor(GRID_HEIGHT / 2 + 2, GRID_WIDTH - 14);
  puts("--| PRESS SPACE TO START |--");
}

void draw_grid(void) {
  for ( temp_y = 0; temp_y < GRID_HEIGHT; temp_y++ ) {
    for ( temp_x = 0; temp_x < GRID_WIDTH; temp_x++ ) {
      switch ( get_cell(temp_x, temp_y) ) {
        case CELL_WALL:
          draw_cell(temp_x, temp_y, COLOR_WALL);
          break;

        case CELL_APPLE:
          draw_cell(temp_x, temp_y, COLOR_APPLE);
          break;

        case DIR_UP:
        case DIR_DOWN:
        case DIR_LEFT:
        case DIR_RIGHT:
          draw_cell(temp_x, temp_y, COLOR_SNAKE);
          break;
      }
    }
  }
}

void draw_snake(void) {
  temp_x = snake_tail.x;
  temp_y = snake_tail.y;

  switch ( snake_tail.direction ) {
    case DIR_UP:    temp_y++; break;
    case DIR_DOWN:  temp_y--; break;
    case DIR_LEFT:  temp_x++; break;
    case DIR_RIGHT: temp_x--; break;
  }

  draw_cell(snake_head.x, snake_head.y, COLOR_SNAKE);
  draw_cell(temp_x, temp_y, COLOR_BG);
}

void draw_apple(void) {
  draw_cell(apple.x, apple.y, COLOR_APPLE);
}

void draw_score(void) {
  ansi_move_cursor(GRID_HEIGHT + 1, 1);
  ansi_color(COLOR_BG);
  ansi_color(COLOR_TEXT);
  printf("SCORE: %d", score);
}

void draw_pause(void) {
  ansi_color(COLOR_BG);
  ansi_color(COLOR_TEXT);
  ansi_move_cursor(GRID_HEIGHT + 1, 1);
  fputs("PAUSED        ", stdout);
}

void draw_help(void) {
  ansi_color(COLOR_BG);
  ansi_color(COLOR_TEXT);
  ansi_move_cursor(GRID_HEIGHT + 1, 25);
  fputs("W: Up  S: Down  A: Left  D: Right  Space: Pause  X: Quit", stdout);
}

void draw_end(void) {
  ansi_color(COLOR_END);
  ansi_move_cursor(GRID_HEIGHT / 2 - 2, GRID_WIDTH - 5);
  puts("GAME OVER!");
  ansi_move_cursor(GRID_HEIGHT / 2, GRID_WIDTH - 5);
  printf("SCORE: %d", score);
  ansi_move_cursor(GRID_HEIGHT / 2 + 2, GRID_WIDTH - 14);
  puts("--| PRESS SPACE TO START |--");
}

void start(void) {
  score = 0;
  cycles_per_tick = CYCLES_PER_TICK;
  direction = DIR_UP;

  state = STATE_RUN;

  ansi_cls();

  init_grid();
  init_snake();
  init_apple();

  new_apple();

  draw_grid();
  draw_score();
  draw_help();
}

void input(void) {
  unsigned char key;

  key = getchar_noblock();

  if ( !key ) return;

  if ( key == QUIT_KEY ) {
    ansi_reset();
    ansi_show_cursor();
    quit();
  }

  switch ( state ) {
    case STATE_START:
    case STATE_END:
      if ( key == START_KEY ) start();
      break;

    case STATE_RUN:
      set_direction(key);
      if ( key == START_KEY ) state = STATE_PAUSE;
      break;

    case STATE_PAUSE:
      if ( key == START_KEY ) state = STATE_RUN;
      break;
  }
}

void update(void) {
  switch ( state ) {
    case STATE_RUN:
      update_snake();
      update_score();
      break;

    default:
      rand();
  }
}

void draw(void) {
  switch ( state ) {
    case STATE_START:
      draw_start();
      break;

    case STATE_RUN:
      draw_snake();
      draw_apple();
      draw_score();
      break;

    case STATE_PAUSE:
      draw_pause();
      break;

    case STATE_END:
      draw_end();
      break;
  }
}

void main(void) {
  static unsigned int i;

  ansi_hide_cursor();
  ansi_color(COLOR_BG);
  ansi_cls();

  while ( 1 ) {
    i = cycles_per_tick;

    while ( i-- ) input();
    update();
    draw();
  }
}
