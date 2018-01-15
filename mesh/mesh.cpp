#include "mesh.h"

mesh::mesh(int x)
{
    switch (x)
    {
    case 0:
        createCube();
        break;

    case 1:
        createSphere();
        break;
    }
}

mesh::mesh(std::vector<float *> v, std::vector<int *> i, std::vector<float *> n) //face or vertex normals??
{
    for (float *c : v)
    {
        vertices.push_back(new vertex(c[0], c[1], c[2]));
    }
    for (int *c : i)
    {
        faces.push_back(new face(
            vertices[c[0]],
            vertices[c[1]],
            vertices[c[2]],
            vertices[c[3]],
            c[0], c[1], c[2], c[3]));
    }
    normals = n;
    calcNormals();
}

mesh::mesh(std::vector<vertex*> v, std::vector<face*> f)
{
    vertices = v;
    faces = f;
    calcNormals();
}

void mesh::subdivideMesh()
{
    int f = faces.size();
    for (int i = 0; i < f; i++)
    {
        subdivideFace(i);
    }
    calcNormals();
}

// works, but because of no N sided polygons, creates break in object
void mesh::subdivideSelected()
{
    for (int i : selectedF)
    {
        subdivideFace(i);
    }
    calcNormals();
}

void mesh::calcNormals()
{
    for (face *f : faces)
    {
        f->calcNormal();
    }
    for (vertex *v : vertices)
    {
        v->calcNormal();
    }
}

void mesh::extrudeFace()
{
    for (int i : selectedF)
    {
        for (vertex *v : faces[i]->getVertices())
        {
            vertices.push_back(new vertex(v->getVertex()));
        }

        int vSize = vertices.size();

        faces.push_back(new face(
            vertices[vSize - 1], vertices[vSize - 2], faces[i]->getVertices()[2], faces[i]->getVertices()[3],
            vSize - 1, vSize - 2, faces[i]->getIndices()[2], faces[i]->getIndices()[3]));

        faces.push_back(new face(
            vertices[vSize - 2], vertices[vSize - 3], faces[i]->getVertices()[1], faces[i]->getVertices()[2],
            vSize - 2, vSize - 3, faces[i]->getIndices()[1], faces[i]->getIndices()[2]));

        faces.push_back(new face(
            vertices[vSize - 3], vertices[vSize - 4], faces[i]->getVertices()[0], faces[i]->getVertices()[1],
            vSize - 3, vSize - 4, faces[i]->getIndices()[0], faces[i]->getIndices()[1]));

        faces.push_back(new face(
            vertices[vSize - 4], vertices[vSize - 1], faces[i]->getVertices()[3], faces[i]->getVertices()[0],
            vSize - 4, vSize - 1, faces[i]->getIndices()[3], faces[i]->getIndices()[0]));

        faces[i] = new face(vertices[vSize - 4], vertices[vSize - 3], vertices[vSize - 2], vertices[vSize - 1], vSize - 4, vSize - 3, vSize - 2, vSize - 1);
        faces[i]->calcNormal();
        faces[i]->move(faces[i]->getNormal()[0] / 10, faces[i]->getNormal()[1] / 10, faces[i]->getNormal()[2] / 10);
    }
    calcNormals();
}

void mesh::resize(float x)
{
    for (vertex *v : vertices)
    {
        v->move(
            v->getVertex()[0] * x,
            v->getVertex()[1] * x,
            v->getVertex()[2] * x);
    }
    //calcNormals();
}

