#include "face.h"

face::face(vertex *i, vertex *j, vertex *k, vertex *l, int a, int b, int c, int d)
{
    i->addFace(this);
    j->addFace(this);
    k->addFace(this);
    l->addFace(this);
    edges.push_back(new edge(i, j));
    edges.push_back(new edge(j, k));
    edges.push_back(new edge(k, l));
    edges.push_back(new edge(l, i));
    indices.push_back(a);
    indices.push_back(b);
    indices.push_back(c);
    indices.push_back(d);
    vertices.push_back(i);
    vertices.push_back(j);
    vertices.push_back(k);
    vertices.push_back(l);
    normal.push_back(0);
    normal.push_back(0);
    normal.push_back(0);
}

std::vector<vertex *> face::getVertices()
{
    return vertices;
}

std::vector<edge *> face::getEdges()
{
    return edges;
}

float *face::getNormal()
{
    return normal.data();
}

int *face::getIndices()
{
    return indices.data();
}

void face::move(float dx, float dy, float dz)
{
    vertices[0]->move(dx, dy, dz);
    vertices[1]->move(dx, dy, dz);
    vertices[2]->move(dx, dy, dz);
    vertices[3]->move(dx, dy, dz);
}

void face::calcNormal()
{
    float ax, ay, az, bx, by, bz, vLength, Nx, Ny, Nz;
    ax = vertices[0]->getVertex()[0] - vertices[1]->getVertex()[0];
    ay = vertices[0]->getVertex()[1] - vertices[1]->getVertex()[1];
    az = vertices[0]->getVertex()[2] - vertices[1]->getVertex()[2];

    bx = vertices[0]->getVertex()[0] - vertices[3]->getVertex()[0];
    by = vertices[0]->getVertex()[1] - vertices[3]->getVertex()[1];
    bz = vertices[0]->getVertex()[2] - vertices[3]->getVertex()[2];

    Nx = ay * bz - az * by;
    Ny = az * bx - ax * bz;
    Nz = ax * by - ay * bx;
    vLength = sqrt(Nx * Nx + Ny * Ny + Nz * Nz);
    normal[0] = Nx / vLength;
    normal[1] = Ny / vLength;
    normal[2] = Nz / vLength;
}