#include "game.h"

#include <GL/glut.h>
#include <cmath>

static void drawText(float x, float y, const std::string &text)
{
    glRasterPos2f(x, y);

    for(char c : text)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
}

static void plotCirclePoint(int x, int y, int cx, int cy)
{
    glVertex2i(cx + x, cy + y);
    glVertex2i(cx + x, cy - y);
    glVertex2i(cx - x, cy + y);
    glVertex2i(cx - x, cy - y);
    glVertex2i(cx + y, cy + x);
    glVertex2i(cx - y, cy + x);
    glVertex2i(cx + y, cy - x);
    glVertex2i(cx - y, cy - x);
}

static void drawCircleMidpoint(int cx, int cy, int r)
{
    int x = 0;
    int y = r;
    int decision = 1 - r;

    glBegin(GL_POINTS);

    plotCirclePoint(x, y, cx, cy);

    while(y > x)
    {
        if(decision < 0)
        {
            x++;
            decision += 2 * x + 1;
        }
        else
        {
            x++;
            y--;
            decision += 2 * (x - y) + 1;
        }

        plotCirclePoint(x, y, cx, cy);
    }

    glEnd();
}

static void drawLineBresenham(int x1, int y1, int x2, int y2)
{
    int dx = std::abs(x2 - x1);
    int dy = std::abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    glBegin(GL_POINTS);

    while(true)
    {
        glVertex2i(x1, y1);

        if(x1 == x2 && y1 == y2)
            break;

        int e2 = 2 * err;

        if(e2 > -dy)
        {
            err -= dy;
            x1 += sx;
        }

        if(e2 < dx)
        {
            err += dx;
            y1 += sy;
        }
    }

    glEnd();
}

static void getWallColor(float &r, float &g, float &b)
{
    static const float colors[4][3] =
    {
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.6f, 0.2f},
        {1.0f, 0.5f, 0.0f},
        {0.7f, 0.2f, 0.8f}
    };

    int level = gGame.level % 4;
    r = colors[level][0];
    g = colors[level][1];
    b = colors[level][2];
}

static void drawMaze()
{
    for(int i = 0; i < MAZE_ROWS; i++)
    {
        for(int j = 0; j < MAZE_COLS; j++)
        {
            float x = j * CELL;
            float y = i * CELL;

            if(maze[i][j] == 1)
            {
                float r = 0.0f;
                float g = 0.0f;
                float b = 1.0f;

                getWallColor(r, g, b);
                glColor3f(r, g, b);

                glBegin(GL_QUADS);

                glVertex2f(x,y);
                glVertex2f(x+CELL,y);
                glVertex2f(x+CELL,y+CELL);
                glVertex2f(x,y+CELL);

                glEnd();

                glColor3f(r, g, b);
                glPointSize(2.0f);

                int x0 = (int)x;
                int y0 = (int)y;
                int x1 = (int)(x + CELL);
                int y1 = (int)(y + CELL);

                drawLineBresenham(x0, y0, x1, y0);
                drawLineBresenham(x1, y0, x1, y1);
                drawLineBresenham(x1, y1, x0, y1);
                drawLineBresenham(x0, y1, x0, y0);
            }
            else if(maze[i][j] == 0)
            {
                glColor3f(1,1,1);
                glPointSize(2.0f);

                drawCircleMidpoint(
                    (int)(x + CELL/2),
                    (int)(y + CELL/2),
                    4
                );
            }
            else if(maze[i][j] == 3)
            {
                glColor3f(1,0,1);
                glPointSize(3.0f);

                drawCircleMidpoint(
                    (int)(x + CELL/2),
                    (int)(y + CELL/2),
                    8
                );
            }
        }
    }
}

