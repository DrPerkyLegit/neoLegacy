#include "stdafx.h"
#include "net.minecraft.commands.h"
#include "net.minecraft.world.entity.player.h"
#include "net.minecraft.world.damagesource.h"
#include "net.minecraft.world.level.h"
#include "BasicTypeContainers.h"
#include "KillCommand.h"
#include "EntityTypeMap.h"


static void killEntity(shared_ptr<Entity> entity)
{
    if (entity->instanceof(eTYPE_LIVINGENTITY))
    {
        auto living = dynamic_pointer_cast<LivingEntity>(entity);
        if (living != nullptr)
        {
            living->hurt(DamageSource::outOfWorld, Float::MAX_VALUE);
            return;
        }

        entity->remove();
        return;  
    }
    entity->remove();
}

EGameCommand KillCommand::getId()
{
    return eGameCommand_Kill;
}

int KillCommand::getPermissionLevel()
{
    return LEVEL_ALL;
}

void KillCommand::execute(shared_ptr<CommandSender> source, byteArray commandData)
{
    shared_ptr<Player> senderPlayer = dynamic_pointer_cast<Player>(source);
    if (senderPlayer == nullptr)
        return;

    Level *level = senderPlayer->level;
    if (level == nullptr)
        return;


    //nothing is the same of @s
    if (commandData.length == 0 || commandData.data == nullptr)
    {
        senderPlayer->hurt(DamageSource::outOfWorld, Float::MAX_VALUE);
        return;
    }

    ByteArrayInputStream bais(commandData);
    DataInputStream dis(&bais);
    wstring targetName = dis.readUTF();

    wstring targetLower = targetName;
    transform(targetLower.begin(), targetLower.end(), targetLower.begin(), towlower);

    //@s
    if (targetLower == L"@s")
    {
        senderPlayer->hurt(DamageSource::outOfWorld, Float::MAX_VALUE);
        return;
    }

    //@a
    if (targetLower == L"@a")
    {
        vector<shared_ptr<Player>> toKill;
        for (auto& p : level->players)
            if (p != nullptr && !p->removed)
                toKill.push_back(p);
        for (auto& p : toKill)
            p->hurt(DamageSource::outOfWorld, Float::MAX_VALUE);
        return;
    }

    //@p
    if (targetLower == L"@p")
    {
        shared_ptr<Player> nearest = level->getNearestPlayer(
            dynamic_pointer_cast<Entity>(senderPlayer), 9999.0);
        if (nearest != nullptr)
        {
            nearest->hurt(DamageSource::outOfWorld, Float::MAX_VALUE);
            source->sendMessage(L"Killed " + nearest->getName() + L".");
        }
        else
        {
            source->sendMessage(L"No player found.");
        }
        return;
    }

    //@r
    if (targetLower == L"@r")
    {
        if (level->players.empty())
        {
            source->sendMessage(L"No player found.");
            return;
        }
        int idx = rand() % (int)level->players.size();
        auto& p = level->players[idx];
        if (p != nullptr && !p->removed)
        {
            p->hurt(DamageSource::outOfWorld, Float::MAX_VALUE);
            source->sendMessage(L"Killed " + p->getName() + L".");
        }
        return;
    }

    // @e
    if (targetLower.size() >= 2 && targetLower.substr(0, 2) == L"@e")
    {
        eINSTANCEOF filterType = eTYPE_NOTSET;
        bool invertFilter      = false;
        bool filterIsPlayer    = false;

        if (targetLower.size() > 3 && targetLower[2] == L'[')
        {
            wstring inner = targetLower.substr(3, targetLower.size() - 4);
            wstring key   = L"type=";
            size_t  pos   = inner.find(key);
            if (pos == wstring::npos)
            {
                source->sendMessage(L"Invalid selector syntax. Usage: @e[type=<entity_type>]");
                return;
            }

            wstring typeStr = inner.substr(pos + key.size());
            if (!typeStr.empty() && typeStr[0] == L'!')
            {
                invertFilter = true;
                typeStr      = typeStr.substr(1);
            }

            if (typeStr == L"player")
            {
                filterIsPlayer = true;
            }
            else
            {
                filterType = EntityTypeMap::getTypeFromName(typeStr);
                if (filterType == eTYPE_NOTSET)
                {
                    source->sendMessage(L"Unknown entity type: " + typeStr);
                    return;
                }
            }
        }

        vector<shared_ptr<Entity>> toKill;
        for (auto& entity : level->entities)
        {
            if (entity == nullptr || entity->removed) continue;

            bool isPlayer = entity->instanceof(eTYPE_PLAYER);
                

            bool matchesFilter;
            if (filterType == eTYPE_NOTSET && !filterIsPlayer)
                matchesFilter = true;
            else if (filterIsPlayer)
                matchesFilter = isPlayer;
            else
                matchesFilter = entity->instanceof(filterType);

            if (invertFilter) matchesFilter = !matchesFilter;

            if (matchesFilter)
                toKill.push_back(entity);
        }

        for (auto& entity : toKill)
            if (!entity->removed)
                killEntity(entity);

        source->sendMessage(L"Killed " + to_wstring(toKill.size()) + L" entities.");
        return;
    }

    // by player name
    shared_ptr<Player> targetPlayer = level->getPlayerByName(targetName);
    if (targetPlayer != nullptr)
    {
        targetPlayer->hurt(DamageSource::outOfWorld, Float::MAX_VALUE);
        source->sendMessage(L"Killed " + targetName + L".");
        return;
    }

    source->sendMessage(L"No entity was found.");
}