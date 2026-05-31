#include "stdafx.h"
#include "com.mojang.nbt.h"
#include "net.minecraft.world.entity.ai.attributes.h"
#include "net.minecraft.world.entity.ai.navigation.h"
#include "net.minecraft.world.entity.ai.goal.h"
#include "net.minecraft.world.level.tile.h"
#include "net.minecraft.world.level.h"
#include "net.minecraft.world.item.h"
#include "net.minecraft.world.entity.player.h"
#include "net.minecraft.world.damagesource.h"
#include "SynchedEntityData.h"
#include "SharedMonsterAttributes.h"
#include "ArmorStand.h"
#include "BlockPos.h"
#include "../Minecraft.Client/ServerLevel.h"
#include "ParticleTypes.h"
#include "Random.h"
#include "AABB.h"
#include "../Minecraft.World/LevelChunk.h"

const Rotations ArmorStand::DEFAULT_HEAD_POSE      (0.0f, 0.0f, 0.0f);
const Rotations ArmorStand::DEFAULT_BODY_POSE      (0.0f, 0.0f, 0.0f);
const Rotations ArmorStand::DEFAULT_LEFT_ARM_POSE  (-15.0f, 0.0f, 10.0f);
const Rotations ArmorStand::DEFAULT_RIGHT_ARM_POSE (-10.0f, 0.0f, -10.0f);
const Rotations ArmorStand::DEFAULT_LEFT_LEG_POSE  (-1.0f, 0.0f, -1.0f);
const Rotations ArmorStand::DEFAULT_RIGHT_LEG_POSE (1.0f, 0.0f, 1.0f);

ArmorStand::ArmorStand(Level* level)
    : LivingEntity(level)
{
    ArmorStand::init();
}

void ArmorStand::init()
{
    registerAttributes();
    defineSynchedData();

    lastHit       = -100LL;
    standDamage   = 0.0f;
    hurtDir       = 1;
    disabledSlots = 0;
    invisible     = false;
    isMarkerFlag  = false;
    heightOffset = 0.0f;
    for (int i = 0; i < equipmentCount; i++)
        equipment[i] = nullptr;

    rotateElytraX = 0.2617994f;
    rotateElytraY = 0.0f;
    rotateElytraZ = -0.2617994f;

    headPose     = DEFAULT_HEAD_POSE;
    bodyPose     = DEFAULT_BODY_POSE;
    leftArmPose  = DEFAULT_LEFT_ARM_POSE;
    rightArmPose = DEFAULT_RIGHT_ARM_POSE;
    leftLegPose  = DEFAULT_LEFT_LEG_POSE;
    rightLegPose = DEFAULT_RIGHT_LEG_POSE;

    byte flags = entityData->getByte(DATA_CLIENT_FLAGS);
    noPhysics = (flags & FLAG_NO_GRAVITY) != 0;

    setSize(0.5f, 1.975f);
}

void ArmorStand::registerAttributes()
{
    LivingEntity::registerAttributes();
    getAttribute(SharedMonsterAttributes::MAX_HEALTH)->setBaseValue(20.0);
    getAttribute(SharedMonsterAttributes::MOVEMENT_SPEED)->setBaseValue(0.0);
    getAttribute(SharedMonsterAttributes::KNOCKBACK_RESISTANCE)->setBaseValue(1.0);
}

void ArmorStand::defineSynchedData()
{
    LivingEntity::defineSynchedData();
    
    entityData->define(3, static_cast<byte>(0));   // DATA_CUSTOM_NAME_VISIBLE
    entityData->define(2, wstring(L""));            // DATA_CUSTOM_NAME
    entityData->define(DATA_CLIENT_FLAGS,   static_cast<byte>(0));
    entityData->define(DATA_HEAD_POSE,      DEFAULT_HEAD_POSE);
    entityData->define(DATA_BODY_POSE,      DEFAULT_BODY_POSE);
    entityData->define(DATA_LEFT_ARM_POSE,  DEFAULT_LEFT_ARM_POSE);
    entityData->define(DATA_RIGHT_ARM_POSE, DEFAULT_RIGHT_ARM_POSE);
    entityData->define(DATA_LEFT_LEG_POSE,  DEFAULT_LEFT_LEG_POSE);
    entityData->define(DATA_RIGHT_LEG_POSE, DEFAULT_RIGHT_LEG_POSE);
}

