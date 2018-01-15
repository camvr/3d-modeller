#include "main.h"

GLubyte *LoadPPM(char *file, int *width, int *height, int *max)
{
    GLubyte *img;
    FILE *fd;
    int n, m;
    int k, nm;
    char c;
    int i;
    char b[100];
    float s;
    int red, green, blue;

    /* first open file and check if it's an ASCII PPM (indicated by P3 at the start) */
    fd = fopen(file, "r");
    fscanf(fd, "%[^\n] ", b);
    if (b[0] != 'P' || b[1] != '3')
    {
        //printf("[%c, %c]\n", b[0], b[1]);
        printf("%s is not a PPM file!\n", file);
        exit(0);
    }
    //printf("%s is a PPM file\n", file);
    fscanf(fd, "%c", &c);

    /* next, skip past the comments - any line starting with #*/
    while (c == '#')
    {
        fscanf(fd, "%[^\n] ", b);
        //printf("%s\n", b);
        fscanf(fd, "%c", &c);
    }
    ungetc(c, fd);

    /* now get the dimensions and max colour value from the image */
    fscanf(fd, "%d %d %d", &n, &m, &k);

    //printf("%d rows  %d columns  max value= %d\n", n, m, k);

    /* calculate number of pixels and allocate storage for this */
    nm = n * m;
    img = (GLubyte *)malloc(3 * sizeof(GLuint) * nm);
    s = 255.0 / k;

    /* for every pixel, grab the read green and blue values, storing them in the image data array */
    for (i = 0; i < nm; i++)
    {
        fscanf(fd, "%d %d %d", &red, &green, &blue);
        img[3 * nm - 3 * i - 3] = red * s;
        img[3 * nm - 3 * i - 2] = green * s;
        img[3 * nm - 3 * i - 1] = blue * s;
    }

    /* finally, set the "return parameters" (width, height, max) and return the image array */
    *width = n;
    *height = m;
    *max = k;

    return img;
}

// Utility
std::vector<std::string> split(std::string str, std::string sep) {
    char* cstr = const_cast<char*>(str.c_str());
    char* current;
    std::vector<std::string> arr;
    current = std::strtok(cstr, sep.c_str());
    while (current != NULL) {
        arr.push_back(current);
        current = std::strtok(NULL, sep.c_str());
    }
    return arr;
}

void drawText(std::string str, float x, float y, float z, float scale) 
{
    glPushMatrix();
    glTranslatef(x, y, z);
    if (scale <= 0) scale = 0;
    glScalef(0.15f * scale, 0.2f * scale, z);
    glLineWidth(3 * scale);
    for (char &c : str)
    {
        glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN , c);
    }
    glPopMatrix();
    glLineWidth(1); // reset line width
}

// Operational Functions
void loadFromFile()
{
    objects.clear();

    // allow user to enter a file name
    printf("\nEnter path and file name: ");
    std::string file;
    std::cin >> file;
    printf("\nLoading file...");

    std::string temp;
    std::vector<std::string> line;
    std::vector<std::string> data;
    std::ifstream objFile(file);
    
    std::vector<float*> vert;
    std::vector<int*> fa;
    std::vector<int*> t;
    std::vector<float*> n;

    float a;
    float b;
    float c;

    if (objFile.is_open())
    {

        while (getline(objFile, temp))
        {
            line = split(temp, " ");
            // printf("%s %s %s\n", line[0].c_str(), line[1].c_str(), line[2].c_str());
            if (!line[0].compare("v")) 
            {
                float *temp = new float[3];
                temp[0] = std::strtof((line[1]).c_str(), 0);
                temp[1] = std::strtof((line[2]).c_str(), 0);
                temp[2] = std::strtof((line[3]).c_str(), 0);
                vert.push_back(temp);
            }
            
            else if (!line[0].compare("f")) 
            {
                int *temp = new int[4];
                for (int i = 1; i < line.size(); i++) {
                    data = split(line[i], "/");
                    temp[i-1] = std::stoi(data[0]) - 1;
                    // Program recalculates normal
                    // int y = std::stoi(data[1]);
                }
                fa.push_back(temp);
            }
        }
        mesh *m = new mesh(vert, fa, n);        
        objects.push_back(m);
        objFile.close();
        printf("Successfully loaded file.\n");
    }
    else
    {
        // error out
        printf("\nERR: File not found. Make sure you enter the correct path/file name");
    }
}

