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

#include "VMapFactory.h"
#include "VMapManager2.h"
#include "VMapDefinitions.h"
#include "WorldModel.h"

#include "Entities/GameObject.h"
#include "World/World.h"
#include "Vmap/GameObjectModel.h"
#include "Server/DBCStores.h"
#include "ModelInstance.h"
#include "Vmap/GameObjectModelVmaps.h"
#include "MotionGenerators/MoveMapSharedDefines.h"

ModelList modelList;
std::map<std::pair<uint32, uint32>, std::vector<std::pair<uint32, uint32>>> tilesPerGoEntryInMap;
std::map<std::tuple<uint32, uint32, uint32, uint32, uint32>, std::pair<bool, uint32>> goDisplayIdTileData;

GameObjectModel::~GameObjectModel()
{
    if (iModel)
        ((VMAP::VMapManager2*)VMAP::VMapFactory::createOrGetVMapManager())->releaseModelInstance(name);
}

bool GameObjectModel::initialize(const GameObject* const pGo, const GameObjectDisplayInfoEntry* const pDisplayInfo)
{
    ModelList::const_iterator it = modelList.find(pDisplayInfo->Displayid);
    if (it == modelList.end())
        return false;

    G3D::AABox mdl_box(it->second.bound);
    // ignore models with no bounds
    if (mdl_box == G3D::AABox::zero())
    {
        sLog.outDebug("Model %s has zero bounds, loading skipped", it->second.name.c_str());
        return false;
    }

    iModel = ((VMAP::VMapManager2*)VMAP::VMapFactory::createOrGetVMapManager())->acquireModelInstance(sWorld.GetDataPath() + "vmaps/", it->second.name);

    if (!iModel)
        return false;

    if (it->second.name.find(".m2") != std::string::npos)
        iModel->setModelFlags(VMAP::MOD_M2);

    name = it->second.name;
    iPos = Vector3(pGo->GetPositionX(), pGo->GetPositionY(), pGo->GetPositionZ());
    phasemask = pGo->GetPhaseMask();
    iScale = pGo->GetObjectScale();
    iInvScale = 1.f / iScale;

    G3D::Matrix3 iRotation = G3D::Matrix3::fromEulerAnglesZYX(pGo->GetOrientation(), 0, 0);
    iInvRot = iRotation.inverse();
    // transform bounding box:
    mdl_box = AABox(mdl_box.low() * iScale, mdl_box.high() * iScale);
    AABox rotated_bounds;
    for (int i = 0; i < 8; ++i)
        rotated_bounds.merge(iRotation * mdl_box.corner(i));

    this->iBound = rotated_bounds + iPos;

#ifdef SPAWN_CORNERS
    // test:
    for (int i = 0; i < 8; ++i)
    {
        Vector3 pos(iBound.corner(i));
        if (Creature* c = const_cast<GameObject*>(pGo)->SummonCreature(24440, pos.x, pos.y, pos.z, 0, TEMPSPAWN_MANUAL_DESPAWN, 0))
        {
            c->setFaction(35);
            c->SetObjectScale(0.1f);
        }
    }
#endif

    return true;
}

GameObjectModel* GameObjectModel::construct(const GameObject* const pGo)
{
    const GameObjectDisplayInfoEntry* info = sGameObjectDisplayInfoStore.LookupEntry(pGo->GetDisplayId());
    if (!info)
        return nullptr;

    GameObjectModel* mdl = new GameObjectModel();
    if (!mdl->initialize(pGo, info))
    {
        delete mdl;
        return nullptr;
    }

    return mdl;
}

bool GameObjectModel::intersectRay(const G3D::Ray& ray, float& MaxDist, bool StopAtFirstHit, uint32 phaseMask, bool ignoreM2Model) const
{
    if (!(phasemask & phaseMask))
        return false;

    float time = ray.intersectionTime(iBound);
    if (time == G3D::inf())
        return false;

    // child bounds are defined in object space:
    Vector3 p = iInvRot * (ray.origin() - iPos) * iInvScale;
    Ray modRay(p, iInvRot * ray.direction());
    float distance = MaxDist * iInvScale;
    bool hit = iModel->IntersectRay(modRay, distance, StopAtFirstHit, ignoreM2Model);
    if (hit)
    {
        distance *= iScale;
        MaxDist = distance;
    }
    return hit;
}

