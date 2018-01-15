#ifndef __TOOL_H__
#define __TOOL_H__

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#endif

#include <vector>
#include <math.h>
#include <stdio.h>
#include "../mesh/mesh.h"

class mesh;

class Tool {
public:
    Tool();
    
    std::vector<int> getSelectedVertices(mesh *m, double modelview[16], double projection[16], int viewport[4], int x, int y);
    int selectObject(std::vector<mesh*> *objects, double modelview[16], double projection[16], int viewport[4], int x, int y);
    float getRad();
    void setRad(float r);

private:
    void vertexPicker(std::vector<int> *selected, std::vector<float*> *vert, mesh *m, double modelview[16], double projection[16], int viewport[4]);
    bool checkSlabIntersect(double s, double e, float min, float max, float& Tnear, float& Tfar);
    bool checkBoxIntersect(double* s, double* e, float* min, float* max, float& intersect);
    float toolRad = 0.25f;
};

#endif