/*
 * This file is part of the CMaNGOS Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _MOVE_MAP_SHARED_DEFINES_H
#define _MOVE_MAP_SHARED_DEFINES_H

#include "Platform/Define.h"
#include <Detour/Include/DetourNavMesh.h>
#include <map>
#include <string>
#include <vector>

#define MMAP_MAGIC 0x4d4d4150   // 'MMAP'
#define MMAP_VERSION 8

struct MmapTileHeader
{
    uint32 mmapMagic;
    uint32 dtVersion;
    uint32 mmapVersion;
    uint32 size;
    uint32 usesLiquids;

    MmapTileHeader() : mmapMagic(MMAP_MAGIC), dtVersion(DT_NAVMESH_VERSION),
        mmapVersion(MMAP_VERSION), size(0), usesLiquids(0) {}
};

enum NavArea
{
    NAV_AREA_EMPTY          = 0,
    // areas 1-60 will be used for destructible areas (currently skipped in vmaps, WMO with flag 1)
    // ground is the highest value to make recast choose ground over water when merging surfaces very close to each other (shallow water would be walkable)
    NAV_AREA_GROUND         = 11,
    NAV_AREA_GROUND_STEEP   = 10, // unused in cmangos - keeping it for structure
    NAV_AREA_WATER          = 9,
    NAV_AREA_MAGMA_SLIME    = 8, // don't need to differentiate between them
    NAV_AREA_GO_1           = 7,
    NAV_AREA_MAX_VALUE      = NAV_AREA_GROUND,
    NAV_AREA_MIN_VALUE      = NAV_AREA_MAGMA_SLIME,
    NAV_AREA_ALL_MASK       = 0x3F // max allowed value
};

enum NavTerrainFlag
{
    NAV_EMPTY        = 0x00,
    NAV_GROUND       = 1 << (NAV_AREA_MAX_VALUE - NAV_AREA_GROUND),
    NAV_GROUND_STEEP = 1 << (NAV_AREA_MAX_VALUE - NAV_AREA_GROUND_STEEP),
    NAV_WATER        = 1 << (NAV_AREA_MAX_VALUE - NAV_AREA_WATER),
    NAV_MAGMA_SLIME  = 1 << (NAV_AREA_MAX_VALUE - NAV_AREA_MAGMA_SLIME),
    NAV_GO_1         = 1 << (NAV_AREA_MAX_VALUE - NAV_AREA_GO_1)
};

struct TileBuilding
{
    std::string modelName;
    double x, y, z, ori;
    double qx, qy, qz, qw;
    uint32 displayId;
    bool byDefault;
    bool tileFlags; // same flags are aggregated as one combination
    uint32 tileNumber;
    uint32 goEntry; // means they are exclusive to each other

    TileBuilding(std::string modelName, double x, double y, double z, double ori, double qx, double qy, double qz, double qw, uint32 displayId, bool byDefault, bool tileFlags, uint32 tileNumber, uint32 goEntry) :
        modelName(modelName), x(x), y(y), z(z), ori(ori), qx(qx), qy(qy), qz(qz), qw(qw), displayId(displayId), byDefault(byDefault), tileFlags(tileFlags), tileNumber(tileNumber), goEntry(goEntry) {}
};

typedef std::map<uint32, std::vector<TileBuilding>> TileBuildings;
// Map containing pathfindable GOs all over the world. After adding entry, need to rebuild mmaps and add their usage through Map::ChangeGOPathfinding
static TileBuildings BuildingMap =
{
    {649u, {TileBuilding(std::string("Coliseum_Intact_Floor.wmo.vmo"), 563.53472900390625, 177.3090362548828125, 398.5718994140625, 3.14159265358979323846 / 2, 0, 0, 0, 0, 9059, false, false, 01, 195527)}},
    {603u, {TileBuilding(std::string("Ul_Ulduar_Trapdoor_02.m2.vmo"), 1805.630126953125, -23.5202598571777343, 451.225433349609375, 0, 0, 0, 0, 0, 8546, false, false, 01, 194232)}},
    {616u, {TileBuilding(std::string("Nexus_Raid_Floating_Platform.wmo.vmo"), 754.34552001953125, 1300.8697509765625, 256.24853515625, 3.141592741012573242, 0, 0, 0, 0, 8387, false, false, 01, 193070)}},
    {631u,
    {TileBuilding(std::string("Icecrownraid_Arthas_Precipice.wmo.vmo"), 503.619781494140625, -2124.654541015625, 836.60699462890625, 3.141592741012573242, 0, 0, 0, 0, 9256, true, true, 0, 202078),
    TileBuilding(std::string("Icecrownraid_Arthas_Precipice_Phase0.wmo.vmo"), 503.619781494140625, -2124.654541015625, 836.60699462890625, 3.141592741012573242, 0, 0, 0, 0, 9276, false, true, 0, 202161),
    TileBuilding(std::string("Icecrownraid_Arthas_Precipice_Phase1.wmo.vmo"), 503.619781494140625, -2124.654541015625, 836.60699462890625, 3.141592741012573242, 0, 0, 0, 0, 9276, false, true, 0, 202161),
    TileBuilding(std::string("Icecrownraid_Arthas_Precipice_Phase2.wmo.vmo"), 503.619781494140625, -2124.654541015625, 836.60699462890625, 3.141592741012573242, 0, 0, 0, 0, 9276, false, true, 0, 202161),
    TileBuilding(std::string("Iceshard_Standing.m2.vmo"), 473.7476806640625, -2096.47705078125, 840.85699462890625, 3.141592741012573242, 0, 0, 0, 0, 9227, false, true, 0x1, 202141),
    TileBuilding(std::string("Iceshard_Standing.m2.vmo"), 473.748291015625, -2152.832275390625, 840.85699462890625, -1.57079577445983886, 0, 0, 0, 0, 9227, false, true, 0x1, 202142),
    TileBuilding(std::string("Iceshard_Standing.m2.vmo"), 533.56048583984375, -2152.831298828125, 840.85699462890625, 0, 0, 0, 0, 0, 9227, false, true, 0x1, 202143),
    TileBuilding(std::string("Iceshard_Standing.m2.vmo"), 533.55987548828125, -2096.47607421875, 840.85699462890625, 1.570795774459838867, 0, 0, 0, 0, 9227, false, true, 0x1, 202144)}}
};

#ifdef MMAP_GENERATOR

namespace MMAP
{
    struct MeshData;

    extern void AddBuildingToMeshData(TileBuilding const* building, MeshData& meshData, char const* workDir = ".");
}

#endif

#endif  // _MOVE_MAP_SHARED_DEFINES_H