ArmorStand::~ArmorStand() {}

bool ArmorStand::hasPhysics()    const { return (entityData->getByte(DATA_CLIENT_FLAGS) & FLAG_NO_GRAVITY) == 0;  }
bool ArmorStand::isBaby()        const { return (entityData->getByte(DATA_CLIENT_FLAGS) & FLAG_SMALL)       != 0; }
bool ArmorStand::isSmall()       const { return isBaby(); }
bool ArmorStand::isShowArms()    const { return (entityData->getByte(DATA_CLIENT_FLAGS) & FLAG_SHOW_ARMS)   != 0; }
bool ArmorStand::isNoBasePlate() const { return (entityData->getByte(DATA_CLIENT_FLAGS) & FLAG_NO_BASEPLATE)!= 0; }
bool ArmorStand::showBasePlate() const { return !isNoBasePlate(); }
bool ArmorStand::isMarker()      const { return (entityData->getByte(DATA_CLIENT_FLAGS) & FLAG_MARKER)      != 0; }
bool ArmorStand::isEffectiveAi() const
{
    if (!LivingEntity::isEffectiveAi()) return false;
    return (entityData->getByte(DATA_CLIENT_FLAGS) & FLAG_NO_GRAVITY) == 0;
}

void ArmorStand::setSmall(bool v)
{
    byte b = entityData->getByte(DATA_CLIENT_FLAGS);
    entityData->set(DATA_CLIENT_FLAGS, v ? (byte)(b | FLAG_SMALL) : (byte)(b & ~FLAG_SMALL));
}
void ArmorStand::setShowArms(bool v)
{
    byte b = entityData->getByte(DATA_CLIENT_FLAGS);
    entityData->set(DATA_CLIENT_FLAGS, v ? (byte)(b | FLAG_SHOW_ARMS) : (byte)(b & ~FLAG_SHOW_ARMS));
}
void ArmorStand::setNoBasePlate(bool v)
{
    byte b = entityData->getByte(DATA_CLIENT_FLAGS);
    entityData->set(DATA_CLIENT_FLAGS, v ? (byte)(b | FLAG_NO_BASEPLATE) : (byte)(b & ~FLAG_NO_BASEPLATE));
}
void ArmorStand::setMarker(bool v)
{
    byte b = entityData->getByte(DATA_CLIENT_FLAGS);
    entityData->set(DATA_CLIENT_FLAGS, v ? (byte)(b | FLAG_MARKER) : (byte)(b & ~FLAG_MARKER));
}
void ArmorStand::setNoGravity(bool v)
{
    byte b = entityData->getByte(DATA_CLIENT_FLAGS);
    entityData->set(DATA_CLIENT_FLAGS, v ? (byte)(b | FLAG_NO_GRAVITY) : (byte)(b & ~FLAG_NO_GRAVITY));
}
void ArmorStand::setInvisible(bool v)
{
    invisible = v;
    Entity::setInvisible(v);
}


Rotations ArmorStand::getHeadPose()     const { return entityData->getRotations(DATA_HEAD_POSE);      }
Rotations ArmorStand::getBodyPose()     const { return entityData->getRotations(DATA_BODY_POSE);      }
Rotations ArmorStand::getLeftArmPose()  const { return entityData->getRotations(DATA_LEFT_ARM_POSE);  }
Rotations ArmorStand::getRightArmPose() const { return entityData->getRotations(DATA_RIGHT_ARM_POSE); }
Rotations ArmorStand::getLeftLegPose()  const { return entityData->getRotations(DATA_LEFT_LEG_POSE);  }
Rotations ArmorStand::getRightLegPose() const { return entityData->getRotations(DATA_RIGHT_LEG_POSE); }

