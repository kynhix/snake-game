#define FRAME_RATE 33333
#define waitNextFrame() usleep(FRAME_RATE)
#define MAX_HIGH_SCORES 10

void init();
void startGameLoop();
void cleanup();
void mainMenu();
void gameOver(int score);

enum MoveDirection { UP, DOWN, LEFT, RIGHT };

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

typedef struct snake_food {
  int x, y;
} snake_food;
