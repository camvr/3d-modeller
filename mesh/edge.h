#ifndef __EDGE_H__
#define __EDGE_H__

#include "stdio.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "vertex.h"

class vertex;

class edge{
public:
    edge(vertex* a, vertex* b);
    std::vector<vertex*> getVertices();
    void move(float dx, float dy, float dz);
    float* getTop();
    float* getBottom();

private:
    std::vector<vertex*> vertices;
    std::vector<float> top;
    std::vector<float> bottom;
};
#endif