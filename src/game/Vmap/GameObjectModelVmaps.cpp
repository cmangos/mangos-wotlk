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

#include "Vmap/GameObjectModelVmaps.h"
#include "Vmap/WorldModel.h"
#include "MotionGenerators/MoveMapSharedDefines.h"
#include "Vmap/VMapDefinitions.h"
#include <G3D/Quat.h>
#include <G3D/Vector3.h>

ModelList GameobjectModelData::LoadGameObjectModelList(std::string fileName)
{
    ModelList model_list;
    FILE* model_list_file = fopen(fileName.c_str(), "rb");
    if (!model_list_file)
        return model_list;

    uint32 name_length, displayId;
    char buff[500];
    while (!feof(model_list_file))
    {
        fread(&displayId, sizeof(uint32), 1, model_list_file);
        fread(&name_length, sizeof(uint32), 1, model_list_file);

        if (name_length >= sizeof(buff))
        {
            // sLog.outDebug("File %s seems to be corrupted", VMAP::GAMEOBJECT_MODELS);
            break;
        }

        fread(&buff, sizeof(char), name_length, model_list_file);
        G3D::Vector3 v1, v2;
        fread(&v1, sizeof(G3D::Vector3), 1, model_list_file);
        fread(&v2, sizeof(G3D::Vector3), 1, model_list_file);

        model_list.emplace(displayId, GameobjectModelData(std::string(buff, name_length), G3D::AABox(v1, v2)));
    }
    fclose(model_list_file);
    return model_list;
}

static const float GRID_SIZE = 533.33333f;

// warning - mmaps use swapped tileX and tileY than recastdemo
std::tuple<std::vector<TileBuilding const*>, std::map<uint32, std::vector<TileBuilding const*>>, std::map<uint32, std::vector<TileBuilding const*>>, std::map<uint32, uint32>> GameobjectModelData::GetTileBuildingData(uint32 mapId, uint32 tileX, uint32 tileY, ModelList& modelList)
{
    std::vector<TileBuilding const*> buildingsByDefault;
    std::map<uint32, std::vector<TileBuilding const*>> buildingsInTile;
    std::map<uint32, std::vector<TileBuilding const*>> buildingsByGroup;
    std::map<uint32, uint32> flagToGroup;

    auto itr = BuildingMap.find(mapId);
    if (itr != BuildingMap.end()) // building GO
    {
        uint32 i = 0;
        for (TileBuilding& data : itr->second)
        {
            auto itr = modelList.find(data.displayId);
            if (itr == modelList.end())
                continue;

            uint32 lowX, lowY, highX, highY;
            std::tie(lowX, lowY, highX, highY) = CalculateBuildingTiles(data, itr->second.bound);

            if (lowX <= tileX && lowY <= tileY && highX >= tileX && highY >= tileY)
            {
                if (data.byDefault)
                    buildingsByDefault.push_back(&data);
                else if (!data.tileFlags)
                    buildingsInTile[data.tileNumber].push_back(&data);
                else
                {
                    uint32 chosenGroup = 0;
                    auto itrFlags = flagToGroup.find(data.tileNumber);
                    if (data.tileNumber > 0 && itrFlags != flagToGroup.end())
                        chosenGroup = itrFlags->second;
                    else
                    {
                        chosenGroup = i;
                        ++i;
                    }

                    buildingsByGroup[chosenGroup].push_back(&data);
                    if (data.tileNumber)
                        flagToGroup[data.tileNumber] = chosenGroup;
                }
            }
        }
    }

    return { buildingsByDefault, buildingsInTile, buildingsByGroup, flagToGroup };
}

std::tuple<uint32, uint32, uint32, uint32> GameobjectModelData::CalculateBuildingTiles(TileBuilding const& building, G3D::AABox mdl_box)
{
    G3D::Matrix3 iRotation = G3D::Matrix3::fromEulerAnglesZYX(building.ori, 0, 0);
    G3D::Vector3 pos(building.x, building.y, building.z);
    // transform bounding box:
    mdl_box = G3D::AABox(mdl_box.low() * 1, mdl_box.high() * 1);
    G3D::AABox rotated_bounds;
    for (uint32 i = 0; i < 8; ++i)
        rotated_bounds.merge(iRotation * mdl_box.corner(i));

    G3D::AABox bounds = rotated_bounds + pos;

    uint32 lowX = 32 - bounds.high().y / GRID_SIZE;
    uint32 lowY = 32 - bounds.high().x / GRID_SIZE;
    uint32 highX = 32 - bounds.low().y / GRID_SIZE;
    uint32 highY = 32 - bounds.low().x / GRID_SIZE;
    return { lowX, lowY, highX, highY };
}
