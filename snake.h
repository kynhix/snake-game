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
  const char *name;
  int score;
  struct high_score_node *next;
  struct high_score_node *prev;
} high_score_node;

typedef struct snake_state {
  high_score_node *high_score_head;
  int snake_speed;
} snake_save;
