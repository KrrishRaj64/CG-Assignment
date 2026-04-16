#include "Machine.hpp"

#include "../assets/Primitives.hpp"

#include <GL/glut.h>
#include <cmath>

namespace {

void drawMachineArm(float baseX, float baseY, float baseZ, float angleDeg, float length, float radius)
{
    glPushMatrix();
    glTranslatef(baseX, baseY, baseZ);
    glRotatef(angleDeg, 0, 0, 1);

    setColor(0.52f, 0.52f, 0.57f);
    drawCylinder(radius, length);

    // Move to the exact arm endpoint so the hand is physically attached.
    glTranslatef(0.0f, length, 0.0f);
    setColor(0.70f, 0.45f, 0.12f);
    drawSphere(radius * 2.0f, radius * 1.6f, radius * 2.0f);

    setColor(0.56f, 0.36f, 0.10f);
    for (int i = 0; i < 3; i++) {
        float ca = i * 120.0f * 3.14159f / 180.0f;
        glPushMatrix();
        glTranslatef(std::cos(ca) * radius * 1.2f, 0.0f, std::sin(ca) * radius * 1.2f);
        glRotatef(-18.0f, 1, 0, 0);
        drawCylinder(radius * 0.38f, radius * 2.0f);
        glPopMatrix();
    }

    glPopMatrix();
}

} // namespace

void drawMachine(const Machine &m)
{
    glPushMatrix();
    glTranslatef(m.x, 0, 0);
    glRotatef(m.bodyAngle, 0, 1, 0);

    setColor(0.28f, 0.28f, 0.72f);
    glPushMatrix();
    glTranslatef(0, 1.0f, 0);
    glScalef(1.10f, 2.0f, 0.82f);
    glutSolidCube(1.0);
    glPopMatrix();

    setColor(0.22f, 0.22f, 0.60f);
    glPushMatrix();
    glTranslatef(0, 1.1f, 0.42f);
    glScalef(0.80f, 1.50f, 0.02f);
    glutSolidCube(1.0);
    glPopMatrix();

    setColor(0.55f, 0.55f, 0.60f);
    float rx[] = {-0.52f, 0.52f, -0.52f, 0.52f};
    float ry[] = {0.15f, 0.15f, 1.85f, 1.85f};
    for (int i = 0; i < 4; i++) {
        glPushMatrix();
        glTranslatef(rx[i], ry[i], 0.42f);
        drawSphere(0.06f, 0.06f, 0.04f);
        glPopMatrix();
    }

    setColor(0.15f, 0.15f, 0.50f);
    for (int i = 0; i < 4; i++) {
        glPushMatrix();
        glTranslatef(0.56f, 0.5f + i * 0.35f, 0);
        glScalef(0.04f, 0.12f, 0.60f);
        glutSolidCube(1.0);
        glPopMatrix();
    }

    float bl = m.lightBlink;
    setColor(bl, 1.0f - bl * 0.5f, 0.1f);
    glPushMatrix();
    glTranslatef(0, 1.80f, 0.44f);
    drawSphere(0.12f, 0.10f, 0.08f);
    glPopMatrix();
    setColor(0.20f, 0.20f, 0.20f);
    glPushMatrix();
    glTranslatef(0, 1.80f, 0.42f);
    glScalef(1, 1, 0.1f);
    glutSolidTorus(0.02f, 0.14f, 8, 20);
    glPopMatrix();

    setColor(0.90f, 0.10f, 0.10f);
    glPushMatrix();
    glTranslatef(0, 0.90f, 0.44f);
    drawSphere(0.11f, 0.08f, 0.08f);
    glPopMatrix();
    setColor(0.30f, 0.10f, 0.10f);
    glPushMatrix();
    glTranslatef(0, 0.90f, 0.41f);
    glScalef(1, 1, 0.1f);
    glutSolidTorus(0.015f, 0.13f, 6, 18);
    glPopMatrix();

    setColor(0.05f, 0.05f, 0.15f);
    glPushMatrix();
    glTranslatef(0, 1.38f, 0.43f);
    glScalef(0.55f, 0.32f, 0.02f);
    glutSolidCube(1.0);
    glPopMatrix();
    float glow = 0.15f + std::sinf(gTime * 2.0f) * 0.05f;
    setColor(glow, glow * 0.5f, glow * 2.0f);
    glPushMatrix();
    glTranslatef(0, 1.38f, 0.44f);
    glScalef(0.48f, 0.26f, 0.01f);
    glutSolidCube(1.0);
    glPopMatrix();

    setColor(0.18f, 0.18f, 0.62f);
    glPushMatrix();
    glTranslatef(0, 0.27f, 0.41f);
    glRotatef(-m.doorOpen * 75.0f, 1, 0, 0);
    glScalef(0.58f, 0.20f, 0.06f);
    glutSolidCube(1.0);
    glPopMatrix();

    if (m.activated) {
        drawMachineArm(-0.66f, 1.34f, 0.22f, m.armAngle, 0.50f + m.armExtend, 0.08f);
        drawMachineArm(0.66f, 1.06f, 0.22f, -m.armAngle * 0.7f, 0.42f + m.armExtend * 0.5f, 0.07f);
    }

    // Scene 5 steam puff
    if (s5SteamPuff) {
        setColor(0.9f, 0.9f, 0.9f);
        glPushMatrix();
        glTranslatef(0, 2.0f, 0);
        drawSphere(s5PuffScale, s5PuffScale, s5PuffScale * 0.5f);
        glPopMatrix();
    }

    glPopMatrix();
}
