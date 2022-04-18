//
// Copyright (c) 2009-2010 Mikko Mononen memon@inside.org
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//

#ifndef RECASTCMANGOSMAP_H
#define RECASTCMANGOSMAP_H

#include <set>
#include <vector>
#include <map>
#include "Tools.h"
#include "MapData.h"
#include "DetourNavMesh.h"
#include "Recast.h"
#include "ChunkyTriMesh.h"
#include "Filelist.h"
#include "G3D\Set.h"
#include "RecastDemoSharedDefines.h"

#ifdef WIN32
#	define snprintf _snprintf
#	define putenv _putenv
#endif

#define MAX_TILE_TO_LOAD 9

typedef std::map<std::string, std::string> TransportDataContainer;
static TransportDataContainer TransportMap =
{
    // List of MO Transport gameobjects
    {"3015", "Transportship.wmo.vmo"},
    {"3031", "Transport_Zeppelin.wmo.vmo"},
    {"7087", "Transportship_Ne.wmo.vmo"},
    // List of Transport gameobjects
    {"0360", "Elevatorcar.m2.vmo"},
    {"0455", "Undeadelevator.m2.vmo"},
    // buildGameObject("Undeadelevatordoor.m2.vmo", 462); // no model on which to path
    {"0561", "Ironforgeelevator.m2.vmo"},
    // buildGameObject("Ironforgeelevatordoor.m2.vmo", 562); // no model on which to path
    {"0807", "Gnomeelevatorcar01.m2.vmo"},
    {"0808", "Gnomeelevatorcar02.m2.vmo"},
    {"0827", "Gnomeelevatorcar03.m2.vmo"},
    {"0852", "Gnomeelevatorcar03.m2.vmo"},
    {"1587", "Gnomehutelevator.m2.vmo"},
    {"2454", "Burningsteppselevator.m2.vmo"},
    {"3831", "Subwaycar.m2.vmo"},
    // TBC+
    {"7026", "Ancdrae_Elevatorpiece.m2.vmo"},
    {"7028", "Mushroombase_Elevator.m2.vmo"},
    {"7043", "Cf_Elevatorplatform.m2.vmo"},
    {"7060", "Cf_Elevatorplatform_Small.m2.vmo"},
    {"7077", "Factoryelevator.m2.vmo"},
    {"7163", "Ancdrae_Elevatorpiece_Netherstorm.m2.vmo"},
    // WOTLK+
    {"6637", "Blackcitadel.wmo.vmo"},
    {"7446", "Transport_Icebreaker_Ship.wmo.vmo"},
    {"7451", "Vr_Elevator_Gate.m2.vmo"},
    {"7452", "Vr_Elevator_Lift.m2.vmo"},
    {"7491", "Vr_Elevator_Gears.m2.vmo"},
    {"7519", "Hf_Elevator_Gate.m2.vmo"},
    {"7520", "Hf_Elevator_Lift_02.m2.vmo"},
    {"7521", "Hf_Elevator_Lift.m2.vmo"},
    {"7546", "Transport_Horde_Zeppelin.wmo.vmo"},
    {"7570", "Transport_Pirate_Ship.wmo.vmo"},
    {"7636", "Transport_Tuskarr_Ship.wmo.vmo"},
    {"7642", "Vrykul_Gondola.m2.vmo"},
    {"7648", "Logrun_Pumpelevator01.m2.vmo"},
    {"7767", "Vrykul_Gondola_02.m2.vmo"},
    {"7793", "Nexus_Elevator_Basestructure_01.m2.vmo"},
    {"7794", "Id_Elevator.m2.vmo"},
    {"7797", "Orc_Fortress_Elevator01.m2.vmo"},
    {"7966", "Org_Arena_Pillar.m2.vmo"},
    {"7973", "Org_Arena_Elevator.m2.vmo"},
    {"8079", "Logrun_Pumpelevator02.m2.vmo"},
    {"8080", "Logrun_Pumpelevator03.m2.vmo"},
    {"8253", "Nd_Hordegunship.wmo.vmo"},
    {"8254", "Nd_Alliancegunship.wmo.vmo"},
    {"8258", "Org_Arena_Yellow_Elevator.m2.vmo"},
    {"8259", "Org_Arena_Axe_Pillar.m2.vmo"},
    {"8260", "Org_Arena_Lightning_Pillar.m2.vmo"},
    {"8261", "Org_Arena_Ivory_Pillar.m2.vmo"},
    {"8277", "Gundrak_Elevator_01.m2.vmo"},
    {"8409", "Nd_Icebreaker_Ship_Bg_Transport.wmo.vmo"},
    {"8410", "Nd_Ship_Ud_Bg_Transport.wmo.vmo"},
    {"8587", "Ulduarraid_Gnomewing_Transport_Wmo.wmo.vmo"},
    {"9001", "Nd_Hordegunship_Bg.wmo.vmo"},
    {"9002", "Nd_Alliancegunship_Bg.wmo.vmo"},
    {"9136", "Icecrown_Elevator.m2.vmo"},
    {"9150", "Nd_Alliancegunship_Icecrown.wmo.vmo"},
    {"9151", "Nd_Hordegunship_Icecrown.wmo.vmo"},
    {"9248", "Icecrown_Elevator02.m2.vmo"},
};

enum ShowLevels
{
    SHOW_LEVEL_NONE,
    SHOW_LEVEL_MAP,
    SHOW_LEVEL_NEIGHBOR_TILES,
    SHOW_LEVEL_ALTERNATIVE_TILES,
    SHOW_LEVEL_TILES,
    SHOW_LEVEL_TRANSPORT
};

