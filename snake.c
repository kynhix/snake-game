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
    global_state.snake_speed = 3;
    return;
  }
  global_state.snake_speed = fgetc(fptr) - '0';
  fscanf(fptr, "%*[\n]"); // skip to next line

  char namebuf[64];
  char scorebuf[64];

  while (fgets(namebuf, sizeof(namebuf), fptr) &&
         fgets(scorebuf, sizeof(scorebuf), fptr)) {

    namebuf[strlen(namebuf) - 1] = '\0';
    scorebuf[strlen(scorebuf) - 1] = '\0';

    high_score_node *high_score = malloc(sizeof(high_score_node));
    high_score->next = NULL;
    high_score->prev = NULL;
    high_score->score = atoi(scorebuf);
    high_score->name = malloc(sizeof(char) * 64);
    strncpy(high_score->name, namebuf, 64);

    if (!global_state.high_score_head) {
      global_state.high_score_head = high_score;
      continue;
    }
    high_score_node *node = global_state.high_score_head;
    while (node->next)
      node = node->next;
    node->next = high_score;
    high_score->prev = node;
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
  while (1) {
    switch (getMenuSelection("Main Menu", 4, choices, 0)) {
    case -1:
      return;
    case 0:
      startGameLoop();
      break;
    case 1:
      highscoreMenu();
      break;
    case 2:
      settingsMenu();
      break;
    case 3:
      return;
    }
  }
}

void settingsMenu() {
  const char *choices[] = {"Snake Speed", "Back"};
  while (1) {
    switch (getMenuSelection("Settings Menu", 2, choices, 0)) {
    case 0:
      speedMenu();
      break;
    default:
      return;
    }
  }
}

void speedMenu() {
  const char *choices[] = {"1", "2", "3", "4", "5", "Back"};
  int speed =
      getMenuSelection("Snake Speed", 6, choices, global_state.snake_speed - 1);
  if (speed < 0 || speed > 4) {
    return;
  }

  global_state.snake_speed = speed + 1;
  saveToFile();
}

void highscoreMenu() {
  WINDOW *win = newwin(12, 100, 0, 0);
  int i = 1;
  box(win, 0, 0);
  mvwprintw(win, 0, 1, "High Scores:");
  mvwprintw(win, 11, 1, "Press Enter to Go Back");
  high_score_node *high_score = global_state.high_score_head;
  if (!high_score) {
    mvwprintw(win, 1, 1, "No high scores yet :(");
  }
  while (high_score) {
    mvwprintw(win, i, 1, "%2d. %-10s - %d", i, high_score->name,
              high_score->score);
    high_score = high_score->next;
    ++i;
  }
  wrefresh(win);
  while (getch() != 10)
    usleep(10000);
  erase();
  refresh();
}

int getMenuSelection(const char *title, int n, const char *choice_names[],
                     int choice) {
  WINDOW *win = newwin(n + 2, 40, 0, 0);
  keypad(win, TRUE); // enable arrow keys

  while (1) {
    drawMenu(win, title, n, choice, choice_names);
    const int ch = wgetch(win);
    if (ch == KEY_UP) {
      if (choice == 0)
        choice = n - 1;
      else
        --choice;
    } else if (ch == KEY_DOWN) {
      if (choice == n - 1)
        choice = 0;
      else
        ++choice;
    } else if (ch == 'q') {
      choice = -1;
      break;
    } else if (ch == 10) {
      break;
    }
  }

  erase();
  refresh();
  delwin(win);
  return choice;
}

void drawMenu(WINDOW *win, const char *title, int n, int choice,
              const char *choice_names[]) {
  // clear previous screen
  werase(win);
  // draw menu border
  box(win, 0, 0);
  mvwprintw(win, 0, 1, "%s", title);
  for (int i = 0; i < n; ++i) {
    if (choice == i) {
      // bold and star selected choice
      wattron(win, A_BOLD);
      mvwprintw(win, i + 1, 3, "* %-18s", choice_names[i]);
      wattroff(win, A_BOLD);
    } else {
      mvwprintw(win, i + 1, 3, "%-20s", choice_names[i]);
    }
  }
  wrefresh(win);
}

void cleanup() { endwin(); }

bool isSkipFrame(int *frame_state) {
  *frame_state -= global_state.snake_speed;
  if (*frame_state > 0) {
    return true;
  }
  *frame_state = 5;
  return false;
}

