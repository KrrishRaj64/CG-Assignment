#pragma once

#include "../world/State.hpp"

void drawGround();
void drawBuilding(float x, float w, float h, float r, float g, float b);
void drawStreetLamp(float x);
void drawBench(float x);
void drawLever(float x, float z, float pull);
void drawProjectile(const Projectile &p);
void drawGift(float x, float y);
void drawTrapBox(float cx, float openAmt);
void drawHUD();
