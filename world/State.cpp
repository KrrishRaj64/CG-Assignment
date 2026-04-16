#include "State.hpp"

#include <GL/glut.h>
#include <cmath>

float gTime = 0.0f;
bool bwMode = true;

std::vector<Projectile> projectiles;
Character tramp;
Machine machine;

int currentScene = 1;
float sceneLocalTime = 0.0f;

int s2Phase = 0;
float s2PhaseTimer = 0.0f;
float s2WalkAroundAngle = 0.0f;
float s2LeverPull = 0.0f;
float s2OrbitStartX = 0.0f;
float s2OrbitStartZ = 0.0f;

bool s4Escaped = false;
bool s5GotGift = false;
float s5GiftY = 1.2f;
bool finalTrip = false;

bool s3BoxSpawned = false;
bool s3ShoeSpawned = false;
bool s3Rapid1 = false;
bool s3Rapid2 = false;

float chaplinHP = 100.0f;
float machineHP = 100.0f;
bool s3FightStarted = false;
bool s3FightFinished = false;

// Scene 5 expansion variables
float s5MachineVibration = 0.0f;
bool s5DoorOpened = false;
float s5DoorAngle = 0.0f;
bool s5LightBlinked = false;
float s5GiftSlide = 0.0f;
bool s5HatGrabbed = false;
float s5HatX = 0.0f, s5HatY = 0.0f, s5HatZ = 0.0f;
float s5HatRotX = 0.0f, s5HatRotY = 0.0f, s5HatRotZ = 0.0f;
bool s5HatOnHead = false;
bool s5Walking = false;
float s5WalkSpeed = 0.0f;
bool s5Waved = false;
bool s5Nodded = false;
bool s5Tripped = false;
float s5TripProgress = 0.0f;
bool s5Fallen = false;
float s5FallProgress = 0.0f;
bool s5HatFlying = false;
float s5HatFlyX = 0.0f, s5HatFlyY = 0.0f, s5HatFlyZ = 0.0f;
float s5HatFlyRot = 0.0f;
bool s5SteamPuff = false;
float s5PuffScale = 0.0f;
bool s5Beeped = false;

void setColor(float r, float g, float b)
{
    float dr = r * 0.62f;
    float dg = g * 0.62f;
    float db = b * 0.62f;
    if (bwMode) {
        float l = (0.299f * dr + 0.587f * dg + 0.114f * db) * 0.65f + 0.02f;
        glColor3f(l, l, l);
    } else {
        glColor3f(dr, dg, db);
    }
}

void spawnProjectile(float ox, float oy, float vx, float vy, int type)
{
    Projectile p;
    p.x = ox;
    p.y = oy;
    p.vx = vx;
    p.vy = vy;
    p.type = type;
    p.active = true;
    projectiles.push_back(p);
}

void updateProjectiles(float dt)
{
    for (auto &p : projectiles) {
        if (!p.active) {
            continue;
        }
        p.x += p.vx * dt;
        p.y += p.vy * dt;
        p.vy -= 9.8f * dt;

        if (p.x < WORLD_LEFT + 0.5f) {
            p.x = WORLD_LEFT + 0.5f;
            p.vx = std::fabs(p.vx) * 0.5f;
        }
        if (p.x > WORLD_RIGHT - 0.5f) {
            p.x = WORLD_RIGHT - 0.5f;
            p.vx = -std::fabs(p.vx) * 0.5f;
        }
        if (p.y < -0.7f) {
            p.active = false;
        }
    }
}

void initCharacter()
{
    tramp.x = SCENE1_START - 1.8f;
    tramp.y = 0.0f;
    tramp.z = 0.0f;
    tramp.facingYaw = 90.0f;
    tramp.faceMood = 0;
    tramp.faceShockBlend = 0.0f;
    tramp.bodyPitch = 0.0f;
    tramp.bodyTilt = 0.0f;
    tramp.hatExtraY = 0.0f;
    tramp.caneAngle = -30.0f;
    tramp.legLAngle = 0.0f;
    tramp.legRAngle = 0.0f;
    tramp.armLAngle = 20.0f;
    tramp.armRAngle = -20.0f;
    tramp.reactionScale = 1.0f;
    tramp.isHit = false;
    tramp.hitTimer = 0.0f;
    tramp.trapped = false;
    tramp.trapTimer = 0.0f;
    tramp.walkPhase = 0.0f;
    tramp.shoeBend = 0.0f;
    tramp.earLean = 0.0f;
    tramp.chestOut = 0.0f;
    tramp.foreArmAngle = 0.0f;
    tramp.headPitch = 0.0f;
    chaplinHP = 100.0f;
    machineHP = 100.0f;
    s3FightStarted = false;
    s3FightFinished = false;
    s2LeverPull = 0.0f;
    s2OrbitStartX = tramp.x;
    s2OrbitStartZ = tramp.z;
}

void initMachine()
{
    machine.x = MACHINE_X;
    machine.bodyAngle = 0.0f;
    machine.armExtend = 0.0f;
    machine.armAngle = 0.0f;
    machine.lightBlink = 0.0f;
    machine.doorOpen = 0.0f;
    machine.activated = false;
    machine.activateTimer = 0.0f;
    machine.spawnTimer = 0.0f;
}
