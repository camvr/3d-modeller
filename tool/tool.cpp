#include "tool.h"

Tool::Tool() {}

std::vector<int> Tool::getSelectedVertices(mesh *m, double modelview[16], double projection[16], int viewport[4], int x, int y)
{
    std::vector<float*> vert = m->getVertices();
    std::vector<int> selected;

    double* m_start = new double[3];
    double* m_end = new double[3];

    double winX = (double)x;
    double winY = viewport[3] - (double)y;

    gluUnProject(winX, winY, 0.0, modelview, projection, viewport, &m_start[0], &m_start[1], &m_start[2]); 

    gluUnProject(winX, winY, 1.0, modelview, projection, viewport, &m_end[0], &m_end[1], &m_end[2]);

    double* R0 = new double[3];
    double* Rd = new double[3];

    double xDiff = m_end[0] - m_start[0];
    double yDiff = m_end[1] - m_start[1];
    double zDiff = m_end[2] - m_start[2];

    double mag = sqrt(xDiff*xDiff + yDiff*yDiff + zDiff*zDiff);
    R0[0] = m_start[0];
    R0[1] = m_start[1];
    R0[2] = m_start[2];

    Rd[0] = xDiff / mag;
    Rd[1] = yDiff / mag;
    Rd[2] = zDiff / mag;
    
    double A = Rd[0]*Rd[0] + Rd[1]*Rd[1] + Rd[2]*Rd[2];
    float vertSize = 0.1;
    
    // Loop through all of the vertices
    for (int i = 0; i < vert.size(); i++)
    {
        double* R0Pc = new double[3];
        R0Pc[0] = R0[0] - (vert.at(i)[0] + m->getPos()[0]);
        R0Pc[1] = R0[1] - (vert.at(i)[1] + m->getPos()[1]);
        R0Pc[2] = R0[2] - (vert.at(i)[2] + m->getPos()[2]);

        double B = 2 * (R0Pc[0]*Rd[0] + R0Pc[1]*Rd[1] + R0Pc[2]*Rd[2]);
        double C = (R0Pc[0]*R0Pc[0] + R0Pc[1]*R0Pc[1] + R0Pc[2]*R0Pc[2]) - (vertSize*vertSize);

        double discriminent = B*B - 4*A*C;
        
        if (discriminent >= -toolRad)
        {
            selected.push_back(i);
        }
    }

    // Select pnly vertices that are in view
    vertexPicker(&selected, &vert, m, modelview, projection, viewport);
    return selected;
}

void Tool::vertexPicker(std::vector<int> *selected, std::vector<float*> *vert, mesh *m, double modelview[16], double projection[16], int viewport[4]) 
{
    std::vector<int> vis;
    // Basic colour picking to determine if selected vertices are visible
    glDisable(GL_LIGHTING);

    // Render mesh (without extras)
    glColor3f(0,0,0);
    m->draw(true);

    // render vertices with unique colour
    unsigned char r, g, b;
    glPushMatrix();
    for (int i : *selected) {
        r = (i & 0x000000FF) >> 0;
        g = (i & 0x0000FF00) >> 8;
        b = (i & 0x00FF0000) >> 16;
        glColor3ub(r, g, b);
        glPushMatrix();
        glTranslatef(vert->at(i)[0] + m->getPos()[0], vert->at(i)[1] + m->getPos()[1], vert->at(i)[2] + m->getPos()[2]);
        glutSolidSphere(0.15f, 6, 6);
        glPopMatrix();
    }
    glPushMatrix();

    glFlush(); // flush changes to buffer

    // test if vertex is visible
    double rX, rY, rZ; // resultant position (z is ignored)
    unsigned char res[3]; // resultant colour
    for (int i : *selected)
    {
        // get screen coords by projecting object
        gluProject(vert->at(i)[0] + m->getPos()[0], vert->at(i)[1] + m->getPos()[1], vert->at(i)[2] + m->getPos()[2], modelview, projection, viewport, &rX, &rY, &rZ);

        r = (i & 0x000000FF) >> 0;
        g = (i & 0x0000FF00) >> 8;
        b = (i & 0x00FF0000) >> 16;
        // get pixel data
        glReadPixels((int)rX, (int)rY, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &res);
        
        if (res[0] == r && res[1] == g && res[2] == b) // in view, mark for selection
            vis.push_back(i);
    }

    *selected = vis;

    glEnable(GL_LIGHTING);
}

int Tool::selectObject(std::vector<mesh*> *objects, double modelview[16], double projection[16], int viewport[4], int x, int y)
{
    double* m_start = new double[3];
    double* m_end = new double[3];

    // window pos of mouse, Y is inverted on Windows
    double winX = (double)x; 
    double winY = viewport[3] - (double)y; 

    // get point on the 'near' plane (third param is set to 0.0)
    gluUnProject(winX, winY, 0.0, modelview, projection, viewport, &m_start[0], &m_start[1], &m_start[2]); 

    // get point on the 'far' plane (third param is set to 1.0)
    gluUnProject(winX, winY, 1.0, modelview, projection, viewport, &m_end[0], &m_end[1], &m_end[2]); 

    // test intersections
    double* R0 = new double[3];
    double* Rd = new double[3];

    double xDiff = m_end[0] - m_start[0];
    double yDiff = m_end[1] - m_start[1];
    double zDiff = m_end[2] - m_start[2];

    double mag = sqrt(xDiff*xDiff + yDiff*yDiff + zDiff*zDiff);
    R0[0] = m_start[0];
    R0[1] = m_start[1];
    R0[2] = m_start[2];

    Rd[0] = xDiff / mag;
    Rd[1] = yDiff / mag;
    Rd[2] = zDiff / mag;

    // find intersection with models
    float minDist = -1;
    int minIndex = -1;

    for (int i = 0; i < objects->size(); i++)
    {
        // Slab Method
        mesh *m = objects->at(i);
        float t = 0;
        
        float** bbox = m->getBoundingBox();

        float f = 0.0f;
        float l = 1.0f;

        if (checkBoxIntersect(R0, Rd, bbox[0], bbox[1], t))
        {
            if (minDist == -1 || f < minDist)
            {
                minDist = t;
                minIndex = i;
            }
        }
    }

    return minIndex;
}

bool Tool::checkSlabIntersect(double s, double e, float min, float max, float& Tnear, float& Tfar)
{
	if (fabs(e) < 1.0E-8) {
        return (s > min && s < max);
    }
	float t1 = (min - s) / e;
	float t2 = (max - s) / e;

    if (t1 > t2) {
		float tmp = t1;
		t1 = t2;
		t2 = tmp;
	}
	
    if (t1 > Tnear)
		Tnear = t1;
    if (t2 < Tfar)
		Tfar = t2;
	if (Tnear > Tfar)
		return false;
	if (Tfar < 0)
		return false;
	
    return true;
}

bool Tool::checkBoxIntersect(double* s, double* e, float* min, float* max, float& intersect)
{
	float Tnear = -100000.0f;
	float Tfar = 100000.0f;

	if (!checkSlabIntersect(s[0], e[0], min[0], max[0], Tnear, Tfar))
		return false;
	if (!checkSlabIntersect(s[1], e[1], min[1], max[1], Tnear, Tfar))
		return false;
	if (!checkSlabIntersect(s[2], e[2], min[2], max[2], Tnear, Tfar))
		return false;
	
	intersect = Tnear;
	return true;
}

float Tool::getRad()
{
    return toolRad;
}

void Tool::setRad(float r)
{
    toolRad = r;
}
