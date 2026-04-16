#include "SceneLogic.hpp"

#include "State.hpp"

#include <cmath>
#include <cstdlib>

void applyWobblyWalk(float dt, float speed)
{
    tramp.walkPhase += speed * dt;
    float ph = tramp.walkPhase;
    tramp.legLAngle = std::sinf(ph) * 34.0f;
    tramp.legRAngle = -tramp.legLAngle;
    tramp.armLAngle = -tramp.legLAngle * 0.45f;
    tramp.armRAngle = tramp.legLAngle * 0.45f;
    tramp.bodyTilt = std::sinf(ph * 2.0f) * 3.5f;
    tramp.y = std::fabs(std::sinf(ph)) * 0.07f;
}

void stopWalk()
{
    tramp.legLAngle = lerp(tramp.legLAngle, 0, 0.42f);
    tramp.legRAngle = lerp(tramp.legRAngle, 0, 0.42f);
    tramp.armLAngle = lerp(tramp.armLAngle, 20.0f, 0.24f);
    tramp.armRAngle = lerp(tramp.armRAngle, -20.0f, 0.24f);
    tramp.foreArmAngle = lerp(tramp.foreArmAngle, 0.0f, 0.28f);
    tramp.bodyPitch = lerp(tramp.bodyPitch, 0.0f, 0.10f);
    tramp.shoeBend = lerp(tramp.shoeBend, 0.0f, 0.42f);
    tramp.y = lerp(tramp.y, 0.0f, 0.30f);
}

static void faceTowards(float tx, float tz, float rate)
{
    float desired = std::atan2(tx - tramp.x, tz - tramp.z) * 57.29578f;
    float diff = desired - tramp.facingYaw;
    while (diff > 180.0f) {
        diff -= 360.0f;
    }
    while (diff < -180.0f) {
        diff += 360.0f;
    }
    tramp.facingYaw += diff * rate;
}

static void moveTowardsXZ(float tx, float tz, float speed, float walkSpeed, float dt)
{
    float dx = tx - tramp.x;
    float dz = tz - tramp.z;
    float d2 = dx * dx + dz * dz;
    if (d2 < 0.0004f) {
        stopWalk();
        tramp.x = lerp(tramp.x, tx, 0.10f);
        tramp.z = lerp(tramp.z, tz, 0.10f);
        return;
    }

    float d = std::sqrt(d2);
    float step = speed * dt;
    if (step > d) {
        step = d;
    }
    tramp.x += dx / d * step;
    tramp.z += dz / d * step;
    applyWobblyWalk(dt, walkSpeed);
    faceTowards(tx, tz, 0.22f);
}

static void holdFightStance(float dt)
{
    tramp.walkPhase += dt * 7.5f;
    float ph = std::sinf(tramp.walkPhase);
    tramp.legLAngle = ph * 12.0f;
    tramp.legRAngle = -ph * 12.0f;
    tramp.armLAngle = lerp(tramp.armLAngle, 42.0f + ph * 8.0f, 0.16f);
    tramp.armRAngle = lerp(tramp.armRAngle, -38.0f - ph * 6.0f, 0.16f);
    tramp.foreArmAngle = lerp(tramp.foreArmAngle, -18.0f + ph * 4.0f, 0.14f);
    tramp.bodyTilt = lerp(tramp.bodyTilt, ph * 2.0f, 0.14f);
    tramp.y = lerp(tramp.y, std::fabs(ph) * 0.03f, 0.15f);
    tramp.caneAngle = lerp(tramp.caneAngle, -26.0f + ph * 10.0f, 0.20f);
}

