#include "edge.h"

edge::edge(vertex *a, vertex *b)
{
    vertices.push_back(a);
    vertices.push_back(b);
}

std::vector<vertex *> edge::getVertices()
{
    return vertices;
}

void edge::move(float dx, float dy, float dz)
{
    vertices[0]->move(dx, dy, dz);
    vertices[1]->move(dx, dy, dz);
}