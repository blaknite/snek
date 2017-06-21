#include <stdio.h>
#include <stdlib.h>

#include "rc2014.h"

#pragma output REGISTER_SP = -1
#pragma output CLIB_MALLOC_HEAP_SIZE = 0

#define SLEEP_DURATION 7500
#define COLOR_BG 40
#define COLOR_TEXT 37
#define COLOR_WALL 44
#define COLOR_SNAKE 45
#define COLOR_APPLE 42
#define COLOR_END 31
#define GRID_WIDTH 40
#define GRID_HEIGHT 24
#define GRID_EMPTY ' '
#define GRID_WALL 'W'
#define GRID_APPLE 'A'
#define DIR_UP 'U'
#define DIR_DOWN 'D'
#define DIR_LEFT 'L'
#define DIR_RIGHT 'R'
#define SCORE_PER_APPLE 10
#define STATE_START 0
#define STATE_RUN 1
#define STATE_PAUSE 2
#define STATE_END 3

typedef struct {
  int x;
  int y;
} cell;

typedef struct {
  int x;
  int y;
  char direction;
} snake_cell;

snake_cell snake_head;
snake_cell snake_tail;
cell apple;

char grid[GRID_WIDTH * GRID_HEIGHT];

int state;

int score;

int grid_index(int x, int y) {
  return y * GRID_WIDTH + x;
}

char get_cell(int x, int y) {
  return grid[grid_index(x, y)];
}

void set_cell(int x, int y, char val) {
  grid[grid_index(x, y)] = val;
}

void draw_cell(int x, int y, int col) {
  rc2014_ansi_move_cursor(y + 1, 2 * x + 1);
  rc2014_ansi_color(col);
  rc2014_print("  ");
}

void init_grid() {
  int x;
  int y;

  for ( y = 0; y < GRID_HEIGHT; ++y ) {
    for ( x = 0; x < GRID_WIDTH; ++x ) {
      if ( y == 0 || y == GRID_HEIGHT - 1 || x == 0 || x == GRID_WIDTH - 1 ) {
        set_cell(x, y, GRID_WALL);
      } else {
        set_cell(x, y, GRID_EMPTY);
      }
    }
  }
}

