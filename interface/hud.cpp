#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#endif
#include <stdio.h>
#include <stdlib.h>

#include "hud.h"

HUD::HUD() {}

void HUD::drawHUD(GLubyte *img, int width, int height)
{
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
    glRasterPos2i(width, height);
    glPixelZoom(-1, 1);
    glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, img);
}

int HUD::iconSelect(int x, bool showSelectSquare)
{
    if (showSelectSquare)
    {
        //handle click
        if (0 <= x && x < 75) //save btn
        {
            drawClickSquare(0, 75);
            return 1;
        }
        else if (75 <= x && x < 167) //open btn
        {
            drawClickSquare(75, 167);
            return 2;
        }
        else if (167 <= x && x < 244) //cube
        {
            drawClickSquare(167, 244);
            return 3;
        }
        else if (244 <= x && x < 317) //sphere
        {
            drawClickSquare(244, 317);
            return 4;
        }
        else if (317 <= x && x < 388) //subdivision
        {
            drawClickSquare(317, 388);
            return 5;
        }
    }
    return -1;
}
void HUD::iconHover(int x, bool showHoverSquare)
{
    //handle hover
    if (showHoverSquare)
    {
        if (0 <= x && x < 75) //save btn
        {
            drawHoverSquare(0, 75);
        }
        else if (75 <= x && x < 167) //open btn
        {
            drawHoverSquare(75, 167);
        }
        else if (167 <= x && x < 244) //cube
        {
            drawHoverSquare(167, 244);
        }
        else if (244 <= x && x < 317) //sphere
        {
            drawHoverSquare(244, 317);
        }
        else if (317 <= x && x < 388) //subdivision
        {
            drawHoverSquare(317, 388);
        }
    }
}
//show when the user is hovering over an icon
//might need a material to show the color due to lighting?
void HUD::drawHoverSquare(int lowerX, int upperX)
{
    glPushMatrix();
    glBegin(GL_QUADS);
    glColor4f(0.6, 0.6, 0.6, 0.5);
    glVertex3f(lowerX, 0, 0.5);
    glVertex3f(lowerX, 90, 0.5);
    glVertex3f(upperX, 90, 0.5);
    glVertex3f(upperX, 0, 0.5);
    glEnd();
    glPopMatrix();
    glutPostRedisplay();
}
//when the user clicks an icon
void HUD::drawClickSquare(int lowerX, int upperX)
{
    glBegin(GL_QUADS);
    glColor4f(0.4, 0.4, 0.4, 0.5);
    glVertex3f(lowerX, 0, 0.5);
    glVertex3f(lowerX, 90, 0.5);
    glVertex3f(upperX, 90, 0.5);
    glVertex3f(upperX, 0, 0.5);
    glEnd();
    glutPostRedisplay();
}
