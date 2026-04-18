#include "Environment.hpp"

#include "Primitives.hpp"

#include <GL/glut.h>
#include <cmath>
#include <cstdio>

namespace {

void drawBar(float x, float y, float w, float h, float pct, float r, float g, float b)
{
    if (pct < 0.0f) {
        pct = 0.0f;
    }
    if (pct > 1.0f) {
        pct = 1.0f;
    }

    glColor3f(0.10f, 0.10f, 0.10f);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();

    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x + 2, y + 2);
    glVertex2f(x + 2 + (w - 4) * pct, y + 2);
    glVertex2f(x + 2 + (w - 4) * pct, y + h - 2);
    glVertex2f(x + 2, y + h - 2);
    glEnd();
}

} // namespace

void drawGround()
{
    setColor(0.40f, 0.40f, 0.40f);
    glBegin(GL_QUADS);
    glVertex3f(-25, -1.0f, -6);
    glVertex3f(25, -1.0f, -6);
    glVertex3f(25, -1.0f, 6);
    glVertex3f(-25, -1.0f, 6);
    glEnd();

    setColor(0.35f, 0.35f, 0.35f);
    for (int i = -12; i <= 12; i += 2) {
        glBegin(GL_LINES);
        glVertex3f((float)i, -0.99f, -6);
        glVertex3f((float)i, -0.99f, 6);
        glEnd();
    }
    for (int j = -3; j <= 3; j += 2) {
        glBegin(GL_LINES);
        glVertex3f(-25, -0.99f, (float)j);
        glVertex3f(25, -0.99f, (float)j);
        glEnd();
    }

    setColor(0.55f, 0.55f, 0.55f);
    glBegin(GL_QUADS);
    glVertex3f(-25, -0.98f, 2.0f);
    glVertex3f(25, -0.98f, 2.0f);
    glVertex3f(25, -0.92f, 2.0f);
    glVertex3f(-25, -0.92f, 2.0f);
    glEnd();
}

void drawBuilding(float x, float w, float h, float r, float g, float b)
{
    setColor(r, g, b);
    glPushMatrix();
    glTranslatef(x, h * 0.5f - 1.0f, -4.5f);
    glScalef(w, h, 0.5f);
    glutSolidCube(1.0);
    glPopMatrix();

    setColor(r + 0.20f, g + 0.20f, b + 0.05f);
    int cols = (int)(w / 0.8f);
    int rows = (int)(h / 1.0f) - 1;
    for (int ci = 0; ci < cols; ci++) {
        for (int ri = 0; ri < rows; ri++) {
            float wx = x - w * 0.5f + 0.5f + ci * 0.8f;
            float wy = (float)ri * 1.0f + 0.2f;
            glPushMatrix();
            glTranslatef(wx, wy, -4.18f);
            glScalef(0.22f, 0.32f, 0.04f);
            glutSolidCube(1.0);
            glPopMatrix();
            setColor(r + 0.30f, g + 0.30f, b + 0.10f);
            glPushMatrix();
            glTranslatef(wx, wy - 0.19f, -4.16f);
            glScalef(0.26f, 0.04f, 0.04f);
            glutSolidCube(1.0);
            glPopMatrix();
        }
    }

    setColor(r - 0.05f, g - 0.05f, b - 0.05f);
    glPushMatrix();
    glTranslatef(x, h - 1.0f, -4.5f);
    glScalef(w + 0.2f, 0.15f, 0.6f);
    glutSolidCube(1.0);
    glPopMatrix();
}

void drawStreetLamp(float x)
{
    setColor(0.22f, 0.22f, 0.22f);
    glPushMatrix();
    glTranslatef(x, 1.5f, -2.5f);
    drawCylinder(0.07f, 3.2f);
    glTranslatef(0, 3.2f, 0);
    glRotatef(90, 0, 1, 0);
    drawCylinder(0.055f, 0.75f);
    setColor(0.18f, 0.18f, 0.18f);
    glTranslatef(0, 0, 0.75f);
    drawSphere(0.20f, 0.25f, 0.20f);
    setColor(0.98f, 0.95f, 0.70f);
    glTranslatef(0, -0.05f, 0);
    drawSphere(0.12f, 0.15f, 0.12f);
    glPopMatrix();
}