void ArmorStand::setHeadPose    (const Rotations& r) { headPose     = r; entityData->set(DATA_HEAD_POSE,      r); }
void ArmorStand::setBodyPose    (const Rotations& r) { bodyPose     = r; entityData->set(DATA_BODY_POSE,      r); }
void ArmorStand::setLeftArmPose (const Rotations& r) { leftArmPose  = r; entityData->set(DATA_LEFT_ARM_POSE,  r); }
void ArmorStand::setRightArmPose(const Rotations& r) { rightArmPose = r; entityData->set(DATA_RIGHT_ARM_POSE, r); }
void ArmorStand::setLeftLegPose (const Rotations& r) { leftLegPose  = r; entityData->set(DATA_LEFT_LEG_POSE,  r); }
void ArmorStand::setRightLegPose(const Rotations& r) { rightLegPose = r; entityData->set(DATA_RIGHT_LEG_POSE, r); }


float ArmorStand::getEyeHeight()
{
    return isSmall() ? (bbHeight * 0.5f) : (bbHeight * 0.9f);
}

bool ArmorStand::isPickable()
{
    if (!LivingEntity::isPickable()) return false;
    return !isMarker();
}

bool ArmorStand::isPushable() { return false; }


bool ArmorStand::ignoreExplosion()
{
    return isInvisible();
}

void ArmorStand::kill()
{
    remove();
}

bool ArmorStand::shouldRenderAtSqrDistance(double distSq)
{
    double size = getBoundingBox()->getSize();
    if (size == 0.0) size = 4.0;
    double limit = size * 64.0;
    return distSq < limit * limit;
}

void ArmorStand::updateBoundingBox(bool markerMode)
{
    float ox = x, oy = y, oz = z;
    if (markerMode) setSize(0.0f, 0.0f);
    else            setSize(0.5f, 1.975f);
    moveTo(ox, oy, oz, yRot, xRot);
}

void ArmorStand::updateInvisibilityStatus()
{
    setInvisible(invisible);
}

void ArmorStand::travel(float xa, float ya)
{
    if (hasPhysics()) {
        float friction = 0.91f;
        int frictionTile = 0;
        if (onGround)
        {
            frictionTile = level->getTile(Mth::floor(x), Mth::floor(bb->y0) - 1, Mth::floor(z));
            friction = 0.6f * 0.91f;
            if (frictionTile > 0)
            {
                friction = Tile::tiles[frictionTile]->friction * 0.91f;
            }
        }

        float friction2 = (0.6f * 0.6f * 0.91f * 0.91f * 0.6f * 0.91f) / (friction * friction * friction);

        float speed;
        if (onGround)
        {
            speed = getSpeed() * friction2;
        }
        else
        {
            speed = flyingSpeed;
        }

        moveRelative(xa, ya, speed);

        friction = 0.91f;
        if (onGround)
        {
            friction = 0.6f * 0.91f;
            if (frictionTile > 0)
            {
                friction = Tile::tiles[frictionTile]->friction * 0.91f;
            }
        }
        if (onLadder())
        {
            float max = 0.15f;
            if (xd < -max) xd = -max;
            if (xd > max) xd = max;
            if (zd < -max) zd = -max;
            if (zd > max) zd = max;
            fallDistance = 0;
            if (yd < -0.15) yd = -0.15;
            bool playerSneaking = isSneaking() && this->instanceof(eTYPE_PLAYER);
            if (playerSneaking && yd < 0) yd = 0;
        }

        move(xd, yd, zd);

        if (horizontalCollision && onLadder())
        {
            yd = 0.2;
        }

        if (!level->isClientSide || (level->hasChunkAt(static_cast<int>(x), 0, static_cast<int>(z)) && level->getChunkAt(static_cast<int>(x), static_cast<int>(z))->loaded))
        {
            yd -= 0.08;
        }
        else if (y > 0)
        {
            yd = -0.1;
        }
        else
        {
            yd = 0;
        }

        yd *= 0.98f;
        xd *= friction;
        zd *= friction;

        walkAnimSpeedO = walkAnimSpeed;
        double xxd = x - xo;
        double zzd = z - zo;
        float wst = Mth::sqrt(xxd * xxd + zzd * zzd) * 4;
        if (wst > 1) wst = 1;
        walkAnimSpeed += (wst - walkAnimSpeed) * 0.4f;
        walkAnimPos += walkAnimSpeed;
    } else {
        move(xd, yd, zd);
    }
}