bool GameObjectModel::Relocate(GameObject const& go)
{
    if (!iModel)
        return false;

    ModelList::const_iterator it = modelList.find(go.GetDisplayId());
    if (it == modelList.end())
        return false;

    G3D::AABox mdl_box(it->second.bound);
    // ignore models with no bounds
    if (mdl_box == G3D::AABox::zero())
    {
        DEBUG_LOG("GameObject model %s has zero bounds, loading skipped", it->second.name.c_str());
        return false;
    }

    iPos = Vector3(go.GetPositionX(), go.GetPositionY(), go.GetPositionZ());

    G3D::Matrix3 iRotation = G3D::Matrix3::fromEulerAnglesZYX(go.GetOrientation(), 0, 0);
    iInvRot = iRotation.inverse();
    // transform bounding box:
    mdl_box = AABox(mdl_box.low() * iScale, mdl_box.high() * iScale);
    AABox rotated_bounds;
    for (int i = 0; i < 8; ++i)
        rotated_bounds.merge(iRotation * mdl_box.corner(i));

    iBound = rotated_bounds + iPos;
#ifdef SPAWN_CORNERS
    // test:
    for (int i = 0; i < 8; ++i)
    {
        Vector3 pos(iBound.corner(i));
        Creature* c = ((GameObject*)&go)->SummonCreature(1, pos.x, pos.y, pos.z, 0, TEMPSUMMON_TIMED_DESPAWN, 4000);
        c->SetFly(true);
        c->SendHeartBeat();
    }
#endif
    return true;
}

void GameObjectModel::LoadGOVmapModels()
{
    modelList = GameobjectModelData::LoadGameObjectModelList(sWorld.GetDataPath() + "vmaps/" + VMAP::GAMEOBJECT_MODELS);
    std::set<std::tuple<uint32, uint32, uint32>> usedTilesPerMap;
    for (auto& data : BuildingMap)
    {
        uint32 mapId = data.first;
        for (TileBuilding& building : data.second)
        {
            ModelList::const_iterator itr = modelList.find(building.displayId);
            if (itr == modelList.end())
                continue;

            uint32 lowX, lowY, highX, highY;
            std::tie(lowX, lowY, highX, highY) = GameobjectModelData::CalculateBuildingTiles(building, itr->second.bound);
            for (uint32 x = lowX; x <= highX; ++x)
            {
                for (uint32 y = lowY; y <= highY; ++y)
                {
                    usedTilesPerMap.insert(std::make_tuple(mapId, x, y));
                    tilesPerGoEntryInMap[std::make_pair(mapId, building.goEntry)].push_back(std::make_pair(x, y));
                }
            }
        }
    }

    for (auto& mapTileData : usedTilesPerMap)
    {
        uint32 mapId, tileX, tileY;
        std::tie(mapId, tileX, tileY) = mapTileData;
        std::vector<TileBuilding const*> buildingsByDefault;
        std::map<uint32, std::vector<TileBuilding const*>> buildingsInTile;
        std::map<uint32, std::vector<TileBuilding const*>> buildingsByGroup;
        std::map<uint32, uint32> flagToGroup;

        // yes I am reusing a function that is computationally expensive - but we do it for a trivial amount of tiles
        std::tie(buildingsByDefault, buildingsInTile, buildingsByGroup, flagToGroup) = GameobjectModelData::GetTileBuildingData(mapId, tileX, tileY, modelList);
        // case of tile numbers - no flag usage
        for (auto& data : buildingsInTile)
            for (auto& building : data.second)
                goDisplayIdTileData[std::make_tuple(mapId, building->goEntry, building->displayId, tileX, tileY)] = std::make_pair(false, data.first);

        for (auto& data : buildingsByGroup)
            for (auto& building : data.second)
                goDisplayIdTileData[std::make_tuple(mapId, building->goEntry, building->displayId, tileX, tileY)] = std::make_pair(true, (1 << (data.first - 1)));
    }
}

std::vector<std::pair<uint32, uint32>> GameObjectModel::GetTilesForGOEntry(uint32 mapId, uint32 goEntry)
{
    return tilesPerGoEntryInMap[std::make_pair(mapId, goEntry)];
}

std::pair<bool, uint32> GameObjectModel::GetTileDataForGoDisplayId(uint32 mapId, uint32 goEntry, uint32 displayId, uint32 tileX, uint32 tileY)
{
    return goDisplayIdTileData[std::make_tuple(mapId, goEntry, displayId, tileX, tileY)];
}