static void drawPacman()
{
    glColor3f(1,1,0);

    float x = gGame.pacX * CELL + CELL/2;
    float y = gGame.pacY * CELL + CELL/2;

    float angleOffset = 0.0f;

    if(gGame.pacDirX == -1)
        angleOffset = 180.0f;
    else if(gGame.pacDirX == 1)
        angleOffset = 0.0f;
    else if(gGame.pacDirY == -1)
        angleOffset = -90.0f;
    else if(gGame.pacDirY == 1)
        angleOffset = 90.0f;

    glBegin(GL_TRIANGLE_FAN);

    glVertex2f(x,y);

    for(int i = 30; i < 330; i++)
    {
        float angle = (i + angleOffset) * 3.1416f / 180.0f;

        glVertex2f(
            x + cos(angle)*15,
            y + sin(angle)*15
        );
    }

    glEnd();
}

static void drawGhosts()
{
    for(size_t i = 0; i < gGhosts.size(); i++)
    {
        const Ghost &ghost = gGhosts[i];

        if(gGame.powerMode)
            glColor3f(0,0,1);
        else
            glColor3f(ghost.colorR, ghost.colorG, ghost.colorB);

        float x = ghost.x * CELL + CELL/2;
        float y = ghost.y * CELL + CELL/2;

        glPointSize(2.0f);
        drawCircleMidpoint((int)x,(int)y,15);

        glColor3f(1,1,1);
        glPointSize(2.0f);
        drawCircleMidpoint((int)(x-5),(int)(y+3),2);
        drawCircleMidpoint((int)(x+5),(int)(y+3),2);
    }
}

static void drawMenu()
{
    glColor3f(1,1,0);
    drawText(300,160,"PACMAN");

    const char *startLabel = gGame.hasStarted ? "Resume" : "Start";

    glColor3f(gGame.menuIndex == 0 ? 1.0f : 1.0f, gGame.menuIndex == 0 ? 1.0f : 1.0f, gGame.menuIndex == 0 ? 0.0f : 1.0f);
    drawText(300,240,startLabel);

    glColor3f(gGame.menuIndex == 1 ? 1.0f : 1.0f, gGame.menuIndex == 1 ? 1.0f : 1.0f, gGame.menuIndex == 1 ? 0.0f : 1.0f);
    drawText(300,280,"High Score");

    glColor3f(gGame.menuIndex == 2 ? 1.0f : 1.0f, gGame.menuIndex == 2 ? 1.0f : 1.0f, gGame.menuIndex == 2 ? 0.0f : 1.0f);
    drawText(300,320,"Exit");

    glColor3f(1,1,1);
    drawText(220,380,"Enter: Select  |  Esc: Exit");
}

static void drawHighScore()
{
    glColor3f(0,1,1);
    drawText(280,200,"HIGH SCORE");

    glColor3f(1,1,1);
    drawText(320,260,std::to_string(gGame.highScore));
    drawText(240,320,"Press B to return");
}

void renderDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT);

    if(gGame.screen == SCREEN_MENU)
    {
        drawMenu();
        glutSwapBuffers();
        return;
    }

    if(gGame.screen == SCREEN_HIGHSCORE)
    {
        drawHighScore();
        glutSwapBuffers();
        return;
    }

    drawMaze();
    drawPacman();
    drawGhosts();

    glColor3f(1,1,1);

    drawText(UI_X,100,"Score: " + std::to_string(gGame.score));
    drawText(UI_X,140,"Lives: " + std::to_string(gGame.lives));

    if(gGame.powerMode)
    {
        glColor3f(1,0,1);
        drawText(UI_X,180,"POWER MODE");
    }

    if(gGame.paused)
    {
        glColor3f(1,1,1);
        drawText(UI_X,220,"PAUSED");
    }

    if(gGame.gameOver)
    {
        glColor3f(1,0,0);
        drawText(300,300,"GAME OVER");
    }

    if(gGame.gameWin)
    {
        glColor3f(0,1,0);
        drawText(300,300,"YOU WIN!");
    }

    glutSwapBuffers();
}

void initRendering()
{
    glClearColor(0,0,0,1);
    gluOrtho2D(0,WIDTH,HEIGHT,0);
}