void updateScene1(float dt, float lt)
{
    const float frontX = machine.x - 0.10f;
    const float frontZ = 0.90f;

    if (lt < 10.0f) {
        tramp.faceMood = 1;
        tramp.faceShockBlend = 0.0f;
        moveTowardsXZ(FAR_MACHINE - 1.05f, 0.0f, 0.95f, 5.2f, dt);
        float flourishT = std::fmod(lt, 7.0f);
        float flourish = 0.0f;
        if (flourishT < 0.85f) {
            flourish = std::sinf(flourishT * 10.0f) * 32.0f;
        }
        tramp.caneAngle = lerp(tramp.caneAngle, -26.0f + std::sinf(tramp.walkPhase * 0.7f) * 10.0f + flourish, 0.13f);
        tramp.chestOut = lerp(tramp.chestOut, 0.25f, 0.05f);
    } else if (lt < 17.0f) {
        // 00:10-00:17: face view + animated smile to shocked :o.
        float p = (lt - 10.0f) / 7.0f;
        // Ease so 0:14 still reads happy and only opens to gasp near the end.
        float morphP = p < 0.72f ? (p * 0.30f) : (0.216f + (p - 0.72f) / 0.28f * 0.784f);
        tramp.faceMood = 5;
        tramp.faceShockBlend = clamp01(morphP);
        moveTowardsXZ(FAR_MACHINE - 0.85f, 0.0f, 0.88f, 4.8f, dt);
        tramp.bodyTilt = lerp(tramp.bodyTilt, 0.0f, 0.08f);
        tramp.caneAngle = lerp(tramp.caneAngle, -24.0f + std::sinf(tramp.walkPhase * 0.6f) * 8.0f, 0.12f);
    } else if (lt < 22.0f) {
        // 00:17-00:22: keep shocked while camera spins to machine view.
        tramp.faceMood = 2;
        tramp.faceShockBlend = 1.0f;
        moveTowardsXZ(frontX, frontZ, 1.25f, 4.3f, dt);
        faceTowards(machine.x, 0.0f, 0.20f);
        tramp.caneAngle = lerp(tramp.caneAngle, -30.0f, 0.12f);
    } else if (lt < 24.0f) {
        // 00:22-00:24: camera spins back 180 to face while expression stays :o.
        tramp.faceMood = 2;
        tramp.faceShockBlend = 1.0f;
        stopWalk();
        tramp.x = lerp(tramp.x, frontX, 0.10f);
        tramp.z = lerp(tramp.z, frontZ, 0.10f);
        tramp.bodyTilt = lerp(tramp.bodyTilt, 0.0f, 0.12f);
    } else if (lt < 25.0f) {
        tramp.faceMood = 2;
        tramp.faceShockBlend = 1.0f;
        stopWalk();
        tramp.x = lerp(tramp.x, frontX, 0.10f);
        tramp.z = lerp(tramp.z, frontZ, 0.10f);
    } else if (lt < 36.0f) {
        tramp.faceMood = 0;
        tramp.faceShockBlend = 0.0f;
        moveTowardsXZ(frontX, frontZ, 1.6f, 4.6f, dt);
        tramp.chestOut = lerp(tramp.chestOut, 0.35f, 0.06f);
        tramp.earLean = lerp(tramp.earLean, 0.08f, 0.06f);
        tramp.caneAngle = lerp(tramp.caneAngle, -30.0f, 0.10f);
    } else if (lt < 42.0f) {
        tramp.faceMood = 4;
        tramp.faceShockBlend = 0.0f;
        stopWalk();
        faceTowards(machine.x, 0.0f, 0.16f);
        tramp.x = lerp(tramp.x, frontX, 0.10f);
        tramp.z = lerp(tramp.z, frontZ, 0.10f);
        tramp.armLAngle = lerp(tramp.armLAngle, 12.0f, 0.12f);
        tramp.armRAngle = lerp(tramp.armRAngle, -16.0f, 0.12f);
        tramp.foreArmAngle = lerp(tramp.foreArmAngle, -6.0f, 0.10f);
        tramp.bodyTilt = lerp(tramp.bodyTilt, 2.5f, 0.10f);
        tramp.chestOut = lerp(tramp.chestOut, 0.55f, 0.10f);
    } else if (lt < 48.5f) {
        // Deliberate face-camera expression beat.
        tramp.faceMood = 2;
        tramp.faceShockBlend = 1.0f;
        stopWalk();
        tramp.facingYaw = lerp(tramp.facingYaw, 0.0f, 0.18f);
        tramp.armLAngle = lerp(tramp.armLAngle, 88.0f, 0.18f);
        tramp.armRAngle = lerp(tramp.armRAngle, -88.0f, 0.18f);
        tramp.foreArmAngle = lerp(tramp.foreArmAngle, -34.0f, 0.16f);
        tramp.bodyTilt = lerp(tramp.bodyTilt, 0.0f, 0.14f);
        tramp.hatExtraY = lerp(tramp.hatExtraY, 0.07f, 0.12f);
        tramp.earLean = lerp(tramp.earLean, 0.0f, 0.12f);
        tramp.caneAngle = lerp(tramp.caneAngle, -34.0f, 0.12f);
        // Force clear shock facial read for close-up.
        tramp.isHit = true;
        tramp.hitTimer = 0.24f;
        tramp.reactionScale = lerp(tramp.reactionScale, 1.07f, 0.12f);
    } else if (lt < 50.0f) {
        tramp.faceMood = 4;
        tramp.faceShockBlend = 0.0f;
        stopWalk();
        // Stand a little farther from the machine before the etiquette bit.
        tramp.x = lerp(tramp.x, machine.x - 0.92f, 0.10f);
        tramp.z = lerp(tramp.z, 0.92f, 0.10f);
        faceTowards(machine.x, 0.0f, 0.16f);
        tramp.armLAngle = lerp(tramp.armLAngle, 20.0f, 0.10f);
        tramp.armRAngle = lerp(tramp.armRAngle, -28.0f, 0.10f);
        tramp.foreArmAngle = lerp(tramp.foreArmAngle, -10.0f, 0.10f);
        tramp.bodyTilt = lerp(tramp.bodyTilt, 2.0f, 0.10f);
        tramp.chestOut = lerp(tramp.chestOut, 0.35f, 0.10f);
        tramp.caneAngle = lerp(tramp.caneAngle, -30.0f, 0.10f);
        tramp.hatExtraY = lerp(tramp.hatExtraY, 0.0f, 0.10f);
    } else if (lt < 51.0f) {
        // Politely removes hat and prepares to bow.
        tramp.faceMood = 1;
        stopWalk();
        tramp.x = lerp(tramp.x, machine.x - 0.92f, 0.12f);
        tramp.z = lerp(tramp.z, 0.92f, 0.12f);
        faceTowards(machine.x, 0.0f, 0.18f);
        float p = clamp01((lt - 50.0f) / 1.0f);
        float e = 0.5f - 0.5f * std::cos(p * 3.14159f);
        tramp.hatExtraY = lerp(tramp.hatExtraY, 0.34f, 0.16f);
        tramp.bodyPitch = lerp(tramp.bodyPitch, 10.0f + e * 10.0f, 0.20f);
        tramp.bodyTilt = lerp(tramp.bodyTilt, 0.0f, 0.16f);
        tramp.armRAngle = lerp(tramp.armRAngle, -56.0f - e * 18.0f, 0.16f);
        tramp.armLAngle = lerp(tramp.armLAngle, 10.0f - e * 4.0f, 0.14f);
        tramp.foreArmAngle = lerp(tramp.foreArmAngle, -18.0f - e * 12.0f, 0.14f);
        tramp.caneAngle = lerp(tramp.caneAngle, -24.0f, 0.14f);
    } else if (lt < 53.0f) {
        // First bow and wait.
        tramp.faceMood = 4;
        stopWalk();
        faceTowards(machine.x, 0.0f, 0.18f);
        float p = clamp01((lt - 51.0f) / 2.0f);
        float e = 0.5f - 0.5f * std::cos(p * 3.14159f);
        tramp.hatExtraY = lerp(tramp.hatExtraY, 0.38f, 0.14f);
        tramp.bodyPitch = lerp(tramp.bodyPitch, 25.0f + e * 15.0f, 0.20f);
        tramp.bodyTilt = lerp(tramp.bodyTilt, 0.0f, 0.14f);
        tramp.armRAngle = lerp(tramp.armRAngle, -64.0f - e * 16.0f, 0.14f);
        tramp.armLAngle = lerp(tramp.armLAngle, 8.0f - e * 6.0f, 0.12f);
        tramp.foreArmAngle = lerp(tramp.foreArmAngle, -24.0f - e * 10.0f, 0.12f);
        tramp.earLean = lerp(tramp.earLean, 0.06f, 0.10f);
    } else if (lt < 57.0f) {
        // Deeper bow, still no response.
        tramp.faceMood = 4;
        stopWalk();
        faceTowards(machine.x, 0.0f, 0.18f);
        float p = clamp01((lt - 53.0f) / 2.0f);
        float e = 0.5f - 0.5f * std::cos(p * 3.14159f);
        tramp.hatExtraY = lerp(tramp.hatExtraY, 0.44f, 0.14f);
        tramp.bodyPitch = lerp(tramp.bodyPitch, 35.0f + e * 15.0f, 0.20f);
        tramp.bodyTilt = lerp(tramp.bodyTilt, 0.0f, 0.14f);
        tramp.armRAngle = lerp(tramp.armRAngle, -74.0f - e * 10.0f, 0.12f);
        tramp.armLAngle = lerp(tramp.armLAngle, 2.0f - e * 6.0f, 0.12f);
        tramp.foreArmAngle = lerp(tramp.foreArmAngle, -30.0f - e * 10.0f, 0.12f);
        tramp.chestOut = lerp(tramp.chestOut, 0.15f, 0.10f);
    } else if (lt < 60.0f) {
        // No response: stands straight, angry, and confronts the audience.
        tramp.faceMood = 3;
        stopWalk();
        tramp.facingYaw = lerp(tramp.facingYaw, -162.0f, 0.14f);
        tramp.hatExtraY = lerp(tramp.hatExtraY, 0.0f, 0.18f);
        tramp.bodyPitch = lerp(tramp.bodyPitch, 0.0f, 0.12f);
        tramp.bodyTilt = lerp(tramp.bodyTilt, 0.0f, 0.16f);
        tramp.armRAngle = lerp(tramp.armRAngle, -46.0f, 0.14f);
        tramp.armLAngle = lerp(tramp.armLAngle, 48.0f, 0.14f);
        tramp.foreArmAngle = lerp(tramp.foreArmAngle, -24.0f, 0.12f);
        tramp.earLean = lerp(tramp.earLean, 0.0f, 0.14f);
        tramp.caneAngle = lerp(tramp.caneAngle, -34.0f, 0.12f);
    }
}

