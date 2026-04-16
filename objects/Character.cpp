#include "Character.hpp"

#include "../assets/Primitives.hpp"
#include "../world/State.hpp"

#include <GL/glut.h>
#include <cmath>

namespace {

constexpr float SKIN_R = 0.70f;
constexpr float SKIN_G = 0.56f;
constexpr float SKIN_B = 0.44f;

float clampRange(float v, float lo, float hi)
{
    return v < lo ? lo : (v > hi ? hi : v);
}

void drawBowlerHat(float extraY)
{
    setColor(0.08f, 0.08f, 0.08f);
    glPushMatrix();
    glTranslatef(0, 0.275f + extraY, 0.0f);
    glScalef(0.52f, 0.05f, 0.42f);
    glutSolidCube(1.0);
    glPopMatrix();

    setColor(0.06f, 0.06f, 0.06f);
    glPushMatrix();
    glTranslatef(0, 0.395f + extraY, 0);
    drawSphere(0.17f, 0.18f, 0.16f);
    glPopMatrix();

    setColor(0.15f, 0.15f, 0.15f);
    glPushMatrix();
    glTranslatef(0.05f, 0.445f + extraY, 0.10f);
    drawSphere(0.024f, 0.018f, 0.020f);
    glPopMatrix();
}

void drawFace(float shock, float curious, int mood)
{
    bool happy = (mood == 1);
    bool gasp = (mood == 2);
    bool angry = (mood == 3);
    bool curiousMood = (mood == 4);
    bool morphHappyToGasp = (mood == 5);
    bool photoShock = (mood == 6);

    if (photoShock) {
        // Square "photo" gag overlay with exaggerated shocked face.
        setColor(0.92f, 0.92f, 0.90f);
        glPushMatrix();
        glTranslatef(0.0f, -0.01f, 0.195f);
        glScalef(0.23f, 0.25f, 0.012f);
        glutSolidCube(1.0);
        glPopMatrix();

        setColor(0.10f, 0.10f, 0.10f);
        glPushMatrix();
        glTranslatef(0.0f, -0.01f, 0.201f);
        glScalef(0.238f, 0.258f, 0.004f);
        glutWireCube(1.0);
        glPopMatrix();

        setColor(0.97f, 0.97f, 0.97f);
        glPushMatrix();
        glTranslatef(-0.070f, 0.048f, 0.205f);
        drawSphere(0.036f, 0.044f, 0.016f);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(0.070f, 0.048f, 0.205f);
        drawSphere(0.036f, 0.044f, 0.016f);
        glPopMatrix();

        setColor(0.02f, 0.02f, 0.02f);
        glPushMatrix();
        glTranslatef(-0.070f, 0.048f, 0.212f);
        drawSphere(0.015f, 0.015f, 0.012f);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(0.070f, 0.048f, 0.212f);
        drawSphere(0.015f, 0.015f, 0.012f);
        glPopMatrix();

        setColor(0.12f, 0.08f, 0.06f);
        glPushMatrix();
        glTranslatef(-0.070f, 0.105f, 0.206f);
        glRotatef(16.0f, 0, 0, 1);
        glScalef(0.092f, 0.013f, 0.010f);
        glutSolidCube(1.0);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(0.070f, 0.105f, 0.206f);
        glRotatef(-16.0f, 0, 0, 1);
        glScalef(0.092f, 0.013f, 0.010f);
        glutSolidCube(1.0);
        glPopMatrix();

        setColor(0.14f, 0.04f, 0.04f);
        glPushMatrix();
        glTranslatef(0.0f, -0.090f, 0.208f);
        drawSphere(0.040f, 0.055f, 0.014f);
        glPopMatrix();
        return;
    }

    float eyeOpen = clampRange(0.24f + shock * 0.52f - curious * 0.05f, 0.16f, 0.78f);
    if (happy) {
        eyeOpen = 0.28f;
    }
    if (gasp) {
        eyeOpen = 0.76f;
    }
    if (angry) {
        eyeOpen = 0.24f;
    }
    if (curiousMood) {
        eyeOpen = 0.36f;
    }
    if (morphHappyToGasp) {
        float p = clampRange(shock, 0.0f, 1.0f);
        // Keep happy face readable for most of the morph, then open quickly to gasp.
        float q = p < 0.72f ? (p * 0.30f) : (0.216f + (p - 0.72f) / 0.28f * 0.784f);
        eyeOpen = lerp(0.30f, 0.76f, clampRange(q, 0.0f, 1.0f));
    }

    float browRaise = shock * 0.10f;
    if (gasp) {
        browRaise = 0.12f;
    }
    if (happy) {
        browRaise = 0.03f;
    }

    setColor(0.86f, 0.86f, 0.86f);
    glPushMatrix();
    glTranslatef(-0.075f, 0.055f + browRaise, 0.19f);
    drawSphere(0.043f, 0.036f * eyeOpen, 0.02f);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.075f, 0.055f + browRaise, 0.19f);
    drawSphere(0.043f, 0.036f * eyeOpen, 0.02f);
    glPopMatrix();

