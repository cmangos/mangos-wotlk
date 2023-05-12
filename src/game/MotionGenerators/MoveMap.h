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

#ifndef _MOVE_MAP_H
#define _MOVE_MAP_H

#include "Common.h"
#include <Detour/Include/DetourAlloc.h>
#include <Detour/Include/DetourNavMesh.h>
#include <Detour/Include/DetourNavMeshQuery.h>

#include <memory>
#include <mutex>

class Unit;

//  memory management
inline void* dtCustomAlloc(size_t size, dtAllocHint /*hint*/)
{
    return (void*)new unsigned char[size];
}

inline void dtCustomFree(void* ptr)
{
    delete[](unsigned char*)ptr;
}

//  move map related classes
namespace MMAP
{
    typedef std::unordered_map<uint32, dtTileRef> MMapTileSet;
    typedef std::unordered_map<uint32, dtNavMeshQuery*> NavMeshQuerySet;
    typedef std::unordered_map<std::thread::id, dtNavMeshQuery*> NavMeshGOQuerySet;

    // dummy struct to hold map's mmap data
    struct MMapData
    {
        MMapData(dtNavMesh* mesh) : navMesh(mesh) {}
        ~MMapData()
        {
            for (auto& navMeshQuerie : navMeshQueries)
                dtFreeNavMeshQuery(navMeshQuerie.second);

            if (navMesh)
                dtFreeNavMesh(navMesh);
        }

        dtNavMesh* navMesh;

        // we have to use single dtNavMeshQuery for every instance, since those are not thread safe
        NavMeshQuerySet navMeshQueries;     // instanceId to query
        MMapTileSet mmapLoadedTiles;        // maps [map grid coords] to [dtTile]
    };

    struct MMapGOData
    {
        MMapGOData(dtNavMesh* mesh) : navMesh(mesh) {}
        ~MMapGOData()
        {
            for (auto& navMeshQuerie : navMeshGOQueries)
                dtFreeNavMeshQuery(navMeshQuerie.second);

            if (navMesh)
                dtFreeNavMesh(navMesh);
        }

        dtNavMesh* navMesh;

        // we have to use single dtNavMeshQuery for every instance, since those are not thread safe
        NavMeshGOQuerySet navMeshGOQueries;  // instanceId to query
    };


    // singleton class
    // holds all access to mmap loading unloading and meshes
    class MMapManager
    {
        public:
            MMapManager() : m_loadedTiles(0) {}
            ~MMapManager();

            bool loadMap(uint32 mapId, uint32 instanceId, int32 x, int32 y, uint32 number);
            bool loadMapData(uint32 mapId, uint32 instanceId);
            void loadAllGameObjectModels(std::vector<uint32> const& displayIds);
            bool loadGameObject(uint32 displayId);
            bool unloadMap(uint32 mapId, uint32 instanceId, int32 x, int32 y);
            bool unloadMap(uint32 mapId);
            bool unloadMapInstance(uint32 mapId, uint32 instanceId);
            bool IsMMapTileLoaded(uint32 mapId, uint32 instanceId, uint32 x, uint32 y) const;

            // the returned [dtNavMeshQuery const*] is NOT threadsafe
            dtNavMeshQuery const* GetNavMeshQuery(uint32 mapId, uint32 instanceId);
            dtNavMeshQuery const* GetModelNavMeshQuery(uint32 displayId);
            dtNavMesh const* GetNavMesh(uint32 mapId, uint32 instanceId);
            dtNavMesh const* GetGONavMesh(uint32 displayId);

            uint32 getLoadedTilesCount() const { return m_loadedTiles; }
            uint32 getLoadedMapsCount() const { return m_loadedMMaps.size(); }

            void ChangeTile(uint32 mapId, uint32 instanceId, uint32 tileX, uint32 tileY, uint32 tileNumber);
        private:
            uint32 packTileID(int32 x, int32 y) const;
            uint64 packInstanceId(uint32 mapId, uint32 instanceId) const;

            std::unordered_map<uint64, std::unique_ptr<MMapData>> m_loadedMMaps;
            uint32 m_loadedTiles;

            std::unordered_map<uint32, std::unique_ptr<MMapGOData>> m_loadedModels;
            std::mutex m_modelsMutex;
    };

    // static class
    // holds all mmap global data
    // access point to MMapManager singleton
    class MMapFactory
    {
        public:
            static MMapManager* createOrGetMMapManager();
            static void clear();
            static void preventPathfindingOnMaps(const char* ignoreMapIds);
            static bool IsPathfindingEnabled(uint32 mapId, const Unit* unit);
            static bool IsPathfindingForceEnabled(const Unit* unit);
            static bool IsPathfindingForceDisabled(const Unit* unit);
    };
}

#endif  // _MOVE_MAP_H