void updateScene2(float dt, float lt)
{
    tramp.faceMood = 4;
    if (lt < 0.15f) {
        s2LeverPull = 0.0f;
    }
    machine.activated = false;
    machine.armExtend = lerp(machine.armExtend, 0.0f, 0.10f);
    machine.armAngle = lerp(machine.armAngle, 0.0f, 0.10f);
    machine.bodyAngle = lerp(machine.bodyAngle, 0.0f, 0.08f);
    machine.lightBlink = lerp(machine.lightBlink, 0.18f, 0.08f);
    s2PhaseTimer += dt;
    static int lastS2 = -1;
    if (lt < 10.0f) {
        s2Phase = 0;
    } else if (lt < 18.0f) {
        s2Phase = 1;
    } else if (lt < 25.0f) {
        s2Phase = 2;
    } else if (lt < 32.0f) {
        s2Phase = 3;
    } else if (lt < 40.0f) {
        s2Phase = 4;
    } else if (lt < 48.0f) {
        s2Phase = 5;
    } else {
        s2Phase = 6;
    }

    if (s2Phase != lastS2) {
        s2PhaseTimer = 0;
        if (s2Phase == 0) {
            float cx = machine.x - 1.05f;
            float cz = 0.55f;
            s2WalkAroundAngle = std::atan2(tramp.z - cz, tramp.x - cx) * 57.29578f;
            s2OrbitStartX = tramp.x;
            s2OrbitStartZ = tramp.z;
        }
        lastS2 = s2Phase;
    }
    float pt = s2PhaseTimer;

    const float arcR = 1.12f;
    const float centerX = machine.x - 1.05f;
    const float centerZ = 0.55f;
    const float leverX = machine.x - 1.05f;
    const float leverZ = 0.52f;

    switch (s2Phase) {
        case 0: {
            tramp.faceMood = 1;
            s2WalkAroundAngle += 22.0f * dt;
            float a = s2WalkAroundAngle * 3.14159f / 180.0f;
            float ox = centerX + std::cos(a) * arcR;
            float oz = centerZ + std::sin(a) * 0.55f;
            float enter = clamp01(pt / 1.0f);
            float w = enter * enter * (3.0f - 2.0f * enter);
            float tx = lerp(s2OrbitStartX, ox, w);
            float tz = lerp(s2OrbitStartZ, oz, w);
            moveTowardsXZ(tx, tz, 1.05f, 3.8f, dt);
            faceTowards(machine.x - 0.08f, 0.12f, 0.22f);
            tramp.caneAngle = lerp(tramp.caneAngle, -30.0f, 0.12f);
            tramp.chestOut = lerp(tramp.chestOut, 0.30f, 0.08f);
            break;
        }
        case 1: {
            moveTowardsXZ(machine.x - 1.00f, 0.98f, 1.0f, 3.7f, dt);
            faceTowards(machine.x - 0.05f, 0.05f, 0.24f);
            tramp.armRAngle = lerp(tramp.armRAngle, -36.0f + std::sinf(pt * 5.0f) * 10.0f, 0.14f);
            tramp.armLAngle = lerp(tramp.armLAngle, 22.0f, 0.12f);
            tramp.foreArmAngle = lerp(tramp.foreArmAngle, -10.0f, 0.12f);
            break;
        }
        case 2: {
            stopWalk();
            tramp.x = lerp(tramp.x, machine.x - 0.96f, 0.10f);
            tramp.z = lerp(tramp.z, 0.88f, 0.10f);
            faceTowards(machine.x - 0.06f, 0.02f, 0.26f);
            tramp.bodyTilt = lerp(tramp.bodyTilt, 4.0f, 0.12f);
            tramp.armLAngle = lerp(tramp.armLAngle, -26.0f, 0.14f);
            tramp.armRAngle = lerp(tramp.armRAngle, -16.0f, 0.14f);
            tramp.caneAngle = lerp(tramp.caneAngle, -38.0f, 0.12f);
            // Occasional funny shocked-photo gag while inspecting.
            float gag = std::sinf((lt - 18.0f) * 3.8f);
            if (gag > 0.86f) {
                tramp.faceMood = 6;
                tramp.armLAngle = lerp(tramp.armLAngle, 86.0f, 0.22f);
                tramp.armRAngle = lerp(tramp.armRAngle, -86.0f, 0.22f);
                tramp.foreArmAngle = lerp(tramp.foreArmAngle, -34.0f, 0.20f);
                tramp.bodyTilt = lerp(tramp.bodyTilt, 0.0f, 0.18f);
            }
            break;
        }
        case 3: {
            stopWalk();
            tramp.x = lerp(tramp.x, leverX - 0.10f, 0.10f);
            tramp.z = lerp(tramp.z, leverZ + 0.22f, 0.10f);
            faceTowards(leverX, leverZ, 0.28f);
            tramp.earLean = lerp(tramp.earLean, 0.18f, 0.12f);
            tramp.chestOut = lerp(tramp.chestOut, 0.75f, 0.10f);
            tramp.armLAngle = lerp(tramp.armLAngle, 18.0f, 0.10f);
            tramp.armRAngle = lerp(tramp.armRAngle, -28.0f, 0.10f);
            if (std::sinf((lt - 25.0f) * 3.1f) > 0.90f) {
                tramp.faceMood = 6;
            }
            break;
        }
        case 4: {
            stopWalk();
            tramp.x = lerp(tramp.x, leverX - 0.05f, 0.10f);
            tramp.z = lerp(tramp.z, leverZ + 0.16f, 0.10f);
            faceTowards(leverX, leverZ, 0.30f);
            tramp.armLAngle = lerp(tramp.armLAngle, -36.0f, 0.14f);
            tramp.armRAngle = lerp(tramp.armRAngle, -6.0f, 0.14f);
            tramp.foreArmAngle = lerp(tramp.foreArmAngle, -24.0f, 0.12f);
            tramp.caneAngle = lerp(tramp.caneAngle, -18.0f, 0.12f);
            break;
        }
        case 5: {
            tramp.faceMood = 4;
            stopWalk();
            tramp.x = lerp(tramp.x, leverX - 0.04f, 0.12f);
            tramp.z = lerp(tramp.z, leverZ + 0.12f, 0.12f);
            faceTowards(leverX, leverZ, 0.32f);
            float tug = std::sinf(pt * 8.0f);
            s2LeverPull = lerp(s2LeverPull, 0.55f + std::max(0.0f, tug) * 0.35f, 0.24f);
            tramp.bodyPitch = lerp(tramp.bodyPitch, 30.0f + std::max(0.0f, tug) * 15.0f, 0.16f);
            tramp.armLAngle = lerp(tramp.armLAngle, -58.0f - std::max(0.0f, tug) * 14.0f, 0.16f);
            tramp.armRAngle = lerp(tramp.armRAngle, -8.0f, 0.12f);
            tramp.foreArmAngle = lerp(tramp.foreArmAngle, -34.0f, 0.16f);
            break;
        }
        case 6: {
            tramp.faceMood = 2;
            moveTowardsXZ(machine.x - 1.28f, 0.72f, 0.95f, 3.6f, dt);
            faceTowards(machine.x, 0.0f, 0.26f);
            tramp.bodyPitch = lerp(tramp.bodyPitch, 0.0f, 0.16f);
            tramp.bodyTilt = lerp(tramp.bodyTilt, 2.0f, 0.10f);
            tramp.armLAngle = lerp(tramp.armLAngle, 30.0f, 0.12f);
            tramp.armRAngle = lerp(tramp.armRAngle, -38.0f, 0.12f);
            tramp.caneAngle = lerp(tramp.caneAngle, -32.0f, 0.10f);
            s2LeverPull = lerp(s2LeverPull, 1.0f, 0.14f);

            // Last curiosity beat: machine comes alive before the fight scene.
            machine.activated = true;
            float aliveP = clamp01(pt / 6.0f);
            if (pt > 6.0f) {
                // Last 5s of curiosity: unmistakable "comes alive" sprout beat.
                float wakeT = pt - 6.0f;
                float q = clamp01(wakeT / 5.0f);
                float punch = std::sinf(wakeT * 8.0f);
                machine.armExtend = lerp(machine.armExtend, 0.95f + q * 0.55f, 0.28f);
                machine.armAngle = lerp(machine.armAngle, 34.0f + punch * (26.0f + q * 20.0f), 0.30f);
                machine.bodyAngle = lerp(machine.bodyAngle, std::sinf(wakeT * 5.0f) * (6.0f + q * 8.0f), 0.22f);
                machine.lightBlink = lerp(machine.lightBlink, 0.70f + std::sinf(wakeT * 24.0f) * 0.28f, 0.28f);
                machine.doorOpen = lerp(machine.doorOpen, 0.22f + q * 0.26f, 0.16f);
            } else {
                machine.armExtend = lerp(machine.armExtend, 0.20f + aliveP * 0.82f, 0.20f);
                machine.armAngle = lerp(machine.armAngle, std::sinf(pt * 4.0f) * (18.0f + aliveP * 44.0f), 0.24f);
                machine.bodyAngle = lerp(machine.bodyAngle, std::sinf(pt * 6.0f) * (2.0f + aliveP * 7.0f), 0.18f);
                machine.lightBlink = lerp(machine.lightBlink, 0.40f + std::sinf(pt * 18.0f) * 0.35f, 0.20f);
                machine.doorOpen = lerp(machine.doorOpen, 0.08f + aliveP * 0.25f, 0.10f);
            }
            break;
        }
    }
}

