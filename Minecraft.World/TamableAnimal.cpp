#include "stdafx.h"
#include "net.minecraft.world.entity.ai.goal.h"
#include "net.minecraft.world.entity.h"
#include "net.minecraft.world.level.h"
#include "SynchedEntityData.h"
#include "ParticleTypes.h"
#include "TamableAnimal.h"

#include "AttributeInstance.h"
#include "BaseAttributeMap.h"
#include "SharedMonsterAttributes.h"

TamableAnimal::TamableAnimal(Level *level) : Animal(level)
{
	sitGoal = new SitGoal(this);
}

TamableAnimal::~TamableAnimal()
{
	if(sitGoal != nullptr) delete sitGoal;
}

void TamableAnimal::defineSynchedData()
{
	Animal::defineSynchedData();
	entityData->define(DATA_FLAGS_ID, static_cast<byte>(0));
	entityData->define(DATA_OWNERUUID_ID, L"");

	entityData->define(DATA_LEVEL, 1);
	entityData->define(DATA_LEVEL_EXP, 0);
}

void TamableAnimal::addAdditonalSaveData(CompoundTag *tag)
{
	Animal::addAdditonalSaveData(tag);
#ifdef _XBOX_ONE
	// 4J Stu Added from later Java version to remove owners from save transfer saves. We will probably want this on other platforms in the future
	if (getOwnerUUID().empty())
	{
		tag->putString(L"OwnerUUID", L"");
	}
	else
	{
		tag->putString(L"OwnerUUID", getOwnerUUID());
	}
#else
	if (getOwnerUUID().empty())
	{
		tag->putString(L"Owner", L"");
	}
	else
	{
		tag->putString(L"Owner", getOwnerUUID());
	}
#endif
	tag->putBoolean(L"Sitting", isSitting());

	tag->putInt(L"TameLevel", getLevel());
}

void TamableAnimal::readAdditionalSaveData(CompoundTag *tag)
{
	Animal::readAdditionalSaveData(tag);
#ifdef _XBOX_ONE
	// 4J Stu Added from later Java version to remove owners from save transfer saves. We will probably want this on other platforms in the future
	wstring owner = L"";
	if(tag->contains(L"OwnerUUID") )
	{
		owner = tag->getString(L"OwnerUUID");
	}
#else
	wstring owner = tag->getString(L"Owner");
#endif
	if (owner.length() > 0)
	{
		setOwnerUUID(owner);
		setTame(true);
	}
	sitGoal->wantToSit(tag->getBoolean(L"Sitting"));
	setSitting(tag->getBoolean(L"Sitting"));

	setLevel(tag->getInt(L"TameLevel"));
}

void TamableAnimal::registerAttributes() {
	getAttributes()->registerAttribute(SharedMonsterAttributes::ATTACK_DAMAGE)->setBaseValue(1);
}

void TamableAnimal::spawnTamingParticles(bool success)
{
	ePARTICLE_TYPE particle = eParticleType_heart;
	if (!success)
	{
		particle = eParticleType_smoke;
	}
	for (int i = 0; i < 7; i++)
	{
		double xa = random->nextGaussian() * 0.02;
		double ya = random->nextGaussian() * 0.02;
		double za = random->nextGaussian() * 0.02;
		level->addParticle(particle, x + random->nextFloat() * bbWidth * 2 - bbWidth, y + .5f + random->nextFloat() * bbHeight, z + random->nextFloat() * bbWidth * 2 - bbWidth, xa, ya, za);
	}
}

void TamableAnimal::handleEntityEvent(byte id)
{
	if (id == EntityEvent::TAMING_SUCCEEDED)
	{
		spawnTamingParticles(true);
	}
	else if (id == EntityEvent::TAMING_FAILED)
	{
		spawnTamingParticles(false);
	}
	else
	{
		Animal::handleEntityEvent(id);
	}
}

bool TamableAnimal::isTame()
{
	return (entityData->getByte(DATA_FLAGS_ID) & 0x04) != 0;
}

void TamableAnimal::setTame(bool value)
{
	byte current = entityData->getByte(DATA_FLAGS_ID);
	if (value)
	{
		entityData->set(DATA_FLAGS_ID, static_cast<byte>(current | 0x04));
	}
	else
	{
		entityData->set(DATA_FLAGS_ID, static_cast<byte>(current & ~0x04));
	}
}

bool TamableAnimal::isSitting()
{
	return (entityData->getByte(DATA_FLAGS_ID) & 0x01) != 0;
}

void TamableAnimal::setSitting(bool value)
{
	byte current = entityData->getByte(DATA_FLAGS_ID);
	if (value)
	{
		entityData->set(DATA_FLAGS_ID, static_cast<byte>(current | 0x01));
	}
	else
	{
		entityData->set(DATA_FLAGS_ID, static_cast<byte>(current & ~0x01));
	}
}

wstring TamableAnimal::getOwnerUUID()
{
	return entityData->getString(DATA_OWNERUUID_ID);
}

