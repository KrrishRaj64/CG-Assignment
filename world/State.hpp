#pragma once

#include <vector>

struct Projectile {
    float x, y, vx, vy;
    int type;
    bool active;
};

struct Character {
    float x, y;
    float z;
    float facingYaw;
    int faceMood;
    float faceShockBlend;
    float bodyPitch;
    float bodyTilt;
    float hatExtraY;
    float caneAngle;
    float legLAngle, legRAngle;
    float armLAngle, armRAngle;
    float reactionScale;
    bool isHit;
    float hitTimer;
    bool trapped;
    float trapTimer;
    float walkPhase;
    float shoeBend;
    float earLean;
    float chestOut;
    float foreArmAngle;
    float headPitch;
};

struct Machine {
    float x, bodyAngle, armExtend, armAngle;
    float lightBlink, doorOpen;
    bool activated;
    float activateTimer, spawnTimer;
};

inline constexpr float WORLD_LEFT = -7.5f;
inline constexpr float WORLD_RIGHT = 7.0f;
inline constexpr float MACHINE_X = 3.5f;
inline constexpr float MACHINE_W = 0.55f;
inline constexpr float CHAR_W = 0.35f;

inline constexpr float NEAR_MACHINE = MACHINE_X - MACHINE_W - CHAR_W - 0.15f;
inline constexpr float MED_MACHINE = MACHINE_X - MACHINE_W - CHAR_W - 0.50f;
inline constexpr float FAR_MACHINE = MACHINE_X - MACHINE_W - CHAR_W - 1.20f;
inline constexpr float SAFE_RIGHT = WORLD_RIGHT - 0.8f;
inline constexpr float SCENE1_START = WORLD_LEFT;

inline constexpr float SCENE1_END = 60.0f;
inline constexpr float SCENE2_END = 120.0f;
inline constexpr float SCENE3_END = 210.0f;
inline constexpr float SCENE4_END = 270.0f;
inline constexpr float SCENE5_END = 300.0f;

extern float gTime;
extern bool bwMode;

extern std::vector<Projectile> projectiles;
extern Character tramp;
extern Machine machine;

extern int currentScene;
extern float sceneLocalTime;

extern int s2Phase;
extern float s2PhaseTimer;
extern float s2WalkAroundAngle;
extern float s2LeverPull;
extern float s2OrbitStartX;
extern float s2OrbitStartZ;

extern bool s4Escaped;
extern bool s5GotGift;
extern float s5GiftY;
extern bool finalTrip;

extern bool s3BoxSpawned;
extern bool s3ShoeSpawned;
extern bool s3Rapid1;
extern bool s3Rapid2;

extern float chaplinHP;
extern float machineHP;
extern bool s3FightStarted;
extern bool s3FightFinished;

// Scene 5 expansion variables
extern float s5MachineVibration;
extern bool s5DoorOpened;
extern float s5DoorAngle;
extern bool s5LightBlinked;
extern float s5GiftSlide;
extern bool s5HatGrabbed;
extern float s5HatX, s5HatY, s5HatZ;
extern float s5HatRotX, s5HatRotY, s5HatRotZ;
extern bool s5HatOnHead;
extern bool s5Walking;
extern float s5WalkSpeed;
extern bool s5Waved;
extern bool s5Nodded;
extern bool s5Tripped;
extern float s5TripProgress;
extern bool s5Fallen;
extern float s5FallProgress;
extern bool s5HatFlying;
extern float s5HatFlyX, s5HatFlyY, s5HatFlyZ;
extern float s5HatFlyRot;
extern bool s5SteamPuff;
extern float s5PuffScale;
extern bool s5Beeped;

void setColor(float r, float g, float b);
inline float lerp(float a, float b, float t) { return a + (b - a) * t; }
inline float clamp01(float t) { return t < 0 ? 0 : (t > 1 ? 1 : t); }
inline float clampX(float x) { return x < WORLD_LEFT ? WORLD_LEFT : (x > WORLD_RIGHT ? WORLD_RIGHT : x); }
inline float easeInOut(float t) { return t * t * (3.0f - 2.0f * t); }

void spawnProjectile(float ox, float oy, float vx, float vy, int type);
void updateProjectiles(float dt);

void initCharacter();
void initMachine();