void mesh::subdivideFace(int f)
{
    int *ind = faces[f]->getIndices();
    int *vInd = new int[5];
    std::vector<vertex *> temp;
    temp.push_back(new vertex(halfPoint(faces[f]->getVertices()[1]->getVertex(), faces[f]->getVertices()[0]->getVertex())));
    temp.push_back(new vertex(halfPoint(faces[f]->getVertices()[2]->getVertex(), faces[f]->getVertices()[1]->getVertex())));
    temp.push_back(new vertex(halfPoint(faces[f]->getVertices()[3]->getVertex(), faces[f]->getVertices()[2]->getVertex())));
    temp.push_back(new vertex(halfPoint(faces[f]->getVertices()[0]->getVertex(), faces[f]->getVertices()[3]->getVertex())));
    temp.push_back(new vertex(
        (temp[0]->getVertex()[0] + temp[1]->getVertex()[0] + temp[2]->getVertex()[0] + temp[3]->getVertex()[0]) / 4, // average 4 points to get center
        (temp[0]->getVertex()[1] + temp[1]->getVertex()[1] + temp[2]->getVertex()[1] + temp[3]->getVertex()[1]) / 4,
        (temp[0]->getVertex()[2] + temp[1]->getVertex()[2] + temp[2]->getVertex()[2] + temp[3]->getVertex()[2]) / 4));

    // check for existence of vertices TODO make more effecient
    bool a = false;
    int x = 0;
    for (int i = 0; i < 5; i++) // loop through all newly created vertices
    {
        for (vertex *v : vertices) // loop through all existing vertices
        {
            if (v->equal(temp[i])) // check for equivalence
            {
                vInd[i] = x;
                temp[i] = v;
                a = true;
            }
            x++;
        }
        if (!a) // temp wasn't in vertices list, add it
        {
            vertices.push_back(temp[i]);
            vInd[i] = vertices.size() - 1;
        }
        a = false;
        x = 0;
    }

    face *tempf = new face(
        vertices[ind[0]],
        temp[0],
        temp[4],
        temp[3],
        ind[0], vInd[0], vInd[4], vInd[3]);
    faces.push_back(new face(
        vertices[ind[1]],
        temp[1],
        temp[4],
        temp[0],
        ind[1], vInd[1], vInd[4], vInd[0]));
    faces.push_back(new face(
        vertices[ind[2]],
        temp[2],
        temp[4],
        temp[1],
        ind[2], vInd[2], vInd[4], vInd[1]));
    faces.push_back(new face(
        vertices[ind[3]],
        temp[3],
        temp[4],
        temp[2],
        ind[3], vInd[3], vInd[4], vInd[2]));
    faces[f] = tempf;

    // if you've got a face selected, add all of its subdivisions to you're selection
    if (std::find(selectedF.begin(), selectedF.end(), f) != selectedF.end())
    {
        selectedF.push_back(faces.size() - 1);
        selectedF.push_back(faces.size() - 2);
        selectedF.push_back(faces.size() - 3);
    }
}

float *mesh::halfPoint(float *a, float *b)
{
    float *c = new float[3];
    c[0] = (b[0] + a[0]) / 2;
    c[1] = (b[1] + a[1]) / 2;
    c[2] = (b[2] + a[2]) / 2;
    return c;
}

void mesh::createCube()
{
    // bottom
    vertices.push_back(new vertex(-0.5, -0.5, -0.5));
    vertices.push_back(new vertex(-0.5, -0.5, 0.5));
    vertices.push_back(new vertex(0.5, -0.5, 0.5));
    vertices.push_back(new vertex(0.5, -0.5, -0.5));

    // top
    vertices.push_back(new vertex(-0.5, 0.5, -0.5));
    vertices.push_back(new vertex(-0.5, 0.5, 0.5));
    vertices.push_back(new vertex(0.5, 0.5, 0.5));
    vertices.push_back(new vertex(0.5, 0.5, -0.5));

    faces.push_back(new face(vertices[0], vertices[3], vertices[2], vertices[1], 0, 3, 2, 1)); // bottom

    faces.push_back(new face(vertices[0], vertices[1], vertices[5], vertices[4], 0, 1, 5, 4)); // sides

    faces.push_back(new face(vertices[1], vertices[2], vertices[6], vertices[5], 1, 2, 6, 5));

    faces.push_back(new face(vertices[2], vertices[3], vertices[7], vertices[6], 2, 3, 7, 6));

    faces.push_back(new face(vertices[3], vertices[0], vertices[4], vertices[7], 3, 0, 4, 7));

    faces.push_back(new face(vertices[4], vertices[5], vertices[6], vertices[7], 4, 5, 6, 7)); // top

    for (face *f : faces)
    {
        f->calcNormal();
    }
    for (vertex *v : vertices)
    {
        v->calcNormal();
    }
}

