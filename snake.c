#include "snake.h"
#include <ncurses.h>
#include <unistd.h>

int main() {
  init();
  mainMenu();
  cleanup();
  return 0;
}

void init() {
  initscr();            // initialize ncurses
  noecho();             // disable echo of input characters
  curs_set(0);          // hide the cursor
  keypad(stdscr, TRUE); // enable keyboard
  // start_color();        // enable color
}

void mainMenu() {
  const char *choices[] = {"Play Game", "High Scores", "Settings", "Exit"};
  const int NUM_CHOICES = 4;
  const char *TITLE = "Main Menu";
  int choice = 0;
  WINDOW *win = newwin(NUM_CHOICES + 2, 40, 0, 0);
  keypad(win, TRUE); // enable arrow keys

  nodelay(win, TRUE);
  while (1) {
    const int c = wgetch(win);
    switch (c) {
    case KEY_UP:
      if (choice == 0)
        choice = NUM_CHOICES - 1;
      else
        --choice;
      break;
    case KEY_DOWN:
      if (choice == NUM_CHOICES - 1)
        choice = 0;
      else
        ++choice;
      break;
    case 10: // Enter key
      break;
    case 'q': // Quit on 'q'
      goto cleanup;
    }
    waitNextFrame();
    werase(win);
    box(win, 0, 0);
    mvwprintw(win, 0, 1, "%s", TITLE);
    for (int i = 0; i < NUM_CHOICES; ++i) {
      if (choice == i) {
        wattron(win, A_REVERSE);
        mvwprintw(win, i + 1, 3, "* %-18s", choices[i]);
        wattroff(win, A_REVERSE);
      } else {
        mvwprintw(win, i + 1, 3, "%-20s", choices[i]);
      }
    }
    wrefresh(win);
  }

cleanup:
  delwin(win);
}

void cleanup() { endwin(); }

void startGameLoop() {
  int x = 10, y = 10, dx = 1, dy = 0;
  while (1) {
    waitNextFrame();
    erase();
    mvaddch(y, x, 'O');
    refresh();

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
