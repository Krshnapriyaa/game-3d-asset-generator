#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include <ctime>

// ---------- COLORS ----------
GLfloat COLORS[][3] = {
    {1.0f, 0.0f, 0.0f},   // red
    {0.0f, 1.0f, 0.0f},   // green
    {0.0f, 0.0f, 1.0f},   // blue
    {1.0f, 1.0f, 0.0f},   // yellow
    {1.0f, 0.0f, 1.0f},   // magenta
    {0.0f, 1.0f, 1.0f},   // cyan
    {1.0f, 0.5f, 0.0f},   // orange
    {0.6f, 0.3f, 0.8f}    // purple
};

// ---------- CAMERA ----------
class Camera {
    double theta, y, dTheta, dy;
public:
    Camera() : theta(0), y(5), dTheta(0.05), dy(0.3) {}
    double getX() { return 15 * cos(theta); }
    double getY() { return y; }
    double getZ() { return 15 * sin(theta); }
    void moveRight() { theta += dTheta; }
    void moveLeft()  { theta -= dTheta; }
    void moveUp()    { y += dy; }
    void moveDown()  { if (y > dy) y -= dy; }
};

// ---------- STRUCTURE DEFINITIONS ----------
struct Building {
    float x, z;
    GLfloat* wallColor;
    GLfloat* roofColor;
    int type; // 0=house, 1=barn, 2=silo
};

// ---------- GLOBALS ----------
Camera camera;
float rotationAngle = 0;
Building buildings[6];

// ---------- UTILITY ----------
GLfloat* randomColor() {
    return COLORS[rand() % 8];
}

// ---------- DRAW FUNCTIONS ----------
void drawCube(float w, float h, float d, GLfloat* color) {
    glPushMatrix();
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
    glScalef(w, h, d);
    glutSolidCube(1.0);
    glPopMatrix();
}

void drawRoof(float w, float h, float d, GLfloat* color) {
    glPushMatrix();
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
    glBegin(GL_TRIANGLES);
    // Front
    glNormal3f(0, 1, 1);
    glVertex3f(-w/2, 0, d/2);
    glVertex3f(w/2, 0, d/2);
    glVertex3f(0, h, d/2);
    // Back
    glNormal3f(0, 1, -1);
    glVertex3f(-w/2, 0, -d/2);
    glVertex3f(w/2, 0, -d/2);
    glVertex3f(0, h, -d/2);
    glEnd();

    glBegin(GL_QUADS);
    glNormal3f(-1, 0, 0);
    glVertex3f(-w/2, 0, -d/2);
    glVertex3f(-w/2, 0, d/2);
    glVertex3f(0, h, d/2);
    glVertex3f(0, h, -d/2);

    glNormal3f(1, 0, 0);
    glVertex3f(w/2, 0, -d/2);
    glVertex3f(w/2, 0, d/2);
    glVertex3f(0, h, d/2);
    glVertex3f(0, h, -d/2);
    glEnd();
    glPopMatrix();
}

void drawHouse(float x, float z, GLfloat* wall, GLfloat* roof) {
    glPushMatrix();
    glTranslated(x, 0, z);
    drawCube(2, 1.2, 2, wall);
    glTranslated(0, 1.2, 0);
    drawRoof(2.2, 1, 2.2, roof);
    glPopMatrix();
}

void drawBarn(float x, float z, GLfloat* wall, GLfloat* roof) {
    glPushMatrix();
    glTranslated(x, 0, z);
    drawCube(3, 2, 2.5, wall);
    glTranslated(0, 2, 0);
    drawRoof(3.5, 1.5, 2.5, roof);
    glPopMatrix();
}

void drawSilo(float x, float z, GLfloat* color) {
    glPushMatrix();
    glTranslated(x, 0, z);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
    GLUquadric* quad = gluNewQuadric();
    gluCylinder(quad, 0.6, 0.6, 3.5, 20, 20);
    glTranslated(0, 3.5, 0);
    glutSolidSphere(0.7, 20, 20);
    gluDeleteQuadric(quad);
    glPopMatrix();
}

void drawGround() {
    glPushMatrix();
    GLfloat groundColor[] = {0.3f, 0.8f, 0.3f}; // pleasant green
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, groundColor);
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);
    for (int x = -20; x < 20; x++) {
        for (int z = -20; z < 20; z++) {
            glVertex3f(x, 0, z);
            glVertex3f(x + 1, 0, z);
            glVertex3f(x + 1, 0, z + 1);
            glVertex3f(x, 0, z + 1);
        }
    }
    glEnd();
    glPopMatrix();
}

// ---------- INIT ----------
void initScene() {
    srand(time(0));
    // Create a few colorful buildings
    for (int i = 0; i < 6; i++) {
        buildings[i].x = -8 + rand() % 16;
        buildings[i].z = -8 + rand() % 16;
        buildings[i].type = rand() % 3;
        buildings[i].wallColor = randomColor();
        buildings[i].roofColor = randomColor();
    }
}

void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);

    GLfloat lightPos0[] = {6, 8, 5, 1};
    GLfloat lightColor0[] = {1, 1, 1, 1};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);

    GLfloat lightPos1[] = {-8, 4, -3, 1};
    GLfloat lightColor1[] = {0.8, 0.8, 1, 1};
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);

    GLfloat lightPos2[] = {0, 10, 0, 1};
    GLfloat lightColor2[] = {1, 0.8, 0.8, 1};
    glLightfv(GL_LIGHT2, GL_POSITION, lightPos2);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, lightColor2);

    glClearColor(0.4, 0.7, 1.0, 1);
    initScene();
}

// ---------- DISPLAY ----------
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(camera.getX(), camera.getY(), camera.getZ(),
              0, 0, 0, 0, 1, 0);

    drawGround();

    // Draw stored colorful buildings
    for (int i = 0; i < 6; i++) {
        Building& b = buildings[i];
        if (b.type == 0)
            drawHouse(b.x, b.z, b.wallColor, b.roofColor);
        else if (b.type == 1)
            drawBarn(b.x, b.z, b.wallColor, b.roofColor);
        else {
            glPushMatrix();
            glRotated(rotationAngle, 0, 1, 0);
            drawSilo(b.x, b.z, b.wallColor);
            glPopMatrix();
        }
    }

    glutSwapBuffers();
}

// ---------- RESHAPE ----------
void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, (float)w / h, 1, 200);
    glMatrixMode(GL_MODELVIEW);
}

// ---------- TIMER ----------
void timer(int v) {
    rotationAngle += 1;
    if (rotationAngle > 360) rotationAngle = 0;
    glutPostRedisplay();
    glutTimerFunc(1000 / 60, timer, v);
}

// ---------- CONTROLS ----------
void special(int key, int, int) {
    switch (key) {
        case GLUT_KEY_LEFT:  camera.moveLeft();  break;
        case GLUT_KEY_RIGHT: camera.moveRight(); break;
        case GLUT_KEY_UP:    camera.moveUp();    break;
        case GLUT_KEY_DOWN:  camera.moveDown();  break;
    }
    glutPostRedisplay();
}

// ---------- MAIN ----------
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1000, 700);
    glutCreateWindow("🌈 Stable Colorful 3D Asset Generator 🌈");

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(special);
    glutTimerFunc(100, timer, 0);

    glutMainLoop();
    return 0;
}