// gonna be long
void mesh::createSphere()
{
    vertices.push_back(new vertex(0, -1.0, 0));
    vertices.push_back(new vertex(0, 1, 0));
    vertices.push_back(new vertex(1.0, 0, 0));

    vertices.push_back(new vertex(0, 0, 1));
    vertices.push_back(new vertex(-1, 0, 0));
    vertices.push_back(new vertex(0, 0, -1));

    vertices.push_back(new vertex(0.75, -0.75, 0));
    vertices.push_back(new vertex(0, -0.75, -0.75));
    vertices.push_back(new vertex(0.75, 0, -0.75));

    vertices.push_back(new vertex(0, -0.75, 0.75));
    vertices.push_back(new vertex(0.75, 0, 0.75));
    vertices.push_back(new vertex(-0.75, -0.75, 0));

    vertices.push_back(new vertex(-0.75, 0, 0.75));
    vertices.push_back(new vertex(-0.75, 0, -0.75));
    vertices.push_back(new vertex(0.75, 0.75, 0));

    vertices.push_back(new vertex(0, 0.750000, -0.750000));
    vertices.push_back(new vertex(0.000000, 0.750000, 0.750000));
    vertices.push_back(new vertex(-0.750000, 0.750000, -0.000000));

    vertices.push_back(new vertex(0.555556, -0.555556, -0.555556));
    vertices.push_back(new vertex(0.555555, -0.555556, 0.555556));
    vertices.push_back(new vertex(-0.555556, -0.555556, 0.555555));

    vertices.push_back(new vertex(-0.555555, -0.555556, -0.555556));
    vertices.push_back(new vertex(0.555556, 0.555556, -0.555555));
    vertices.push_back(new vertex(0.555555, 0.555556, 0.555556));

    vertices.push_back(new vertex(-0.555556, 0.555556, 0.555556));
    vertices.push_back(new vertex(-0.555556, 0.555556, -0.555556));

    faces.push_back(new face(vertices[0], vertices[7], vertices[18], vertices[6], 0, 7, 18, 6));
    faces.push_back(new face(vertices[0], vertices[6], vertices[19], vertices[9], 0, 6, 19, 9));
    faces.push_back(new face(vertices[0], vertices[9], vertices[20], vertices[11], 0, 9, 20, 11));

    faces.push_back(new face(vertices[0], vertices[11], vertices[21], vertices[7], 0, 11, 21, 7));
    faces.push_back(new face(vertices[1], vertices[14], vertices[22], vertices[15], 1, 14, 22, 15));
    faces.push_back(new face(vertices[1], vertices[15], vertices[25], vertices[17], 1, 15, 25, 17));

    faces.push_back(new face(vertices[1], vertices[17], vertices[24], vertices[16], 1, 17, 24, 16));
    faces.push_back(new face(vertices[1], vertices[16], vertices[23], vertices[14], 1, 16, 23, 14));
    faces.push_back(new face(vertices[2], vertices[6], vertices[18], vertices[8], 2, 6, 18, 8));

    faces.push_back(new face(vertices[2], vertices[8], vertices[22], vertices[14], 2, 8, 22, 14));
    faces.push_back(new face(vertices[2], vertices[14], vertices[23], vertices[10], 2, 14, 23, 10));
    faces.push_back(new face(vertices[2], vertices[10], vertices[19], vertices[6], 2, 10, 19, 6));

    faces.push_back(new face(vertices[3], vertices[9], vertices[19], vertices[10], 3, 9, 19, 10));
    faces.push_back(new face(vertices[3], vertices[10], vertices[23], vertices[16], 3, 10, 23, 16));
    faces.push_back(new face(vertices[3], vertices[16], vertices[24], vertices[12], 3, 16, 24, 12));

    faces.push_back(new face(vertices[3], vertices[12], vertices[20], vertices[9], 3, 12, 20, 9));
    faces.push_back(new face(vertices[4], vertices[11], vertices[20], vertices[12], 4, 11, 20, 12));
    faces.push_back(new face(vertices[4], vertices[12], vertices[24], vertices[17], 4, 12, 24, 17));

    faces.push_back(new face(vertices[4], vertices[17], vertices[25], vertices[13], 4, 17, 25, 13));
    faces.push_back(new face(vertices[4], vertices[13], vertices[21], vertices[11], 4, 13, 21, 11));
    faces.push_back(new face(vertices[5], vertices[15], vertices[22], vertices[8], 5, 15, 22, 8));

    faces.push_back(new face(vertices[5], vertices[8], vertices[18], vertices[7], 5, 8, 18, 7));
    faces.push_back(new face(vertices[5], vertices[7], vertices[21], vertices[13], 5, 7, 21, 13));
    faces.push_back(new face(vertices[5], vertices[13], vertices[25], vertices[15], 5, 13, 25, 15));

    for (face *f : faces)
    {
        f->calcNormal();
    }
    for (vertex *v : vertices)
    {
        v->calcNormal();
    }
}