void ArmorStand::tick()
{
    float lockedRot = this->yRot;
    LivingEntity::tick();
    if (onGround)
    {
        BlockPos pos((int)floorf(x), (int)floorf(y) - 1, (int)floorf(z));
        int blockId = level->getTile(pos.getX(), pos.getY(), pos.getZ());
        if (blockId == Tile::topSnow->id)
        {
            int meta = level->getData(pos.getX(), pos.getY(), pos.getZ());
            float snowHeight = ((meta & 0x7) + 1) * 0.125f;
            moveTo(x, floorf(y) + snowHeight, z, yRot, xRot);
        }
    }
    this->yRot      = lockedRot;
    this->yRotO     = lockedRot;
    this->yBodyRot  = lockedRot;
    this->yBodyRotO = lockedRot;
    this->yHeadRot  = lockedRot;
    this->yHeadRotO = lockedRot;

    if ((long long)tickCount - lastHit > 20)
        standDamage = 0.0f;

    auto syncPose = [&](int slot, Rotations& local)
    {
        Rotations net = entityData->getRotations(slot);
        if (local != net) { local = net; entityData->set(slot, net); }
    };
    syncPose(DATA_HEAD_POSE,      headPose);
    syncPose(DATA_BODY_POSE,      bodyPose);
    syncPose(DATA_LEFT_ARM_POSE,  leftArmPose);
    syncPose(DATA_RIGHT_ARM_POSE, rightArmPose);
    syncPose(DATA_LEFT_LEG_POSE,  leftLegPose);
    syncPose(DATA_RIGHT_LEG_POSE, rightLegPose);

    bool markerNow = (entityData->getByte(DATA_CLIENT_FLAGS) & FLAG_MARKER) != 0;
    if (isMarkerFlag && !markerNow)
    {
        float ox = x, oy = y, oz = z;
        setSize(0.5f, 1.975f);
        moveTo(ox, oy, oz, yRot, xRot);
        isMarkerFlag = false;
    }
    else if (!isMarkerFlag && markerNow)
    {
        float ox = x, oy = y, oz = z;
        setSize(0.0f, 0.0f);
        moveTo(ox, oy, oz, yRot, xRot);
        isMarkerFlag = true;
    }
}


void ArmorStand::brokenByAnything()
{
    for (int i = 0; i < equipmentCount; i++)
    {
        if (equipment[i] && equipment[i]->count > 0)
        {
            spawnAtLocation(equipment[i], 0.0f);
            equipment[i] = nullptr;
        }
    }
}

void ArmorStand::brokenByPlayer(shared_ptr<Player> player, DamageSource* source)
{
    spawnAtLocation(Item::armor_stand_Id, 1);
    brokenByAnything();
}

void ArmorStand::causeDamage(float damage)
{
    float h = getHealth();
    h -= damage;
    if (h <= 0.5f)
    {
        spawnAtLocation(Item::armor_stand_Id, 1);
        brokenByAnything();
        remove();
    }
    else
    {
        setHealth(h);
    }
}

