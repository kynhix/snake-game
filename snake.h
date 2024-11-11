#define FRAME_RATE 33333
#define waitNextFrame() usleep(FRAME_RATE);

void init();
void startGameLoop();
void cleanup();
void mainMenu();

enum MoveDirection { UP, DOWN, LEFT, RIGHT };