// Select a set of vertices
void mesh::setSelectedV(std::vector<int> s)
{
    selectedE.clear(); // can't select a face and a vertex at same time
    selectedF.clear();
    selectedV = s;
}
// select a set of faces
void mesh::setSelectedF(std::vector<int> s)
{
    selectedE.clear();
    selectedV.clear();
    selectedF = s;
}

void mesh::unSelect()
{
    selectedE.clear();
    selectedF.clear();
    selectedV.clear();
}

void mesh::setVertices(std::vector<float *> v)
{
    vertices.clear();
    for (float *c : v)
    {
        vertices.push_back(new vertex(c[0], c[1], c[2]));
    }
}

std::vector<float *> mesh::getVertices()
{
    std::vector<float *> fVertex;
    for (vertex *v : vertices)
    {
        fVertex.push_back(v->getVertex());
    }
    return fVertex;
}

std::vector<int *> mesh::getIndices()
{
    indices.clear();
    for (face *f : faces)
    {
        indices.push_back(f->getIndices());
    }
    return indices;
}

std::vector<float *> mesh::getNormals()
{
    // vertex normals
    normals.clear();
    for (face *f : faces)
    {
        normals.push_back(f->getNormal());
    }
    return normals;
}

std::vector<std::vector<float *> > mesh::getFaces()
{
    std::vector<std::vector<float *> > a;
    std::vector<float *> b;
    for (face *f : faces)
    {
        b.clear();
        for (vertex *v : f->getVertices())
        {
            b.push_back(v->getVertex());
        }
        a.push_back(b);
    }
    return a;
}

std::vector<face *> mesh::getFaceList()
{
    return faces;
}

void mesh::moveSelected(float dx, float dy, float dz)
{
    if (!selectedV.empty())
    {
        for (int i : selectedV)
        {
            vertices[i]->move(dx, dy, dz);
            
        }
    }
    
    else if (!selectedF.empty())
    {
        for (int i : selectedF)
        {
            faces[i]->move(dx, dy, dz);
        }
    }
    calcNormals();
}

float *mesh::getPos()
{
    return pos;
}

void mesh::movePos(float dx, float dy, float dz)
{
    pos[0] += dx;
    pos[1] += dy;
    pos[2] += dz;
}