void drawBench(float x)
{
    setColor(0.45f, 0.28f, 0.12f);
    glPushMatrix();
    glTranslatef(x, -0.50f, -1.5f);
    glScalef(1.5f, 0.08f, 0.14f);
    glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(x, -0.18f, -1.74f);
    glScalef(1.5f, 0.52f, 0.08f);
    glutSolidCube(1.0);
    glPopMatrix();

    setColor(0.20f, 0.20f, 0.22f);
    float lxs[] = {x - 0.60f, x + 0.60f};
    for (float lx : lxs) {
        glPushMatrix();
        glTranslatef(lx, -0.88f, -1.5f);
        drawCylinder(0.05f, 0.38f);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(lx, -0.88f, -1.72f);
        drawCylinder(0.04f, 0.38f);
        glPopMatrix();
    }
}

void drawLever(float x, float z, float pull)
{
    float p = clamp01(pull);

    // Base stand.
    setColor(0.18f, 0.18f, 0.20f);
    glPushMatrix();
    glTranslatef(x, -0.80f, z);
    glScalef(0.18f, 0.34f, 0.18f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Lever arm pivots from upright to pulled-down.
    glPushMatrix();
    glTranslatef(x, -0.62f, z);
    glRotatef(lerp(8.0f, -42.0f, p), 0, 0, 1);
    setColor(0.62f, 0.62f, 0.66f);
    glPushMatrix();
    glTranslatef(0.0f, 0.22f, 0.0f);
    glScalef(0.04f, 0.44f, 0.04f);
    glutSolidCube(1.0);
    glPopMatrix();

    setColor(0.75f, 0.16f, 0.16f);
    glPushMatrix();
    glTranslatef(0.0f, 0.44f, 0.0f);
    drawSphere(0.06f, 0.06f, 0.06f);
    glPopMatrix();
    glPopMatrix();
}

void drawProjectile(const Projectile &p)
{
    glPushMatrix();
    glTranslatef(p.x, p.y, 0);
    glRotatef(gTime * 220.0f, 1, 1, 0);
    if (p.type == 0) {
        setColor(0.72f, 0.52f, 0.22f);
        glScalef(0.32f, 0.28f, 0.28f);
        glutSolidCube(1.0);
    } else if (p.type == 1) {
        setColor(0.12f, 0.08f, 0.04f);
        glScalef(0.36f, 0.14f, 0.24f);
        glutSolidCube(1.0);
    } else {
        setColor(0.07f, 0.07f, 0.07f);
        drawSphere(0.22f, 0.14f, 0.22f);
        glTranslatef(0, -0.10f, 0);
        glScalef(1, 0.12f, 1);
        glutSolidTorus(0.02f, 0.26f, 8, 24);
    }
    glPopMatrix();
}

void drawGift(float x, float y)
{
    glPushMatrix();
    glTranslatef(x, y, 0);
    setColor(0.07f, 0.07f, 0.07f);
    drawSphere(0.24f, 0.20f, 0.24f);
    glTranslatef(0, -0.12f, 0);
    glScalef(1, 0.10f, 1);
    glutSolidTorus(0.02f, 0.28f, 8, 24);
    glPopMatrix();
}

void drawTrapBox(float cx, float openAmt)
{
    float a = 1.0f - openAmt;
    if (a <= 0) {
        return;
    }
    setColor(0.55f * a, 0.28f * a, 0.05f * a);
    glPushMatrix();
    glTranslatef(cx, 0, 0);
    glScalef(1.0f * a + 0.30f, 1.6f * a + 0.10f, 0.6f);
    glutWireCube(1.0);
    glPopMatrix();
}


#include <string>
#include <vector>
#include <cstring>

struct StoryBeat {
    int scene;
    float startTime;
    float endTime;
    const char* text;
    float boxX;
    float boxY;
    int wrapPx;
};

static const StoryBeat kStoryBeats[] = {
    // Scene 1
    {1,  1.0f,  6.0f, "Charlie chaplin is walking down the street", 50.0f, 400.0f, 300},
    {1,  7.0f, 12.0f, "And then he notices a \"machine\"", 50.0f, 400.0f, 300},
    {1, 13.0f, 18.0f, "With a shocked expression he walks to the machine and inspects it", 500.0f, 400.0f, 300},
    {1, 19.0f, 25.0f, "With suprise and curiosity written all over his face", 50.0f, 100.0f, 300},
    {1, 35.0f, 50.0f, "And he, as you can clearly see, is AWESTRUCK by the distinct yet creative appearance of this 'machine'", 500.0f, 400.0f, 320},
    {1, 50.0f, 55.0f, "Then as any gentleman would, he bows down to the machine", 50.0f, 350.0f, 300},
    {1, 55.0f, 60.0f, "The machine doesn't bow down, this makes him very furious", 500.0f, 100.0f, 300},

    // Scene 2
    {2,  2.0f,  15.0f, "His whole body starts shaking, wait wait- that's not charlie chaplin- let's make him curious again", 500.0f, 400.0f, 350},
    {2, 26.0f, 34.0f, "and as he curiously explores the machine, he suddenly looks left, wait he's noticed something else?", 50.0f, 400.0f, 350},
    {2, 35.0f, 40.0f, "Wait what- why is he looking at us, skip to next scene-", 500.0f, 100.0f, 350},
    {2, 40.0f,  48.0f, "-notices the lever and tries to pull it but takes some fiddling to get it to work", 50.0f, 400.0f, 350},
    {2, 49.0f, 60.0f, "AND WOAHH, THE MACHINE IS ALIVEE", 500.0f, 450.0f, 300},

    // Scene 3
    {4, 03.0f, 06.0f, "I BELEIVE I CAN FLYYYYYY, I BELIEVE I CAN TOUCH THE SKYY, I THINK ABT IT EVERY NIGHT AND DAY, SPREAD MY WINGS-", 50.0f, 450.0f, 350},
    {4, 06.0f, 10.0f, "-become a chicken? Skip to next scene-", 500.0f, 100.0f, 350},
    {4, 10.0f, 20.0f, "-ster is even more angry, who will save us from the monster??", 50.0f, 400.0f, 350},
    {4, 20.0f, 30.0f, "And charlie is back to normal? Anyways Charlie is running towards the lever with all his might and energy", 500.0f, 450.0f, 350},

    // Scene 4
    {4, 30.0f, 35.0f, "And sucessfully turns off the lever", 50.0f, 100.0f, 350},
    {4, 35.0f, 40.0f, "As the machine wails it's arms for the last time", 500.0f, 400.0f, 350},
    // {4, 20.0f, 45.0f, "the machine finally turns off", 50.0f, 450.0f, 300},
    {4, 50.0f, 55.0f, "the machine finally turns off", 50.0f, 450.0f, 300},
    {4,  55.0f,  60.0f, "But something seems off to Charlie", 50.0f, 400.0f, 300},

    // Scene 5
    {5, 0.0f, 15.0f, "Charlie is shocked and couldn't react in time as the machine exploded-", 500.0f, 450.0f, 350},
    {5, 16.0f, 20.0f, "Wow Charlie Chaplin's hat came back like a frisbee", 50.0f, 100.0f, 350},
    {5, 20.0f, 23.0f, "Then charlie sees the bottom of the machine open, it surrenders? Or permanently broken? We'll never know", 500.0f, 400.0f, 350},
    {5, 24.0f, 28.0f, "What a wonderful story of Charlie Chaplin with '0 visual errors', nothing more can go wrong-", 50.0f, 450.0f, 350},
    {5, 28.5f, 30.0f, "Oops- ok that's now how Charlie should trip, need to fix that code too-", 500.0f, 100.0f, 350},
};


std::vector<std::string> wrapText(const char* text, int maxPx) {
    std::vector<std::string> lines;
    std::string currentLine = "";
    std::string currentWord = "";
    int lineWidth = 0;
    
    for (int i = 0; text[i] != '\0'; ++i) {
        char c = text[i];
        if (c == ' ') {
            int wordWidth = 0;
            for(char wc : currentWord) wordWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, wc);
            int spaceWidth = glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, ' ');
            
            if (lineWidth + wordWidth + (lineWidth > 0 ? spaceWidth : 0) > maxPx) {
                if(!currentLine.empty()) {
                    lines.push_back(currentLine);
                    currentLine = currentWord;
                    lineWidth = wordWidth;
                } else {
                    lines.push_back(currentWord);
                    lineWidth = 0;
                }
            } else {
                if(!currentLine.empty()) {
                    currentLine += " ";
                    lineWidth += spaceWidth;
                }
                currentLine += currentWord;
                lineWidth += wordWidth;
            }
            currentWord = "";
        } else {
            currentWord += c;
        }
    }
    if (!currentWord.empty()) {
        int wordWidth = 0;
        for(char wc : currentWord) wordWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, wc);
        int spaceWidth = glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, ' ');
        if (lineWidth + wordWidth + (lineWidth > 0 ? spaceWidth : 0) > maxPx && !currentLine.empty()) {
            lines.push_back(currentLine);
            lines.push_back(currentWord);
        } else {
            if(!currentLine.empty()) currentLine += " ";
            currentLine += currentWord;
            lines.push_back(currentLine);
        }
    }
    if(lines.empty() && strlen(text) > 0) lines.push_back(text);
    return lines;
}

