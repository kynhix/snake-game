#include "snake.h"
#include <ncurses.h>
#include <unistd.h>

int main() {
  init();
  startGameLoop();
  cleanup();
  return 0;
}

void init() {
  initscr();            // initialize ncurses
  noecho();             // disable echo of input characters
  curs_set(0);          // hide the cursor
  keypad(stdscr, TRUE); // enable keyboard
  start_color();        // enable color
}

void cleanup() { endwin(); }

void startGameLoop() {
  int x = 10, y = 10, dx = 1, dy = 0;
  while (1) {
    clear();
    mvaddch(y, x, 'O');
    refresh();
    usleep(10000);

    int ch = getch();
    if (ch == KEY_LEFT) {
      dx = -1;
      dy = 0;
    } else if (ch == KEY_RIGHT) {
      dx = 1;
      dy = 0;
    } else if (ch == KEY_UP) {
      dx = 0;
      dy = -1;
    } else if (ch == KEY_DOWN) {
      dx = 0;
      dy = 1;
    }

    x += dx;
    y += dy;
  }
}
