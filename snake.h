#define FRAME_RATE 33333
#define waitNextFrame() usleep(FRAME_RATE)
#define MAX_HIGH_SCORES 10

void init();
void startGameLoop();
void cleanup();
void mainMenu();
void gameOver(int score);

enum MoveDirection { UP, DOWN, LEFT, RIGHT };

typedef struct high_score_node_t {
  const char *name;
  int score;
  struct high_score_node_t *next;
  struct high_score_node_t *prev;
} high_score_node_t;

typedef struct snake_state_t {
  high_score_node_t *high_score_head;
  int snake_speed;
} snake_save;

typedef struct snake_cell_t {
  int x, y;
  struct snake_cell_t *next;
} snake_cell_t;