void drawStoryBox() {
    const StoryBeat* currentBeat = nullptr;
    for (const auto& beat : kStoryBeats) {
        if (beat.scene == currentScene && sceneLocalTime >= beat.startTime && sceneLocalTime <= beat.endTime) {
            currentBeat = &beat;
            break;
        }
    }
    
    if (!currentBeat) return;
    
    // Typewriter effect
    float duration = currentBeat->endTime - currentBeat->startTime;
    float elapsed = sceneLocalTime - currentBeat->startTime;
    int totalChars = strlen(currentBeat->text);
    float cps = 14.0f; // chars per second
    int charsToShow = (int)(elapsed * cps);
    if (charsToShow > totalChars) charsToShow = totalChars;
    
    std::string visibleText(currentBeat->text, charsToShow);
    if (charsToShow < totalChars && (int)(sceneLocalTime * 2) % 2 == 0) visibleText += "_";
    
    auto lines = wrapText(visibleText.c_str(), currentBeat->wrapPx);
    
    float boxW = currentBeat->wrapPx + 30.0f;
    float boxH = lines.size() * 24.0f + 20.0f;
    float bx = currentBeat->boxX;
    float by = currentBeat->boxY;
    
    // clamp strictly to screen coordinates
    if (bx < 12.0f) bx = 12.0f;
    if (by < 12.0f) by = 12.0f;
    if (bx + boxW > 900.0f - 12.0f) bx = 900.0f - 12.0f - boxW;
    if (by + boxH > 600.0f - 12.0f) by = 600.0f - 12.0f - boxH;
    
    drawBar(bx, by, boxW, boxH, 1.0f, 0.1f, 0.1f, 0.1f);   
    glColor3f(1.0f, 1.0f, 1.0f);
    
    float textY = by + boxH - 24.0f;
    for (const auto& line : lines) {
        glRasterPos2f(bx + 15.0f, textY);
        for (char c : line) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }
        textY -= 24.0f;
    }
}


