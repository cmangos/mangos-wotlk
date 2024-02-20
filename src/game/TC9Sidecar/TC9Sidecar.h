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

#ifndef _TC9_SIDECAR_H
#define _TC9_SIDECAR_H

#include "Common.h"
#include "AsyncTask.h"
#include "Globals/ObjectMgr.h"
//#include "AsyncCallbackProcessor.h"

#define MAX_MAP_ID 800 // Probably too much, but let's lean towards caution.

class ToCloud9Sidecar
{
private:
    ToCloud9Sidecar();
    ~ToCloud9Sidecar() {};

public:
    static ToCloud9Sidecar* instance();

    void Init(uint16 port, int realmId);
    void Deinit();

    bool ClusterModeEnabled() { return _clusterModeEnabled; }

    bool IsMapAssigned(uint32 mapId);

    void SetupHooks();
    void SetupGrpcHandlers();

    void ProcessHooks();
    void ProcessGrpcRequests();
    void ProcessAsyncTasks();

    uint32 GenerateCharacterGuid();
    uint32 GenerateItemGuid();
    uint32 GenerateInstanceGuid();

private:
    static void OnMapsReassigned(uint32* addedMaps, int addedMapsSize, uint32* removedMaps, int removedMapsSize);

    bool _clusterModeEnabled;

    bool _assignedMapsByID[MAX_MAP_ID];
    
//    AsyncCallbackProcessor<AsyncTask<bool>> _asyncTasksProcessor;
};

#define sToCloud9Sidecar ToCloud9Sidecar::instance()

template<HighGuid high>
class DistributedObjectGuidGenerator: public ObjectGuidGenerator<high>
{
    public:                                                 // constructors
        typedef std::function<uint32(void)> GenerateFunction;

        explicit DistributedObjectGuidGenerator() :  m_generateFunction(SelectGeneratorFunction(high)) 
        {
            ObjectGuidGenerator<high>::m_nextGuid = 0;
        }

    public:                                                 // modifiers
        uint32 Generate() 
        {
            uint32 res = ObjectGuidGenerator<high>::m_nextGuid;
            if (res == 0)
                res = m_generateFunction();
            ObjectGuidGenerator<high>::m_nextGuid = m_generateFunction();
            return res;
        }

    public:                                                 // accessors
        uint32 GetNextAfterMaxUsed() const { return ObjectGuidGenerator<high>::m_nextGuid; }

        static GenerateFunction SelectGeneratorFunction(HighGuid h) 
        {
            switch (h) 
            {
                case HIGHGUID_PLAYER:
                    return std::bind(&ToCloud9Sidecar::GenerateCharacterGuid, sToCloud9Sidecar);
                case HIGHGUID_ITEM:
                    return std::bind(&ToCloud9Sidecar::GenerateItemGuid, sToCloud9Sidecar);
                case HIGHGUID_INSTANCE:
                    return std::bind(&ToCloud9Sidecar::GenerateInstanceGuid, sToCloud9Sidecar);
                default:
                    sLog.outError("FATAL: unsupported distributed guid generator, high guid: %d", h);
                    return NULL;
            }
        }

    private:                                                // fields
        GenerateFunction m_generateFunction;
};


#endif // _TC9_SIDECAR_H