    setColor(0.0f, 0.0f, 0.0f);
    glPushMatrix();
    glTranslatef(-0.075f, 0.055f + browRaise, 0.205f);
    drawSphere(0.016f, 0.016f, 0.016f);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.075f, 0.055f + browRaise, 0.205f);
    drawSphere(0.016f, 0.016f, 0.016f);
    glPopMatrix();

    setColor(0.0f, 0.0f, 0.0f);
    glPushMatrix();
    glTranslatef(-0.075f, 0.108f + browRaise, 0.198f);
    float browL = 12.0f + shock * 18.0f;
    float browR = -12.0f - shock * 18.0f;
    if (angry) {
        browL = -20.0f;
        browR = 20.0f;
    } else if (happy) {
        browL = 4.0f;
        browR = -4.0f;
    } else if (curiousMood) {
        browL = 18.0f;
        browR = -8.0f;
    }
    glRotatef(browL, 0, 0, 1);
    glScalef(0.09f, 0.012f, 0.02f);
    glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.075f, 0.108f + browRaise, 0.198f);
    glRotatef(browR, 0, 0, 1);
    glScalef(0.09f, 0.012f, 0.02f);
    glutSolidCube(1.0);
    glPopMatrix();

    setColor(0.04f, 0.04f, 0.04f);
    glPushMatrix();
    glTranslatef(-0.032f, -0.060f, 0.206f);
    glScalef(0.052f, 0.064f, 0.03f);
    glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.032f, -0.060f, 0.206f);
    glScalef(0.052f, 0.064f, 0.03f);
    glutSolidCube(1.0);
    glPopMatrix();

    setColor(0.14f, 0.05f, 0.05f);
    glPushMatrix();
    glTranslatef(0.0f, -0.133f, 0.197f);
    if (gasp || shock > 0.45f) {
        drawSphere(0.034f, 0.040f, 0.02f);
    } else if (morphHappyToGasp) {
        float p = clampRange(shock, 0.0f, 1.0f);
        // Hold smile longer so mid-morph does not look sad/flat.
        if (p < 0.78f) {
            float t = p / 0.78f;
            float ang = lerp(28.0f, 8.0f, t);
            float width = lerp(0.080f, 0.062f, t);
            glPushMatrix();
            glTranslatef(-0.035f, 0.002f, 0.0f);
            glRotatef(ang, 0, 0, 1);
            glScalef(width, 0.012f, 0.02f);
            glutSolidCube(1.0);
            glPopMatrix();
            glPushMatrix();
            glTranslatef(0.035f, 0.002f, 0.0f);
            glRotatef(-ang, 0, 0, 1);
            glScalef(width, 0.012f, 0.02f);
            glutSolidCube(1.0);
            glPopMatrix();
            glPushMatrix();
            glTranslatef(0.0f, -0.006f, 0.0f);
            glScalef(0.040f, 0.010f, 0.02f);
            glutSolidCube(1.0);
            glPopMatrix();
        } else {
            float t = (p - 0.78f) / 0.22f;
            drawSphere(lerp(0.018f, 0.034f, t), lerp(0.020f, 0.040f, t), 0.02f);
        }
    } else if (happy) {
        // If this is the Scene 4 happy close-up window, draw a compact arc mouth
        // (small continuous arc attached to the spherical face) so it reads clearly.
        if (currentScene == 4 && sceneLocalTime >= 50.0f && sceneLocalTime < 55.0f) {
            glPushMatrix();
            glTranslatef(0.0f, -0.133f, 0.197f);
            // Draw a thin pink arc for the close-up: fewer, smaller spheres and slightly forward.
            setColor(0.96f, 0.62f, 0.68f);
            for (int i = 0; i <= 6; ++i) {
                float t = (float)i / 6.0f;
                float ang = (-26.0f + t * 52.0f) * 0.0174532925f;
                float rx = std::sin(ang) * 0.040f;
                float ry = - (1.0f - std::cos(ang)) * 0.0085f;
                glPushMatrix();
                // move slightly forward on Z so the arc reads against the face sphere.
                glTranslatef(rx, ry, 0.006f);
                drawSphere(0.006f, 0.006f, 0.006f);
                glPopMatrix();
            }
            // restore default color for subsequent drawing
            setColor(0.14f, 0.05f, 0.05f);
            glPopMatrix();
        } else {
            // Draw a broad upward smile so the happy mood reads clearly on camera.
            glPushMatrix();
            glTranslatef(-0.038f, 0.004f, 0.0f);
            glRotatef(36.0f, 0, 0, 1);
            glScalef(0.092f, 0.013f, 0.02f);
            glutSolidCube(1.0);
            glPopMatrix();
            glPushMatrix();
            glTranslatef(0.038f, 0.004f, 0.0f);
            glRotatef(-36.0f, 0, 0, 1);
            glScalef(0.092f, 0.013f, 0.02f);
            glutSolidCube(1.0);
            glPopMatrix();
            glPushMatrix();
            glTranslatef(0.0f, -0.005f, 0.0f);
            glScalef(0.046f, 0.011f, 0.02f);
            glutSolidCube(1.0);
            glPopMatrix();
        }
    } else if (angry) {
        glScalef(0.12f, 0.015f, 0.02f);
        glutSolidCube(1.0);
    } else if (curiousMood) {
        glPushMatrix();
        glTranslatef(-0.022f, -0.002f, 0.0f);
        glRotatef(12.0f, 0, 0, 1);
        glScalef(0.055f, 0.010f, 0.02f);
        glutSolidCube(1.0);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(0.022f, -0.002f, 0.0f);
        glRotatef(-8.0f, 0, 0, 1);
        glScalef(0.055f, 0.010f, 0.02f);
        glutSolidCube(1.0);
        glPopMatrix();
    } else {
        float smile = 0.012f + curious * 0.020f;
        glScalef(0.10f, smile, 0.02f);
        glutSolidCube(1.0);
    }
    glPopMatrix();
}

