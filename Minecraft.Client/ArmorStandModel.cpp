
#include "stdafx.h"
#include "ModelPart.h"
#include "ArmorStandModel.h"
#include "../Minecraft.World/ArmorStand.h"

ArmorStandModel::ArmorStandModel(float scale) : HumanoidModel(scale)
{
    texWidth  = 64;
    texHeight = 64;
    
    head = new ModelPart(this, 0, 0);
    head->addBox(-1.0f, -7.0f, -1.0f, 2, 7, 2, scale);
    head->setPos(0.0f, 0.0f, 0.0f);
    head->compile(1.0f / 16.0f);

    
    hair = new ModelPart(this, 0, 0);

    
    body = new ModelPart(this, 0, 26);
    body->addBox(-6.0f, 0.0f, -1.5f, 12, 3, 3, scale);
    body->setPos(0.0f, 0.0f, 0.0f);
    body->compile(1.0f / 16.0f);

    
    arm1 = new ModelPart(this, 24, 0);
    arm1->addBox(-2.0f, -2.0f, -1.0f, 2, 12, 2, scale);
    arm1->setPos(-5.0f, 2.0f, 0.0f);
    arm1->compile(1.0f / 16.0f);

    
    arm0 = new ModelPart(this, 32, 16);
    arm0->mirror();
    arm0->addBox(0.0f, -2.0f, -1.0f, 2, 12, 2, scale);
    arm0->setPos(5.0f, 2.0f, 0.0f);
    arm0->compile(1.0f / 16.0f);

    
    leg0 = new ModelPart(this, 8, 0);
    leg0->addBox(-1.0f, 0.0f, -1.0f, 2, 11, 2, scale);
    leg0->setPos(-1.9f, 12.0f, 0.0f);
    leg0->compile(1.0f / 16.0f);

    
    leg1 = new ModelPart(this, 40, 16);
    leg1->mirror();
    leg1->addBox(-1.0f, 0.0f, -1.0f, 2, 11, 2, scale);
    leg1->setPos(1.9f, 12.0f, 0.0f);
    leg1->compile(1.0f / 16.0f);

    
    rightBodyStick = new ModelPart(this, 16, 0);
    rightBodyStick->addBox(-3.0f, 3.0f, -1.0f, 2, 7, 2, scale);
    rightBodyStick->setPos(0.0f, 0.0f, 0.0f);
    rightBodyStick->visible = false; 
    rightBodyStick->compile(1.0f / 16.0f);

    
    leftBodyStick = new ModelPart(this, 48, 16);
    leftBodyStick->addBox(1.0f, 3.0f, -1.0f, 2, 7, 2, scale);
    leftBodyStick->setPos(0.0f, 0.0f, 0.0f);
    leftBodyStick->visible = false;
    leftBodyStick->compile(1.0f / 16.0f);

    
    shoulderStick = new ModelPart(this, 0, 48);
    shoulderStick->addBox(-4.0f, 10.0f, -1.0f, 8, 2, 2, scale);
    shoulderStick->setPos(0.0f, 0.0f, 0.0f);
    shoulderStick->compile(1.0f / 16.0f);

    
    basePlate = new ModelPart(this, 0, 32);
    basePlate->mirror();
    basePlate->addBox(-6.0f, 11.0f, -6.0f, 12, 1, 12, scale);
    basePlate->setPos(0.0f, 12.0f, 0.0f);
    basePlate->compile(1.0f / 16.0f);
}

void ArmorStandModel::setupPose(
    float hX,  float hY,  float hZ,
    float bX,  float bY,  float bZ,
    float lAX, float lAY, float lAZ,
    float rAX, float rAY, float rAZ,
    float lLX, float lLY, float lLZ,
    float rLX, float rLY, float rLZ)
{
    head->xRot = hX;  head->yRot = hY;  head->zRot = hZ;
    if (hair) { hair->xRot = hX; hair->yRot = hY; hair->zRot = hZ; }

    body->xRot = bX;  body->yRot = bY;  body->zRot = bZ;
    rightBodyStick->xRot = bX; rightBodyStick->yRot = bY; rightBodyStick->zRot = bZ;
    leftBodyStick->xRot  = bX; leftBodyStick->yRot  = bY; leftBodyStick->zRot  = bZ;
    shoulderStick->xRot  = bX; shoulderStick->yRot  = bY; shoulderStick->zRot  = bZ;

    arm1->xRot = lAX; arm1->yRot = lAY; arm1->zRot = lAZ; 
    arm0->xRot = rAX; arm0->yRot = rAY; arm0->zRot = rAZ; 

    leg1->xRot = lLX; leg1->yRot = lLY; leg1->zRot = lLZ; 
    leg0->xRot = rLX; leg0->yRot = rLY; leg0->zRot = rLZ; 
}

void ArmorStandModel::setupAnim(float time, float r, float bob, float yRot, float xRot,
                                 float scale, shared_ptr<Entity> entity,
                                 unsigned int uiBitmaskOverrideAnim)
{
    
}

void ArmorStandModel::render(shared_ptr<Entity> entity,
                              float time, float r, float bob,
                              float yRot, float xRot,
                              float scale, bool usecompiled)
{
    shared_ptr<ArmorStand> stand = dynamic_pointer_cast<ArmorStand>(entity);
    if (stand)
    {
        bool armsVis    = stand->isShowArms();
        bool baseVis    = stand->showBasePlate();
        bool isSmallSt  = stand->isSmall();

        arm0->visible = armsVis;
        arm1->visible = armsVis;

        rightBodyStick->visible = !isSmallSt;
        leftBodyStick->visible  = !isSmallSt;
        shoulderStick->visible  = !isSmallSt;
        basePlate->visible      = baseVis;
    }

    HumanoidModel::render(entity, time, r, bob, yRot, xRot, scale, usecompiled);

    rightBodyStick->render(scale, usecompiled);
    leftBodyStick->render(scale, usecompiled);
    shoulderStick->render(scale, usecompiled);
    basePlate->render(scale, usecompiled);
}