void updateScene3(float dt, float lt)
{
    tramp.faceMood = 3;
    static float machineHitCooldown = 0.0f;
    static float chaplinHitCooldown = 0.0f;
    static float blockSpawnTimer = 0.0f;

    machine.activateTimer += dt;
    machine.activated = true;

    if (machineHitCooldown > 0.0f) {
        machineHitCooldown -= dt;
    }
    if (chaplinHitCooldown > 0.0f) {
        chaplinHitCooldown -= dt;
    }

    if (!s3FightStarted && lt > 1.4f) {
        s3FightStarted = true;
    }

    float leftEdgeX = machine.x - 2.30f;
    float contactX = machine.x - 1.55f;

    if (!s3FightStarted) {
        moveTowardsXZ(leftEdgeX, 0.0f, 1.8f, 4.8f, dt);
        faceTowards(machine.x, 0.0f, 0.30f);
        holdFightStance(dt);
        machine.armExtend = lerp(machine.armExtend, 0.20f, 0.10f);
        machine.armAngle = lerp(machine.armAngle, 0.0f, 0.12f);
        machine.bodyAngle = lerp(machine.bodyAngle, 0.0f, 0.10f);
        machine.lightBlink = 0.3f + std::sinf(lt * 4.0f) * 0.1f;
        updateProjectiles(dt);
        return;
    }

    float fightT = lt - 1.4f;
    float cycle = std::fmod(fightT, 2.6f);
    float cycle2 = std::fmod(fightT + 1.3f, 2.6f);

    // Keep fighters at lane ends with short in/out steps for each exchange.
    float targetX = leftEdgeX + 0.10f;
    if (cycle > 0.75f && cycle < 1.45f) {
        targetX = contactX - 0.20f;
    } else if (cycle > 1.45f && cycle < 2.20f) {
        targetX = leftEdgeX + 0.02f;
    }

    moveTowardsXZ(targetX, 0.0f, 1.65f, 5.8f, dt);
    faceTowards(machine.x, 0.0f, 0.34f);
    holdFightStance(dt);

    // Machine strike cycle.
    float attackExtend = 0.20f;
    float attackAngle = 0.0f;
    bool machineActiveStrike = false;
    if (cycle < 0.75f) {
        attackExtend = 0.25f;
        attackAngle = -34.0f + cycle * 18.0f;
    } else if (cycle < 1.45f) {
        float p = (cycle - 0.75f) / 0.70f;
        attackExtend = 0.25f + p * 1.10f;
        attackAngle = -16.0f + p * 74.0f;
        machineActiveStrike = true;
    } else {
        float p = (cycle - 1.45f) / 1.15f;
        attackExtend = 1.35f - p * 1.10f;
        attackAngle = 58.0f - p * 62.0f;
    }

    machine.armExtend = lerp(machine.armExtend, attackExtend, 0.28f);
    machine.armAngle = lerp(machine.armAngle, attackAngle, 0.24f);
    machine.bodyAngle = lerp(machine.bodyAngle, std::sinf(fightT * 3.4f) * 7.0f, 0.16f);
    machine.lightBlink = 0.55f + std::sinf(fightT * 18.0f) * 0.40f;

    bool chaplinActiveStrike = false;
    if (cycle2 < 1.2f) {
        tramp.armRAngle = lerp(tramp.armRAngle, -62.0f, 0.20f);
        tramp.foreArmAngle = lerp(tramp.foreArmAngle, -26.0f, 0.20f);
        tramp.caneAngle = lerp(tramp.caneAngle, 26.0f, 0.22f);
        chaplinActiveStrike = cycle2 > 0.62f;
    } else {
        tramp.armRAngle = lerp(tramp.armRAngle, -34.0f, 0.20f);
        tramp.foreArmAngle = lerp(tramp.foreArmAngle, -12.0f, 0.20f);
        tramp.caneAngle = lerp(tramp.caneAngle, -22.0f, 0.22f);
    }

    // Machine throws blocks periodically; getting hit by these also reduces Chaplin HP.
    blockSpawnTimer += dt;
    if (blockSpawnTimer > 1.15f) {
        blockSpawnTimer = 0.0f;
        float a = machine.armAngle * 0.0174533f;
        float len = 0.50f + machine.armExtend;
        float tipX = machine.x - 0.55f - std::sin(a) * len;
        float tipY = 1.35f + std::cos(a) * len;
        // Aim the block at Chaplin chest height so throws read as attacks.
        float tx = tramp.x;
        float ty = tramp.y + 0.88f;
        float timeToTarget = 0.55f;
        float vx = (tx - tipX) / timeToTarget;
        float vy = (ty - tipY + 4.9f * timeToTarget * timeToTarget) / timeToTarget;
        spawnProjectile(tipX, tipY, vx, vy, 0);
    }
    updateProjectiles(dt);

    // Contact-based damage only when in strike windows and range.
    float machineStrikeX = machine.x - 1.63f;
    float chaplinRange = std::fabs(tramp.x - machineStrikeX);
    if (machineActiveStrike && chaplinRange < 0.30f && machineHitCooldown <= 0.0f) {
        chaplinHP -= 9.0f;
        machineHitCooldown = 0.48f;
        tramp.isHit = true;
        tramp.hitTimer = 0.12f;
        tramp.reactionScale = 1.0f + std::sinf(tramp.hitTimer * 24.0f) * 0.10f;
        tramp.hatExtraY = 0.05f + std::fabs(std::sinf(tramp.hitTimer * 16.0f)) * 0.10f;
    } else {
        tramp.isHit = false;
        tramp.hitTimer = lerp(tramp.hitTimer, 0.0f, 0.20f);
        tramp.reactionScale = lerp(tramp.reactionScale, 1.0f, 0.12f);
        tramp.hatExtraY = lerp(tramp.hatExtraY, 0.0f, 0.10f);
    }

    float chaplinStrikeX = machine.x - 1.72f;
    float chaplinStrikeRange = std::fabs(tramp.x - chaplinStrikeX);
    if (chaplinActiveStrike && chaplinStrikeRange < 0.38f && chaplinHitCooldown <= 0.0f) {
        machineHP -= 7.0f;
        chaplinHitCooldown = 0.42f;
    }

    for (auto &p : projectiles) {
        if (!p.active) {
            continue;
        }
        float hitDx = std::fabs(p.x - tramp.x);
        float hitDy = std::fabs(p.y - (tramp.y + 0.85f));
        if (hitDx < 0.40f && hitDy < 0.48f) {
            p.active = false;
            chaplinHP -= 8.0f;
            tramp.isHit = true;
            tramp.hitTimer = 0.16f;
            tramp.reactionScale = 1.10f;
            tramp.hatExtraY = 0.10f;
        }
    }

    if (chaplinHP < 0.0f) {
        chaplinHP = 0.0f;
        s3FightFinished = true;
    }
    if (machineHP < 0.0f) {
        machineHP = 0.0f;
    }
}

