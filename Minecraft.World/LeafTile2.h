#pragma once

#include "LeafTile.h"

class IconRegister;
class LeafTile2 : public LeafTile
{
public:
	static const int ACACIA_LEAF = 0;
	static const int DARK_OAK_LEAF = 1;

	static const int LEAF2_NAMES_SIZE = 2;
	static const unsigned int LEAF2_NAMES[LEAF2_NAMES_SIZE];

private:
	//index 0, fancy;  index 1, fast
	static const wstring TEXTURES[2][2];
	Icon *icons[2][2];

public:
	LeafTile2(int id);

	virtual Icon *getTexture(int face, int data);
	virtual unsigned int getDescriptionId(int iData = -1);
	virtual void registerIcons(IconRegister *iconRegister);
	virtual void spawnResources(Level *level, int x, int y, int z, int data, float odds, int playerBonusLevel) override;
	
	
	virtual int getColor(int data);
	virtual int getColor(LevelSource *level, int x, int y, int z, int data);

	
	virtual void playerDestroy(Level *level, shared_ptr<Player> player, int x, int y, int z, int data);
};