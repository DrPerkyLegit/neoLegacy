#include "stdafx.h"
#include "EntityTypeMap.h"

static const unordered_map<wstring, eINSTANCEOF> s_nameToType = {
    // animals
    { L"pig",              eTYPE_PIG              },
    { L"cow",              eTYPE_COW              },
    { L"sheep",            eTYPE_SHEEP            },
    { L"chicken",          eTYPE_CHICKEN          },
    { L"horse",            eTYPE_HORSE            },
    { L"wolf",             eTYPE_WOLF             },
    { L"ocelot",           eTYPE_OCELOT           },
    { L"rabbit",           eTYPE_RABBIT           },
    { L"mooshroom",        eTYPE_MUSHROOMCOW      },
    { L"squid",            eTYPE_SQUID            },
    { L"bat",              eTYPE_BAT              },
    // neutral/passive
    { L"villager",         eTYPE_VILLAGER         },
    { L"snowgolem",        eTYPE_SNOWMAN          },
    { L"irongolem",        eTYPE_VILLAGERGOLEM    },
    // monsters
    { L"zombie",           eTYPE_ZOMBIE           },
    { L"skeleton",         eTYPE_SKELETON         },
    { L"creeper",          eTYPE_CREEPER          },
    { L"spider",           eTYPE_SPIDER           },
    { L"cavespider",       eTYPE_CAVESPIDER       },
    { L"enderman",         eTYPE_ENDERMAN         },
    { L"silverfish",       eTYPE_SILVERFISH       },
    { L"blaze",            eTYPE_BLAZE            },
    { L"witch",            eTYPE_WITCH            },
    { L"ghast",            eTYPE_GHAST            },
    { L"slime",            eTYPE_SLIME            },
    { L"magmacube",        eTYPE_LAVASLIME        },
    { L"zombie_pigman",     eTYPE_PIGZOMBIE        },
    { L"witherboss",           eTYPE_WITHERBOSS       },
    { L"enderdragon",      eTYPE_ENDERDRAGON      },
    { L"giant",            eTYPE_GIANT            },
    { L"endermite",        eTYPE_ENDERMITE        },
    { L"guardian",         eTYPE_GUARDIAN         },
    { L"elder_guardian",    eTYPE_ELDER_GUARDIAN   },
    // minecart
    { L"minecart",         eTYPE_MINECART         },
    { L"minecart_chest",   eTYPE_MINECART_CHEST   },
    { L"minecart_hopper",  eTYPE_MINECART_HOPPER  },
    { L"minecart_tnt",     eTYPE_MINECART_TNT     },
    { L"minecart_furnace", eTYPE_MINECART_FURNACE },
    { L"minecart_spawner", eTYPE_MINECART_SPAWNER },
    // projectiles
    { L"arrow",            eTYPE_ARROW            },
    { L"snowball",         eTYPE_SNOWBALL         },
    { L"egg",              eTYPE_THROWNEGG        },
    { L"enderpearl",       eTYPE_THROWNENDERPEARL },
    { L"potion",           eTYPE_THROWNPOTION     },
    { L"expbottle",        eTYPE_THROWNEXPBOTTLE  },
    { L"large_fireball",         eTYPE_LARGE_FIREBALL   },
    { L"small_fireball",    eTYPE_SMALL_FIREBALL   },
    { L"wither_skull",      eTYPE_WITHER_SKULL     },
    { L"dragon_fireball",   eTYPE_DRAGON_FIREBALL  },
    { L"fireworks_rocket", eTYPE_FIREWORKS_ROCKET },
    { L"eyeofender",       eTYPE_EYEOFENDERSIGNAL },
    // hanging
    { L"painting",         eTYPE_PAINTING         },
    { L"item_frame",       eTYPE_ITEM_FRAME       },
    { L"leash_knot",       eTYPE_LEASHFENCEKNOT   },
    // others
    { L"item",             eTYPE_ITEMENTITY       },
    { L"xp_orb",           eTYPE_EXPERIENCEORB    },
    { L"boat",             eTYPE_BOAT             },
    { L"tnt",              eTYPE_PRIMEDTNT        },
    { L"falling_block",    eTYPE_FALLINGTILE      },
    { L"armor_stand",      eTYPE_ARMORSTAND       },
    { L"ender_crystal",    eTYPE_ENDER_CRYSTAL    },
    { L"lightning_bolt",   eTYPE_LIGHTNINGBOLT    },
};

static const unordered_map<eINSTANCEOF, wstring> s_typeToName = []()
{
    unordered_map<eINSTANCEOF, wstring> m;
    for (auto& pair : s_nameToType)
    {
        if (m.find(pair.second) == m.end())
            m[pair.second] = pair.first;
    }
    return m;
}();

const unordered_map<wstring, eINSTANCEOF>& EntityTypeMap::getNameToTypeMap()
{
    return s_nameToType;
}

const unordered_map<eINSTANCEOF, wstring>& EntityTypeMap::getTypeToNameMap()
{
    return s_typeToName;
}

eINSTANCEOF EntityTypeMap::getTypeFromName(const wstring& name)
{
    wstring lower = name;
    transform(lower.begin(), lower.end(), lower.begin(), towlower);
    auto it = s_nameToType.find(lower);
    if (it != s_nameToType.end())
        return it->second;
    return eTYPE_NOTSET;
}

wstring EntityTypeMap::getNameFromType(eINSTANCEOF type)
{
    auto it = s_typeToName.find(type);
    if (it != s_typeToName.end())
        return it->second;
    return L"";
}

bool EntityTypeMap::isValidType(const wstring& name)
{
    return getTypeFromName(name) != eTYPE_NOTSET;
}