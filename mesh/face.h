#ifndef __FACE_H__
#define __FACE_H__

#include "stdio.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "vertex.h"
#include "edge.h"
class vertex;
class edge;

class face
{

  public:
    face(vertex *i, vertex *j, vertex *k, vertex *l, int a, int b, int c, int d);
    std::vector<vertex *> getVertices();
    std::vector<edge *> getEdges();
    float *getNormal();
    void calcNormal();
    int *getIndices();
    float *getTop();
    float *getBottom();
    void move(float dx, float dy, float dz);

  private:
    std::vector<edge *> edges;
    std::vector<vertex *> vertices;
    std::vector<int> indices;
    std::vector<float> normal;
    std::vector<float> top;
    std::vector<float> bottom;
};
#endif
