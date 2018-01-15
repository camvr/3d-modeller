#ifndef __VERTEX_H__
#define __VERTEX_H__

#include "stdio.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include "face.h"

class face;

class vertex
{
public:
  vertex(float a, float b, float c);
  vertex(float *a);
  float *getVertex();
  float *getNormal();
  std::vector<face *> getFaces();
  void addFace(face *f);
  void move(float a, float b, float c);
  void calcNormal();
  bool equal(vertex *a);

private:
  std::vector<float> xyz;
  std::vector<face *> faces;
  std::vector<float> normal;
};
#endif