bool ArmorStand::hurt(DamageSource* source, float damage)
{
    if (isInvulnerable() || level->isClientSide || removed || isMarker()) return false;

    if (source->getMsgId() == eEntityDamageType_Suffocate) return false;

    if (dynamic_cast<EntityDamageSource*>(source) != nullptr)
    {
        shared_ptr<Entity> attacker = source->getDirectEntity();
        if (attacker && attacker->instanceof(eTYPE_PLAYER) &&
            !dynamic_pointer_cast<Player>(attacker)->isAllowedToHurtEntity(shared_from_this()))
            return false;
    }

    if (source->isExplosion())
    {
        spawnAtLocation(Item::armor_stand_Id, 1);
        brokenByAnything();
        remove();
        return true;
    }

    if (dynamic_cast<EntityDamageSource*>(source) != nullptr)
    {
        shared_ptr<Entity> attacker = source->getEntity();
        if (attacker && attacker->instanceof(eTYPE_PLAYER) &&
            dynamic_pointer_cast<Player>(attacker)->abilities.instabuild)
        {
            level->broadcastEntityEvent(shared_from_this(), (byte)31);
            remove();
            return true;
        }
    }

    if (source->isFire())
    {
        if (isInWater()) return false;
        float h = getHealth() - 0.15f;
        setHealth(h);
        if (h <= 0.5f)
        {
            brokenByAnything();
            remove();
        }
        return false;
    }

    hurtDir = -hurtDir;
    standDamage += damage * 10.0f;
    lastHit = (long long)tickCount;
    level->broadcastEntityEvent(shared_from_this(), (byte)32);

    if (standDamage >= 40.0f)
    {
        spawnAtLocation(Item::armor_stand_Id, 1);
        brokenByAnything();
        remove();
    }

    return true;
}


void ArmorStand::pushEntities()
{
    AABB* myBox = getBoundingBox();
    AABB* grown = myBox->grow(0.2, 0.0, 0.2);
    auto* entities = level->getEntities(shared_from_this(), grown);
    if (!entities) return;

    for (auto& e : *entities)
    {
        if (!e) continue;

        
        if (!e->instanceof(eTYPE_MINECART)) continue;

        
        if (distanceToSqr(e) > 0.2) continue;

        
        e->push(shared_from_this());
    }
}


shared_ptr<ItemInstance> ArmorStand::getCarriedItem()           { return equipment[SLOT_WEAPON]; }
shared_ptr<ItemInstance> ArmorStand::getCarried(int slot)       { return (slot >= 0 && slot < equipmentCount) ? equipment[slot] : nullptr; }
shared_ptr<ItemInstance> ArmorStand::getArmor(int pos)
{
    int idx = pos + 1;
    return (idx >= 1 && idx < equipmentCount) ? equipment[idx] : nullptr;
}
void ArmorStand::setEquippedSlot(int slot, shared_ptr<ItemInstance> item)
{
    if (slot >= 0 && slot < equipmentCount) equipment[slot] = item;
}
ItemInstanceArray ArmorStand::getEquipmentSlots() { return ItemInstanceArray(equipment, equipmentCount); }

int ArmorStand::getEquipmentSlotForItem(shared_ptr<ItemInstance> item) const
{
    if (!item) return SLOT_WEAPON;
    return Mob::getEquipmentSlotForItem(item);
}

bool ArmorStand::setSlot(int inventorySlot, shared_ptr<ItemInstance> item)
{
    int localSlot;
    if (inventorySlot == 0x63) localSlot = SLOT_WEAPON;
    else
    {
        localSlot = inventorySlot - 0x63;
        if (localSlot < 0 || localSlot >= equipmentCount) return false;
    }
    if (item)
    {
        int naturalSlot = getEquipmentSlotForItem(item);
        if (naturalSlot != localSlot)
        {
            if (localSlot == SLOT_HELM)
            {
                auto bi = dynamic_cast<TileItem*>(item->getItem());
                if (!bi) return false;
            }
            else return false;
        }
    }
    setEquippedSlot(localSlot, item);
    return true;
}