void drawHUD()
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 900, 0, 600);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    // High-contrast HUD backplates so text remains readable in both color and B/W.
    setColor(0.03f, 0.03f, 0.03f);
    // REMOVED TOP BLACK BAR

    // Keep bottom black bar for hints
    glBegin(GL_QUADS);
    glVertex2f(0, 26);
    glVertex2f(900, 26);
    glVertex2f(900, 0);
    glVertex2f(0, 0);
    glEnd();

    drawStoryBox();

    char buf[64];
    std::snprintf(buf, sizeof(buf), "%d:%02d", (int)(gTime / 60), (int)gTime % 60);
    glColor3f(1.0f, 1.0f, 1.0f);
    // Moved timer to bottom right
    glRasterPos2f(850, 8); 
    for (char *c = buf; *c; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }

    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(18, 12);
    const char *hint = "[B] toggle colour   [ESC] quit";
    for (const char *s = hint; *s; s++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *s);
    }

    if (currentScene == 3) {
        float cp = chaplinHP / 100.0f;
        float mp = machineHP / 100.0f;

        drawBar(24.0f, 546.0f, 300.0f, 22.0f, cp, 1.0f, 1.0f, 1.0f);
        drawBar(576.0f, 546.0f, 300.0f, 22.0f, mp, 1.0f, 1.0f, 1.0f);

        glColor3f(1.0f, 1.0f, 1.0f);
        glRasterPos2f(26.0f, 571.0f);
        const char *cLab = "CHAPLIN";
        for (const char *s = cLab; *s; s++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *s);
        }

        glRasterPos2f(738.0f, 571.0f);
        const char *mLab = "MACHINE";
        for (const char *s = mLab; *s; s++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *s);
        }

        glRasterPos2f(410.0f, 571.0f);
        const char *fight = "FIGHT";
        for (const char *s = fight; *s; s++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *s);
        }
    }

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}
