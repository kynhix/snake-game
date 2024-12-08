#include <ncurses.h>

#define FRAME_RATE 33333
#define waitNextFrame() usleep(FRAME_RATE)
#define MAX_HIGH_SCORES 10

void init();
void startGameLoop();
void cleanup();
void mainMenu();
void settingsMenu();
void speedMenu();
void highscoreMenu();
void gameOver(int score);
void drawMenu(WINDOW *win, const char *title, int n, int choice,
              const char *choice_names[]);
int getMenuSelection(const char *title, int n, const char *choice_names[],
                     int choice);

typedef enum MoveDirection { UP, DOWN, LEFT, RIGHT } MoveDirection;
MoveDirection getMoveDirection(int ch, MoveDirection move);
void getMoveDelta(int *x, int *y, MoveDirection move);

typedef struct high_score_node {
  char *name;
  int score;
  struct high_score_node *next;
  struct high_score_node *prev;
} high_score_node;

typedef struct snake_state {
  high_score_node *high_score_head;
  int snake_speed;
} snake_state;

void loadFromSave();
void saveToFile();

typedef struct snake_cell {
  int x, y;
  struct snake_cell *next;
} snake_cell;

snake_cell *createSnakeCell(int x, int y, snake_cell *next);
void freeSnake(snake_cell *head);
void removeSnakeTail(snake_cell *head);
bool isSnakeColliding(snake_cell *head);

typedef struct snake_food {
  int x, y;
} snake_food;
void spawnFoodOnEmptySquare(snake_food *food, snake_cell *head);

void drawSnake(snake_cell *head, bool is_dead);
void drawSnakeGame(snake_cell *head, snake_food food, int score);
void drawSnakeGameBorder(int score);
void playSnakeDeathAnimation(snake_cell *head, int score);