void ArmorStand::swapItem(shared_ptr<Player> player, int slot)
{
    shared_ptr<ItemInstance> standItem  = equipment[slot];
    shared_ptr<ItemInstance> playerItem = player->getCarriedItem();
    int disabledFlags = disabledSlots;

    if (!standItem)
    {
        if ((disabledFlags & (1 << (slot + 0x10))) != 0) return;
        if (!playerItem) goto do_take;
    }
    if ((disabledFlags & (1 << (slot + 8))) != 0) goto do_take;
    if (playerItem)
    {
        if (player->abilities.instabuild)
        {
            auto toPlace = ItemInstance::clone(playerItem);
            toPlace->count = 1;
            setEquippedSlot(slot, toPlace);
        }
        else
        {
            auto toPlace = ItemInstance::clone(playerItem);
            toPlace->count = 1;
            setEquippedSlot(slot, toPlace);
            if (standItem)
            {
                if (playerItem->count > 1) { playerItem->remove(1); if (!player->inventory->add(standItem)) spawnAtLocation(standItem, 0.0f); }
                else player->inventory->setItem(player->inventory->selected, standItem);
            }
            else playerItem->remove(1);
        }
        return;
    }
do_take:
    if (standItem) { player->inventory->setItem(player->inventory->selected, standItem); setEquippedSlot(slot, nullptr); }
}

bool ArmorStand::interact(shared_ptr<Player> player)
{
    if (level->isClientSide) return true;
    if (isMarker())          return false;
    if (player->isSpectator()) return true;

   
    float dX        = this->x - player->x;
    float dZ        = this->z - player->z;
    float distHoriz = sqrtf(dX * dX + dZ * dZ);
    float pitchRad  = player->xRot * (3.14159265f / 180.0f);
    float lookYDiff = -tanf(pitchRad) * distHoriz;
    float hitY      = (player->y + 1.62f + lookYDiff) - this->y;

    if (isSmall()) hitY *= 2.0f;

    shared_ptr<ItemInstance> carried = player->getCarriedItem();

    int targetSlot;
    if (carried)
    {
        targetSlot = getEquipmentSlotForItem(carried);
    }
    else
    {
        if      (hitY >= 0.1f && hitY < 0.55f) targetSlot = SLOT_BOOTS;
        else if (hitY >= 0.55f && hitY < 0.9f) targetSlot = SLOT_LEGGINGS;
        else if (hitY >= 0.9f  && hitY < 1.6f) targetSlot = SLOT_CHEST;
        else if (hitY >= 1.6f)                 targetSlot = SLOT_HELM;
        else                                   targetSlot = SLOT_WEAPON;
    }

    if (targetSlot == SLOT_WEAPON && !isShowArms()) return false;

    
    if ((disabledSlots & (1 << targetSlot)) != 0) return false;

    shared_ptr<ItemInstance> standItem = equipment[targetSlot];

    if (carried)
    {
        auto toPlace = ItemInstance::clone(carried);
        toPlace->count = 1;
        setEquippedSlot(targetSlot, toPlace);

        if (standItem)
        {
            if (carried->count > 1)
            {
                carried->remove(1);
                if (!player->inventory->add(standItem))
                    spawnAtLocation(standItem, 0.0f);
            }
            else
            {
                player->inventory->setItem(player->inventory->selected, standItem);
            }
        }
        else
        {
            carried->remove(1);
        }
    }
    else if (standItem)
    {
        player->inventory->setItem(player->inventory->selected, standItem);
        setEquippedSlot(targetSlot, nullptr);
    }

    return true;
}
bool ArmorStand::interactAt(shared_ptr<Player> player, const Vec3& hitVec)
{
    if (isMarker()) return false;
    if (level->isClientSide) return true;
    if (player->isSpectator()) return true;

    shared_ptr<ItemInstance> carried = player->getCarriedItem();

    int targetSlot;
    if (carried)
    {
        
        targetSlot = getEquipmentSlotForItem(carried);
    }
    else
    {
        
        float hitY = (float)hitVec.y;
        bool isSmallStand = isSmall();
        if (isSmallStand) hitY *= 2.0f;

        float bootsMax = isSmallStand ? 0.9f  : 0.55f;
        float legsMin  = isSmallStand ? 1.2f  : 0.9f;
        float legsMax  = isSmallStand ? 1.9f  : 1.6f;

        targetSlot = SLOT_WEAPON;
        if (hitY >= 0.1f && hitY < bootsMax)
            targetSlot = SLOT_BOOTS;
        else if (hitY >= legsMin && hitY < legsMax)
            targetSlot = SLOT_LEGGINGS;
        if (hitY >= 1.6f && equipment[SLOT_HELM])
            targetSlot = SLOT_HELM;
    }

    
    if (targetSlot == SLOT_WEAPON && !isShowArms()) return false;

    
    if ((disabledSlots & (1 << targetSlot)) != 0) return false;

    swapItem(player, targetSlot);
    return true;
}