void saveToFile()
{
    printf("Enter file name to save as: ");
    std::string file;
    std::cin >> file;
    printf("Saving file...\n");

    std::ofstream objFile(file);
    if (objFile.is_open() && !objects.empty())
    {
        mesh *m = objects.at(currObject);
        std::vector<float*> ver = m->getVertices();
        std::vector<float*> norm = m->getNormals();
        std::vector<int*> f = m->getIndices();
        for(float* v : ver)
        {
            objFile << "v " << v[0] << " " << v[1] << " " << v[2] << std::endl;
        }
        for(float* n : norm)
        {
            objFile << "vn " << n[0] << " " << n[1] << " " << n[2] << std::endl;
        }
        int x = 1;
        for(int* i : f)
        {
            objFile << "f " << i[0]+1 << "//" << x << " " << i[1]+1 << "//" << x << " " << i[2]+1 << "//" << x << " " << i[3]+1 << "//" << x << std::endl;
            x++;
        }
        
        objFile.close();
        printf("Successfully saved file.\n");
    }
    else
    {
        // error out
        printf("\nERR: Could not save file.");
    }
}

void toolbar(int HUDval)
{
    switch (HUDval)
    {
    case 1:
        //save selected
        //printf("Save icon selected");
        saveToFile();
        break;
    case 2:
        //load selected
        //printf("load icon selected");
        loadFromFile();
        break;
    case 3:
        //cube selected, insert unit cube @ origin
        //printf("cube icon selected");
        objects.push_back(new mesh(0));
        currObject = objects.size()-1;
        break;
    case 4:
        //sphere selected, insert unit sphere @ origin
        //printf("sphere icon selected");
        objects.push_back(new mesh(1));
        currObject = objects.size()-1;
        break;
    case 5:
        //subdivision selected
        //printf("subdiv icon selected");
        if(objects.size() > 0){
            objects.at(currObject)->subdivideMesh();
        }
        break;
    }
    glutPostRedisplay();
}

//keyboard inputs
void keyboard(unsigned char key, int xIn, int yIn)
{
    int mod = glutGetModifiers();
    
    switch (key)
    {
    case 27: // esc key
        exit(0);
        break;

    case 'r': // Switch to front view
        sceneRot[0] = 0;
        sceneRot[1] = 0;
        sceneRot[2] = 0;
        break;

    case 127:
        if(objects.size() > 0)
        {
            objects.erase(objects.begin()+currObject);
            currObject = 0;
        }
        break;

    case 'v': // switch view modes
        perspective = !perspective;
        break;

    case '1': // sculpt tool
        currTool = SCULPTER;
        toolName = "Sculpt Tool";
        break;
    
    case '2': // move tool
        currTool = MOVER;
        toolName = "Move Tool";
        break;

    case '3': // select tool
        currTool = SELECTOR;
        toolName = "Selection Tool";
        break;

    case 'x': // lock x axis
        if (!lockAxis[3]) {
            lockAxis[0] = true;
            lockAxis[3] = true;
        } else {
            lockAxis[0] = false;
            lockAxis[3] = false;
        }
        break;

    case 'y': // lock y axis
        if (!lockAxis[3]) {
            lockAxis[1] = true;
            lockAxis[3] = true;
        } else {
            lockAxis[1] = false;
            lockAxis[3] = false;
        }
        break;

    case 'z': // lock z axis
        if (!lockAxis[3]) {
            lockAxis[2] = true;
            lockAxis[3] = true;
        } else {
            lockAxis[2] = false;
            lockAxis[3] = false;
        }
        break;
        
    }
}

void special(int key, int xIn, int yIn)
{
}

