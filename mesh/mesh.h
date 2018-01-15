#ifndef __MESH_H__
#define __MESH_H__

#include "stdio.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

#include "face.h"
#include "vertex.h"
#include "edge.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#endif

class vertex;
class face;
class edge;

class mesh {
public:
    mesh(int x);
    mesh(std::vector<float*> v, std::vector<int*> i, std::vector<float*> n); // overload for constructing from file data
    mesh(std::vector<vertex*> v, std::vector<face*> f);
    void subdivideMesh();
    void subdivideSelected();
    void setSelectedV(std::vector<int> s);
    void setSelectedF(std::vector<int> s);
    void unSelect();
    void setVertices(std::vector<float*> v);
    void moveSelected(float dx, float dy, float dz);
    std::vector<float*> getVertices();
    std::vector<std::vector<float *> > getFaces();
    std::vector<int*> getIndices();
    std::vector<float*> getNormals();
    std::vector<face*> getFaceList();
    float* getPos();
    void movePos(float dx, float dy, float dz);
    void draw(bool meshOnly);
    void extrudeFace();
    void resize(float x);
    float** getBoundingBox();
private:
    float pos[3] = {0, 0, 0}; // center point of mesh

    std::vector<vertex*> vertices;
    std::vector<edge*> edges;
    std::vector<face*> faces;

    void subdivideFace(int f);
    float* halfPoint(float *a, float *b);
    void calcNormals();
    void createCube();
    void createSphere();

    std::vector<int*> indices;
    std::vector<float*> normals;
    std::vector<int> selectedV;
    std::vector<int> selectedF;
    std::vector<int> selectedE;
};

#endif