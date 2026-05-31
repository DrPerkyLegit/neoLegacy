#pragma once
#include "LivingEntityRenderer.h"
#include "HumanoidArmorLayer.h"
#include "ResourceLocation.h"
#include <vector>

class ArmorStandModel;
class LivingEntity;
class Entity;
class RenderLayer;
class CustomHeadLayer;

class ArmorStandRenderer : public LivingEntityRenderer {
public:
    class ArmorStandArmorLayer : public HumanoidArmorLayer {
    public:
        explicit ArmorStandArmorLayer(LivingEntityRenderer* renderer);
        virtual ~ArmorStandArmorLayer() {}
        virtual void createArmorModels() override;
    };

protected:
    std::vector<RenderLayer*> renderLayers;
    ArmorStandArmorLayer*     armorLayer;
    CustomHeadLayer*          headLayer;

public:
    void addLayer(RenderLayer* layer)           { renderLayers.push_back(layer); }
    void addLayer(ArmorStandArmorLayer* layer)  { armorLayer = layer; }
    ArmorStandArmorLayer* getArmorLayer()       { return armorLayer; }

    static ResourceLocation LOC_ARMOR_STAND;

    ArmorStandRenderer();
    virtual ~ArmorStandRenderer();

    virtual ResourceLocation* getTextureLocation(shared_ptr<Entity> entity) override;
    virtual bool              shouldShowName(shared_ptr<LivingEntity> mob) override;

    virtual void setupRotations(shared_ptr<LivingEntity> mob,
                                float bob, float bodyRot, float a) override;
    virtual void render(shared_ptr<Entity> entity,
                        double x, double y, double z,
                        float rot, float a) override;
    virtual void renderModel(shared_ptr<LivingEntity> mob,
                             float wp, float ws, float bob,
                             float headRotMinusBodyRot,
                             float headRotx, float scale) override;
    virtual int  prepareArmor(shared_ptr<LivingEntity> mob, int layer, float a) override;
    void prepareCarriedItem(shared_ptr<Entity> mob, shared_ptr<ItemInstance> item);

    virtual void additionalRendering(shared_ptr<LivingEntity> mob, float a) override;
};