float** mesh::getBoundingBox()
{
    float** bbox = new float*[2];
    bbox[0] = new float[3];
    bbox[0][0] = 999999;
    bbox[0][1] = 999999;
    bbox[0][2] = 999999;
    bbox[1] = new float[3];
    bbox[1][0] = -999999;
    bbox[1][1] = -999999;
    bbox[1][2] = -999999;


    float* vert;
    for (vertex *v : vertices) {
        vert = v->getVertex();
        bbox[0][0] = std::min(bbox[0][0], vert[0] + pos[0]);
        bbox[0][1] = std::min(bbox[0][0], vert[1] + pos[1]);
        bbox[0][2] = std::min(bbox[0][0], vert[2] + pos[2]);
        bbox[1][0] = std::max(bbox[0][0], vert[0] + pos[0]);
        bbox[1][1] = std::max(bbox[0][0], vert[1] + pos[1]);
        bbox[1][2] = std::max(bbox[0][0], vert[2] + pos[2]);
    }
    return bbox;
}

void mesh::draw(bool meshOnly)
{
    glPushMatrix();
    glTranslatef(pos[0], pos[1], pos[2]);

    glBegin(GL_QUADS);
    for (int i = 0; i < faces.size(); i++)
    {
        // don't draw selected faces (unless meshOnly mode)
        if (meshOnly || std::find(selectedF.begin(), selectedF.end(), i) == selectedF.end())
        {
            glNormal3fv(faces[i]->getNormal());
            glVertex3fv(faces[i]->getVertices()[0]->getVertex());

            glNormal3fv(faces[i]->getNormal());
            glVertex3fv(faces[i]->getVertices()[1]->getVertex());

            glNormal3fv(faces[i]->getNormal());
            glVertex3fv(faces[i]->getVertices()[2]->getVertex());

            glNormal3fv(faces[i]->getNormal());
            glVertex3fv(faces[i]->getVertices()[3]->getVertex());
        }
    }
    glEnd();

    if (!meshOnly)
    {
        // draw in all edges with red lines
        glDisable(GL_LIGHTING);
        glColor3f(1, 0, 0);
        glLineWidth(1.3);
        glBegin(GL_LINES);
        for (face *f : faces)
        {
            for (edge *e : f->getEdges())
            {
                glVertex3fv(e->getVertices()[0]->getVertex());
                glVertex3fv(e->getVertices()[1]->getVertex());
            }
        }
        glEnd();
        glEnable(GL_LIGHTING);

        // draw in selected objects with a green colour
        float emi[3] = {0, 1, 0}; // green
        glMaterialfv(GL_FRONT, GL_AMBIENT, emi);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, emi);
        glMaterialfv(GL_FRONT, GL_SPECULAR, emi);
        glMaterialfv(GL_FRONT, GL_SHININESS, emi);
        // draw in selected vertices
        for (int i : selectedV)
        {
            glPushMatrix();
            glTranslatef(
                vertices[i]->getVertex()[0],
                vertices[i]->getVertex()[1],
                vertices[i]->getVertex()[2]);
            glutSolidSphere(0.02, 8, 8); // change these to look nice
            glPopMatrix();
        }

        // draw in selected faces as green faces
        for (int i : selectedF)
        {
            glBegin(GL_QUADS);
            glNormal3fv(faces[i]->getNormal());
            glVertex3fv(faces[i]->getVertices()[0]->getVertex());

            glNormal3fv(faces[i]->getNormal());
            glVertex3fv(faces[i]->getVertices()[1]->getVertex());

            glNormal3fv(faces[i]->getNormal());
            glVertex3fv(faces[i]->getVertices()[2]->getVertex());

            glNormal3fv(faces[i]->getNormal());
            glVertex3fv(faces[i]->getVertices()[3]->getVertex());

            glEnd();
        }

        // draw all vertices with a yellow circle
        /*float cmat[3] = {1, 1, 0}; // yellow
        glMaterialfv(GL_FRONT, GL_AMBIENT, cmat);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, cmat);
        glMaterialfv(GL_FRONT, GL_SPECULAR, cmat);
        glMaterialfv(GL_FRONT, GL_SHININESS, cmat);
        for (vertex *v : vertices)
        {
            glPushMatrix();
            glTranslatef(v->getVertex()[0], v->getVertex()[1], v->getVertex()[2]);
            glutSolidSphere(0.005, 3, 3);
            glPopMatrix();
        }*/
    }
    glPopMatrix();
}
