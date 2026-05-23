#include "game.h"
#include <fstream>
#include <cstdlib>
#include <ctime>

static const int kBaseMaze[MAZE_ROWS][MAZE_COLS] =
{
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},

    {1,3,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,3,1},

    {1,0,1,1,1,1,0,1,1,0,1,0,1,1,0,1,1,1,1,0,1},

    {1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},

    {1,0,1,0,1,1,0,1,1,1,1,1,1,1,0,1,1,0,1,0,1},

    {1,0,0,0,1,0,0,0,0,1,0,1,0,0,0,0,1,0,0,0,1},

    {1,1,1,0,1,0,1,1,0,1,0,1,0,1,1,0,1,0,1,1,1},

    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},

    {1,0,1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,0,1},

    {1,0,0,0,1,0,0,0,0,0,2,0,0,0,0,0,1,0,0,0,1},

    {1,1,1,0,1,0,1,1,0,0,2,0,0,1,1,0,1,0,1,1,1},

    {1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1},

    {1,0,1,1,1,1,1,0,1,0,0,0,1,0,1,1,1,1,1,0,1},

    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},

    {1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1},

    {1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1},

    {1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1},

    {1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,1},

    {1,0,1,1,1,0,1,1,1,1,0,1,1,1,1,0,1,1,1,0,1},

    {1,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,1},

    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};
int maze[MAZE_ROWS][MAZE_COLS];

GameState gGame;
std::vector<Ghost> gGhosts;

static void applyLevelEdits(int level)
{
    if(level == 1)
    {
        maze[4][4] = 0;
        maze[4][16] = 0;
        maze[16][4] = 0;
        maze[16][16] = 0;
    }
    else if(level == 2)
    {
        maze[8][9] = 0;
        maze[8][11] = 0;
        maze[12][9] = 0;
        maze[12][11] = 0;
        maze[10][8] = 0;
        maze[10][12] = 0;
    }
    else if(level == 3)
    {
        maze[2][3] = 0;
        maze[2][17] = 0;
        maze[18][3] = 0;
        maze[18][17] = 0;
        maze[6][7] = 0;
        maze[6][13] = 0;
        maze[14][7] = 0;
        maze[14][13] = 0;
    }
}

static void resetMaze(int level)
{
    for(int i = 0; i < MAZE_ROWS; i++)
    {
        for(int j = 0; j < MAZE_COLS; j++)
            maze[i][j] = kBaseMaze[i][j];
    }

    applyLevelEdits(level);
}

static void initGhosts()
{
    gGhosts.clear();

    Ghost blinky;
    blinky.x = 10;
    blinky.y = 9;
    blinky.dirX = 0;
    blinky.dirY = -1;
    blinky.homeX = 10;
    blinky.homeY = 9;
    blinky.targetX = gGame.pacX;
    blinky.targetY = gGame.pacY;
    blinky.mode = GHOST_PATROL;
    blinky.pathCooldown = 0;
    blinky.name = "Blinky";
    blinky.colorR = 1.0f;
    blinky.colorG = 0.0f;
    blinky.colorB = 0.0f;
    gGhosts.push_back(blinky);

    Ghost pinky;
    pinky.x = 9;
    pinky.y = 11;
    pinky.dirX = 0;
    pinky.dirY = 1;
    pinky.homeX = 9;
    pinky.homeY = 11;
    pinky.targetX = gGame.pacX;
    pinky.targetY = gGame.pacY;
    pinky.mode = GHOST_PATROL;
    pinky.pathCooldown = 0;
    pinky.name = "Pinky";
    pinky.colorR = 1.0f;
    pinky.colorG = 0.5f;
    pinky.colorB = 0.7f;
    gGhosts.push_back(pinky);

    Ghost inky;
    inky.x = 11;
    inky.y = 11;
    inky.dirX = 0;
    inky.dirY = 1;
    inky.homeX = 11;
    inky.homeY = 11;
    inky.targetX = gGame.pacX;
    inky.targetY = gGame.pacY;
    inky.mode = GHOST_PATROL;
    inky.pathCooldown = 0;
    inky.name = "Inky";
    inky.colorR = 0.0f;
    inky.colorG = 0.9f;
    inky.colorB = 1.0f;
    gGhosts.push_back(inky);

    Ghost clyde;
    clyde.x = 10;
    clyde.y = 12;
    clyde.dirX = 0;
    clyde.dirY = 1;
    clyde.homeX = 10;
    clyde.homeY = 12;
    clyde.targetX = gGame.pacX;
    clyde.targetY = gGame.pacY;
    clyde.mode = GHOST_PATROL;
    clyde.pathCooldown = 0;
    clyde.name = "Clyde";
    clyde.colorR = 1.0f;
    clyde.colorG = 0.5f;
    clyde.colorB = 0.0f;
    gGhosts.push_back(clyde);
}
static int loadHighScore()
{
    std::ifstream fin("highscore.txt");
    int score = 0;

    if(fin >> score)
        return score;

    return 0;
}