void updateScene4(float dt, float lt)
{
    tramp.faceMood = 2;
    static bool clearedProjectiles = false;
    if (lt < 0.05f) {
        clearedProjectiles = false;
        s4Escaped = false;
        s2LeverPull = 0.0f;
    }
    if (!clearedProjectiles) {
        projectiles.clear();
        clearedProjectiles = true;
    }

    machine.activateTimer += dt;
    machine.activated = true;
    machine.lightBlink = lerp(machine.lightBlink, 0.45f + std::sinf(lt * 7.0f) * 0.28f, 0.18f);

    const float leverX = machine.x - 1.05f;
    const float leverZ = 0.52f;

    if (lt < 3.0f) {
        // Clamp grabs him and the machine winds up.
        float p = clamp01(lt / 3.0f);
        tramp.trapped = true;
        tramp.x = lerp(tramp.x, machine.x - 0.74f, 0.20f);
        tramp.z = lerp(tramp.z, 0.0f, 0.20f);
        tramp.y = lerp(tramp.y, 0.0f, 0.16f);
        faceTowards(machine.x, 0.0f, 0.22f);
        machine.armExtend = lerp(machine.armExtend, 0.88f, 0.22f);
        machine.armAngle = lerp(machine.armAngle, -8.0f + p * 12.0f, 0.18f);
        machine.bodyAngle = lerp(machine.bodyAngle, -2.0f, 0.12f);
        tramp.bodyPitch = lerp(tramp.bodyPitch, 10.0f, 0.12f);
        tramp.bodyTilt = lerp(tramp.bodyTilt, -12.0f, 0.12f);
        tramp.armLAngle = lerp(tramp.armLAngle, -54.0f, 0.16f);
        tramp.armRAngle = lerp(tramp.armRAngle, 48.0f, 0.16f);
        tramp.caneAngle = lerp(tramp.caneAngle, 8.0f, 0.14f);
        stopWalk();
        return;
    }

    if (lt < 6.8f) {
        // Machine strike -> launch -> unstable midair correction (same staged mechanics as Scene 5 fall).
        float p = clamp01((lt - 3.0f) / 3.8f);
        float e = easeInOut(p);
        tramp.trapped = false;
        s4Escaped = p > 0.55f;

        tramp.x = lerp(machine.x - 0.74f, WORLD_LEFT - 1.35f, e);
        tramp.z = lerp(0.0f, -0.16f, e);

        // Arc with extra drop in the second half to make the collapse read naturally.
        float arc = std::sinf(p * 3.14159f) * 0.56f;
        float drop = (p > 0.58f) ? (p - 0.58f) * 0.52f : 0.0f;
        tramp.y = arc - drop;

        // Buckle then twist while trying to recover in-air.
        tramp.bodyPitch = lerp(10.0f, 58.0f, e);
        tramp.bodyTilt = lerp(-12.0f, -66.0f, e);
        tramp.legLAngle = lerp(tramp.legLAngle, 34.0f, 0.18f);
        tramp.legRAngle = lerp(tramp.legRAngle, -30.0f, 0.18f);
        tramp.hatExtraY = std::sinf(p * 3.14159f) * 0.30f;
        tramp.armLAngle = lerp(-54.0f, -88.0f, e);
        tramp.armRAngle = lerp(48.0f, 90.0f, e);
        tramp.caneAngle = lerp(8.0f, -58.0f, e);

        machine.armExtend = lerp(machine.armExtend, 1.08f, 0.24f);
        machine.armAngle = lerp(machine.armAngle, 64.0f, 0.26f);
        machine.bodyAngle = lerp(machine.bodyAngle, 10.0f, 0.20f);
        return;
    }

    if (lt < 10.0f) {
        // Ground contact: knee/hand brace fails into flat back finish.
        float p = clamp01((lt - 6.8f) / 3.2f);
        float e = easeInOut(p);
        tramp.trapped = false;
        tramp.x = lerp(WORLD_LEFT - 1.35f, WORLD_LEFT + 0.35f, e);
        tramp.z = lerp(-0.16f, 0.02f, e);

        float kneeDrop = lerp(-0.10f, -0.24f, clamp01(p / 0.45f));
        float backDrop = lerp(-0.24f, -0.40f, clamp01((p - 0.45f) / 0.55f));
        tramp.y = (p < 0.45f) ? kneeDrop : backDrop;

        tramp.facingYaw = lerp(tramp.facingYaw, -150.0f, 0.10f);
        tramp.bodyPitch = lerp(58.0f, 90.0f, e);
        tramp.bodyTilt = lerp(-20.0f, 0.0f, e);
        tramp.legLAngle = lerp(tramp.legLAngle, -10.0f, 0.18f);
        tramp.legRAngle = lerp(tramp.legRAngle, 8.0f, 0.18f);
        tramp.armLAngle = lerp(-88.0f, -12.0f, e);
        tramp.armRAngle = lerp(90.0f, 12.0f, e);
        tramp.caneAngle = lerp(-58.0f, -22.0f, e);
        tramp.hatExtraY = lerp(0.30f, 0.00f, e);

        machine.armExtend = lerp(machine.armExtend, 0.52f, 0.10f);
        machine.armAngle = lerp(machine.armAngle, 22.0f, 0.10f);
        machine.bodyAngle = lerp(machine.bodyAngle, 2.0f, 0.10f);
        machine.lightBlink = lerp(machine.lightBlink, 0.36f, 0.10f);
        return;
    }

    if (lt < 23.0f) {
        // Wake-up happens offscreen, so the visible beat remains the flat landing.
        float p = easeInOut(clamp01((lt - 10.0f) / 13.0f));
        tramp.trapped = false;
        tramp.x = WORLD_LEFT - 1.65f;
        tramp.z = -0.20f;
        tramp.y = lerp(-0.40f, 0.0f, p);
        tramp.bodyPitch = lerp(90.0f, 0.0f, p);
        tramp.bodyTilt = lerp(0.0f, 0.0f, p);
        tramp.armLAngle = lerp(-12.0f, -28.0f, p);
        tramp.armRAngle = lerp(12.0f, -14.0f, p);
        tramp.caneAngle = lerp(-22.0f, -28.0f, p);
        tramp.hatExtraY = 0.0f;
        machine.armExtend = lerp(machine.armExtend, 0.22f + std::sinf((lt - 10.0f) * 2.8f) * 0.08f, 0.12f);
        machine.armAngle = lerp(machine.armAngle, std::sinf((lt - 10.0f) * 3.8f) * 24.0f, 0.16f);
        machine.bodyAngle = lerp(machine.bodyAngle, std::sinf((lt - 10.0f) * 2.2f) * 6.0f, 0.10f);
        machine.lightBlink = lerp(machine.lightBlink, 0.30f + std::sinf((lt - 10.0f) * 7.0f) * 0.18f, 0.10f);
        return;
    }

    if (lt < 32.0f) {
    // Re-enter at the landing side and wobble upright toward the lever.
    float p = easeInOut(clamp01((lt - 23.0f) / 9.0f));
    float targetX = lerp(WORLD_LEFT + 0.40f, leverX - 0.10f, p);
    // Nudge Chaplin a touch behind the lever approach so the camera can read his face
    // as he takes a short backward step before the close-up.
    tramp.x = clampX(targetX - 0.06f + std::sinf((lt - 23.0f) * 5.0f) * 0.05f);
        tramp.z = lerp(0.04f, leverZ + 0.06f, p);
        applyWobblyWalk(dt, 6.4f);
        tramp.bodyPitch = lerp(tramp.bodyPitch, 0.0f, 0.14f);
        tramp.bodyTilt = lerp(tramp.bodyTilt, std::sinf((lt - 23.0f) * 2.0f) * 3.5f, 0.12f);
        tramp.caneAngle = lerp(tramp.caneAngle, -28.0f + std::sinf((lt - 23.0f) * 2.6f) * 8.0f, 0.10f);
        tramp.hatExtraY = lerp(tramp.hatExtraY, 0.0f, 0.10f);
        tramp.faceMood = 1;
        faceTowards(leverX, leverZ, 0.22f);
        machine.armExtend = lerp(machine.armExtend, 0.18f + std::sinf((lt - 23.0f) * 4.0f) * 0.08f, 0.14f);
        machine.armAngle = lerp(machine.armAngle, std::sinf((lt - 23.0f) * 5.0f) * 28.0f, 0.16f);
        machine.bodyAngle = lerp(machine.bodyAngle, std::sinf((lt - 23.0f) * 2.6f) * 10.0f, 0.10f);
        machine.lightBlink = lerp(machine.lightBlink, 0.18f + std::sinf((lt - 23.0f) * 6.0f) * 0.20f, 0.12f);
        return;
    }

    if (lt < 40.0f) {
        // Accidental lever pull shuts the machine off.
        float p = easeInOut(clamp01((lt - 32.0f) / 8.0f));
        s2LeverPull = lerp(s2LeverPull, 1.0f, 0.22f);
        tramp.x = lerp(tramp.x, leverX - 0.02f, 0.12f);
        tramp.z = lerp(tramp.z, leverZ + 0.10f, 0.12f);
        tramp.y = lerp(tramp.y, -0.02f, 0.12f);
        tramp.bodyPitch = lerp(tramp.bodyPitch, 28.0f, 0.12f);
        tramp.bodyTilt = lerp(tramp.bodyTilt, 12.0f, 0.12f);
        tramp.armLAngle = lerp(tramp.armLAngle, -60.0f, 0.14f);
        tramp.armRAngle = lerp(tramp.armRAngle, -10.0f, 0.12f);
        tramp.foreArmAngle = lerp(tramp.foreArmAngle, -28.0f, 0.12f);
        tramp.caneAngle = lerp(tramp.caneAngle, -14.0f, 0.12f);
        machine.activated = true;
        machine.armExtend = lerp(machine.armExtend, 0.86f, 0.18f);
        machine.armAngle = lerp(machine.armAngle, 38.0f + std::sinf(p * 10.0f) * 18.0f, 0.20f);
        machine.bodyAngle = lerp(machine.bodyAngle, std::sinf(p * 12.0f) * 14.0f, 0.20f);
        machine.lightBlink = lerp(machine.lightBlink, 0.78f, 0.14f);
        if (p > 0.35f) {
            machine.activated = false;
        }
        return;
    }

    if (lt < 48.0f) {
        // Machine spins down and Chaplin stares at the mess.
        float p = easeInOut(clamp01((lt - 40.0f) / 8.0f));
        s2LeverPull = lerp(s2LeverPull, 1.0f, 0.05f);
        tramp.x = lerp(tramp.x, machine.x - 1.06f, 0.08f);
        tramp.z = lerp(tramp.z, 0.02f, 0.08f);
        tramp.y = lerp(tramp.y, 0.0f, 0.14f);
        tramp.faceMood = 4;
        tramp.bodyPitch = lerp(tramp.bodyPitch, 6.0f, 0.10f);
        tramp.bodyTilt = lerp(tramp.bodyTilt, 2.0f, 0.10f);
        tramp.armLAngle = lerp(tramp.armLAngle, -22.0f, 0.10f);
        tramp.armRAngle = lerp(tramp.armRAngle, -14.0f, 0.10f);
        tramp.foreArmAngle = lerp(tramp.foreArmAngle, -8.0f, 0.10f);
        tramp.caneAngle = lerp(tramp.caneAngle, -24.0f, 0.10f);
        machine.activated = false;
        machine.armExtend = lerp(machine.armExtend, 0.0f, 0.10f);
        machine.armAngle = lerp(machine.armAngle, 0.0f, 0.10f);
        machine.bodyAngle = lerp(machine.bodyAngle, 0.0f, 0.08f);
        machine.lightBlink = lerp(machine.lightBlink, 0.0f, 0.14f);
        machine.doorOpen = lerp(machine.doorOpen, 0.0f, 0.08f);
        if (p > 0.75f) {
            s4Escaped = true;
        }
        return;
    }

    if (lt < 55.0f) {
        // Happy close-up after stopping the machine.
        s2LeverPull = lerp(s2LeverPull, 1.0f, 0.03f);
        tramp.faceMood = 1;
        tramp.trapped = false;
        // Place Chaplin a little further behind so the close-up camera has room to show his face.
        // Increase the backing-off so the camera has clear line-of-sight.
        tramp.x = lerp(tramp.x, machine.x - 1.25f, 0.06f);
        tramp.z = lerp(tramp.z, 0.0f, 0.05f);
        tramp.y = lerp(tramp.y, 0.0f, 0.08f);
        tramp.bodyPitch = lerp(tramp.bodyPitch, 0.0f, 0.08f);
        tramp.bodyTilt = lerp(tramp.bodyTilt, 0.0f, 0.08f);
        tramp.armLAngle = lerp(tramp.armLAngle, 8.0f, 0.08f);
        tramp.armRAngle = lerp(tramp.armRAngle, -10.0f, 0.08f);
        tramp.foreArmAngle = lerp(tramp.foreArmAngle, 0.0f, 0.08f);
        tramp.caneAngle = lerp(tramp.caneAngle, -24.0f, 0.08f);
        tramp.hatExtraY = lerp(tramp.hatExtraY, 0.0f, 0.06f);
        machine.activated = false;
        machine.armExtend = lerp(machine.armExtend, 0.0f, 0.08f);
        machine.armAngle = lerp(machine.armAngle, 0.0f, 0.08f);
        machine.bodyAngle = lerp(machine.bodyAngle, 0.0f, 0.08f);
        machine.lightBlink = lerp(machine.lightBlink, 0.0f, 0.08f);
        machine.doorOpen = lerp(machine.doorOpen, 0.0f, 0.08f);
        return;
    }

    // Hold a calm finish pose so Scene 5 can take over cleanly.
    s2LeverPull = lerp(s2LeverPull, 1.0f, 0.03f);
    tramp.faceMood = 1;
    tramp.trapped = false;
    tramp.x = lerp(tramp.x, machine.x - 1.00f, 0.05f);
    tramp.z = lerp(tramp.z, 0.0f, 0.05f);
    tramp.y = lerp(tramp.y, 0.0f, 0.10f);
    tramp.bodyPitch = lerp(tramp.bodyPitch, 0.0f, 0.08f);
    tramp.bodyTilt = lerp(tramp.bodyTilt, 0.0f, 0.08f);
    tramp.armLAngle = lerp(tramp.armLAngle, 10.0f, 0.08f);
    tramp.armRAngle = lerp(tramp.armRAngle, -12.0f, 0.08f);
    tramp.foreArmAngle = lerp(tramp.foreArmAngle, 0.0f, 0.08f);
    tramp.caneAngle = lerp(tramp.caneAngle, -28.0f, 0.08f);
    machine.activated = false;
    machine.armExtend = lerp(machine.armExtend, 0.0f, 0.08f);
    machine.armAngle = lerp(machine.armAngle, 0.0f, 0.08f);
    machine.bodyAngle = lerp(machine.bodyAngle, 0.0f, 0.06f);
    machine.lightBlink = lerp(machine.lightBlink, 0.0f, 0.08f);
}

