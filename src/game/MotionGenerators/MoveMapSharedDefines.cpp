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

#include "MotionGenerators/MoveMapSharedDefines.h"

#ifdef MMAP_GENERATOR // TODO: Implement real time mmap gen in core

#include "../../contrib/mmap/src/TerrainBuilder.h"
#include "Vmap/WorldModel.h"
#include <G3D/Quat.h>
#include <G3D/Vector3.h>

namespace MMAP
{
    void AddBuildingToMeshData(TileBuilding const* building, MMAP::MeshData& meshData, char const* workDir)
    {
        VMAP::WorldModel m;
        std:string fullName(workDir);
        fullName += "/vmaps/";
        fullName += building->modelName;
        if (!m.readFile(fullName))
        {
            printf("* Unable to open file\n");
            return;
        }

        // Load model data into navmesh
        std::vector<VMAP::GroupModel> groupModels;
        m.getGroupModels(groupModels);

        // all M2s need to have triangle indices reversed
        bool isM2 = building->modelName.find(".m2") != building->modelName.npos || building->modelName.find(".M2") != building->modelName.npos;

        G3D::Vector3 pos(building->x, building->y, building->z);
        G3D::Quat rot(0, 0, sin(building->ori / 2), cos(building->ori / 2));
        G3D::Matrix3 matrix = rot.toRotationMatrix();
        for (std::vector<VMAP::GroupModel>::iterator it = groupModels.begin(); it != groupModels.end(); ++it)
        {
            // transform data
            std::vector<Vector3> tempVertices;
            std::vector<VMAP::MeshTriangle> tempTriangles;
            VMAP::WmoLiquid* liquid = nullptr;

            (*it).getMeshData(tempVertices, tempTriangles, liquid);

            for (auto& vertex : tempVertices)
            {
                if (!isM2)
                {
                    vertex.x = -vertex.x;
                    vertex.y = -vertex.y;
                }
                vertex = (vertex * matrix) + pos;
            }

            int offset = meshData.solidVerts.size() / 3;

            G3D::Array<uint8> tempTypes;
            tempTypes.resize(tempTriangles.size());
            std::fill(tempTypes.begin(), tempTypes.end(), NAV_AREA_GROUND);

            MMAP::TerrainBuilder::copyVertices(tempVertices, meshData.solidVerts);
            MMAP::TerrainBuilder::copyIndices(tempTriangles, meshData.solidTris, offset, isM2);
            meshData.solidType.append(tempTypes);
        }
    }
}

#endif