static void saveHighScore(int score)
{
    std::ofstream fout("highscore.txt");
    fout << score;
}

void initGame()
{
    gGame.score = 0;
    gGame.lives = 3;
    gGame.paused = false;
    gGame.gameOver = false;
    gGame.gameWin = false;
    gGame.powerMode = false;
    gGame.powerTimer = 0;
    gGame.pacX = PAC_START_X;
    gGame.pacY = PAC_START_Y;
    gGame.pacDirX = 0;
    gGame.pacDirY = 0;
    gGame.tick = 0;

    gGame.screen = SCREEN_MENU;
    gGame.menuIndex = 0;
    gGame.highScore = loadHighScore();
    gGame.hasStarted = false;
    gGame.level = 0;

    resetMaze(gGame.level);
    initGhosts();

    std::srand((unsigned int)std::time(nullptr));
}

void setPacmanDirection(int dx, int dy)
{
    if(gGame.gameOver || gGame.gameWin)
        return;

    gGame.pacDirX = dx;
    gGame.pacDirY = dy;
}

void togglePause()
{
    if(gGame.screen != SCREEN_GAME)
        return;

    gGame.paused = !gGame.paused;
}

void openMenu()
{
    gGame.screen = SCREEN_MENU;
    gGame.menuIndex = 0;
    gGame.paused = false;
}

void openHighScore()
{
    gGame.screen = SCREEN_HIGHSCORE;
    gGame.paused = false;
}

void startNewGame()
{
    gGame.level = 0;
    resetMaze(gGame.level);

    gGame.score = 0;
    gGame.lives = 3;
    gGame.paused = false;
    gGame.gameOver = false;
    gGame.gameWin = false;
    gGame.powerMode = false;
    gGame.powerTimer = 0;
    gGame.pacX = PAC_START_X;
    gGame.pacY = PAC_START_Y;
    gGame.pacDirX = 0;
    gGame.pacDirY = 0;
    gGame.tick = 0;

    initGhosts();

    gGame.hasStarted = true;
    gGame.screen = SCREEN_GAME;
}

void resumeGame()
{
    if(!gGame.hasStarted)
    {
        startNewGame();
        return;
    }

    gGame.paused = false;
    gGame.screen = SCREEN_GAME;
}

void advanceLevel()
{
    gGame.level = (gGame.level + 1) % 4;
    resetMaze(gGame.level);

    gGame.gameOver = false;
    gGame.gameWin = false;
    gGame.powerMode = false;
    gGame.powerTimer = 0;
    gGame.pacX = PAC_START_X;
    gGame.pacY = PAC_START_Y;
    gGame.pacDirX = 0;
    gGame.pacDirY = 0;
    gGame.tick = 0;

    initGhosts();
}

void menuMove(int dir)
{
    const int menuCount = 3;
    gGame.menuIndex = (gGame.menuIndex + dir + menuCount) % menuCount;
}

void menuSelect()
{
    switch(gGame.menuIndex)
    {
        case 0:
            if(gGame.hasStarted)
                resumeGame();
            else
                startNewGame();
            break;

        case 1:
            openHighScore();
            break;

        case 2:
            std::exit(0);
            break;
    }
}

void movePacman()
{
    if(gGame.gameOver || gGame.paused || gGame.gameWin)
        return;

    if(gGame.pacDirX == 0 && gGame.pacDirY == 0)
        return;

    int nextX = gGame.pacX + gGame.pacDirX;
    int nextY = gGame.pacY + gGame.pacDirY;

    if(canMoveTo(nextX, nextY))
    {
        gGame.pacX = nextX;
        gGame.pacY = nextY;

        eatDots();
        checkCollision();
    }
}

void updateGame()
{
    if(gGame.screen != SCREEN_GAME)
        return;

    gGame.tick++;

    movePacman();
    moveGhosts();

    if(gGame.powerMode)
    {
        gGame.powerTimer--;

        if(gGame.powerTimer <= 0)
        {
            gGame.powerMode = false;
        }
    }

    checkCollision();
    validatePositions();
    if(gGame.score > gGame.highScore)
{
    gGame.highScore = gGame.score;
    saveHighScore(gGame.highScore);
}
    if(gGame.gameOver || gGame.gameWin)
    {
        if(gGame.score > gGame.highScore)
            gGame.highScore = gGame.score;
    }
}
