#include "game.h"

#include <cstdio>

bool canMoveTo(int x, int y)
{
    if(x < 0 || x >= MAZE_COLS)
        return false;

    if(y < 0 || y >= MAZE_ROWS)
        return false;

    return maze[y][x] != 1;
}

void eatDots()
{
    if(maze[gGame.pacY][gGame.pacX] == 0)
    {
        maze[gGame.pacY][gGame.pacX] = 2;
        gGame.score += 10;
    }

    if(maze[gGame.pacY][gGame.pacX] == 3)
    {
        maze[gGame.pacY][gGame.pacX] = 2;
        gGame.score += 50;
        gGame.powerMode = true;
        gGame.powerTimer = 32;
    }

    bool allEaten = true;

    for(int i = 0; i < MAZE_ROWS; i++)
    {
        for(int j = 0; j < MAZE_COLS; j++)
        {
            if(maze[i][j] == 0 || maze[i][j] == 3)
                allEaten = false;
        }
    }

    if(allEaten)
        advanceLevel();
}

void checkCollision()
{
    for(size_t i = 0; i < gGhosts.size(); i++)
    {
        Ghost &ghost = gGhosts[i];

        if(gGame.pacX == ghost.x && gGame.pacY == ghost.y)
        {
            if(gGame.powerMode)
            {
                gGame.score += 200;
                ghost.x = ghost.homeX;
                ghost.y = ghost.homeY;
                ghost.dirX = 0;
                ghost.dirY = -1;

                std::printf("[COLLISION] Pacman ate %s at (%d,%d)\n", ghost.name.c_str(), ghost.x, ghost.y);
            }
            else
            {   if(ghost.name=="Blinky"){
                continue;
                }
                gGame.lives--;
                gGame.pacX = PAC_START_X;
                gGame.pacY = PAC_START_Y;
                gGame.pacDirX = 0;
                gGame.pacDirY = 0;

                for(size_t g = 0; g < gGhosts.size(); g++)
                {
                    gGhosts[g].x = gGhosts[g].homeX;
                    gGhosts[g].y = gGhosts[g].homeY;
                    gGhosts[g].dirX = 0;
                    gGhosts[g].dirY = -1;
                }

                std::printf("[COLLISION] Ghost hit Pacman. Lives left: %d\n", gGame.lives);

                if(gGame.lives <= 0)
                    gGame.gameOver = true;
            }
        }
    }
}

void validatePositions()
{
    if(!canMoveTo(gGame.pacX, gGame.pacY))
    {
        std::printf("[DEBUG] Invalid Pacman position (%d,%d)\n", gGame.pacX, gGame.pacY);
        gGame.pacX = PAC_START_X;
        gGame.pacY = PAC_START_Y;
        gGame.pacDirX = 0;
        gGame.pacDirY = 0;
    }

    for(size_t i = 0; i < gGhosts.size(); i++)
    {
        Ghost &ghost = gGhosts[i];

        if(!canMoveTo(ghost.x, ghost.y))
        {
            std::printf("[DEBUG] Invalid ghost position %s (%d,%d)\n", ghost.name.c_str(), ghost.x, ghost.y);
            ghost.x = ghost.homeX;
            ghost.y = ghost.homeY;
            ghost.dirX = 0;
            ghost.dirY = -1;
        }
    }
}