void init_snake() {
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

void init_apple() {
  apple.x = 1;
  apple.y = 1;
}

void set_direction(char key) {
  switch ( key ) {
    case 'w':
    case 'W':
      if ( snake_head.direction != DIR_DOWN ) snake_head.direction = DIR_UP;
      break;

    case 'a':
    case 'A':
      if ( snake_head.direction != DIR_RIGHT ) snake_head.direction = DIR_LEFT;
      break;

    case 's':
    case 'S':
      if ( snake_head.direction != DIR_UP ) snake_head.direction = DIR_DOWN;
      break;

    case 'd':
    case 'D':
      if ( snake_head.direction != DIR_LEFT ) snake_head.direction = DIR_RIGHT;
      break;
  }
}

void new_apple() {
  int x = 0;
  int y = 0;

  while ( get_cell(x, y) != GRID_EMPTY ) {
    x = rand() % GRID_WIDTH;
    y = rand() % GRID_HEIGHT;
  }

  set_cell(apple.x, apple.y, GRID_EMPTY);
  apple.x = x;
  apple.y = y;
  set_cell(apple.x, apple.y, GRID_APPLE);
}

void end() {
  state = STATE_END;
  rc2014_ansi_cls();
}

void update_snake_head() {
  char val;

  set_cell(snake_head.x, snake_head.y, snake_head.direction);

  switch ( snake_head.direction ) {
    case DIR_UP:
      snake_head.y--;
      break;

    case DIR_DOWN:
      snake_head.y++;
      break;

    case DIR_LEFT:
      snake_head.x--;
      break;

    case DIR_RIGHT:
      snake_head.x++;
      break;
  }

  val = get_cell(snake_head.x, snake_head.y);

  if ( val == GRID_EMPTY || val == GRID_APPLE ) return;

  end();
}

void update_snake_tail() {
  int x = snake_tail.x;
  int y = snake_tail.y;

  if ( apple.x == snake_head.x && apple.y == snake_head.y ) return;

  switch ( get_cell(x, y) ) {
    case DIR_UP:
      snake_tail.y--;
      break;

    case DIR_DOWN:
      snake_tail.y++;
      break;

    case DIR_LEFT:
      snake_tail.x--;
      break;

    case DIR_RIGHT:
      snake_tail.x++;
      break;
  }

  snake_tail.direction = get_cell(x, y);

  set_cell(x, y, GRID_EMPTY);
}

void update_score() {
  if ( apple.x != snake_head.x || apple.y != snake_head.y ) return;
  new_apple();
  score += SCORE_PER_APPLE;
}

void draw_start() {
  rc2014_ansi_color(31);
  rc2014_ansi_move_cursor(GRID_HEIGHT / 2 - 6, GRID_WIDTH - 12);
  rc2014_print("####  #   #  ####  #  #");
  rc2014_ansi_color(32);
  rc2014_ansi_move_cursor(GRID_HEIGHT / 2 - 5, GRID_WIDTH - 12);
  rc2014_print("#     ##  #  #     # #");
  rc2014_ansi_color(33);
  rc2014_ansi_move_cursor(GRID_HEIGHT / 2 - 4, GRID_WIDTH - 12);
  rc2014_print("####  # # #  ###   ##");
  rc2014_ansi_color(35);
  rc2014_ansi_move_cursor(GRID_HEIGHT / 2 - 3, GRID_WIDTH - 12);
  rc2014_print("   #  #  ##  #     # #");
  rc2014_ansi_color(36);
  rc2014_ansi_move_cursor(GRID_HEIGHT / 2 - 2, GRID_WIDTH - 12);
  rc2014_print("####  #   #  ####  #  #");

  rc2014_ansi_color(COLOR_TEXT);

  rc2014_ansi_move_cursor(GRID_HEIGHT / 2, GRID_WIDTH - 13);
  rc2014_print("Created by Grant Colegate");

  rc2014_ansi_move_cursor(GRID_HEIGHT / 2 + 2, GRID_WIDTH - 14);
  rc2014_print("--| PRESS SPACE TO START |--");
}

void draw_grid() {
  int x;
  int y;
  char val;
  char col;

  rc2014_ansi_color(COLOR_BG);
  rc2014_ansi_cls();

  for ( y = 0; y < GRID_HEIGHT; ++y ) {
    for ( x = 0; x < GRID_WIDTH; ++x ) {
      val = get_cell(x, y);

      switch ( val ) {
        case GRID_WALL:
          col = COLOR_WALL;
          break;

        case GRID_APPLE:
          col = COLOR_APPLE;
          break;

        case DIR_UP:
        case DIR_DOWN:
        case DIR_LEFT:
        case DIR_RIGHT:
          col = COLOR_SNAKE;
          break;

        default:
          col = COLOR_BG;
          break;
      }

      if ( col != COLOR_BG ) draw_cell(x, y, col);
    }
  }
}

void draw_snake_head() {
  draw_cell(snake_head.x, snake_head.y, COLOR_SNAKE);
}

void draw_snake_tail() {
  int x = snake_tail.x;
  int y = snake_tail.y;

  switch ( snake_tail.direction ) {
    case DIR_UP:
      y++;
      break;

    case DIR_DOWN:
      y--;
      break;

    case DIR_LEFT:
      x++;
      break;

    case DIR_RIGHT:
      x--;
      break;
  }

  draw_cell(x, y, COLOR_BG);
}

void draw_apple() {
  draw_cell(apple.x, apple.y, COLOR_APPLE);
}

void draw_score() {
  char str[14];
  sprintf(str, "SCORE: %d", score);
  rc2014_ansi_move_cursor(GRID_HEIGHT + 1, 1);
  rc2014_ansi_color(COLOR_BG);
  rc2014_ansi_color(COLOR_TEXT);
  rc2014_print(str);
}

void draw_pause() {
  rc2014_ansi_color(COLOR_BG);
  rc2014_ansi_color(COLOR_TEXT);
  rc2014_ansi_move_cursor(GRID_HEIGHT + 1, 1);
  rc2014_print("PAUSED        ");
}

void draw_end() {
  char str[5];

  rc2014_ansi_color(COLOR_END);
  rc2014_ansi_move_cursor(GRID_HEIGHT / 2 - 2, GRID_WIDTH - 5);
  rc2014_print("GAME OVER!");
  rc2014_ansi_move_cursor(GRID_HEIGHT / 2, GRID_WIDTH - 8);
  rc2014_print("FINAL SCORE: ");
  sprintf(str, "%d", score);
  rc2014_print(str);
  rc2014_ansi_move_cursor(GRID_HEIGHT / 2 + 2, GRID_WIDTH - 14);
  rc2014_print("--| PRESS SPACE TO START |--");
}

void sleep() {
  int i = SLEEP_DURATION - score;
  while ( i > 0 ) i--;
}

void start() {
  score = 0;

  state = STATE_RUN;

  rc2014_ansi_cls();

  init_grid();
  init_snake();
  init_apple();

  new_apple();

  draw_grid();

  rc2014_ansi_color(COLOR_BG);
  rc2014_ansi_color(COLOR_TEXT);
  rc2014_ansi_move_cursor(GRID_HEIGHT + 1, 38);
  rc2014_print("W: Up  S: Down  A: Left  D: Right  P: Pause");

  draw_score();
}

// Input
void keypressed(char key) {
  switch ( state ) {
    case STATE_START:
      if ( key == ' ' ) start();
      break;

    case STATE_RUN:
      set_direction(key);
      if ( key == 'p' ) state = STATE_PAUSE;
      break;

    case STATE_PAUSE:
      if ( key == 'p' ) state = STATE_RUN;
      break;

    case STATE_END:
      if ( key == ' ' ) start();
      break;
  }
}

// Update
void update() {
  switch ( state ) {
    case STATE_RUN:
      update_snake_head();
      update_snake_tail();
      update_score();
      break;
  }
}

// Draw
void draw() {
  switch ( state ) {
    case STATE_START:
      draw_start();
      break;

    case STATE_RUN:
      draw_snake_head();
      draw_snake_tail();
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

// Main game loop
void main() {
  char key;

  rc2014_ansi_hide_cursor();
  rc2014_ansi_color(COLOR_BG);
  rc2014_ansi_cls();
  state = STATE_START;

  while ( 1 ) {
    if ( rc2014_uart_rx_ready() ) {
      key = rc2014_uart_rx();
      keypressed(key);
    }
    update();
    draw();
    sleep();
  }
}