void drawHead(float earLean, float shock, float curious, int mood)
{
    glPushMatrix();
    glTranslatef(earLean * 0.5f, 0, 0);

    setColor(SKIN_R, SKIN_G, SKIN_B);
    drawSphere(0.20f, 0.22f, 0.20f);

    // Keep ear semicircles exactly the same visible tone as face skin.
    glDisable(GL_LIGHTING);
    setColor(SKIN_R, SKIN_G, SKIN_B);
    glPushMatrix();
    glTranslatef(-0.20f, -0.005f, 0);
    drawSphere(0.045f, 0.060f, 0.038f);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.20f, -0.005f, 0);
    drawSphere(0.045f, 0.060f, 0.038f);
    glPopMatrix();
    glEnable(GL_LIGHTING);

    setColor(SKIN_R, SKIN_G, SKIN_B);
    glPushMatrix();
    glTranslatef(0, -0.012f, 0.19f);
    drawSphere(0.042f, 0.036f, 0.034f);
    glPopMatrix();

    drawFace(shock, curious, mood);
    if (currentScene != 5 || s5HatOnHead) {
        drawBowlerHat(shock * 0.07f);
    }

    glPopMatrix();
}

void drawArm(float shoulderAngle, float elbowAngle, bool isLeft, bool holdCane, float caneAngle)
{
    float side = isLeft ? -1.0f : 1.0f;
    glPushMatrix();
    glTranslatef(side * 0.25f, 0.50f, side * 0.08f);
    glRotatef(shoulderAngle, 1, 0, 0);

    setColor(0.10f, 0.10f, 0.10f);
    glPushMatrix();
    glTranslatef(0, -0.17f, 0);
    glScalef(0.10f, 0.34f, 0.10f);
    glutSolidCube(1.0);
    glPopMatrix();

    glTranslatef(0, -0.34f, 0);
    drawSphere(0.05f, 0.05f, 0.05f);
    glRotatef(elbowAngle, 1, 0, 0);

    setColor(0.13f, 0.13f, 0.13f);
    glPushMatrix();
    glTranslatef(0, -0.15f, 0);
    glScalef(0.09f, 0.30f, 0.09f);
    glutSolidCube(1.0);
    glPopMatrix();

    glTranslatef(0, -0.28f, 0);
    setColor(0.86f, 0.71f, 0.56f);
    drawSphere(0.055f, 0.055f, 0.05f);

    if (holdCane) {
        glPushMatrix();
        glRotatef(16.0f, 0, 1, 0);
        glRotatef(caneAngle, 1, 0, 0);
        setColor(0.56f, 0.36f, 0.12f);
        glPushMatrix();
        glTranslatef(0.03f, -0.42f, 0.01f);
        glScalef(0.028f, 0.84f, 0.028f);
        glutSolidCube(1.0);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0.03f, -0.84f, 0.01f);
        glutSolidTorus(0.007f, 0.042f, 8, 12);
        glPopMatrix();
        glPopMatrix();
    }
    glPopMatrix();
}

