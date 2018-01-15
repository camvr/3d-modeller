#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include <string>
#include <cstring>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#endif

// Include local modules
#include "mesh/mesh.h"
#include "tool/tool.h"
#include "interface/hud.h"

#define ZOOM_SPEED 0.3f
#define MIN_CAM_DIST 2.0f
#define MAX_CAM_DIST 30.0f

enum STATEenum {
    IDLE,
    SCULPT_TOOL_SELECTED,
    MOVE_TOOL_SELECTED,
    SELECT_TOOL_SELECTED,
    CAMERA_ROTATE,
    CAMERA_SHIFT
};

enum TOOLenum {
    SCULPTER,
    MOVER,
    SELECTOR
};

enum MATERIALenum { // i.e http://devernay.free.fr/cours/opengl/materials.html
    OBSIDIAN,
    PEARL,
    CHROME,
    BRASS,
    BLACK_PLASTIC,
    WHITE_PLASTIC,
    BLACK_RUBBER
};

struct Material {
    float amb[3];
    float dif[3];
    float spc[3];
    float shine;
};

// Material list
Material materials[] = {{{0.05375,0.05,0.06625},{0.18275,0.17,0.22525},{0.332741, 0.328634, 0.346435},0.3}, // Obsidian
                        {{0.25,0.20725,0.20725},{1,0.829,0.829},{0.296648,0.296648,0.296648},0.088}, // pearl
                        {{0.25,0.25,0.25},{0.4,0.4,0.4},{0.774597,0.774597,0.774597},0.6}, // chrome
                        {{0.329412,0.223529,0.027451},{0.780392,0.568627,0.113725},{0.992157,0.941176,0.807843},0.21794872}, // brass
                        {{0,0,0},{0.01,0.01,0.01},{0.5,0.5,0.5},0.25}, // black plastic
                        {{0,0,0},{0.55,0.55,0.55},{0.7,0.7,0.7},0.25}, // white plastic
                        {{0.02,0.02,0.02},{0.01,0.01,0.01},{0.4,0.4,0.4},0.078125}}; // black rubber

// Globals
STATEenum STATE = IDLE;

float camPos[] = {0, 0, 5};
float camTarget[] = {0, 0, 0};
float camDist = 5; // Set initially to z-component of camPos
float worldOffset[] = {0, 0};

double matModelView[16];
double matProjection[16];
int viewport[4];

float sceneRot[] = {0, 5.0f*sin(M_PI/2.0f), 0}; // corner view
int xprev = 0;
int yprev = 0;

MATERIALenum currMat;

HUD *hud;
GLubyte *image;
int width, height, max;
int winX = 1000;
int winY = 800;

// Control Variables and Options
TOOLenum currTool = SCULPTER;
std::string toolName = "Sculpt Tool";
bool showAxis = true;
bool showFloor = true;
bool perspective = true;
bool lockAxis[4] = {false, false, false, false};
int currObject = 0;

// Light position
float lightPos0[] = {5,5,5};

std::vector<mesh*> objects;
Tool *brush = new Tool();

// Functions
void keyboard(unsigned char key, int xIn, int yIn);
void special(int key, int xIn, int yIn);
void mouse(int btn, int state, int x, int y);
void mouseMotion(int x, int y);
void mousePassiveMotion(int x, int y);
void draw3dScene();
void display();
void reshape(int w, int h);
void FPSTimer(int value);
void printInstructions();
void init();

// Utility
std::vector<std::string> split(std::string str, std::string sep);
void drawText(std::string str, float x, float y, float z, float scale);

// Operational Functions
GLubyte * LoadPPM(char *file, int *width, int *height, int *max);
void loadFromFile(std::string file);
void saveToFile(std::string file);
#endif