int ArmorStand::test_interactAt(shared_ptr<Player> player, const Vec3& hitVec)
{
    if (isMarker()) return 0;
    if (player->isSpectator()) return 0;
    shared_ptr<ItemInstance> carried = player->getCarriedItem();
    int playerSlot = carried ? getEquipmentSlotForItem(carried) : SLOT_WEAPON;
    bool hasStandItem = equipment[playerSlot] != nullptr;
    if ((disabledSlots & (1 << playerSlot)) != 0 && (disabledSlots & 1) != 0) return 0;
    if (!carried) return hasStandItem ? 2 : 0;
    return hasStandItem ? 3 : 1;
}


void ArmorStand::readPose(CompoundTag* poseTag)
{
    auto loadRot = [&](const wchar_t* key, const Rotations& def, int dataSlot)
    {
        Rotations r = def;
        
        ListTag<FloatTag>* list = static_cast<ListTag<FloatTag>*>(
            static_cast<void*>(poseTag->getList(key)));
        if (list && list->size() >= 3)
            r = Rotations(list);
        entityData->set(dataSlot, r);
    };
    loadRot(L"Head",     DEFAULT_HEAD_POSE,      DATA_HEAD_POSE);
    loadRot(L"Body",     DEFAULT_BODY_POSE,      DATA_BODY_POSE);
    loadRot(L"LeftArm",  DEFAULT_LEFT_ARM_POSE,  DATA_LEFT_ARM_POSE);
    loadRot(L"RightArm", DEFAULT_RIGHT_ARM_POSE, DATA_RIGHT_ARM_POSE);
    loadRot(L"LeftLeg",  DEFAULT_LEFT_LEG_POSE,  DATA_LEFT_LEG_POSE);
    loadRot(L"RightLeg", DEFAULT_RIGHT_LEG_POSE, DATA_RIGHT_LEG_POSE);

    headPose     = entityData->getRotations(DATA_HEAD_POSE);
    bodyPose     = entityData->getRotations(DATA_BODY_POSE);
    leftArmPose  = entityData->getRotations(DATA_LEFT_ARM_POSE);
    rightArmPose = entityData->getRotations(DATA_RIGHT_ARM_POSE);
    leftLegPose  = entityData->getRotations(DATA_LEFT_LEG_POSE);
    rightLegPose = entityData->getRotations(DATA_RIGHT_LEG_POSE);
}