void mouse(int btn, int state, int x, int y)
{
    if (btn == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            if (STATE == IDLE)
            {
                glGetDoublev(GL_MODELVIEW_MATRIX, matModelView);
                glGetDoublev(GL_PROJECTION_MATRIX, matProjection);
                glGetIntegerv(GL_VIEWPORT, viewport);

                if (0 <= x && x <= winX && 0 <= y && y <= 90)
                {
                    // visually let the user know they are clicking on an icon, if they are
                    int HUDval = hud->iconSelect(x, true);
                    toolbar(HUDval);
                }
                else if (currTool == SCULPTER)
                {
                    if (!objects.empty()) {
                        STATE = SCULPT_TOOL_SELECTED;
                        // select verticies
                        objects.at(currObject)->setSelectedV(brush->getSelectedVertices(objects.at(currObject), matModelView, matProjection, viewport, x, y));
                    }
                }
                else if (currTool == MOVER)
                {
                    STATE = MOVE_TOOL_SELECTED;
                }
                else if (currTool == SELECTOR)
                {
                    int newCO = brush->selectObject(&objects, matModelView, matProjection, viewport, x, y);
                    currObject = (newCO == -1) ? currObject : newCO;
                }
            }
        }
        else if (state == GLUT_UP)
        {
            if (STATE == SCULPT_TOOL_SELECTED)
            {
                // clear selected
                if (!objects.empty())
                    objects.at(currObject)->unSelect();
            }
            STATE = IDLE;
        }
    }
    else if (btn == GLUT_RIGHT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            if (STATE == IDLE)
            { // allow camera movement
                if (glutGetModifiers() == GLUT_ACTIVE_SHIFT)
                {
                    STATE = CAMERA_SHIFT;
                }
                else
                {
                    STATE = CAMERA_ROTATE;
                }
            }
        }
        else if (state == GLUT_UP)
        {
            STATE = IDLE;
        }
    }
    else if (btn == 3)
    {
        if (camDist <= MIN_CAM_DIST)
        {
            camDist = MIN_CAM_DIST;
        }
        else
        {
            camDist -= ZOOM_SPEED;
            camPos[2] = camDist;
        }
    }
    else if (btn == 4)
    {
        if (camDist >= MAX_CAM_DIST)
        {
            camDist = MAX_CAM_DIST;
        }
        else
        {
            camDist += ZOOM_SPEED;
            camPos[2] = camDist;
        }
    }
}

void mouseMotion(int x, int y)
{
    if (STATE == SCULPT_TOOL_SELECTED)
    {
        if (!objects.empty())
        {
            // move verticies
            float dy = (camDist * (yprev - y) / 1500.0f) * (800.0f/winY);
            float dx = (camDist * (x - xprev) / 1500.0f) * (1000.0f/winX);

            if (lockAxis[0])
            {
                objects.at(currObject)->moveSelected(dx, 0, 0);
            }
            else if (lockAxis[1])
            {
                objects.at(currObject)->moveSelected(0, dx, 0);
            }
            else if (lockAxis[2])
            {
                objects.at(currObject)->moveSelected(0, 0, dx);
            }
            else // freely move
            {
                float dX = dx * cos((M_PI / 180) * sceneRot[1]);
                float dY = dy;
                float dZ = dx * sin((M_PI / 180) * sceneRot[1]);
                objects.at(currObject)->moveSelected(dX, dY, dZ);
            }
        }
    }
    else if (STATE == MOVE_TOOL_SELECTED)
    {
        if (objects.size() > 0)
        {
            // move object
            float dy = (camDist * (yprev - y) / 1500.0f) * (800.0f/winY);
            float dx = (camDist * (x - xprev) / 1500.0f) * (1000.0f/winX);

            if (lockAxis[0])
            {
                objects.at(currObject)->movePos(dx, 0, 0);
            }
            else if (lockAxis[1])
            {
                objects.at(currObject)->movePos(0, dx, 0);
            }
            else if (lockAxis[2])
            {
                objects.at(currObject)->movePos(0, 0, dx);
            }
            else // freely move
            {
                float dX = dx * cos((M_PI / 180) * sceneRot[1]); // tweak me
                float dY = dy;
                float dZ = dx * sin((M_PI / 180) * sceneRot[1]);
                objects.at(currObject)->movePos(dX, dY, dZ);
            }
        }
    }
    else if (STATE == CAMERA_ROTATE)
    {
        int dy = y - yprev;
        int dx = x - xprev;

        sceneRot[1] += (float)dx / camDist;
        if (sceneRot[1] > 360) sceneRot[1] -= 360;
        if (sceneRot[1] < -360) sceneRot[1] += 360;

        // Limit this view to directly above/below the object
        sceneRot[0] += ((float)dy / camDist) * cos((M_PI / 180) * sceneRot[1]);
        if (sceneRot[0] >= 90) sceneRot[0] = 90;
        if (sceneRot[0] <= -90) sceneRot[0] = -90;
        sceneRot[2] += ((float)dy / camDist) * sin((M_PI / 180) * sceneRot[1]);
        if (sceneRot[2] >= 90) sceneRot[2] = 90;
        if (sceneRot[2] <= -90) sceneRot[2] = -90;
    }
    else if (STATE == CAMERA_SHIFT)
    {
        worldOffset[0] += (camDist * (x - xprev) / 1500.0f) * (1000.0f/winX);
        worldOffset[1] += (camDist * (yprev - y) / 1500.0f) * (800.0f/winY);
    }

    // Update previous mouse coords
    xprev = x;
    yprev = y;
}

