#ifndef __HUD_H__
#define __HUD_H__

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#endif


class HUD {
public:
   HUD();

   void drawHUD(GLubyte* img, int width, int height);
   int iconSelect(int x, bool showSelectSquare);
   void iconHover(int x, bool showHoverSquare);

private:
   GLubyte *HUDTexture;
   void drawClickSquare(int lowerX, int upperX);
   void drawHoverSquare(int lowerX, int upperX);
};

#endif