void updateScene5(float dt, float lt)
{
    static bool s5TripPhysicsInit = false;
    static float s5FallPitch = 0.0f;
    static float s5FallOmega = 0.0f;
    static float s5FallY = 0.08f;
    static float s5ForwardVel = 0.0f;

    tramp.faceMood = 1;
    machine.activated = false;
    machine.bodyAngle = lerp(machine.bodyAngle, 0.0f, 0.04f);
    machine.armAngle = lerp(machine.armAngle, 0.0f, 0.04f);
    machine.armExtend = lerp(machine.armExtend, 0.0f, 0.04f);

    if (lt < 0.05f) {
        s5TripPhysicsInit = false;
        s5FallPitch = 0.0f;
        s5FallOmega = 0.0f;
        s5FallY = 0.08f;
        s5ForwardVel = 0.0f;
        s5GotGift = true;
        s5GiftY = 1.2f;
        finalTrip = false;
        s2LeverPull = 1.0f;
        // Initialize Scene 5 expansion variables
        s5MachineVibration = 1.0f;
        s5DoorOpened = false;
        s5DoorAngle = 0.0f;
        s5LightBlinked = false;
        s5GiftSlide = 0.0f;
        s5HatGrabbed = false;
        s5HatX = machine.x - 0.3f;
        s5HatY = s5GiftY;
        s5HatZ = 0.0f;
        s5HatRotX = 0.0f;
        s5HatRotY = 0.0f;
        s5HatRotZ = 0.0f;
        s5HatOnHead = true;
        s5Walking = false;
        s5WalkSpeed = 0.0f;
        s5Waved = false;
        s5Nodded = false;
        s5Tripped = false;
        s5TripProgress = 0.0f;
        s5Fallen = false;
        s5FallProgress = 0.0f;
        s5HatFlying = false;
        s5HatFlyX = 0.0f;
        s5HatFlyY = 0.0f;
        s5HatFlyZ = 0.0f;
        s5HatFlyRot = 0.0f;
        s5SteamPuff = false;
        s5PuffScale = 0.0f;
        s5Beeped = false;
    }

    // 4:25–4:30 — Machine Stabilizes
    if (lt < 5.0f) {
        // Machine vibration slows
        s5MachineVibration = lerp(s5MachineVibration, 0.0f, 0.05f);
        machine.bodyAngle = std::sinf(lt * 10.0f) * s5MachineVibration * 5.0f;
        machine.armAngle = std::sinf(lt * 12.0f) * s5MachineVibration * 10.0f;
        // Character cautiously peeks from behind cane
        tramp.x = lerp(tramp.x, machine.x - 1.08f, 0.06f);
        tramp.z = lerp(tramp.z, 0.0f, 0.06f);
        tramp.y = lerp(tramp.y, 0.0f, 0.12f);
        tramp.bodyPitch = lerp(tramp.bodyPitch, 5.0f, 0.08f); // slight forward lean
        tramp.armRAngle = lerp(tramp.armRAngle, -25.0f, 0.08f); // cane behind
        tramp.caneAngle = lerp(tramp.caneAngle, -30.0f, 0.08f);
        return;
    }

    // 4:26 — Tap machine
    if (lt < 6.0f) {
        // Tap once gently
        float p = (lt - 5.0f);
        tramp.armRAngle = lerp(tramp.armRAngle, -35.0f + std::sinf(p * 20.0f) * 5.0f, 0.15f);
        tramp.foreArmAngle = lerp(tramp.foreArmAngle, -10.0f + std::sinf(p * 20.0f) * 3.0f, 0.15f);
        // Small impulse to machine
        machine.bodyAngle += std::sinf(p * 20.0f) * 2.0f;
        return;
    }

    // 4:27 — Steam puff (blast wind-up)
    if (lt < 7.0f) {
        s5HatOnHead = true;
        s5SteamPuff = true;
        s5PuffScale = lerp(s5PuffScale, 1.5f, 0.2f);
        if (lt > 6.5f) s5PuffScale = lerp(s5PuffScale, 0.0f, 0.3f);
        s5HatX = tramp.x;
        s5HatY = tramp.y + 0.66f;
        s5HatZ = tramp.z + 0.01f;
        return;
    }

    // 4:28 — Blast: hat ejects like a frisbee.
    if (lt < 8.6f) {
        float p = clamp01((lt - 7.0f) / 1.6f);
        tramp.y = std::sinf(p * 3.14159f) * 0.28f;
        tramp.x = lerp(tramp.x, machine.x - 1.55f, 0.20f);
        tramp.armLAngle = lerp(tramp.armLAngle, 60.0f, 0.2f);
        tramp.armRAngle = lerp(tramp.armRAngle, -60.0f, 0.2f);
        tramp.bodyPitch = lerp(tramp.bodyPitch, -20.0f, 0.2f);
        tramp.faceMood = 2;

        if (!s5HatFlying) {
            s5HatFlying = true;
            s5HatOnHead = false;
            s5HatFlyX = tramp.x;
            s5HatFlyY = tramp.y + 0.66f;
            s5HatFlyZ = tramp.z + 0.01f;
            s5HatFlyRot = 0.0f;
        }

        float e = easeInOut(p);
        s5HatFlyX = lerp(s5HatFlyX, WORLD_RIGHT + 1.40f, e);
        s5HatFlyY = lerp(s5HatFlyY, 1.90f, e) + std::sinf(p * 3.14159f) * 0.20f;
        s5HatFlyZ = 0.10f + std::sinf(p * 6.28318f) * 0.22f;
        s5HatFlyRot += 900.0f * dt;
        s5HatRotY += 860.0f * dt;
        s5HatRotZ = s5HatFlyRot;
        s5HatX = s5HatFlyX;
        s5HatY = s5HatFlyY;
        s5HatZ = s5HatFlyZ;
        return;
    }

    // 4:29 — Second reaction: hat boomerangs back to head.
    if (lt < 9.6f) {
        float p = clamp01((lt - 8.6f) / 1.0f);
        float e = easeInOut(p);
        float headX = tramp.x;
        float headY = tramp.y + 0.66f;
        float headZ = tramp.z + 0.01f;

        tramp.y = lerp(tramp.y, 0.0f, 0.15f);
        tramp.bodyPitch = lerp(tramp.bodyPitch, 0.0f, 0.15f);
        tramp.armLAngle = lerp(tramp.armLAngle, 0.0f, 0.15f);
        tramp.armRAngle = lerp(tramp.armRAngle, -20.0f, 0.15f);
        tramp.bodyTilt = lerp(tramp.bodyTilt, -5.0f, 0.15f);
        tramp.faceMood = 1;

        s5HatFlyX = lerp(WORLD_RIGHT + 1.40f, headX, e);
        s5HatFlyY = lerp(1.90f, headY, e) + std::sinf(p * 3.14159f) * 0.16f;
        s5HatFlyZ = lerp(0.10f, headZ, e) + std::sinf(p * 6.28318f) * 0.07f;
        s5HatFlyRot += 760.0f * dt;
        s5HatRotY += 780.0f * dt;
        s5HatRotZ = s5HatFlyRot;
        s5HatX = s5HatFlyX;
        s5HatY = s5HatFlyY;
        s5HatZ = s5HatFlyZ;

        if (p > 0.96f) {
            s5HatOnHead = true;
            s5HatFlying = false;
            s5HatRotX = 0.0f;
            s5HatRotY = 0.0f;
            s5HatRotZ = 0.0f;
            s5HatX = headX;
            s5HatY = headY;
            s5HatZ = headZ;
        }
        return;
    }

    // 4:29.6 — Straighten posture proudly
    if (lt < 9.4f) {
        s5HatOnHead = true;
        s5HatFlying = false;
        tramp.y = lerp(tramp.y, 0.0f, 0.16f);
        tramp.bodyPitch = lerp(tramp.bodyPitch, 0.0f, 0.16f);
        tramp.armLAngle = lerp(tramp.armLAngle, 0.0f, 0.16f);
        tramp.armRAngle = lerp(tramp.armRAngle, -20.0f, 0.16f);
        tramp.bodyTilt = lerp(tramp.bodyTilt, -5.0f, 0.16f);
        tramp.faceMood = 1;
        s5HatX = tramp.x;
        s5HatY = tramp.y + 0.66f;
        s5HatZ = tramp.z + 0.01f;
        return;
    }

    // 4:30–4:45 — Keep hat attached and aligned to head coordinates.
    if (lt >= 10.0f && lt < 25.0f && !s5HatFlying) {
        s5HatOnHead = true;
        s5HatX = tramp.x;
        s5HatY = tramp.y + 0.66f;
        s5HatZ = tramp.z + 0.01f;
        s5HatRotX = 0.0f;
        s5HatRotY = 0.0f;
        s5HatRotZ = lerp(s5HatRotZ, 0.0f, 0.15f);
    }

    // 4:30 — Door opens
    if (lt < 10.0f) {
        s5DoorOpened = true;
        s5DoorAngle = lerp(s5DoorAngle, 90.0f, 0.1f);
        machine.doorOpen = s5DoorAngle / 90.0f;
        tramp.bodyPitch = lerp(tramp.bodyPitch, 12.0f, 0.08f);
        tramp.earLean = lerp(tramp.earLean, 0.08f, 0.08f);
        return;
    }

    // 4:31 — Curious lean
    if (lt < 11.0f) {
        tramp.bodyPitch = lerp(tramp.bodyPitch, 18.0f, 0.08f);
        tramp.headPitch = lerp(tramp.headPitch, 8.0f, 0.08f);
        return;
    }

    // 4:32 — Light blinks
    if (lt < 12.0f) {
        s5LightBlinked = !s5LightBlinked;
        machine.lightBlink = s5LightBlinked ? 0.8f : 0.1f;
        return;
    }

    // 4:33 — Gift emerges
    if (lt < 13.0f) {
        s5GiftSlide = lerp(s5GiftSlide, 1.0f, 0.08f);
        s5GotGift = true;
        return;
    }

    // 4:34 — React and settle
    if (lt < 14.0f) {
        tramp.faceMood = 2;
        tramp.bodyPitch = lerp(tramp.bodyPitch, -6.0f, 0.18f);
        tramp.armLAngle = lerp(tramp.armLAngle, 22.0f, 0.18f);
        tramp.armRAngle = lerp(tramp.armRAngle, -24.0f, 0.18f);
        return;
    }

    // 4:35 — Compose
    if (lt < 15.0f) {
        tramp.faceMood = 1;
        tramp.bodyPitch = lerp(tramp.bodyPitch, -2.0f, 0.12f);
        tramp.caneAngle = lerp(tramp.caneAngle, -34.0f, 0.12f);
        return;
    }

    // 4:36 — Door shuts
    if (lt < 16.0f) {
        s5DoorAngle = lerp(s5DoorAngle, 0.0f, 0.1f);
        machine.doorOpen = s5DoorAngle / 90.0f;
        return;
    }

    // 4:37 — Small proud nod
    if (lt < 17.0f) {
        tramp.headPitch = lerp(tramp.headPitch, 9.0f, 0.12f);
        tramp.chestOut = lerp(tramp.chestOut, 0.2f, 0.1f);
        return;
    }

    // 4:38 — Settle posture
    if (lt < 18.0f) {
        tramp.headPitch = lerp(tramp.headPitch, 0.0f, 0.10f);
        tramp.bodyPitch = lerp(tramp.bodyPitch, -4.0f, 0.10f);
        return;
    }

    // 4:39 — Ready to walk
    if (lt < 19.0f) {
        tramp.bodyPitch = lerp(tramp.bodyPitch, -5.0f, 0.10f);
        tramp.chestOut = lerp(tramp.chestOut, 0.30f, 0.10f);
        tramp.caneAngle = lerp(tramp.caneAngle, -40.0f, 0.10f);
        return;
    }

    // 4:40–4:46 — Forward walk with explicit machine bypass waypoints.
    if (lt < 26.0f) {
        s5Walking = true;
        float walkSpeed = (lt < 22.0f) ? 3.4f : ((lt < 24.0f) ? 3.9f : 4.4f);
        s5ForwardVel = walkSpeed * 0.55f;

        if (lt < 21.20f) {
            moveTowardsXZ(machine.x - 0.75f, 0.78f, 1.95f, walkSpeed, dt);
        } else if (lt < 23.60f) {
            moveTowardsXZ(machine.x + 0.90f, 0.78f, 2.05f, walkSpeed, dt);
        } else {
            moveTowardsXZ(7.25f, 0.08f, 2.15f, walkSpeed, dt);
        }

        // Small expressive beats while maintaining stable gait.
        if (lt >= 21.0f && lt < 22.0f) {
            tramp.armLAngle = lerp(tramp.armLAngle, 34.0f + std::sinf((lt - 21.0f) * 9.0f) * 8.0f, 0.18f);
        }
        if (lt >= 23.0f && lt < 24.0f) {
            tramp.headPitch = lerp(tramp.headPitch, 11.0f, 0.16f);
        }
        return;
    }

    // 4:47 — Trigger: toe catches, support knee buckles, CoM still moving forward.
    if (lt < 26.45f) {
        s5Tripped = true;
        float p = clamp01((lt - 26.0f) / 0.45f);
        tramp.x += s5ForwardVel * (1.0f - 0.65f * p) * dt;
        tramp.y = lerp(tramp.y, 0.05f, 0.18f);
        tramp.bodyPitch = lerp(tramp.bodyPitch, 16.0f, 0.20f);
        tramp.bodyTilt = lerp(tramp.bodyTilt, -8.0f, 0.20f);
        tramp.legRAngle = lerp(tramp.legRAngle, 32.0f, 0.24f);
        tramp.legLAngle = lerp(tramp.legLAngle, -16.0f, 0.24f);
        tramp.armLAngle = lerp(tramp.armLAngle, 26.0f, 0.20f);
        tramp.armRAngle = lerp(tramp.armRAngle, 10.0f, 0.20f);
        s5FallY = tramp.y;
        s5FallPitch = tramp.bodyPitch;
        s5TripPhysicsInit = false;
        return;
    }

    // 4:47.45–4:49 — Physics fall: forward pivot around feet + gravity drop.
    if (lt < 29.20f) {
        if (!s5TripPhysicsInit) {
            s5TripPhysicsInit = true;
            s5FallPitch = tramp.bodyPitch;
            s5FallOmega = 62.0f + s5ForwardVel * 22.0f;
            s5FallY = tramp.y;
        }

        tramp.faceMood = 2;
        float angAcc = 210.0f;   // deg/s^2
        float g = -2.8f;         // scene-scaled gravity on root
        s5FallOmega += angAcc * dt;
        s5FallPitch += s5FallOmega * dt;
        if (s5FallPitch > 90.0f) {
            s5FallPitch = 90.0f;
        }

        // Inertia keeps body moving forward while feet catch.
        float inertia = clamp01((90.0f - s5FallPitch) / 90.0f);
        tramp.x += (s5ForwardVel * 0.75f) * inertia * dt;

        // Gravity drop to ground, hard-stop at y=0.
        s5FallY += g * dt;
        if (s5FallY < 0.0f) {
            s5FallY = 0.0f;
        }

        tramp.y = s5FallY;
        tramp.bodyPitch = s5FallPitch;
        tramp.bodyTilt = lerp(tramp.bodyTilt, 2.0f, 0.16f);
        tramp.legLAngle = lerp(tramp.legLAngle, -18.0f, 0.18f);
        tramp.legRAngle = lerp(tramp.legRAngle, 22.0f, 0.18f);
        tramp.armLAngle = lerp(tramp.armLAngle, -68.0f, 0.18f);
        tramp.armRAngle = lerp(tramp.armRAngle, 24.0f, 0.18f);
        tramp.headPitch = lerp(tramp.headPitch, -18.0f, 0.16f);
        tramp.reactionScale = lerp(tramp.reactionScale, 0.94f, 0.12f);

        // Impact stop condition.
        if (tramp.bodyPitch >= 90.0f && tramp.y <= 0.0f) {
            tramp.bodyPitch = 90.0f;
            tramp.y = 0.0f;
        }

        s5HatOnHead = true;
        s5HatFlying = false;
        s5HatX = tramp.x;
        s5HatY = tramp.y + 0.66f;
        s5HatZ = tramp.z + 0.01f;
        return;
    }

    // 4:49.2–5:00 — Aftermath hold with hat restored on head.
    s5HatOnHead = true;
    s5HatFlying = false;
    tramp.y = lerp(tramp.y, 0.0f, 0.18f);
    tramp.bodyPitch = lerp(tramp.bodyPitch, 90.0f, 0.16f);
    tramp.bodyTilt = lerp(tramp.bodyTilt, 2.0f, 0.14f);
    tramp.legLAngle = lerp(tramp.legLAngle, -22.0f, 0.12f);
    tramp.legRAngle = lerp(tramp.legRAngle, 24.0f, 0.12f);
    tramp.armLAngle = lerp(tramp.armLAngle, -64.0f, 0.12f);
    tramp.armRAngle = lerp(tramp.armRAngle, 18.0f, 0.12f);
    tramp.headPitch = lerp(tramp.headPitch, -14.0f, 0.10f);
    tramp.reactionScale = lerp(tramp.reactionScale, 0.96f, 0.08f);
    s5HatX = tramp.x;
    s5HatY = tramp.y + 0.66f;
    s5HatZ = tramp.z + 0.01f;
}