void mousePassiveMotion(int x, int y)
{
    // button highlighting in HUD controlled here
    bool showHoverSquare = false;
    if (0 <= x && x <= winX && 0 <= y && y <= 90)
    {
        showHoverSquare = true;
    }
    hud->iconHover(x, showHoverSquare);

    xprev = x;
    yprev = y;
}

// Render 3D viewport for modelling scene
void draw3dScene()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // set view
    if (perspective) {
        gluPerspective(45, (float)winX/winY, 1, 100);
    } else {
        glOrtho(-2.0, 2.0, -2.0, 2.0, 1, 100);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    gluLookAt(camPos[0], camPos[1], camPos[2], camTarget[0], camTarget[1], camTarget[2], 0, 1, 0);

    // lights and values
    glPushMatrix();
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
    glPopMatrix();

    // DRAWING SCENE
    glTranslatef(worldOffset[0], worldOffset[1], 0); // shift world view by 2D offset
    glRotatef(sceneRot[1], 0, 1, 0); // Rotate world
    glRotatef(sceneRot[2], 0, 0, 1);
    glRotatef(sceneRot[0], 1, 0, 0);
    

    // draw axis and floor
    if (showAxis)
    {
        glDisable(GL_LIGHTING);
        glPushMatrix();
        glBegin(GL_LINES);
        glColor3f(1, 0, 0); // x
        glVertex3f(0, 0, 0);
        glVertex3f(5, 0, 0);
        glColor3f(0.5, 0, 0); // -x
        glVertex3f(0, 0, 0);
        glVertex3f(-5, 0, 0);

        glColor3f(0, 1, 0); // y
        glVertex3f(0, 0, 0);
        glVertex3f(0, 5, 0);
        glColor3f(0, 0.5, 0); // -y
        glVertex3f(0, 0, 0);
        glVertex3f(0, -5, 0);

        glColor3f(0, 0, 1); // z
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, 5);
        glColor3f(0, 0, 0.5); // -z
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, -5);
        glEnd();
        glPopMatrix();
        glEnable(GL_LIGHTING);
    }

    if (showFloor)
    {
        glDisable(GL_LIGHTING);
        glPushMatrix();
        glBegin(GL_LINES);
        glColor3f(0.25, 0.25, 0.25);
        for (int i = -5; i <= 5; i++) {
            if (!showAxis || i != 0)
            {
                glVertex3f(i, 0, -5);
                glVertex3f(i, 0, 5);

                glVertex3f(-5, 0, i);
                glVertex3f(5, 0, i);
            }
        }
        glEnd();
        glPopMatrix();
        glEnable(GL_LIGHTING);
    }

    // Draw the mesh

    for (int i = 0; i < objects.size(); i++)
    {
        glMaterialfv(GL_FRONT, GL_AMBIENT, materials[currMat].amb);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, materials[currMat].dif);
        glMaterialfv(GL_FRONT, GL_SPECULAR, materials[currMat].spc);
        glMaterialf(GL_FRONT, GL_SHININESS, materials[currMat].shine);
        if (i == currObject)
            objects.at(i)->draw(false);
        else
            objects.at(i)->draw(true);
    }
}

void drawInterface()
{
    // display text
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, winX, 0.0, winY);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glPushAttrib(GL_CURRENT_BIT);
    glColor3f(0.75,0.75,0.75);
    drawText(toolName, winX-230, winY-30, 0, 1);
    if (lockAxis[0])
    {
        glColor3f(1,0,0);
        drawText("X-Axis Lock", winX-230, winY-70, 0, 0.75);
    }
    else if (lockAxis[1])
    {
        glColor3f(0,1,0);
        drawText("Y-Axis Lock", winX-230, winY-70, 0, 0.75);
    }
    else if (lockAxis[2])
    {
        glColor3f(0,0,1);
        drawText("Z-Axis Lock", winX-230, winY-70, 0, 0.75);
    }
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glPopAttrib();

    // to draw in front
    glPushMatrix();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, winX, 0, winY + 45);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0, winY - 135, 0);
    hud->drawHUD(image, width, height);
    glPopMatrix();

    glEnable(GL_LIGHTING);

    glFlush();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawInterface();
    draw3dScene();
    
    glutSwapBuffers();
}


void FPSTimer(int value)
{ // 60fps
    glutTimerFunc(17, FPSTimer, 0);
    glutPostRedisplay();
}