void drawLeg(float hipAngle, float kneeBias, bool isLeft, float shoeBend)
{
    float side = isLeft ? -1.0f : 1.0f;
    glPushMatrix();
    glTranslatef(side * 0.11f, -0.02f, side * 0.03f);
    glRotatef(hipAngle, 1, 0, 0);

    setColor(0.08f, 0.08f, 0.08f);
    glPushMatrix();
    glTranslatef(0, -0.20f, 0);
    glScalef(0.12f, 0.40f, 0.12f);
    glutSolidCube(1.0);
    glPopMatrix();

    glTranslatef(0, -0.40f, 0);
    drawSphere(0.055f, 0.055f, 0.055f);
    glRotatef(kneeBias, 1, 0, 0);

    glPushMatrix();
    glTranslatef(0, -0.18f, 0);
    glScalef(0.105f, 0.36f, 0.105f);
    glutSolidCube(1.0);
    glPopMatrix();

    glTranslatef(0, -0.36f, 0);
    setColor(0.86f, 0.71f, 0.56f);
    drawSphere(0.045f, 0.045f, 0.045f);

    setColor(0.05f, 0.05f, 0.05f);
    glPushMatrix();
    glRotatef(shoeBend, 1, 0, 0);
    glTranslatef(side * 0.05f, -0.03f, 0.10f);
    glScalef(0.21f, 0.08f, 0.30f);
    glutSolidCube(1.0);
    glPopMatrix();
    glPopMatrix();
}

} // namespace

