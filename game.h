#pragma once

#include <vector>
#include <string>

constexpr int WIDTH = 800;
constexpr int HEIGHT = 800;
constexpr int CELL = 32;
constexpr int MAZE_ROWS = 21;
constexpr int MAZE_COLS = 21;
constexpr int UI_X = MAZE_COLS * CELL + 20;
constexpr int PAC_START_X = 10;
constexpr int PAC_START_Y = 17;

enum GhostMode
{
    GHOST_CHASE = 0,
    GHOST_PATROL = 1
};

enum ScreenType
{
    SCREEN_MENU = 0,
    SCREEN_GAME = 1,
    SCREEN_HIGHSCORE = 2
};

struct Ghost
{
    int x;
    int y;
    int dirX;
    int dirY;
    int homeX;
    int homeY;
    int targetX;
    int targetY;
    int mode;
    int pathCooldown;
    std::string name;
    float colorR;
    float colorG;
    float colorB;
};

struct GameState
{
    int score;
    int lives;
    bool paused;
    bool gameOver;
    bool gameWin;
    bool powerMode;
    int powerTimer;
    int pacX;
    int pacY;
    int pacDirX;
    int pacDirY;
    int tick;
    int screen;
    int menuIndex;
    int highScore;
    bool hasStarted;
    int level;
};

extern int maze[MAZE_ROWS][MAZE_COLS];
extern GameState gGame;
extern std::vector<Ghost> gGhosts;

void initGame();
void updateGame();
void setPacmanDirection(int dx, int dy);
void togglePause();
void openMenu();
void openHighScore();
void startNewGame();
void resumeGame();
void menuMove(int dir);
void menuSelect();
void advanceLevel();

void renderDisplay();
void initRendering();

bool canMoveTo(int x, int y);
void eatDots();
void checkCollision();
void validatePositions();

void movePacman();
void moveGhosts();