void startGameLoop() {
  MoveDirection movement = RIGHT;
  int score = 0;

  snake_cell *head = createSnakeCell(1, 1, NULL);
  snake_food food;
  food.x = 5;
  food.y = 5;

  int frame_state = 0;

  nodelay(stdscr, TRUE);
  while (1) {
    drawSnakeGame(head, food, score);

    waitNextFrame();

    if (isSkipFrame(&frame_state)) {
      // Key detection
      continue;
    }

    // Key detection
    movement = getMoveDirection(getch(), movement);

    // Movement
    int dx = 0, dy = 0;
    getMoveDelta(&dx, &dy, movement);

    // apply movement
    head = createSnakeCell(head->x + dx, head->y + dy, head);

    if (isSnakeColliding(head)) {
      break;
    }

    if (head->x == food.x && head->y == food.y) {
      spawnFoodOnEmptySquare(&food, head);
      score++;
    } else {
      // remove snake tail
      removeSnakeTail(head);
    }
  }

  // free allocated memory
  freeSnake(head);

  // clear screen
  erase();
  refresh();

  // game over screen
  gameOver(score);
}

void drawSnakeGame(snake_cell *head, snake_food food, int score) {
  erase();
  // draw snek
  mvaddch(head->y, head->x, '@');
  snake_cell *node = head->next;
  // snek body
  while (node) {
    mvaddch(node->y, node->x, '0');
    node = node->next;
  }
  // draw food
  mvaddch(food.y, food.x, '*');
  // draw border
  box(stdscr, 0, 0);
  mvwprintw(stdscr, 0, 1, "Score: %d", score);
  refresh();
}

bool isSnakeColliding(snake_cell *head) {
  // bounds checking
  int w, h;
  getmaxyx(stdscr, h, w);
  if (head->x < 1 || head->y < 1 || head->x >= w - 1 || head->y >= h - 1) {
    return true;
  }

  // body collision
  snake_cell *node = head->next;
  while (node) {
    if (node->x == head->x && node->y == head->y) {
      return true;
    }
    node = node->next;
  }

  return false;
}

void spawnFoodOnEmptySquare(snake_food *food, snake_cell *head) {
  int w, h;
  getmaxyx(stdscr, h, w);
  w = w - 2;
  h = h - 2;

  int n = w * h;
  int valid_cells[n];
  for (int i = 0; i < n; ++i)
    valid_cells[i] = i;

  while (head) {
    valid_cells[(head->x - 1) + (head->y - 1) * (w + 1)] = 0;
    head = head->next;
  }

  int m = n - 1;
  while (m > -1 && !valid_cells[m])
    --m;

  if (m == -1) {
    // WINNER
    return;
  }
  for (int i = 0; i < m; ++i) {
    if (valid_cells[i])
      continue;

    valid_cells[i] = valid_cells[m];
    valid_cells[m] = 0;

    while (!valid_cells[--m])
      ;
  }

  const int r = rand() % (m + 1);
  food->x = r % (w + 1) + 1;
  food->y = r / (w + 1) + 1;
}

MoveDirection getMoveDirection(int ch, MoveDirection move) {
  if (ch == KEY_LEFT && move != RIGHT) {
    return LEFT;
  } else if (ch == KEY_RIGHT && move != LEFT) {
    return RIGHT;
  } else if (ch == KEY_UP && move != DOWN) {
    return UP;
  } else if (ch == KEY_DOWN && move != UP) {
    return DOWN;
  }
  return move;
}

void getMoveDelta(int *dx, int *dy, MoveDirection move) {
  if (move == LEFT) {
    *dx = -1;
  } else if (move == RIGHT) {
    *dx = 1;
  } else if (move == UP) {
    *dy = -1;
  } else if (move == DOWN) {
    *dy = 1;
  }
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

  if (!global_state.high_score_head ||
      score > global_state.high_score_head->score) {
    high_score_node *high_score = malloc(sizeof(high_score_node));
    high_score->score = score;
    high_score->next = global_state.high_score_head;
    high_score->name = malloc(sizeof(char) * 100);
    strcpy(high_score->name, "Kris Yay :D");
    global_state.high_score_head = high_score;
    for (int i = 0; i < 10 && high_score != NULL; ++i) {
      high_score = high_score->next;
    }
    // remove 10th highest score
    if (high_score) {
      high_score->prev->next = NULL;
      free(high_score);
    }

    saveToFile();
  }

  const int high_score =
      global_state.high_score_head ? global_state.high_score_head->score : 0;

  // game over screen
  box(win, 0, 0);
  mvwprintw(win, 0, 1, "Game Over");
  mvwprintw(win, 1, 1, "Score: %d", score);
  mvwprintw(win, 2, 1, "High Score: %d", high_score);
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
