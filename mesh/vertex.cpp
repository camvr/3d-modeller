#include "vertex.h"

vertex::vertex(float a, float b, float c)
{
    xyz.push_back(a);
    xyz.push_back(b);
    xyz.push_back(c);
    normal.push_back(0);
    normal.push_back(0);
    normal.push_back(0);
}

vertex::vertex(float *a)
{
    xyz.push_back(a[0]);
    xyz.push_back(a[1]);
    xyz.push_back(a[2]);
    normal.push_back(0);
    normal.push_back(0);
    normal.push_back(0);
}

float *vertex::getVertex()
{
    return xyz.data();
}

float *vertex::getNormal()
{
    return normal.data();
}

std::vector<face *> vertex::getFaces()
{
    return faces;
}

void vertex::addFace(face *f)
{
    faces.push_back(f);
}

void vertex::move(float a, float b, float c)
{
    xyz[0] += a;
    xyz[1] += b;
    xyz[2] += c;
}

void vertex::calcNormal()
{
    if (!faces.empty())
    {
        double sumx, sumy, sumz, vLength;
        for (face *f : faces)
        {
            sumx += double(f->getNormal()[0]);
            sumy += double(f->getNormal()[1]);
            sumz += double(f->getNormal()[2]);
        }
        vLength = sqrt(sumx * sumx + sumy * sumy + sumz * sumz);
        normal[0] = sumx / vLength;
        normal[1] = sumy / vLength;
        normal[2] = sumz / vLength;
    }
}

bool vertex::equal(vertex* a)
{
    return xyz[0]==a->getVertex()[0] && xyz[1]==a->getVertex()[1] && xyz[2]==a->getVertex()[2];
}