void TamableAnimal::setOwnerUUID(const wstring &name)
{
	entityData->set(DATA_OWNERUUID_ID, name);
}

shared_ptr<Entity> TamableAnimal::getOwner()
{
	return level->getPlayerByUUID(getOwnerUUID());
}

SitGoal *TamableAnimal::getSitGoal()
{
	return sitGoal;
}

bool TamableAnimal::wantsToAttack(shared_ptr<LivingEntity> target, shared_ptr<LivingEntity> owner)
{
	return true;
}

Team *TamableAnimal::getTeam()
{
	if (isTame())
	{
		shared_ptr<LivingEntity> owner = dynamic_pointer_cast<LivingEntity>(getOwner());
		if (owner != nullptr)
		{
			return owner->getTeam();
		}
	}
	return Animal::getTeam();
}

bool TamableAnimal::isAlliedTo(shared_ptr<LivingEntity> other)
{
	if (isTame())
	{
		shared_ptr<LivingEntity> owner = dynamic_pointer_cast<LivingEntity>(getOwner());
		if (other == owner)
		{
			return true;
		}
		if (owner != nullptr)
		{
			return owner->isAlliedTo(other);
		}
	}
	return Animal::isAlliedTo(other);
}

int TamableAnimal::getHealthCap() {
	return 20;
}

int TamableAnimal::getAttackCap() {
	return 1;
}

int TamableAnimal::getMovementCap() {
	return 0.3f;
}

void TamableAnimal::setStatsFromPair(shared_ptr<TamableAnimal> parent1, shared_ptr<TamableAnimal> parent2) {
	{
		float finalHealth = getMaxHealth();

		if (random->nextFloat() < 0.5f) {
			finalHealth = parent1->getMaxHealth();
		} else {
			finalHealth = parent2->getMaxHealth();
		}

		if (random->nextFloat() < 0.15f) {
			
			if (random->nextFloat() < 0.05f) {
				int randomValue = random->nextInt(10);
				finalHealth += max(randomValue, 4);
			} else {
				finalHealth += random->nextInt(5);
			}
		}
		
		getAttribute(SharedMonsterAttributes::MAX_HEALTH)->setBaseValue(min(finalHealth, getHealthCap()));
	}

	{
		float finalSpeed = getAttribute(SharedMonsterAttributes::MOVEMENT_SPEED)->getBaseValue();

		if (random->nextFloat() < 0.5f) {
			finalSpeed = parent1->getAttribute(SharedMonsterAttributes::MOVEMENT_SPEED)->getBaseValue();
		} else {
			finalSpeed = parent2->getAttribute(SharedMonsterAttributes::MOVEMENT_SPEED)->getBaseValue();
		}

		if (random->nextFloat() < 0.10f) {

			if (random->nextFloat() < 0.02f) {
				finalSpeed += random->nextFloat() * 2;
			} else {
				finalSpeed += random->nextFloat() / 2;
			}
		}

		getAttribute(SharedMonsterAttributes::MOVEMENT_SPEED)->setBaseValue(min(finalSpeed, getMovementCap()));
	}

	{
		int finalDamage = getAttribute(SharedMonsterAttributes::ATTACK_DAMAGE)->getBaseValue();

		if (random->nextFloat() < 0.5f) {
			finalDamage = parent1->getAttribute(SharedMonsterAttributes::ATTACK_DAMAGE)->getBaseValue();
		} else {
			finalDamage = parent2->getAttribute(SharedMonsterAttributes::ATTACK_DAMAGE)->getBaseValue();
		}

		if (random->nextFloat() < 0.08f) {
			if (random->nextFloat() < 0.02f) {
				int randomValue = random->nextInt(5);
				finalDamage += max(randomValue, 2);
			} else {
				finalDamage += random->nextInt(2);
			}
		}

		getAttribute(SharedMonsterAttributes::ATTACK_DAMAGE)->setBaseValue(min(finalDamage, getAttackCap()));
	}

	{
		int parent1Level = parent1->getLevel();
		int parent2Level = parent2->getLevel();

		int higher = max(parent1Level, parent2Level);
		int lower = min(parent1Level, parent2Level);

		int finalLevel = 0;
		
		if (random->nextFloat() < 0.6f) {
			finalLevel = higher;
		} else {
			finalLevel = lower;
		}

		float mutationRoll = random->nextFloat();

		if (mutationRoll < 0.025) {
			int randomValue = random->nextInt(10);
			finalLevel += max(randomValue, 2);
		} else if (mutationRoll < 0.1f) {
			finalLevel += 2;
		}

		setLevel(finalLevel);
	}
}

int TamableAnimal::getLevel() {
	return entityData->getInteger(DATA_LEVEL);
}

void TamableAnimal::setLevel(int level) {
	entityData->set(DATA_LEVEL, level);
}

void TamableAnimal::updateNametag(wstring name) {
	//setCustomName(L"\u00a7f" + name + L"\u00a72 Level " + std::to_wstring(getLevel()) + L"\u00a7r");
	//setCustomNameVisible(true);
}
