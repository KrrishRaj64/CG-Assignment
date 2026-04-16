#include "App.hpp"

#include "SceneLogic.hpp"
#include "State.hpp"
#include "../assets/Environment.hpp"
#include "../objects/Character.hpp"
#include "../objects/Machine.hpp"

#include <GL/glut.h>
#include <cmath>
#include <cstdlib>

namespace {

bool isMachineOccluding(float camX, float camY, float camZ, float tx, float ty, float tz)
{
    const float halfX = MACHINE_W + 0.22f;
    const float halfY = 1.10f;
    const float halfZ = 0.44f;

    for (int i = 1; i < 16; ++i) {
        float t = (float)i / 16.0f;
        float sx = lerp(camX, tx, t);
        float sy = lerp(camY, ty, t);
        float sz = lerp(camZ, tz, t);
        if (std::fabs(sx - machine.x) < halfX && std::fabs(sy - 1.0f) < halfY && std::fabs(sz) < halfZ) {
            return true;
        }
    }
    return false;
}

void setOcclusionSafeFaceCam(float &camX, float &camY, float &camZ, float tx, float ty, float tz, float fx, float fz)
{
    float sx = -fz;
    float sz = fx;

    if (!isMachineOccluding(camX, camY, camZ, tx, ty, tz)) {
        return;
    }

    float dir = (machine.x > tx) ? -1.0f : 1.0f;
    for (int k = 1; k <= 6; ++k) {
        float slip = 0.24f * (float)k * dir;
        float testX = camX + sx * slip;
        float testZ = camZ + sz * slip;
        if (!isMachineOccluding(testX, camY, testZ, tx, ty, tz)) {
            camX = testX;
            camZ = testZ;
            return;
        }
    }

    camY += 0.28f;
}

void keepOutsideMachine(float &x, float &z, float margin = 0.22f)
{
    float halfX = MACHINE_W + margin;
    float halfZ = 0.44f + margin;
    if (std::fabs(x - machine.x) < halfX && std::fabs(z) < halfZ) {
        float dx = (x - machine.x);
        float dz = z;
        if (std::fabs(dx) < std::fabs(dz)) {
            x = machine.x + (dx >= 0.0f ? halfX : -halfX);
        } else {
            z = (dz >= 0.0f ? halfZ : -halfZ);
        }
    }
}

void setCinematicCamera()
{
    float lt = sceneLocalTime;
    float bob = std::sinf(gTime * 1.0f) * 0.04f;

    if (currentScene == 1) {
        if (lt >= 10.0f && lt < 17.0f) {
            float yr = tramp.facingYaw * 0.0174533f;
            float fx = std::sin(yr);
            float fz = std::cos(yr);
            float hx = tramp.x;
            float hy = tramp.y + 1.02f;
            float hz = tramp.z;
            float camX = hx + fx * 1.95f - fz * 0.28f;
            float camY = hy + 0.14f + bob;
            float camZ = hz + fz * 1.95f + fx * 0.28f;
            gluLookAt(camX, camY, camZ, hx, hy, hz, 0, 1, 0);
            return;
        }

        if (lt >= 17.0f && lt < 22.0f) {
            // Stable machine reveal shot to avoid clipping during spin path.
            float camX = machine.x - 2.10f;
            float camY = 1.95f + bob;
            float camZ = 3.10f;
            gluLookAt(camX, camY, camZ, machine.x, 0.95f, 0.05f, 0, 1, 0);
            return;
        }

        if (lt >= 22.0f && lt < 24.0f) {
            float hx = tramp.x;
            float hy = tramp.y + 1.02f;
            float hz = tramp.z;
            float toMx = machine.x - hx;
            float toMz = -hz;
            float d = std::sqrt(toMx * toMx + toMz * toMz);
            if (d < 0.001f) {
                d = 0.001f;
            }
            float fx = toMx / d;
            float fz = toMz / d;
            float sx = -fz;
            float sz = fx;
            float camX = hx + fx * 1.10f + sx * 1.00f;
            float camY = hy + 0.18f + bob;
            float camZ = hz + fz * 1.10f + sz * 1.00f;
            keepOutsideMachine(camX, camZ, 0.30f);
            gluLookAt(camX, camY, camZ, hx, hy, hz, 0, 1, 0);
            return;
        }

        if (lt >= 24.0f && lt < 25.0f) {
            float hx = tramp.x;
            float hy = tramp.y + 1.02f;
            float hz = tramp.z;
            float toMx = machine.x - hx;
            float toMz = -hz;
            float d = std::sqrt(toMx * toMx + toMz * toMz);
            if (d < 0.001f) {
                d = 0.001f;
            }
            float fx = toMx / d;
            float fz = toMz / d;
            float sx = -fz;
            float sz = fx;
            float p = lt - 24.0f;
            float camX = hx + fx * (1.15f + p * 0.25f) + sx * (0.90f - p * 0.35f);
            float camY = hy + 0.18f + bob;
            float camZ = hz + fz * (1.15f + p * 0.25f) + sz * (0.90f - p * 0.35f);
            keepOutsideMachine(camX, camZ, 0.30f);
            gluLookAt(camX, camY, camZ, hx, hy, hz, 0, 1, 0);
            return;
        }

        if (lt < 26.0f) {
            float yr = tramp.facingYaw * 0.0174533f;
            float fx = std::sin(yr);
            float fz = std::cos(yr);
            float hx = tramp.x;
            float hy = tramp.y + 1.00f;
            float hz = tramp.z;
            // Dolly-in-front shot that tracks Chaplin and moves backward with him.
            float camX = hx + fx * 2.15f - fz * 0.24f;
            float camY = hy + 0.14f + bob;
            float camZ = hz + fz * 2.15f + fx * 0.24f;
            gluLookAt(camX, camY, camZ, hx, hy, hz, 0, 1, 0);
            return;
        }

        // Expression close-up: fixed safe front-quarter shot (avoids machine/body occlusion).
        if (lt > 36.0f && lt < 42.0f) {
            float hx = tramp.x;
            float hy = tramp.y + 1.00f;
            float hz = tramp.z;
            float camX = hx - 0.18f;
            float camY = hy + 0.08f;
            float camZ = hz + 2.05f;
            gluLookAt(camX, camY, camZ, hx, hy, hz, 0, 1, 0);
            return;
        }

        if (lt > 42.0f && lt < 48.5f) {
            float hx = tramp.x;
            float hy = tramp.y + 1.02f;
            float hz = tramp.z;
            float camX = hx - 0.22f;
            float camY = hy + 0.10f;
            float camZ = hz + 2.10f;
            gluLookAt(camX, camY, camZ, hx, hy, hz, 0, 1, 0);
            return;
        }

        if (lt >= 55.0f && lt < 60.0f) {
            float p = (lt - 55.0f) / 5.0f;
            float yr = tramp.facingYaw * 0.0174533f;
            float fx = std::sin(yr);
            float fz = std::cos(yr);
            float rx = fz;
            float rz = -fx;
            float hx = tramp.x;
            float hy = tramp.y + 1.03f;
            float hz = tramp.z;
            // Start near his back-quarter, pan around to front while keeping face target.
            float theta = 3.14159f * (1.0f - p);
            float r = 2.15f;
            float camX = hx + fx * std::cos(theta) * r + rx * std::sin(theta) * 0.55f;
            float camY = hy + 0.10f;
            float camZ = hz + fz * std::cos(theta) * r + rz * std::sin(theta) * 0.55f;
            gluLookAt(camX, camY, camZ, hx, hy, hz, 0, 1, 0);
            return;
        }

        // Side-left composition after 0:26.
        float cx = tramp.x - 2.8f;
        gluLookAt(cx, 2.0f + bob, 5.4f, tramp.x + 0.55f, 0.92f, tramp.z, 0, 1, 0);
        return;
    }

    if (currentScene == 2) {
        if (lt < 8.0f) {
            float cx = (tramp.x + machine.x) * 0.5f;
            gluLookAt(cx - 2.4f, 2.0f + bob, 5.1f, cx, 0.95f, 0.28f, 0, 1, 0);
            return;
        }

        if (lt >= 8.0f && lt < 18.0f) {
            float hx = tramp.x;
            float hy = tramp.y + 1.02f;
            float hz = tramp.z;
            float dx = machine.x - hx;
            float dz = -hz;
            float d = std::sqrt(dx * dx + dz * dz);
            if (d < 0.0001f) {
                d = 0.0001f;
            }
            float fx = dx / d;
            float fz = dz / d;
            float sx = -fz;
            float sz = fx;
            float camX = hx + fx * 1.00f + sx * 1.05f;
            float camY = hy + 0.20f + bob;
            float camZ = hz + fz * 1.00f + sz * 1.05f;
            setOcclusionSafeFaceCam(camX, camY, camZ, hx, hy, hz, fx, fz);
            gluLookAt(camX, camY, camZ, hx, hy, hz, 0, 1, 0);
            return;
        }

        if (lt >= 18.0f && lt < 34.0f) {
            float tx = machine.x - 0.10f;
            float ty = 1.05f;
            float tz = 0.00f;
            float ox = tramp.x - 1.15f;
            float oy = tramp.y + 1.65f + bob;
            float oz = tramp.z + 1.85f;
            gluLookAt(ox, oy, oz, tx, ty, tz, 0, 1, 0);
            return;
        }

        if (lt >= 34.0f && lt < 40.0f) {
            float hx = tramp.x;
            float hy = tramp.y + 1.02f;
            float hz = tramp.z;
            float yr = tramp.facingYaw * 0.0174533f;
            float fx = std::sin(yr);
            float fz = std::cos(yr);
            float camX = hx + fx * 2.10f - fz * 0.22f;
            float camY = hy + 0.14f + bob;
            float camZ = hz + fz * 2.10f + fx * 0.22f;
            setOcclusionSafeFaceCam(camX, camY, camZ, hx, hy, hz, fx, fz);
            gluLookAt(camX, camY, camZ, hx, hy, hz, 0, 1, 0);
            return;
        }

        if (lt >= 40.0f && lt < 48.0f) {
            // Lever action shot: in front of Chaplin's face, pulled back slightly,
            // with a mild downward look so both Chaplin and lever stay readable.
            float yr = tramp.facingYaw * 0.0174533f;
            float fx = std::sin(yr);
            float fz = std::cos(yr);
            float rx = fz;
            float rz = -fx;
            float hx = tramp.x;
            float hy = tramp.y + 1.02f;
            float hz = tramp.z;

            float camX = hx + fx * 3.20f - rx * 0.35f;
            float camY = hy + 1.15f + bob;
            float camZ = hz + fz * 3.20f - rz * 0.35f;

            float tx = lerp(hx, machine.x - 1.05f, 0.62f);
            float ty = 0.78f;
            float tz = lerp(hz, 0.50f, 0.62f);
            gluLookAt(camX, camY, camZ, tx, ty, tz, 0, 1, 0);
            return;
        }

        if (lt >= 48.0f && lt < 50.0f) {
            float p = (lt - 48.0f) / 2.0f;
            float camX = lerp(machine.x + 2.30f, machine.x + 1.30f, p);
            float camY = lerp(2.05f, 2.10f, p) + bob;
            float camZ = lerp(0.35f, 3.80f, p);
            float tx = lerp(machine.x - 0.95f, machine.x - 0.30f, p);
            float ty = lerp(0.95f, 1.18f, p);
            float tz = lerp(0.20f, 0.0f, p);
            gluLookAt(camX, camY, camZ, tx, ty, tz, 0, 1, 0);
            return;
        }

        float centerX = (tramp.x + machine.x) * 0.5f;
        gluLookAt(centerX - 2.0f, 2.15f + bob, 4.9f, machine.x - 0.35f, 1.00f, 0.0f, 0, 1, 0);
        return;
    }

    if (currentScene == 3) {
        // Fixed fight camera.
        float centerX = (machine.x + tramp.x) * 0.5f;
        gluLookAt(centerX, 1.7f, 5.0f, centerX, 0.95f, 0.0f, 0, 1, 0);
        return;
    }

    if (currentScene == 4) {
        if (lt < 3.0f) {
            // Clamp/wind-up shot.
            float camX = machine.x - 1.7f;
            float camY = 2.15f + bob;
            float camZ = 4.8f;
            gluLookAt(camX, camY, camZ, machine.x - 0.8f, 0.95f, 0.0f, 0, 1, 0);
            return;
        }

        if (lt < 6.6f) {
            // Follow the throw the other way as he exits frame.
            float p = easeInOut(clamp01((lt - 3.0f) / 3.6f));
            float camX = lerp(machine.x - 0.6f, WORLD_LEFT - 0.3f, p);
            float camY = lerp(2.05f, 1.75f, p) + bob;
            float camZ = lerp(4.4f, 3.2f, p);
            float tx = lerp(machine.x - 0.75f, WORLD_LEFT - 1.0f, p);
            float ty = lerp(1.05f, 0.35f, p);
            float tz = lerp(0.0f, -0.10f, p);
            gluLookAt(camX, camY, camZ, tx, ty, tz, 0, 1, 0);
            return;
        }

        if (lt < 10.0f) {
            // Landed flat: camera lifts slightly so the pose reads clearly from above.
            float p = easeInOut(clamp01((lt - 6.8f) / 3.2f));
            float camX = lerp(WORLD_LEFT - 0.35f, WORLD_LEFT + 0.70f, p);
            float camY = lerp(2.65f, 2.45f, p) + bob;
            float camZ = lerp(2.15f, 2.85f, p);
            float tx = lerp(WORLD_LEFT - 0.70f, WORLD_LEFT + 0.35f, p);
            float ty = lerp(-0.10f, -0.22f, p);
            float tz = lerp(-0.10f, 0.02f, p);
            gluLookAt(camX, camY, camZ, tx, ty, tz, 0, 1, 0);
            return;
        }

        if (lt < 23.0f) {
            // Keep the wake-up offscreen and hold on the machine.
            float camX = machine.x - 1.75f;
            float camY = 2.12f + bob;
            float camZ = 4.85f;
            gluLookAt(camX, camY, camZ, machine.x - 0.95f, 0.95f, 0.0f, 0, 1, 0);
            return;
        }

        if (lt < 32.0f) {
            // Rejoin Chaplin as he comes back and walks properly to the lever.
            // Move camera a touch further left for a clearer face read.
            float camX = tramp.x - 1.80f;
            float camY = 2.02f + bob;
            float camZ = 4.55f;
            float tx = lerp(tramp.x, machine.x - 1.05f, 0.55f);
            float ty = tramp.y + 0.78f;
            float tz = 0.18f;
            gluLookAt(camX, camY, camZ, tx, ty, tz, 0, 1, 0);
            return;
        }

        if (lt >= 50.0f && lt < 55.0f) {
            // Happy close-up after stopping the machine.
            // Rotate the camera 30 degrees to the right around Chaplin (world-space)
            // so the face is brought into view even if the machine is nearby.
            const float angDeg = -30.0f; // negative = rotate to the right
            const float A = angDeg * 0.017453292519943295f;
            const float ca = std::cos(A);
            const float sa = std::sin(A);
            float yr = tramp.facingYaw * 0.0174533f;
            float fx = std::sin(yr);
            float fz = std::cos(yr);
            float sx = -fz; // left vector
            float sz = fx;
            // rotate forward vector by A using forward * cos + left * sin
            float rx = fx * ca + sx * sa;
            float rz = fz * ca + sz * sa;
            float hx = tramp.x;
            float hy = tramp.y + 1.02f;
            float hz = tramp.z;
            float camDist = 2.20f;
            float camX = hx + rx * camDist + sx * 0.28f;
            float camY = hy + 0.45f;
            float camZ = hz + rz * camDist + sz * 0.12f;
            float lookX = hx + rx * 0.12f;
            float lookY = hy + 0.12f;
            float lookZ = hz + rz * 0.08f;
            gluLookAt(camX, camY, camZ, lookX, lookY, lookZ, 0, 1, 0);
            return;
        }

        // Original wide position while the machine transforms back to normal.
        gluLookAt(machine.x - 1.6f, 2.2f, 5.0f, machine.x - 1.1f, 0.95f, 0.0f, 0, 1, 0);
        return;
    }

    // Finale side angle.
    if (lt < 15.0f) {
        gluLookAt(machine.x - 2.4f, 2.0f, 5.0f, tramp.x, 0.9f, 0.0f, 0, 1, 0);
        return;
    }

    // Keep frame mostly fixed; tiny follow only after mid-walk so forward travel reads clearly.
    float follow = clamp01((tramp.x - (machine.x + 1.2f)) / 5.0f);
    float camX = lerp(machine.x - 2.25f, tramp.x - 2.40f, follow * 0.18f);
    float lookX = lerp(machine.x + 1.10f, tramp.x + 0.90f, follow * 0.22f);
    gluLookAt(camX, 1.88f + bob, 5.25f, lookX, 0.54f, 0.0f, 0, 1, 0);
}

} // namespace

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    setCinematicCamera();

    drawGround();
    drawBuilding(-8.5f, 3.5f, 5.2f, 0.45f, 0.42f, 0.38f);
    drawBuilding(-4.8f, 2.5f, 4.2f, 0.38f, 0.36f, 0.32f);
    drawBuilding(-1.0f, 2.0f, 3.5f, 0.42f, 0.40f, 0.35f);
    drawBuilding(1.8f, 1.9f, 3.2f, 0.40f, 0.37f, 0.33f);
    drawBuilding(4.6f, 2.2f, 4.1f, 0.36f, 0.34f, 0.31f);
    drawBuilding(6.5f, 2.8f, 4.8f, 0.40f, 0.38f, 0.34f);
    drawBuilding(9.1f, 2.6f, 4.5f, 0.38f, 0.36f, 0.32f);
    drawStreetLamp(-5.8f);
    drawStreetLamp(-2.2f);
    drawStreetLamp(1.2f);
    drawStreetLamp(4.9f);
    drawBench(-2.5f);

    drawLever(machine.x - 1.05f, 0.52f, s2LeverPull);
    drawMachine(machine);

    // The wire trap box looked like a debug hitbox in captures, so keep it hidden.

    for (auto &p : projectiles) {
        if (p.active) {
            drawProjectile(p);
        }
    }

    if (currentScene == 5 && !s5GotGift) {
        drawGift(machine.x - 0.3f, s5GiftY);
    }

    drawCharacter(tramp);
    drawHUD();

    glutSwapBuffers();
}

void timerCB(int)
{
    updateAll(0.016f);
    glutPostRedisplay();
    glutTimerFunc(16, timerCB, 0);
}

void keyboard(unsigned char key, int, int)
{
    if (key == 'b' || key == 'B') {
        bwMode = !bwMode;
    }
    if (key == 27) {
        std::exit(0);
    }
}

void reshape(int w, int h)
{
    if (!h) {
        h = 1;
    }
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / h, 0.1, 200.0);
    glMatrixMode(GL_MODELVIEW);
}

void init()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);

    GLfloat amb[] = {0.22f, 0.22f, 0.22f, 1.0f};
    GLfloat dif[] = {0.58f, 0.56f, 0.52f, 1.0f};
    GLfloat pos[] = {5.0f, 10.0f, 8.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
    glLightfv(GL_LIGHT0, GL_POSITION, pos);

    glClearColor(0.38f, 0.37f, 0.35f, 1.0f);

    initCharacter();
    initMachine();
}
