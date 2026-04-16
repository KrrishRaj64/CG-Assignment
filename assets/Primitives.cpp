#include "Primitives.hpp"

#include <GL/glut.h>

void drawCylinder(float radius, float height, int slices)
{
    GLUquadric *q = gluNewQuadric();
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    gluCylinder(q, radius, radius, height, slices, 1);
    gluDisk(q, 0, radius, slices, 1);
    glTranslatef(0, 0, height);
    gluDisk(q, 0, radius, slices, 1);
    glPopMatrix();
    gluDeleteQuadric(q);
}

void drawSphere(float rx, float ry, float rz)
{
    glPushMatrix();
    glScalef(rx, ry, rz);
    glutSolidSphere(1.0, 22, 22);
    glPopMatrix();
}