CompoundTag* ArmorStand::writePose()
{
    CompoundTag* pose = new CompoundTag();
    auto save = [&](const wchar_t* key, const Rotations& cur, const Rotations& def)
    { if (cur != def) pose->put(key, cur.save()); };
    save(L"Head",     headPose,     DEFAULT_HEAD_POSE);
    save(L"Body",     bodyPose,     DEFAULT_BODY_POSE);
    save(L"LeftArm",  leftArmPose,  DEFAULT_LEFT_ARM_POSE);
    save(L"RightArm", rightArmPose, DEFAULT_RIGHT_ARM_POSE);
    save(L"LeftLeg",  leftLegPose,  DEFAULT_LEFT_LEG_POSE);
    save(L"RightLeg", rightLegPose, DEFAULT_RIGHT_LEG_POSE);
    return pose;
}

void ArmorStand::readAdditionalSaveData(CompoundTag* tag)
{
    LivingEntity::readAdditionalSaveData(tag);

    if (tag->contains(L"ArmorStandEquipment", 9))
    {
        ListTag<CompoundTag>* eqList = static_cast<ListTag<CompoundTag>*>(
            static_cast<void*>(tag->getList(L"ArmorStandEquipment")));
        if (eqList)
            for (int i = 0; i < equipmentCount && i < eqList->size(); i++)
                equipment[i] = ItemInstance::fromTag(eqList->get(i));
    }

    setInvisible   (tag->getBoolean(L"Invisible"));
    setSmall       (tag->getBoolean(L"Small"));
    setShowArms    (tag->getBoolean(L"ShowArms"));
    setNoBasePlate (tag->getBoolean(L"NoBasePlate"));
    setNoGravity   (tag->getBoolean(L"NoGravity"));
    setMarker      (tag->getBoolean(L"Marker"));
    disabledSlots = tag->getInt(L"DisabledSlots");

    isMarkerFlag = isMarker();
    noPhysics    = (entityData->getByte(DATA_CLIENT_FLAGS) & FLAG_NO_GRAVITY) != 0;

    CompoundTag* pose = tag->getCompound(L"Pose");
    if (pose && !pose->isEmpty()) readPose(pose);
}

void ArmorStand::addAdditonalSaveData(CompoundTag* tag)
{
    LivingEntity::addAdditonalSaveData(tag);

    ListTag<CompoundTag>* eqList = new ListTag<CompoundTag>();
    for (int i = 0; i < equipmentCount; i++)
    {
        CompoundTag* itemTag = new CompoundTag();
        if (equipment[i]) equipment[i]->save(itemTag);
        eqList->add(itemTag);
    }
    tag->put(L"ArmorStandEquipment", eqList);

    if (isInvisible())    tag->putBoolean(L"Invisible",   true);
    tag->putBoolean(L"Small",        isSmall());
    tag->putBoolean(L"ShowArms",     isShowArms());
    tag->putBoolean(L"NoBasePlate",  isNoBasePlate());
    tag->putInt    (L"DisabledSlots",disabledSlots);
    if (isMarker()) tag->putBoolean(L"Marker", true);
    tag->putBoolean(L"NoGravity",    noPhysics);
    tag->put(L"Pose", writePose());
}


void ArmorStand::handleEntityEvent(byte id)
{
    if (id == 31)
    {
        int woodId = Tile::wood ? Tile::wood->id : 5;
        ePARTICLE_TYPE p = PARTICLE_TILECRACK(woodId, 0);
        for (int i = 0; i < 10; i++)
        {
            double xa = random->nextGaussian() * 0.01;
            double ya = -0.05;
            double za = random->nextGaussian() * 0.01;
            double px = x + (random->nextFloat() * 0.1f - 0.05f);
            double py = y + (bbHeight * 0.6f) + (random->nextFloat() * 0.2f);
            double pz = z + (random->nextFloat() * 0.1f - 0.05f);
            level->addParticle(p, px, py, pz, xa, ya, za);
        }
    }
    else if (id == 32) lastHit = (long long)tickCount;
    else LivingEntity::handleEntityEvent(id);
}