// Print instructions to console
void printInstructions()
{
    printf("3DModeller\n============================\n");
    printf("'esc'        Exit the program\n");
    printf("'delete'     Delete currently selected object\n");
    printf("r            Reset the camera position\n");
    printf("v            Switch between perspective and orthographic view\n");
    printf("1            Select Sculpt tool\n");
    printf("2            Select Move tool\n");
    printf("3            Select object selector tool\n");
    printf("x            Lock movement to x-axis\n");
    printf("y            Lock movement to y-axis\n");
    printf("z            Lock movement to z-axis\n\n");
    printf("Left mouse            Used for the tools and interacting with objects\n");
    printf("Right mouse           Used for rotating the view around\n");
    printf("shift + Right mouse   Used to shift the view around\n");
    printf("Middle click          Opens a glutMenu with extra options\n\n");
    printf("See README for extra info.\n");
}

// on reshape
void reshape(int w, int h)
{
    winX = w;
    winY = h;
    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (float)w/h, 1, 100);

	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, w, h);
}

// Main window initialization
void init()
{
    image = LoadPPM("ppm/toolbar_v1.ppm", &width, &height, &max);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glEnable(GL_NORMALIZE);

    glShadeModel(GL_SMOOTH);

    glClearColor(0.1, 0.1, 0.1, 0);

    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, 1, 100);

    // Backface culling and depth test
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    currMat = BLACK_PLASTIC;
}

void attributeMenu(int val)
{
    switch (val)
    {
    case 0:
        currTool = SCULPTER;
        toolName = "Sculpt Tool";
        break;
    case 1:
        currTool = MOVER;
        toolName = "Move Tool";
        break;
    case 2:
        currTool = SELECTOR;
        toolName = "Selection Tool";
        break;
    }
}

void toolRadMenu(int val)
{
    switch(val)
    {
    case 0:
        brush->setRad(0.1f);
        break;
    case 1:
        brush->setRad(0.25f);
        break;
    case 2:
        brush->setRad(0.5f);
        break;
    case 3:
        brush->setRad(1.0f);
        break;
    case 4:
        brush->setRad(1.5f);
        break;
    }
}

void colorMenu(int val)
{
    switch (val)
    {
    case 0:
        currMat = OBSIDIAN;
        break;
    case 1:
        currMat = PEARL;
        break;
    case 2:
        currMat = CHROME;
        break;
    case 3:
        currMat = BRASS;
        break;
    case 4:
        currMat = BLACK_PLASTIC;
        break;
    case 5:
        currMat =  WHITE_PLASTIC;
        break;
    case 6:
        currMat = BLACK_RUBBER;
        break;
    }
}

void mainMenu(int val){}

void createOurMenu(void)
{
    int attributeSelect_menu = glutCreateMenu(attributeMenu);
    glutAddMenuEntry("Sculpt Tool", 0);
    glutAddMenuEntry("Move Tool", 1);
    glutAddMenuEntry("Selector Tool", 2);

    int colorSelect_menu = glutCreateMenu(colorMenu);
    glutAddMenuEntry("Obsidian", 0);
    glutAddMenuEntry("Pearl", 1);
    glutAddMenuEntry("Chrome", 2);
    glutAddMenuEntry("Brass", 3);
    glutAddMenuEntry("Black Plastic", 4);
    glutAddMenuEntry("White Plastic", 5);
    glutAddMenuEntry("Black Rubber", 6);

    int toolRadius_menu = glutCreateMenu(toolRadMenu);
    glutAddMenuEntry("1", 0);
    glutAddMenuEntry("5", 1);
    glutAddMenuEntry("10", 2);
    glutAddMenuEntry("15", 3);
    glutAddMenuEntry("20", 4);

    int main_menu = glutCreateMenu(mainMenu);
    glutAddSubMenu("Object Material", colorSelect_menu);
    glutAddSubMenu("Current Tool", attributeSelect_menu);
    glutAddSubMenu("Tool Radius", toolRadius_menu);
    glutAddMenuEntry("Cancel", -1);

    glutAttachMenu(GLUT_MIDDLE_BUTTON); //scroll wheel
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

    glutInitWindowSize(winX, winY);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("3dModeller");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMotionFunc(mouseMotion);
    glutMouseFunc(mouse);
    glutPassiveMotionFunc(mousePassiveMotion);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutTimerFunc(17, FPSTimer, 0);

    init();
    createOurMenu();
    printInstructions();

    glutMainLoop();
    return (0);
}
