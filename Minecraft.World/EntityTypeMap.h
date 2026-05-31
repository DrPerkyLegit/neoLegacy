#pragma once
#include "../Minecraft.World/Class.h"
#include <unordered_map>
#include <string>
using namespace std;

class EntityTypeMap
{
public:

    static eINSTANCEOF getTypeFromName(const wstring& name);

    static wstring getNameFromType(eINSTANCEOF type);


    static bool isValidType(const wstring& name);

private:
    static const unordered_map<wstring, eINSTANCEOF>& getNameToTypeMap();
    static const unordered_map<eINSTANCEOF, wstring>& getTypeToNameMap();
};