void drawCharacter(const Character &c)
{
    float shock = clampRange(std::fabs(c.bodyTilt) / 24.0f + (c.isHit ? 0.65f : 0.0f) + (c.trapped ? 0.30f : 0.0f), 0.0f, 1.0f);
    float curious = clampRange(c.chestOut * 0.90f + std::fabs(c.earLean) * 2.8f, 0.0f, 1.0f);
    bool pronePose =
        (currentScene == 4 && sceneLocalTime >= 6.8f && sceneLocalTime < 10.0f);
    if (c.faceMood == 1) {
        shock = 0.0f;
        curious = 0.85f;
    } else if (c.faceMood == 2) {
        shock = 1.0f;
        curious = 0.10f;
    } else if (c.faceMood == 3) {
        shock = 0.35f;
        curious = 0.20f;
    } else if (c.faceMood == 5) {
        shock = clampRange(c.faceShockBlend, 0.0f, 1.0f);
        curious = 0.40f * (1.0f - shock);
    }
    float torsoTilt = clampRange(c.bodyTilt, -35.0f, 35.0f);
    float torsoPitch = clampRange(c.bodyPitch, -90.0f, 90.0f);
    float chestPitch = torsoPitch;
    float headPitch = c.headPitch;

    if (pronePose) {
        glPushMatrix();
        glTranslatef(c.x, c.y + 0.01f, c.z);
        glRotatef(c.facingYaw, 0, 1, 0);

        // Lay the body flat on the ground plane instead of reusing the upright rig.
        setColor(0.12f, 0.12f, 0.12f);
        glPushMatrix();
        glTranslatef(0.0f, 0.03f, 0.0f);
        glScalef(0.62f, 0.10f, 0.28f);
        glutSolidCube(1.0);
        glPopMatrix();

        setColor(0.18f, 0.18f, 0.18f);
        glPushMatrix();
        glTranslatef(-0.22f, 0.02f, 0.10f);
        glRotatef(-26.0f, 0, 1, 0);
        glScalef(0.22f, 0.05f, 0.08f);
        glutSolidCube(1.0);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(0.22f, 0.02f, 0.10f);
        glRotatef(26.0f, 0, 1, 0);
        glScalef(0.22f, 0.05f, 0.08f);
        glutSolidCube(1.0);
        glPopMatrix();

        setColor(0.96f, 0.96f, 0.92f);
        glPushMatrix();
        glTranslatef(0.0f, 0.04f, 0.16f);
        glScalef(0.14f, 0.08f, 0.02f);
        glutSolidCube(1.0);
        glPopMatrix();

        setColor(0.07f, 0.07f, 0.07f);
        glPushMatrix();
        glTranslatef(0.0f, 0.07f, 0.00f);
        glScalef(0.14f, 0.03f, 0.03f);
        glutSolidCube(1.0);
        glPopMatrix();

        // Head placed away from the torso so it does not merge into the lower body.
        glPushMatrix();
        glTranslatef(0.00f, 0.19f, 0.18f);
        drawHead(c.earLean, 0.0f, 0.20f, c.faceMood);
        glPopMatrix();

        // Arms laid out flat and separated.
        glPushMatrix();
        glTranslatef(-0.30f, 0.05f, 0.05f);
        glRotatef(-72.0f, 0, 1, 0);
        setColor(0.10f, 0.10f, 0.10f);
        glPushMatrix();
        glTranslatef(0.0f, 0.00f, 0.00f);
        glScalef(0.08f, 0.22f, 0.08f);
        glutSolidCube(1.0);
        glPopMatrix();
        setColor(0.13f, 0.13f, 0.13f);
        glPushMatrix();
        glTranslatef(0.0f, -0.16f, 0.00f);
        glScalef(0.07f, 0.18f, 0.07f);
        glutSolidCube(1.0);
        glPopMatrix();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0.30f, 0.05f, 0.05f);
        glRotatef(72.0f, 0, 1, 0);
        setColor(0.10f, 0.10f, 0.10f);
        glPushMatrix();
        glTranslatef(0.0f, 0.00f, 0.00f);
        glScalef(0.08f, 0.22f, 0.08f);
        glutSolidCube(1.0);
        glPopMatrix();
        setColor(0.13f, 0.13f, 0.13f);
        glPushMatrix();
        glTranslatef(0.0f, -0.16f, 0.00f);
        glScalef(0.07f, 0.18f, 0.07f);
        glutSolidCube(1.0);
        glPopMatrix();
        glPopMatrix();

        // Legs extended flat behind the torso.
        glPushMatrix();
        glTranslatef(-0.14f, -0.02f, -0.18f);
        glRotatef(-4.0f, 0, 1, 0);
        setColor(0.08f, 0.08f, 0.08f);
        glPushMatrix();
        glTranslatef(0.0f, -0.18f, 0.0f);
        glScalef(0.10f, 0.34f, 0.10f);
        glutSolidCube(1.0);
        glPopMatrix();
        glTranslatef(0.0f, -0.34f, 0.0f);
        setColor(0.86f, 0.71f, 0.56f);
        drawSphere(0.045f, 0.045f, 0.045f);
        setColor(0.05f, 0.05f, 0.05f);
        glPushMatrix();
        glTranslatef(-0.02f, -0.01f, 0.08f);
        glScalef(0.18f, 0.07f, 0.24f);
        glutSolidCube(1.0);
        glPopMatrix();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0.14f, -0.02f, -0.18f);
        glRotatef(4.0f, 0, 1, 0);
        setColor(0.08f, 0.08f, 0.08f);
        glPushMatrix();
        glTranslatef(0.0f, -0.18f, 0.0f);
        glScalef(0.10f, 0.34f, 0.10f);
        glutSolidCube(1.0);
        glPopMatrix();
        glTranslatef(0.0f, -0.34f, 0.0f);
        setColor(0.86f, 0.71f, 0.56f);
        drawSphere(0.045f, 0.045f, 0.045f);
        setColor(0.05f, 0.05f, 0.05f);
        glPushMatrix();
        glTranslatef(0.02f, -0.01f, 0.08f);
        glScalef(0.18f, 0.07f, 0.24f);
        glutSolidCube(1.0);
        glPopMatrix();
        glPopMatrix();

        glPopMatrix();
        return;
    }

    glPushMatrix();
    glTranslatef(c.x, c.y, c.z);
    glRotatef(c.facingYaw, 0, 1, 0);

    float chestZ = c.chestOut * 0.04f;
    float bowSuppression = 1.0f - clampRange(std::fabs(torsoPitch) / 18.0f, 0.0f, 1.0f);
    float legL = c.legLAngle * bowSuppression;
    float legR = c.legRAngle * bowSuppression;

    float kneeL = 3.0f + std::fabs(legL) * 0.15f;
    float kneeR = 3.0f + std::fabs(legR) * 0.15f;
    drawLeg(legL, kneeL, true, c.shoeBend * bowSuppression);
    drawLeg(legR, kneeR, false, 0.0f);

    glPushMatrix();
    glScalef(1.0f, c.reactionScale, 1.0f);
    glTranslatef(0, 0.08f, 0);
    glRotatef(chestPitch, 1, 0, 0);
    glRotatef(torsoTilt, 0, 0, 1);

    setColor(0.12f, 0.12f, 0.12f);
    glPushMatrix();
    glTranslatef(0, 0.30f, chestZ);
    glScalef(0.44f, 0.62f, 0.28f);
    glutSolidCube(1.0);
    glPopMatrix();

    setColor(0.18f, 0.18f, 0.18f);
    glPushMatrix();
    glTranslatef(-0.08f, 0.36f, 0.14f + chestZ);
    glRotatef(-15, 0, 0, 1);
    glScalef(0.08f, 0.20f, 0.03f);
    glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.08f, 0.36f, 0.14f + chestZ);
    glRotatef(15, 0, 0, 1);
    glScalef(0.08f, 0.20f, 0.03f);
    glutSolidCube(1.0);
    glPopMatrix();

    setColor(0.96f, 0.96f, 0.92f);
    glPushMatrix();
    glTranslatef(0, 0.30f, 0.16f + chestZ);
    glScalef(0.09f, 0.30f, 0.02f);
    glutSolidCube(1.0);
    glPopMatrix();

    setColor(0.08f, 0.08f, 0.08f);
    glPushMatrix();
    glTranslatef(0, 0.43f, 0.17f + chestZ);
    glScalef(0.11f, 0.05f, 0.03f);
    glutSolidCube(1.0);
    glPopMatrix();

    setColor(0.07f, 0.07f, 0.07f);
    glPushMatrix();
    glTranslatef(0, -0.02f, chestZ);
    glScalef(0.45f, 0.05f, 0.30f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    // Head lift for the Scene 4 happy close-up: keep face clearly above the torso
    // while remaining relative to the body movement.
    float headLift = 0.0f;
    if (currentScene == 4 && sceneLocalTime >= 50.0f && sceneLocalTime < 55.0f) {
        // Small upward offset (in world units) applied to the head translation.
        headLift = 0.12f;
    }
    // Combine hat extra offset and dynamic head lift; head lift follows body transform.
    glTranslatef(0, 0.70f + c.hatExtraY * 0.10f + headLift, 0.02f);
    glRotatef(headPitch, 1, 0, 0);
    drawHead(c.earLean, shock, curious, c.faceMood);
    glPopMatrix();

    drawArm(c.armLAngle, c.foreArmAngle, true, false, 0.0f);
    drawArm(c.armRAngle, c.foreArmAngle * 0.7f, false, true, c.caneAngle);

    glPopMatrix();

    // Scene 5 detached hat when it is not on Chaplin's head.
    if (currentScene == 5 && !s5HatOnHead) {
        glPushMatrix();
        glTranslatef(s5HatX, s5HatY, s5HatZ);
        glRotatef(s5HatRotX, 1, 0, 0);
        glRotatef(s5HatRotY, 0, 1, 0);
        glRotatef(s5HatRotZ, 0, 0, 1);
        drawBowlerHat(0.0f);
        glPopMatrix();
    }

    glPopMatrix();
}
