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
      switch (choice) {
      case 0:
        startGameLoop();
        break;
      case 1:
        // todo implement high score
        break;
      case 2:
        // todo implement settings
        break;
      case 3:
        goto cleanup;
      }
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
        wattron(win, A_BOLD);
        mvwprintw(win, i + 1, 3, "* %-18s", choices[i]);
        wattroff(win, A_BOLD);
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
  int x = 1, y = 1, dx = 1, dy = 0;
  enum MoveDirection movement = RIGHT;
  nodelay(stdscr, TRUE);

  while (1) {
    waitNextFrame();
    erase();
    mvaddch(y, x, 'O');
    box(stdscr, 0, 0);
    refresh();

    int ch = getch();

    if (ch == KEY_LEFT && movement != RIGHT) {
      movement = LEFT;
    } else if (ch == KEY_RIGHT && movement != LEFT) {
      movement = RIGHT;
    } else if (ch == KEY_UP && movement != DOWN) {
      movement = UP;
    } else if (ch == KEY_DOWN && movement != UP) {
      movement = DOWN;
    }

    if (movement == LEFT) {
      dx = -1;
      dy = 0;
    } else if (movement == RIGHT) {
      dx = 1;
      dy = 0;
    } else if (movement == UP) {
      dx = 0;
      dy = -1;
    } else if (movement == DOWN) {
      dx = 0;
      dy = 1;
    }

    x += dx;
    y += dy;

    int w, h;
    getmaxyx(stdscr, h, w);
    if (x < 1 || y < 1 || x == w || y == h) {
      goto gameover;
      return;
    }
  }

gameover:
  erase();
  refresh();
  gameOver(0);
}

void gameOver(int score) {
  int choice = 0;
  WINDOW *win = newwin(6, 40, 0, 0);

  box(win, 0, 0);
  mvwprintw(win, 0, 1, "Game Over");
  mvwprintw(win, 1, 1, "Score: %d", score);
  mvwprintw(win, 2, 1, "High Score: ");
  mvwprintw(win, 4, 1, "Press enter to conitnue");
  wrefresh(win);
  while (wgetch(win) != 10)
    ;
  clear();
  refresh();
  delwin(win);
}