struct SelectedTile
{
    SelectedTile(unsigned int x, unsigned int y, unsigned int Id, float const* bmin, float const* bmax) :
        tx(x), ty(y), id(Id)
    {
        rcVcopy(bMin, bmin);
        rcVcopy(bMax, bmax);
    }

    unsigned int tx;
    unsigned int ty;
    unsigned int id;
    float bMin[3];
    float bMax[3];
};

class CMaNGOS_Map
{
protected:
	enum DrawMode
	{
		DRAWMODE_NAVMESH,
		DRAWMODE_NAVMESH_TRANS,
		DRAWMODE_NAVMESH_BVTREE,
		DRAWMODE_NAVMESH_NODES,
		DRAWMODE_NAVMESH_PORTALS,
		DRAWMODE_NAVMESH_INVIS,
		DRAWMODE_MESH,
		DRAWMODE_VOXELS,
		DRAWMODE_VOXELS_WALKABLE,
		DRAWMODE_COMPACT,
		DRAWMODE_COMPACT_DISTANCE,
		DRAWMODE_COMPACT_REGIONS,
		DRAWMODE_REGION_CONNECTIONS,
		DRAWMODE_RAW_CONTOURS,
		DRAWMODE_BOTH_CONTOURS,
		DRAWMODE_CONTOURS,
		DRAWMODE_POLYMESH,
		DRAWMODE_POLYMESH_DETAIL,
		MAX_DRAWMODE
	};

	DrawMode m_drawMode;
    bool m_DrawMapMesh;
    bool m_DrawVMapMesh;

	int m_maxTiles;
	int m_maxPolysPerTile;
	float m_tileSize;

	unsigned int m_tileCol;
	float m_tileBmin[3];
	float m_tileBmax[3];
	float m_tileBuildTime;
	float m_tileMemUsage;
	int m_tileTriCount;

    char m_tileFileName[256];
    MapInfos* m_MapInfos;

    char m_searchMask[8];
    bool m_isBuilded;
    bool m_bigBaseUnit;

    int m_mapID;
    unsigned int m_tileX, m_tileY;
    bool m_showLevelOld;
    ShowLevels m_showLevel;
    float m_LastClick[3];
    unsigned int m_LastTileX;
    unsigned int m_LastTileY;
    SelectedTile* m_SelectedTile;
    float m_GridBMin[3];

    FileList m_MapFiles;
    FileList m_VMapFiles;
    FileList m_MMapFiles;
    std::set <uint32> m_TilesFound;
    std::set <uint32> m_MapsFound;
    std::set <uint32> m_NeighborTiles;
    std::vector <std::string> m_AlternateTiles;
    int m_currentAlternate;
    string m_TileButtonStr;
    string m_transportButtonStr;
    bool m_GeomChanged;
    bool m_switchingTileVersion;

    BuildContext* m_ctx;
    class dtNavMesh* m_navMesh;
    class dtNavMeshQuery* m_navQuery;
    unsigned char m_navMeshDrawFlags;
    float m_cellSize;
    float m_cellHeight;
    float m_agentHeight;
    float m_agentRadius;
    float m_agentMaxClimb;
    float m_agentMaxSlope;
    float m_regionMinSize;
    float m_regionMergeSize;
    float m_edgeMaxLen;
    float m_edgeMaxError;
    float m_vertsPerPoly;
    float m_detailSampleDist;
    float m_detailSampleMaxError;

    SamplePartitionType m_partitionType;
    SampleTool* m_tool;
    SampleToolState* m_toolStates[MAX_TOOLS];

    void initToolStates();
    void resetToolStates();
    void renderToolStates();
    void setTool(SampleTool* tool);
    void renderOverlayToolStates(double* proj, double* model, int* view);
public:
	CMaNGOS_Map();
	virtual ~CMaNGOS_Map();

	void handleSettings();
	void handleTools();
	void handleDebugMode();
	void handleRender();
	void handleRenderOverlay(double* proj, double* model, int* view);
    bool ShowLevel(int height, int width);


    void setContext(BuildContext* ctx) { m_ctx = ctx; }
    void handleClick(const float* s, const float* p, bool shift, bool control);
    void handleUpdate(const float dt);
    void updateToolStates(const float dt);
    class dtNavMesh* getNavMesh() { return m_navMesh; }
    class dtNavMeshQuery* getNavMeshQuery() { return m_navQuery; }
    //class dtCrowd* getCrowd() { return m_crowd; }
    float getAgentRadius() { return m_agentRadius; }
    float getAgentHeight() { return m_agentHeight; }
    float getAgentClimb() { return m_agentMaxClimb; }

    void resetCommonSettings();
    bool buildMoveMapTile();
    void handleExtraSettings();
    void ClearAllGeoms();
    void scanFoldersForTiles();
    bool ShowMapLevel(int height, int width);
    bool ShowNeighborTiles(int height, int width);
    bool ShowTilesLevel(int height, int width);
    bool ShowTransportLevel(int height, int width);
    bool ShowAlternativeLevel(int height, int width);
    bool LoadTileData(unsigned int tx, unsigned int ty, unsigned int tileId = 0);
    bool GeomChanged();
    inline void GetGeomBounds(float const* &bmin, float const* &bmax) { bmin = m_MapInfos->BMin(); bmax = m_MapInfos->BMax(); }
    void ScanFoldersForMaps();
    void Init();
    int GetMapId() const { return m_mapID; };

    inline MapInfos const* GetMapInfos() const { return m_MapInfos; }
    void RemoveTileData(unsigned int tx, unsigned int ty);
    bool GetGridCoord(float const* pos, unsigned int &tx, unsigned int &ty);
    void SelectTile(float const* p);
};


#endif // RECASTCMANGOSMAP_H