void updateAll(float dt)
{
    static float prevX = tramp.x;
    static float prevZ = tramp.z;

    gTime += dt;
    sceneLocalTime += dt;
    if (gTime < SCENE1_END) {
        currentScene = 1;
    } else if (gTime < SCENE2_END) {
        currentScene = 2;
    } else if (gTime < SCENE3_END) {
        currentScene = 3;
    } else if (gTime < SCENE4_END) {
        currentScene = 4;
    } else {
        currentScene = 5;
    }

    static int lastScene = 0;
    if (currentScene != lastScene) {
        sceneLocalTime = 0;
        projectiles.clear();
        lastScene = currentScene;
    }

    if (currentScene == 3 && s3FightFinished) {
        gTime = SCENE3_END + 0.02f;
        currentScene = 4;
        sceneLocalTime = 0.0f;
        lastScene = 4;
    }

    float lt = sceneLocalTime;
    switch (currentScene) {
        case 1:
            updateScene1(dt, lt);
            break;
        case 2:
            updateScene2(dt, lt);
            break;
        case 3:
            updateScene3(dt, lt);
            break;
        case 4:
            updateScene4(dt, lt);
            break;
        case 5:
            updateScene5(dt, lt);
            break;
    }

    // Auto-face walking direction only when scenes are not driving explicit look targets.
    bool expressionBeat = (currentScene == 1 && sceneLocalTime > 42.0f && sceneLocalTime < 47.0f);
    bool explicitLook =
        expressionBeat ||
        (currentScene == 1 && sceneLocalTime > 34.0f) ||
        (currentScene == 2) ||
        (currentScene == 3) ||
        (currentScene == 4) ||
        (currentScene == 5);
    float dx = tramp.x - prevX;
    float dz = tramp.z - prevZ;
    if (!explicitLook && (dx * dx + dz * dz) > 0.00002f) {
        float walkFacing = std::atan2(dx, dz) * 57.29578f;
        tramp.facingYaw = lerp(tramp.facingYaw, walkFacing, 0.22f);
    }
    prevX = tramp.x;
    prevZ = tramp.z;

    // Keep him from intersecting machine footprint with smooth push-out.
    if (currentScene != 3) {
        float halfW = MACHINE_W + CHAR_W + 0.06f;
        float halfD = 0.46f;
        float dxm = tramp.x - machine.x;
        float dzm = tramp.z;
        if (std::fabs(dxm) < halfW && std::fabs(dzm) < halfD) {
            float px = halfW - std::fabs(dxm);
            float pz = halfD - std::fabs(dzm);
            if (px < pz) {
                tramp.x += (dxm >= 0.0f ? px : -px);
            } else {
                tramp.z += (dzm >= 0.0f ? pz : -pz);
            }
        }
    }

    while (tramp.caneAngle > 180.0f) {
        tramp.caneAngle -= 360.0f;
    }
    while (tramp.caneAngle < -180.0f) {
        tramp.caneAngle += 360.0f;
    }
    if (tramp.caneAngle > 85.0f) {
        tramp.caneAngle = 85.0f;
    }
    if (tramp.caneAngle < -95.0f) {
        tramp.caneAngle = -95.0f;
    }

    // Exit program at exactly 5:00 (30 seconds sceneLocalTime into Scene 5)
    if (currentScene == 5 && lt >= 30.0f) {
        exit(0);
    }

    if (tramp.reactionScale > 1.22f) {
        tramp.reactionScale = 1.22f;
    }
    if (tramp.reactionScale < 0.82f) {
        tramp.reactionScale = 0.82f;
    }

    while (tramp.facingYaw > 180.0f) {
        tramp.facingYaw -= 360.0f;
    }
    while (tramp.facingYaw < -180.0f) {
        tramp.facingYaw += 360.0f;
    }

    if (tramp.foreArmAngle > 35.0f) {
        tramp.foreArmAngle = 35.0f;
    }
    if (tramp.foreArmAngle < -40.0f) {
        tramp.foreArmAngle = -40.0f;
    }

    bool allowLargeTilt =
        (currentScene == 4) ||
        (currentScene == 5);
    if (!allowLargeTilt) {
        if (tramp.bodyTilt > 25.0f) {
            tramp.bodyTilt = 25.0f;
        }
        if (tramp.bodyTilt < -25.0f) {
            tramp.bodyTilt = -25.0f;
        }
        if (tramp.y > 0.22f) {
            tramp.y = 0.22f;
        }
        if (tramp.y < -0.14f) {
            tramp.y = -0.14f;
        }
    }

    if (tramp.armLAngle > 90.0f) {
        tramp.armLAngle = 90.0f;
    }
    if (tramp.armLAngle < -95.0f) {
        tramp.armLAngle = -95.0f;
    }
    if (tramp.armRAngle > 90.0f) {
        tramp.armRAngle = 90.0f;
    }
    if (tramp.armRAngle < -95.0f) {
        tramp.armRAngle = -95.0f;
    }

    if (machine.armAngle > 70.0f) {
        machine.armAngle = 70.0f;
    }
    if (machine.armAngle < -70.0f) {
        machine.armAngle = -70.0f;
    }
    if (machine.armExtend > 1.10f) {
        machine.armExtend = 1.10f;
    }
    if (machine.armExtend < 0.0f) {
        machine.armExtend = 0.0f;
    }

    // Prevent leg crossover and residual walking gait while standing.
    float legLimit = (currentScene == 5) ? 65.0f : 36.0f;
    if (tramp.legLAngle > legLimit) {
        tramp.legLAngle = legLimit;
    }
    if (tramp.legLAngle < -legLimit) {
        tramp.legLAngle = -legLimit;
    }
    if (tramp.legRAngle > legLimit) {
        tramp.legRAngle = legLimit;
    }
    if (tramp.legRAngle < -legLimit) {
        tramp.legRAngle = -legLimit;
    }

    if ((dx * dx + dz * dz) < 0.0012f && currentScene != 3 && currentScene != 5) {
        tramp.legLAngle = lerp(tramp.legLAngle, 0.0f, 0.48f);
        tramp.legRAngle = lerp(tramp.legRAngle, 0.0f, 0.48f);
        tramp.shoeBend = lerp(tramp.shoeBend, 0.0f, 0.48f);
        tramp.y = lerp(tramp.y, 0.0f, 0.32f);
    }
}
