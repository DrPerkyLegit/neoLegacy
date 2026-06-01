#pragma once
#include "Mob.h"
#include "Rotations.h"
#include "SynchedEntityData.h"
#include "Vec3.h"
#include "Random.h"

class ArmorStand : public LivingEntity
{
public:
    eINSTANCEOF GetType() override { return eTYPE_ARMORSTAND; }
    static Entity* create(Level* level) { return new ArmorStand(level); }

    static const Rotations DEFAULT_HEAD_POSE;
    static const Rotations DEFAULT_BODY_POSE;
    static const Rotations DEFAULT_LEFT_ARM_POSE;
    static const Rotations DEFAULT_RIGHT_ARM_POSE;
    static const Rotations DEFAULT_LEFT_LEG_POSE;
    static const Rotations DEFAULT_RIGHT_LEG_POSE;

private:
    static const int DATA_CLIENT_FLAGS   = 0xA;
    static const int DATA_HEAD_POSE      = 0xB;
    static const int DATA_BODY_POSE      = 0xC;
    static const int DATA_LEFT_ARM_POSE  = 0xD;
    static const int DATA_RIGHT_ARM_POSE = 0xE;
    static const int DATA_LEFT_LEG_POSE  = 0xF;
    static const int DATA_RIGHT_LEG_POSE = 0x10;

    static const int FLAG_SMALL       = 1;
    static const int FLAG_NO_GRAVITY  = 2;
    static const int FLAG_SHOW_ARMS   = 4;
    static const int FLAG_NO_BASEPLATE= 8;
    static const int FLAG_MARKER      = 16;

public:
    static const int SLOT_WEAPON    = 0;
    static const int SLOT_BOOTS     = 1;
    static const int SLOT_LEGGINGS  = 2;
    static const int SLOT_CHEST     = 3;
    static const int SLOT_HELM      = 4;
    static const int equipmentCount = 5;

private:
    shared_ptr<ItemInstance> equipment[equipmentCount];

    Rotations headPose;
    Rotations bodyPose;
    Rotations leftArmPose;
    Rotations rightArmPose;
    Rotations leftLegPose;
    Rotations rightLegPose;

    int  disabledSlots;
    bool invisible;
    bool isMarkerFlag;
    bool noPhysics;
    float standDamage;
public:
    float rotateElytraX;
    float rotateElytraY;
    float rotateElytraZ;

public:
    long long lastHit;
    int hurtDir;

    explicit ArmorStand(Level* level);
    virtual ~ArmorStand();

    bool hasPhysics()    const;
    bool isBaby()        const;
    bool isSmall()       const;
    bool isShowArms()    const;
    bool isNoBasePlate() const;
    bool showBasePlate() const;
    bool isMarker()      const;
    bool isEffectiveAi() const;

    void setSmall       (bool v);
    void setShowArms    (bool v);
    void setNoBasePlate (bool v);
    void setMarker      (bool v);
    void setNoGravity   (bool v);
    void setInvisible   (bool v);

    Rotations getHeadPose()     const;
    Rotations getBodyPose()     const;
    Rotations getLeftArmPose()  const;
    Rotations getRightArmPose() const;
    Rotations getLeftLegPose()  const;
    Rotations getRightLegPose() const;

    void setHeadPose    (const Rotations& r);
    void setBodyPose    (const Rotations& r);
    void setLeftArmPose (const Rotations& r);
    void setRightArmPose(const Rotations& r);
    void setLeftLegPose (const Rotations& r);
    void setRightLegPose(const Rotations& r);

   
    virtual bool useNewAi()       override { return false; }
    virtual void tick()           override;
    virtual bool hurt(DamageSource* source, float damage) override;
    virtual bool isPickable()     override;
    virtual bool isPushable()     override;

    
    bool ignoreExplosion();
    void kill();

    virtual bool isAttackable()   override { return true; }
    
    virtual bool shouldShowName() override { return false; }
    virtual bool isInWall()       override { return false; }

    
    
    virtual float getEyeHeight()  override;

    virtual bool shouldRenderAtSqrDistance(double distSq) override;

    virtual void push(shared_ptr<Entity> entity) override {}
    virtual void push(double xa, double ya, double za) override {}
    virtual void pushEntities() override;

    
    virtual void doPush(shared_ptr<Entity> e) override {}

    virtual int   getHurtSound()   override { return -1; }
    virtual int   getDeathSound()  override { return -1; }
    virtual float getSoundVolume() override { return 0.0f; }
    virtual bool  makeStepSound()  override { return false; }

    virtual wstring getAName()       override { return L""; }
    virtual wstring getDisplayName() override { return L""; }
    virtual wstring getNetworkName() override { return L""; }

    virtual bool interact(shared_ptr<Player> player) override;
    virtual bool interactAt(shared_ptr<Player> player, const Vec3& hitVec);
    virtual int  test_interactAt(shared_ptr<Player> player, const Vec3& hitVec);

    void brokenByAnything();
    void brokenByPlayer(shared_ptr<Player> player, DamageSource* source);
    void causeDamage(float damage);

    void updateBoundingBox(bool markerMode);
    void updateInvisibilityStatus();

    virtual void travel(float xz, float ya);

    virtual shared_ptr<ItemInstance> getCarriedItem()                                    override;
    virtual shared_ptr<ItemInstance> getCarried(int slot)                                override;
    virtual shared_ptr<ItemInstance> getArmor(int pos)                                  override;
    virtual void                     setEquippedSlot(int slot, shared_ptr<ItemInstance> item) override;
    virtual ItemInstanceArray        getEquipmentSlots()                                 override;
    bool                             setSlot(int inventorySlot, shared_ptr<ItemInstance> item);
    void                             swapItem(shared_ptr<Player> player, int slot);
    int                              getEquipmentSlotForItem(shared_ptr<ItemInstance> item) const;

    virtual void readAdditionalSaveData(CompoundTag* tag) override;
    virtual void addAdditonalSaveData  (CompoundTag* tag) override;
    void         readPose(CompoundTag* poseTag);
    CompoundTag* writePose();

    virtual void handleEntityEvent(byte eventId) override;

protected:
    virtual void defineSynchedData() override;
    virtual void registerAttributes() override;

private:
    void init();
};