#include "game.h"

#include <GL/glut.h>
#include <cstdlib>

static void displayCallback()
{
    renderDisplay();
}

static void timerCallback(int)
{
    updateGame();
    glutPostRedisplay();
    glutTimerFunc(350, timerCallback, 0);
}

static void keyboardCallback(int key, int, int)
{
    if(gGame.screen == SCREEN_MENU)
    {
        if(key == GLUT_KEY_UP)
            menuMove(-1);
        else if(key == GLUT_KEY_DOWN)
            menuMove(1);

        return;
    }

    if(gGame.screen == SCREEN_HIGHSCORE)
        return;

    switch(key)
    {
        case GLUT_KEY_UP:
            setPacmanDirection(0, -1);
            break;

        case GLUT_KEY_DOWN:
            setPacmanDirection(0, 1);
            break;

        case GLUT_KEY_LEFT:
            setPacmanDirection(-1, 0);
            break;

        case GLUT_KEY_RIGHT:
            setPacmanDirection(1, 0);
            break;
    }
}

static void normalKeysCallback(unsigned char key, int, int)
{
    switch(key)
    {
        case 13:
            if(gGame.screen == SCREEN_MENU)
                menuSelect();
            break;

        case 'b':
        case 'B':
            if(gGame.screen == SCREEN_HIGHSCORE)
                openMenu();
            break;

        case 'm':
        case 'M':
            if(gGame.screen == SCREEN_GAME)
                openMenu();
            break;

        case 'p':
        case 'P':
            togglePause();
            break;

        case 27:
            std::exit(0);
    }
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Pacman GLUT");

    initRendering();
    initGame();

    glutDisplayFunc(displayCallback);
    glutSpecialFunc(keyboardCallback);
    glutKeyboardFunc(normalKeysCallback);
    glutTimerFunc(0, timerCallback, 0);

    glutMainLoop();
    return 0;
}

