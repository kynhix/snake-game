#include "snake.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

snake_state global_state;

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
  loadFromSave();
  // start_color();        // enable color
}

void loadFromSave() {
  global_state.high_score_head = NULL;
  FILE *fptr = fopen("snake.txt", "r");
  if (!fptr) {
    global_state.snake_speed = 1;
    return;
  }

  global_state.snake_speed = fgetc(fptr) - '0';
  char namebuf[100];
  char scorebuf[100];

  while (fgets(namebuf, sizeof(namebuf), fptr) &&
         fgets(scorebuf, sizeof(scorebuf), fptr)) {
    // Remove newline characters
    namebuf[strcspn(namebuf, "\n")] = 0;
    scorebuf[strcspn(scorebuf, "\n")] = 0;

    // Allocate and check new node
    high_score_node *newScore = malloc(sizeof(high_score_node));
    if (!newScore) {
      fclose(fptr);
      return;
    }

    // Allocate and check name string
    newScore->name = malloc(strlen(namebuf) + 1); // +1 for null terminator
    if (!newScore->name) {
      free(newScore);
      fclose(fptr);
      return;
    }

    // Copy data
    strcpy(newScore->name, namebuf);
    newScore->score = atoi(scorebuf);

    // Initialize pointers
    newScore->next = NULL;
    newScore->prev = NULL;

    // Link into list
    if (!global_state.high_score_head) {
      global_state.high_score_head = newScore;
    } else {
      newScore->next = global_state.high_score_head;
      global_state.high_score_head->prev = newScore;
      global_state.high_score_head = newScore;
    }
  }

  fclose(fptr);
}

void saveToFile() {
  FILE *fptr = fopen("snake.txt", "w");
  if (!fptr) {
    return;
  }

  // Save snake speed
  fprintf(fptr, "%d\n", global_state.snake_speed);

  // Save high scores
  high_score_node *current = global_state.high_score_head;
  while (current != NULL) {
    fprintf(fptr, "%s\n%d\n", current->name, current->score);
    current = current->next;
  }

  fclose(fptr);
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
    // key detection
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

    // clear previous screen
    werase(win);

    // draw menu border
    box(win, 0, 0);
    mvwprintw(win, 0, 1, "%s", TITLE);
    for (int i = 0; i < NUM_CHOICES; ++i) {
      if (choice == i) {
        // bold and star selected choice
        wattron(win, A_BOLD);
        mvwprintw(win, i + 1, 3, "* %-18s", choices[i]);
        wattroff(win, A_BOLD);
      } else {
        mvwprintw(win, i + 1, 3, "%-20s", choices[i]);
      }
    }
    wrefresh(win);

    waitNextFrame();
  }

cleanup:
  delwin(win);
}

void cleanup() { endwin(); }

void startGameLoop() {
  enum MoveDirection movement = RIGHT;
  int score = 0;

  nodelay(stdscr, TRUE);
  snake_cell *snake_head = createSnakeCell(1, 1, NULL);

  while (1) {
    erase();
    // draw snek
    mvaddch(snake_head->y, snake_head->x, '@');
    snake_cell *node = snake_head->next;
    // snek body
    while (node) {
      mvaddch(node->y, node->x, '0');
      node = node->next;
    }
    // draw border
    box(stdscr, 0, 0);
    refresh();

    int ch = getch();

    // Key detection
    if (ch == KEY_LEFT && movement != RIGHT) {
      movement = LEFT;
    } else if (ch == KEY_RIGHT && movement != LEFT) {
      movement = RIGHT;
    } else if (ch == KEY_UP && movement != DOWN) {
      movement = UP;
    } else if (ch == KEY_DOWN && movement != UP) {
      movement = DOWN;
    }

    // Movement
    int dx = 0, dy = 0;
    if (movement == LEFT) {
      dx = -1;
    } else if (movement == RIGHT) {
      dx = 1;
    } else if (movement == UP) {
      dy = -1;
    } else if (movement == DOWN) {
      dy = 1;
    }

    // apply movement
    snake_head =
        createSnakeCell(snake_head->x + dx, snake_head->y + dy, snake_head);

    removeSnakeTail(snake_head);

    // get max w and h of screen
    int w, h;
    getmaxyx(stdscr, h, w);
    // check if snake is colliding with a wall
    if (snake_head->x < 1 || snake_head->y < 1 || snake_head->x == w ||
        snake_head->y == h) {
      break;
    }

    waitNextFrame();
  }

  // free allocated memory
  freeSnake(snake_head);

  erase();
  refresh();
  gameOver(0);
}

snake_cell *createSnakeCell(int x, int y, snake_cell *next) {
  snake_cell *new_cell = malloc(sizeof(snake_cell));
  new_cell->next = next;
  new_cell->x = x;
  new_cell->y = y;
  return new_cell;
}

void freeSnake(snake_cell *head) {
  while (head) {
    snake_cell *old = head;
    head = head->next;
    free(old);
  }
}

void removeSnakeTail(snake_cell *head) {
  if (!head->next) {
    return;
  }
  while (head) {
    if (!head->next->next) {
      free(head->next);
      head->next = NULL;
    }
    head = head->next;
  }
}

void gameOver(int score) {
  int choice = 0;
  WINDOW *win = newwin(6, 40, 0, 0);

  // game over screen
  box(win, 0, 0);
  mvwprintw(win, 0, 1, "Game Over");
  mvwprintw(win, 1, 1, "Score: %d", score);
  mvwprintw(win, 2, 1, "High Score: ");
  mvwprintw(win, 4, 1, "Press enter to continue");
  wrefresh(win);

  // wait for user to press enter
  while (wgetch(win) != 10)
    usleep(10000);

  // cleanup
  clear();
  refresh();
  delwin(win);

  // back to main menu
}
