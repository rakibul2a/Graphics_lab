#include "game.h"

#include <queue>
#include <cstdlib>
#include <cstdio>
#include <climits>

static int manhattanDist(int x1, int y1, int x2, int y2)
{
    int dx = x1 - x2;
    int dy = y1 - y2;

    if(dx < 0) dx = -dx;
    if(dy < 0) dy = -dy;

    return dx + dy;
}

static bool isReverse(const Ghost &ghost, int dx, int dy)
{
    return dx == -ghost.dirX && dy == -ghost.dirY;
}

static void getValidDirs(int x, int y, std::vector<std::pair<int,int>> &dirs)
{
    dirs.clear();

    if(canMoveTo(x, y - 1)) dirs.push_back({0, -1});
    if(canMoveTo(x, y + 1)) dirs.push_back({0, 1});
    if(canMoveTo(x - 1, y)) dirs.push_back({-1, 0});
    if(canMoveTo(x + 1, y)) dirs.push_back({1, 0});
}

static bool bfsNextStep(int startX, int startY, int targetX, int targetY, int &outDx, int &outDy)
{
    if(!canMoveTo(targetX, targetY))
        return false;

    int dist[MAZE_ROWS][MAZE_COLS];

    for(int i = 0; i < MAZE_ROWS; i++)
    {
        for(int j = 0; j < MAZE_COLS; j++)
            dist[i][j] = -1;
    }

    std::queue<std::pair<int,int>> q;
    dist[targetY][targetX] = 0;
    q.push({targetX, targetY});

    while(!q.empty())
    {
        std::pair<int,int> cell = q.front();
        q.pop();

        int x = cell.first;
        int y = cell.second;

        const int dirs[4][2] = { {0,-1},{0,1},{-1,0},{1,0} };

        for(int i = 0; i < 4; i++)
        {
            int nx = x + dirs[i][0];
            int ny = y + dirs[i][1];

            if(canMoveTo(nx, ny) && dist[ny][nx] == -1)
            {
                dist[ny][nx] = dist[y][x] + 1;
                q.push({nx, ny});
            }
        }
    }

    int bestDist = INT_MAX;
    int bestDx = 0;
    int bestDy = 0;

    const int dirs[4][2] = { {0,-1},{0,1},{-1,0},{1,0} };

    for(int i = 0; i < 4; i++)
    {
        int nx = startX + dirs[i][0];
        int ny = startY + dirs[i][1];

        if(canMoveTo(nx, ny) && dist[ny][nx] >= 0)
        {
            if(dist[ny][nx] < bestDist)
            {
                bestDist = dist[ny][nx];
                bestDx = dirs[i][0];
                bestDy = dirs[i][1];
            }
        }
    }

    if(bestDist == INT_MAX)
        return false;

    outDx = bestDx;
    outDy = bestDy;
    return true;
}

static void chooseRandomDirection(Ghost &ghost)
{
    std::vector<std::pair<int,int>> dirs;
    getValidDirs(ghost.x, ghost.y, dirs);

    if(dirs.empty())
        return;

    if(dirs.size() > 1)
    {
        for(size_t i = 0; i < dirs.size();)
        {
            if(isReverse(ghost, dirs[i].first, dirs[i].second))
                dirs.erase(dirs.begin() + i);
            else
                i++;
        }

        if(dirs.empty())
            getValidDirs(ghost.x, ghost.y, dirs);
    }

    int idx = std::rand() % (int)dirs.size();
    ghost.dirX = dirs[idx].first;
    ghost.dirY = dirs[idx].second;
    ghost.targetX = ghost.x + ghost.dirX;
    ghost.targetY = ghost.y + ghost.dirY;

    std::printf("[AI] %s random dir (%d,%d) target (%d,%d)\n", ghost.name.c_str(), ghost.dirX, ghost.dirY, ghost.targetX, ghost.targetY);
}

static void chooseChaseDirection(Ghost &ghost)
{
    int dx = 0;
    int dy = 0;

    if(bfsNextStep(ghost.x, ghost.y, gGame.pacX, gGame.pacY, dx, dy))
    {
        if(isReverse(ghost, dx, dy))
        {
            std::vector<std::pair<int,int>> dirs;
            getValidDirs(ghost.x, ghost.y, dirs);

            for(size_t i = 0; i < dirs.size(); i++)
            {
                if(!isReverse(ghost, dirs[i].first, dirs[i].second))
                {
                    dx = dirs[i].first;
                    dy = dirs[i].second;
                    break;
                }
            }
        }

        ghost.dirX = dx;
        ghost.dirY = dy;
        ghost.targetX = gGame.pacX;
        ghost.targetY = gGame.pacY;

        std::printf("[AI] %s chase dir (%d,%d) target (%d,%d)\n", ghost.name.c_str(), ghost.dirX, ghost.dirY, ghost.targetX, ghost.targetY);
    }
    else
    {
        chooseRandomDirection(ghost);
    }
}

void moveGhosts()
{
    if(gGame.gameOver || gGame.paused || gGame.gameWin)
        return;

    for(size_t i = 0; i < gGhosts.size(); i++)
    {
        Ghost &ghost = gGhosts[i];

        if(ghost.pathCooldown > 0)
            ghost.pathCooldown--;

        int distance = manhattanDist(ghost.x, ghost.y, gGame.pacX, gGame.pacY);

        if(ghost.pathCooldown <= 0 || !canMoveTo(ghost.x + ghost.dirX, ghost.y + ghost.dirY))
        {
            if(ghost.mode == GHOST_CHASE)
            {
                chooseChaseDirection(ghost);
            }
            else
            {
                if(distance > 8)
                    chooseRandomDirection(ghost);
                else
                    chooseChaseDirection(ghost);
            }

            ghost.pathCooldown = (gGame.score >= 500) ? 1 : 7;
        }

        int nextX = ghost.x + ghost.dirX;
        int nextY = ghost.y + ghost.dirY;

        if(canMoveTo(nextX, nextY))
        {
            ghost.x = nextX;
            ghost.y = nextY;
        }
        else
        {
            ghost.pathCooldown = 0;
        }
    }
}
