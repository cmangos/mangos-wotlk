/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
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

/* ScriptData
SDName: Instance_Mount_Hyjal
SD%Complete: 100
SDComment: Instance Data Scripts and functions to acquire mobs and set encounter status for use in various Hyjal Scripts
SDCategory: Caverns of Time, Mount Hyjal
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "hyjal.h"
#include "Entities/TemporarySpawn.h"

/* Battle of Mount Hyjal encounters:
0 - Rage Winterchill event
1 - Anetheron event
2 - Kaz'rogal event
3 - Azgalor event
4 - Archimonde event
*/

instance_mount_hyjal::instance_mount_hyjal(Map* pMap) : ScriptedInstance(pMap),
    m_hyjalWaves(0), m_invasionWaves(0), m_nextWaveTimer(0), m_nextInvasionWaveTimer(0), m_retreatTimer(0), m_infernalsTimer(0), m_failTimer(0), m_startCheck(false)
{
    Initialize();
}

static const float aArchimondeSpawnLoc[4] = { 5601.938f, -3446.284f, 1577.485f, 3.700098f };

std::array<HyjalWave, MAX_WAVES> InitHyjalWaves()
{
    std::array<HyjalWave, MAX_WAVES> data;
    // First boss
    // Wave 1: 10 Ghouls
    data[0].waveTimer = 130000;
    data[0].waveMobs.push_back({ NPC_GHOUL , 4925.643f, -1528.42f, 1327.342f, 4.127055f, ALLY_GROUND });
    data[0].waveMobs.push_back({ NPC_GHOUL , 4921.256f, -1522.268f, 1328.227f, 4.26371f, ALLY_GROUND });
    data[0].waveMobs.push_back({ NPC_GHOUL , 4929.174f, -1523.59f, 1326.968f, 4.103232f, ALLY_GROUND });
    data[0].waveMobs.push_back({ NPC_GHOUL , 4924.306f, -1518.115f, 1327.88f, 4.266161f, ALLY_GROUND });
    data[0].waveMobs.push_back({ NPC_GHOUL , 4935.302f, -1520.679f, 1326.767f, 4.032117f, ALLY_GROUND });
    data[0].waveMobs.push_back({ NPC_GHOUL , 4929.829f, -1517.265f, 1327.309f, 4.168236f, ALLY_GROUND });
    data[0].waveMobs.push_back({ NPC_GHOUL , 4937.893f, -1515.675f, 1326.887f, 4.077524f, ALLY_GROUND });
    data[0].waveMobs.push_back({ NPC_GHOUL , 4931.888f, -1511.627f, 1327.553f, 4.19019f, ALLY_GROUND });
    data[0].waveMobs.push_back({ NPC_GHOUL , 4925.925f, -1511.79f, 1328.051f, 4.410969f, ALLY_GROUND });
    data[0].waveMobs.push_back({ NPC_GHOUL , 4920.874f, -1515.936f, 1328.602f, 4.318599f, ALLY_GROUND });

    // Wave 2: 10 Ghouls and 2 Crypt Fiends
    data[1].waveTimer = 130000;
    data[1].waveMobs.push_back({ NPC_GHOUL , 4916.404f, -1520.643f, 1329.461f, 4.363451f, ALLY_GROUND });
    data[1].waveMobs.push_back({ NPC_GHOUL , 4919.443f, -1524.3f, 1328.708f, 4.2793f, ALLY_GROUND });
    data[1].waveMobs.push_back({ NPC_GHOUL , 4923.728f, -1528.388f, 1327.614f, 4.311669f, ALLY_GROUND });
    data[1].waveMobs.push_back({ NPC_GHOUL , 4927.414f, -1532.622f, 1327.135f, 4.043822f, ALLY_GROUND });
    data[1].waveMobs.push_back({ NPC_GHOUL , 4920.349f, -1516.241f, 1328.749f, 4.324855f, ALLY_GROUND });
    data[1].waveMobs.push_back({ NPC_GHOUL , 4924.708f, -1519.513f, 1327.774f, 4.380139f, ALLY_GROUND });
    data[1].waveMobs.push_back({ NPC_GHOUL , 4928.422f, -1523.765f, 1327.044f, 4.240041f, ALLY_GROUND });
    data[1].waveMobs.push_back({ NPC_GHOUL , 4932.152f, -1529.665f, 1326.947f, 4.007116f, ALLY_GROUND });
    data[1].waveMobs.push_back({ NPC_GHOUL , 4916.519f, -1528.107f, 1329.58f, 4.352018f, ALLY_GROUND });
    data[1].waveMobs.push_back({ NPC_GHOUL , 4920.124f, -1532.635f, 1328.399f, 4.381913f, ALLY_GROUND });
    data[1].waveMobs.push_back({ NPC_CRYPT , 4926.602f, -1514.812f, 1327.568f, 4.374004f, ALLY_GROUND });
    data[1].waveMobs.push_back({ NPC_CRYPT , 4933.663f, -1522.479f, 1326.8f, 4.066925f, ALLY_GROUND });

    // Wave 3: 6 Ghouls and 6 Crypt Fiends
    data[2].waveTimer = 130000;
    data[2].waveMobs.push_back({ NPC_GHOUL , 4927.044f, -1537.156f, 1327.366f, 3.983671f, ALLY_GROUND });
    data[2].waveMobs.push_back({ NPC_GHOUL , 4923.918f, -1532.033f, 1327.583f, 4.248679f, ALLY_GROUND });
    data[2].waveMobs.push_back({ NPC_GHOUL , 4917.58f, -1529.782f, 1329.129f, 4.512907f, ALLY_GROUND });
    data[2].waveMobs.push_back({ NPC_GHOUL , 4914.535f, -1525.046f, 1330.081f, 4.644515f, ALLY_GROUND });
    data[2].waveMobs.push_back({ NPC_GHOUL , 4907.775f, -1523.272f, 1332.014f, 4.520949f, ALLY_GROUND });
    data[2].waveMobs.push_back({ NPC_GHOUL , 4904.907f, -1516.222f, 1332.705f, 4.907058f, ALLY_GROUND });
    data[2].waveMobs.push_back({ NPC_CRYPT , 4931.415f, -1533.642f, 1327.038f, 3.88825f, ALLY_GROUND });
    data[2].waveMobs.push_back({ NPC_CRYPT , 4926.614f, -1528.546f, 1327.198f, 4.107586f, ALLY_GROUND });
    data[2].waveMobs.push_back({ NPC_CRYPT , 4921.408f, -1522.906f, 1328.219f, 4.281098f, ALLY_GROUND });
    data[2].waveMobs.push_back({ NPC_CRYPT , 4913.245f, -1520.054f, 1330.439f, 4.426145f, ALLY_GROUND });
    data[2].waveMobs.push_back({ NPC_CRYPT , 4934.147f, -1524.828f, 1326.84f, 4.034865f, ALLY_GROUND });
    data[2].waveMobs.push_back({ NPC_CRYPT , 4927.279f, -1517.326f, 1327.657f, 4.207614f, ALLY_GROUND });

    // Wave 4: 6 Ghouls, 4 Crypt Fiends and 2 Necromancers
    data[3].waveTimer = 130000;
    data[3].waveMobs.push_back({ NPC_GHOUL , 4911.118f, -1523.113f, 1331.097f, 4.451728f, ALLY_GROUND });
    data[3].waveMobs.push_back({ NPC_GHOUL , 4908.977f, -1517.954f, 1331.429f, 4.808149f, ALLY_GROUND });
    data[3].waveMobs.push_back({ NPC_GHOUL , 4914.319f, -1527.248f, 1330.165f, 4.426172f, ALLY_GROUND });
    data[3].waveMobs.push_back({ NPC_GHOUL , 4917.809f, -1531.797f, 1329.036f, 4.265389f, ALLY_GROUND });
    data[3].waveMobs.push_back({ NPC_GHOUL , 4921.933f, -1535.177f, 1327.973f, 4.07961f, ALLY_GROUND });
    data[3].waveMobs.push_back({ NPC_GHOUL , 4926.549f, -1538.521f, 1327.489f, 3.969737f, ALLY_GROUND });
    data[3].waveMobs.push_back({ NPC_CRYPT , 4916.115f, -1516.368f, 1329.683f, 4.457922f, ALLY_GROUND });
    data[3].waveMobs.push_back({ NPC_CRYPT , 4926.343f, -1516.999f, 1327.856f, 4.22424f, ALLY_GROUND });
    data[3].waveMobs.push_back({ NPC_CRYPT , 4930.89f, -1522.958f, 1326.893f, 4.079469f, ALLY_GROUND });
    data[3].waveMobs.push_back({ NPC_CRYPT , 4931.164f, -1532.178f, 1326.95f, 3.921505f, ALLY_GROUND });
    data[3].waveMobs.push_back({ NPC_NECRO , 4919.989f, -1521.773f, 1328.61f, 4.489554f, ALLY_GROUND });
    data[3].waveMobs.push_back({ NPC_NECRO , 4923.608f, -1526.632f, 1327.75f, 4.337822f, ALLY_GROUND });

    // Wave 5: 2 Ghouls, 6 Crypt Fiends and 4 Necromancers
    data[4].waveTimer = 130000;
    data[4].waveMobs.push_back({ NPC_GHOUL , 4918.185f, -1527.17f, 1329.019f, 4.315125f, ALLY_GROUND });
    data[4].waveMobs.push_back({ NPC_GHOUL , 4922.89f, -1529.576f, 1327.829f, 4.322945f, ALLY_GROUND });
    data[4].waveMobs.push_back({ NPC_CRYPT , 4916.001f, -1517.164f, 1329.591f, 4.620493f, ALLY_GROUND });
    data[4].waveMobs.push_back({ NPC_CRYPT , 4920.521f, -1519.719f, 1328.516f, 4.33359f, ALLY_GROUND });
    data[4].waveMobs.push_back({ NPC_CRYPT , 4925.456f, -1524.336f, 1327.433f, 4.172694f, ALLY_GROUND });
    data[4].waveMobs.push_back({ NPC_CRYPT , 4931.456f, -1528.238f, 1326.907f, 4.034986f, ALLY_GROUND });
    data[4].waveMobs.push_back({ NPC_CRYPT , 4924.255f, -1513.54f, 1328.496f, 4.435135f, ALLY_GROUND });
    data[4].waveMobs.push_back({ NPC_CRYPT , 4931.574f, -1518.703f, 1327.023f, 4.120836f, ALLY_GROUND });
    data[4].waveMobs.push_back({ NPC_NECRO , 4915.204f, -1524.542f, 1329.935f, 4.359166f, ALLY_GROUND });
    data[4].waveMobs.push_back({ NPC_NECRO , 4912.781f, -1522.514f, 1330.384f, 4.703914f, ALLY_GROUND });
    data[4].waveMobs.push_back({ NPC_NECRO , 4925.974f, -1531.158f, 1327.249f, 4.053604f, ALLY_GROUND });
    data[4].waveMobs.push_back({ NPC_NECRO , 4930.198f, -1534.038f, 1327.047f, 4.007935f, ALLY_GROUND });

    // Wave 6: 6 Ghouls and 6 Abominations
    data[5].waveTimer = 130000;
    data[5].waveMobs.push_back({ NPC_GHOUL , 4931.506f, -1535.571f, 1327.243f, 3.844688f, ALLY_GROUND });
    data[5].waveMobs.push_back({ NPC_GHOUL , 4928.218f, -1534.305f, 1327.114f, 4.060686f, ALLY_GROUND });
    data[5].waveMobs.push_back({ NPC_GHOUL , 4922.387f, -1531.344f, 1327.953f, 4.150617f, ALLY_GROUND });
    data[5].waveMobs.push_back({ NPC_GHOUL , 4918.271f, -1528.786f, 1328.984f, 4.293182f, ALLY_GROUND });
    data[5].waveMobs.push_back({ NPC_GHOUL , 4915.333f, -1526.436f, 1329.728f, 4.613498f, ALLY_GROUND });
    data[5].waveMobs.push_back({ NPC_GHOUL , 4911.082f, -1522.356f, 1331.09f, 4.456543f, ALLY_GROUND });
    data[5].waveMobs.push_back({ NPC_ABOMI , 4934.243f, -1529.936f, 1327.063f, 3.907663f, ALLY_GROUND });
    data[5].waveMobs.push_back({ NPC_ABOMI , 4928.805f, -1528.162f, 1326.991f, 4.164741f, ALLY_GROUND });
    data[5].waveMobs.push_back({ NPC_ABOMI , 4921.543f, -1524.08f, 1328.201f, 4.243358f, ALLY_GROUND });
    data[5].waveMobs.push_back({ NPC_ABOMI , 4915.95f, -1520.491f, 1329.678f, 4.372918f, ALLY_GROUND });
    data[5].waveMobs.push_back({ NPC_ABOMI , 4933.451f, -1522.371f, 1326.794f, 4.04113f, ALLY_GROUND });
    data[5].waveMobs.push_back({ NPC_ABOMI , 4922.121f, -1517.213f, 1328.356f, 4.288848f, ALLY_GROUND });

    // Wave 7: 4 Ghouls, 4 Abominations and 4 Necromancers
    data[6].waveTimer = 130000;
    data[6].waveMobs.push_back({ NPC_GHOUL , 4926.464f, -1534.441f, 1327.286f, 3.976079f, ALLY_GROUND });
    data[6].waveMobs.push_back({ NPC_GHOUL , 4922.677f, -1530.785f, 1327.851f, 4.142037f, ALLY_GROUND });
    data[6].waveMobs.push_back({ NPC_GHOUL , 4919.348f, -1526.814f, 1328.665f, 4.473605f, ALLY_GROUND });
    data[6].waveMobs.push_back({ NPC_GHOUL , 4915.953f, -1522.815f, 1329.489f, 4.605725f, ALLY_GROUND });
    data[6].waveMobs.push_back({ NPC_NECRO , 4927.556f, -1531.928f, 1327.122f, 4.001671f, ALLY_GROUND });
    data[6].waveMobs.push_back({ NPC_NECRO , 4925.604f, -1529.334f, 1327.388f, 4.093475f, ALLY_GROUND });
    data[6].waveMobs.push_back({ NPC_NECRO , 4919.795f, -1523.094f, 1328.692f, 4.317783f, ALLY_GROUND });
    data[6].waveMobs.push_back({ NPC_NECRO , 4918.63f, -1521.644f, 1328.854f, 4.530917f, ALLY_GROUND });
    data[6].waveMobs.push_back({ NPC_ABOMI , 4931.853f, -1531.563f, 1326.989f, 3.921482f, ALLY_GROUND });
    data[6].waveMobs.push_back({ NPC_ABOMI , 4927.954f, -1526.69f, 1327.047f, 4.106788f, ALLY_GROUND });
    data[6].waveMobs.push_back({ NPC_ABOMI , 4923.524f, -1522.353f, 1327.761f, 4.385348f, ALLY_GROUND });
    data[6].waveMobs.push_back({ NPC_ABOMI , 4918.477f, -1516.516f, 1329.144f, 4.403982f, ALLY_GROUND });

    // Wave 8: 6 Ghouls, 4 Crypt Fiends, 2 Abominations and 2 Necromancers
    data[7].waveTimer = 190000;
    data[7].waveMobs.push_back({ NPC_GHOUL , 4932.013f, -1531.015f, 1326.96f, 3.99057f, ALLY_GROUND });
    data[7].waveMobs.push_back({ NPC_GHOUL , 4928.318f, -1528.833f, 1327.05f, 4.041089f, ALLY_GROUND });
    data[7].waveMobs.push_back({ NPC_GHOUL , 4923.604f, -1527.729f, 1327.728f, 4.165579f, ALLY_GROUND });
    data[7].waveMobs.push_back({ NPC_GHOUL , 4920.151f, -1527.905f, 1328.456f, 4.43657f, ALLY_GROUND });
    data[7].waveMobs.push_back({ NPC_GHOUL , 4914.406f, -1527.226f, 1330.206f, 4.355446f, ALLY_GROUND });
    data[7].waveMobs.push_back({ NPC_GHOUL , 4909.197f, -1525.781f, 1331.64f, 4.478852f, ALLY_GROUND });
    data[7].waveMobs.push_back({ NPC_CRYPT , 4932.52f, -1524.659f, 1326.851f, 4.025184f, ALLY_GROUND });
    data[7].waveMobs.push_back({ NPC_CRYPT , 4925.131f, -1521.898f, 1327.548f, 4.20156f, ALLY_GROUND });
    data[7].waveMobs.push_back({ NPC_CRYPT , 4917.794f, -1521.453f, 1329.094f, 4.332634f, ALLY_GROUND });
    data[7].waveMobs.push_back({ NPC_CRYPT , 4911.284f, -1520.284f, 1330.72f, 4.747531f, ALLY_GROUND });
    data[7].waveMobs.push_back({ NPC_ABOMI , 4929.808f, -1515.532f, 1327.685f, 4.187524f, ALLY_GROUND });
    data[7].waveMobs.push_back({ NPC_ABOMI , 4921.793f, -1513.606f, 1328.815f, 4.356327f, ALLY_GROUND });
    data[7].waveMobs.push_back({ NPC_NECRO , 4936.591f, -1518.104f, 1326.803f, 4.044622f, ALLY_GROUND });
    data[7].waveMobs.push_back({ NPC_NECRO , 4915.962f, -1510.813f, 1329.748f, 4.515066f, ALLY_GROUND });

    // Rage Winterchill
    data[8].waveTimer = 0;
    data[8].waveMobs.push_back({ NPC_WINTERCHILL , 4936.221f, -1513.435f, 1327.188f, 4.104067f, ALLY_GROUND });

    // Second boss
    // Wave 1: 10 Ghouls
    data[9].waveTimer = 130000;
    data[9].waveMobs.push_back({ NPC_GHOUL , 4932.953f, -1529.599f, 1326.7f, 3.937371f, ALLY_GROUND });
    data[9].waveMobs.push_back({ NPC_GHOUL , 4929.082f, -1526.454f, 1326.852f, 4.18398f, ALLY_GROUND });
    data[9].waveMobs.push_back({ NPC_GHOUL , 4924.633f, -1523.052f, 1327.445f, 4.347703f, ALLY_GROUND });
    data[9].waveMobs.push_back({ NPC_GHOUL , 4920.136f, -1519.804f, 1328.422f, 4.497755f, ALLY_GROUND });
    data[9].waveMobs.push_back({ NPC_GHOUL , 4916.486f, -1516.639f, 1329.328f, 4.447559f, ALLY_GROUND });
    data[9].waveMobs.push_back({ NPC_GHOUL , 4938.097f, -1525.316f, 1326.922f, 3.921502f, ALLY_GROUND });
    data[9].waveMobs.push_back({ NPC_GHOUL , 4934.7f, -1520.92f, 1326.68f, 4.134113f, ALLY_GROUND });
    data[9].waveMobs.push_back({ NPC_GHOUL , 4930.074f, -1517.03f, 1327.01f, 4.167439f, ALLY_GROUND });
    data[9].waveMobs.push_back({ NPC_GHOUL , 4924.334f, -1513.492f, 1328.07f, 4.281028f, ALLY_GROUND });
    data[9].waveMobs.push_back({ NPC_GHOUL , 4920.155f, -1509.878f, 1329.028f, 4.366851f, ALLY_GROUND });

    // Wave 2: 8 Ghouls and 4 Abominations
    data[10].waveTimer = 130000;
    data[10].waveMobs.push_back({ NPC_GHOUL , 4928.501f, -1534.467f, 1327.107f, 4.047384f, ALLY_GROUND });
    data[10].waveMobs.push_back({ NPC_GHOUL , 4924.636f, -1530.77f, 1327.51f, 4.116479f, ALLY_GROUND });
    data[10].waveMobs.push_back({ NPC_GHOUL , 4922.069f, -1528.371f, 1328.02f, 4.365864f, ALLY_GROUND });
    data[10].waveMobs.push_back({ NPC_GHOUL , 4918.227f, -1525.646f, 1328.961f, 4.330956f, ALLY_GROUND });
    data[10].waveMobs.push_back({ NPC_GHOUL , 4915.26f, -1522.313f, 1329.863f, 4.439423f, ALLY_GROUND });
    data[10].waveMobs.push_back({ NPC_GHOUL , 4911.429f, -1518.958f, 1330.918f, 4.55756f, ALLY_GROUND });
    data[10].waveMobs.push_back({ NPC_GHOUL , 4919.851f, -1532.629f, 1328.433f, 4.18836f, ALLY_GROUND });
    data[10].waveMobs.push_back({ NPC_GHOUL , 4914.406f, -1526.891f, 1330.03f, 4.644628f, ALLY_GROUND });
    data[10].waveMobs.push_back({ NPC_ABOMI , 4933.69f, -1529.706f, 1326.988f, 4.013412f, ALLY_GROUND });
    data[10].waveMobs.push_back({ NPC_ABOMI , 4929.332f, -1525.465f, 1326.957f, 4.192708f, ALLY_GROUND });
    data[10].waveMobs.push_back({ NPC_ABOMI , 4922.8f, -1520.3f, 1328.015f, 4.276646f, ALLY_GROUND });
    data[10].waveMobs.push_back({ NPC_ABOMI , 4917.733f, -1516.502f, 1329.268f, 4.577129f, ALLY_GROUND });

    // Wave 3: 4 Ghouls, 4 Crypt Fiends and 4 Necromancers
    data[11].waveTimer = 130000;
    data[11].waveMobs.push_back({ NPC_GHOUL , 4926.166f, -1533.865f, 1327.382f, 4.047344f, ALLY_GROUND });
    data[11].waveMobs.push_back({ NPC_GHOUL , 4922.134f, -1530.295f, 1327.968f, 4.163887f, ALLY_GROUND });
    data[11].waveMobs.push_back({ NPC_GHOUL , 4917.754f, -1526.04f, 1329.193f, 4.296716f, ALLY_GROUND });
    data[11].waveMobs.push_back({ NPC_GHOUL , 4913.225f, -1521.825f, 1330.442f, 4.41712f, ALLY_GROUND });
    data[11].waveMobs.push_back({ NPC_CRYPT , 4931.624f, -1530.073f, 1326.916f, 3.952546f, ALLY_GROUND });
    data[11].waveMobs.push_back({ NPC_CRYPT , 4927.168f, -1526.048f, 1327.128f, 4.241773f, ALLY_GROUND });
    data[11].waveMobs.push_back({ NPC_CRYPT , 4921.812f, -1521.547f, 1328.144f, 4.285402f, ALLY_GROUND });
    data[11].waveMobs.push_back({ NPC_CRYPT , 4916.725f, -1516.875f, 1329.484f, 4.441412f, ALLY_GROUND });
    data[11].waveMobs.push_back({ NPC_NECRO , 4937.741f, -1523.788f, 1326.978f, 3.99891f, ALLY_GROUND });
    data[11].waveMobs.push_back({ NPC_NECRO , 4933.576f, -1519.664f, 1326.835f, 4.073932f, ALLY_GROUND });
    data[11].waveMobs.push_back({ NPC_NECRO , 4927.942f, -1514.643f, 1327.973f, 4.229518f, ALLY_GROUND });
    data[11].waveMobs.push_back({ NPC_NECRO , 4922.644f, -1509.858f, 1329.055f, 4.32917f, ALLY_GROUND });

    // Wave 4: 6 Crypt Fiends, 4 Necromancers and 2 Banshees
    data[12].waveTimer = 130000;
    data[12].waveMobs.push_back({ NPC_CRYPT , 4939.017f, -1523.412f, 1326.9f, 3.937352f, ALLY_GROUND });
    data[12].waveMobs.push_back({ NPC_CRYPT , 4933.433f, -1518.303f, 1326.739f, 4.111102f, ALLY_GROUND });
    data[12].waveMobs.push_back({ NPC_CRYPT , 4927.944f, -1514.354f, 1327.556f, 4.347644f, ALLY_GROUND });
    data[12].waveMobs.push_back({ NPC_CRYPT , 4921.462f, -1510.275f, 1328.794f, 4.385396f, ALLY_GROUND });
    data[12].waveMobs.push_back({ NPC_CRYPT , 4940.061f, -1513.149f, 1326.812f, 4.128702f, ALLY_GROUND });
    data[12].waveMobs.push_back({ NPC_CRYPT , 4933.439f, -1507.623f, 1327.997f, 4.198108f, ALLY_GROUND });
    data[12].waveMobs.push_back({ NPC_NECRO , 4932.806f, -1527.553f, 1326.7f, 4.022399f, ALLY_GROUND });
    data[12].waveMobs.push_back({ NPC_NECRO , 4927.911f, -1522.446f, 1326.982f, 4.268787f, ALLY_GROUND });
    data[12].waveMobs.push_back({ NPC_NECRO , 4922.838f, -1518.548f, 1327.799f, 4.435117f, ALLY_GROUND });
    data[12].waveMobs.push_back({ NPC_NECRO , 4918.32f, -1515.146f, 1328.966f, 4.567501f, ALLY_GROUND });
    data[12].waveMobs.push_back({ NPC_BANSH , 4923.421f, -1526.854f, 1327.707f, 4.182356f, ALLY_GROUND });
    data[12].waveMobs.push_back({ NPC_BANSH , 4918.667f, -1522.755f, 1328.861f, 4.306505f, ALLY_GROUND });

    // Wave 5: 6 Ghouls, 4 Banshees and 2 Necromancers
    data[13].waveTimer = 130000;
    data[13].waveMobs.push_back({ NPC_GHOUL , 4933.623f, -1530.23f, 1326.732f, 3.913238f, ALLY_GROUND });
    data[13].waveMobs.push_back({ NPC_GHOUL , 4930.487f, -1526.874f, 1326.768f, 4.142032f, ALLY_GROUND });
    data[13].waveMobs.push_back({ NPC_GHOUL , 4926.575f, -1522.183f, 1327.146f, 4.174441f, ALLY_GROUND });
    data[13].waveMobs.push_back({ NPC_GHOUL , 4922.776f, -1518.178f, 1327.836f, 4.297525f, ALLY_GROUND });
    data[13].waveMobs.push_back({ NPC_GHOUL , 4919.374f, -1514.975f, 1328.728f, 4.541776f, ALLY_GROUND });
    data[13].waveMobs.push_back({ NPC_GHOUL , 4915.127f, -1510.888f, 1329.914f, 4.652884f, ALLY_GROUND });
    data[13].waveMobs.push_back({ NPC_BANSH , 4935.26f, -1522.895f, 1326.708f, 4.003616f, ALLY_GROUND });
    data[13].waveMobs.push_back({ NPC_BANSH , 4931.459f, -1517.77f, 1326.869f, 4.239964f, ALLY_GROUND });
    data[13].waveMobs.push_back({ NPC_BANSH , 4927.077f, -1514.236f, 1327.657f, 4.235619f, ALLY_GROUND });
    data[13].waveMobs.push_back({ NPC_BANSH , 4923.01f, -1509.547f, 1328.694f, 4.484927f, ALLY_GROUND });
    data[13].waveMobs.push_back({ NPC_NECRO , 4938.147f, -1516.22f, 1326.668f, 4.06968f, ALLY_GROUND });
    data[13].waveMobs.push_back({ NPC_NECRO , 4929.385f, -1507.941f, 1328.319f, 4.366895f, ALLY_GROUND });

    // Wave 6: 6 Ghouls, 2 Abominations and 4 Necromancers
    data[14].waveTimer = 130000;
    data[14].waveMobs.push_back({ NPC_GHOUL , 4935.932f, -1528.83f, 1326.905f, 3.980172f, ALLY_GROUND });
    data[14].waveMobs.push_back({ NPC_GHOUL , 4932.223f, -1525.755f, 1326.723f, 4.117352f, ALLY_GROUND });
    data[14].waveMobs.push_back({ NPC_GHOUL , 4928.167f, -1523.22f, 1326.946f, 4.253203f, ALLY_GROUND });
    data[14].waveMobs.push_back({ NPC_GHOUL , 4924.045f, -1520.065f, 1327.56f, 4.392459f, ALLY_GROUND });
    data[14].waveMobs.push_back({ NPC_GHOUL , 4919.923f, -1517.707f, 1328.485f, 4.321369f, ALLY_GROUND });
    data[14].waveMobs.push_back({ NPC_GHOUL , 4915.889f, -1515.049f, 1329.545f, 4.627302f, ALLY_GROUND });
    data[14].waveMobs.push_back({ NPC_ABOMI , 4933.285f, -1518.861f, 1326.729f, 4.107613f, ALLY_GROUND });
    data[14].waveMobs.push_back({ NPC_ABOMI , 4926.564f, -1513.743f, 1327.779f, 4.246144f, ALLY_GROUND });
    data[14].waveMobs.push_back({ NPC_NECRO , 4937.41f, -1523.689f, 1326.784f, 3.957389f, ALLY_GROUND });
    data[14].waveMobs.push_back({ NPC_NECRO , 4935.644f, -1512.89f, 1327.037f, 4.208555f, ALLY_GROUND });
    data[14].waveMobs.push_back({ NPC_NECRO , 4931.938f, -1509.754f, 1327.863f, 4.20155f, ALLY_GROUND });
    data[14].waveMobs.push_back({ NPC_NECRO , 4921.09f, -1511.109f, 1328.757f, 4.518619f, ALLY_GROUND });

    // Wave 7: 2 Ghouls, 4 Crypt Fiends, 4 Abominations and 2 Banshees
    data[15].waveTimer = 130000;
    data[15].waveMobs.push_back({ NPC_GHOUL , 4924.866f, -1525.098f, 1327.391f, 4.175267f, ALLY_GROUND });
    data[15].waveMobs.push_back({ NPC_GHOUL , 4921.645f, -1521.122f, 1328.076f, 4.447429f, ALLY_GROUND });
    data[15].waveMobs.push_back({ NPC_CRYPT , 4929.167f, -1529.471f, 1326.892f, 4.012544f, ALLY_GROUND });
    data[15].waveMobs.push_back({ NPC_CRYPT , 4917.498f, -1517.405f, 1329.074f, 4.365274f, ALLY_GROUND });
    data[15].waveMobs.push_back({ NPC_CRYPT , 4934.654f, -1526.132f, 1326.76f, 4.013417f, ALLY_GROUND });
    data[15].waveMobs.push_back({ NPC_CRYPT , 4930.837f, -1520.827f, 1326.786f, 4.219054f, ALLY_GROUND });
    data[15].waveMobs.push_back({ NPC_ABOMI , 4926.241f, -1517.236f, 1327.395f, 4.362504f, ALLY_GROUND });
    data[15].waveMobs.push_back({ NPC_ABOMI , 4921.272f, -1511.85f, 1328.661f, 4.511899f, ALLY_GROUND });
    data[15].waveMobs.push_back({ NPC_ABOMI , 4940.836f, -1520.848f, 1326.926f, 3.946306f, ALLY_GROUND });
    data[15].waveMobs.push_back({ NPC_ABOMI , 4936.27f, -1516.31f, 1326.689f, 4.092718f, ALLY_GROUND });
    data[15].waveMobs.push_back({ NPC_BANSH , 4932.069f, -1510.972f, 1327.676f, 4.191054f, ALLY_GROUND });
    data[15].waveMobs.push_back({ NPC_BANSH , 4926.97f, -1506.623f, 1328.652f, 4.312508f, ALLY_GROUND });

    // Wave 8: 4 Ghouls, 2 Crypt Fiends, 4 Abominations, 2 Banshees and 2 Necromancers
    data[16].waveTimer = 190000;
    data[16].waveMobs.push_back({ NPC_GHOUL , 4927.992f, -1530.346f, 1327.017f, 4.149098f, ALLY_GROUND });
    data[16].waveMobs.push_back({ NPC_GHOUL , 4922.845f, -1525.778f, 1327.841f, 4.203334f, ALLY_GROUND });
    data[16].waveMobs.push_back({ NPC_GHOUL , 4917.346f, -1520.755f, 1329.185f, 4.398687f, ALLY_GROUND });
    data[16].waveMobs.push_back({ NPC_GHOUL , 4911.636f, -1516.439f, 1330.68f, 4.561438f, ALLY_GROUND });
    data[16].waveMobs.push_back({ NPC_ABOMI , 4932.755f, -1526.406f, 1326.707f, 4.094298f, ALLY_GROUND });
    data[16].waveMobs.push_back({ NPC_ABOMI , 4926.729f, -1521.291f, 1327.152f, 4.182318f, ALLY_GROUND });
    data[16].waveMobs.push_back({ NPC_ABOMI , 4921.313f, -1517.25f, 1328.153f, 4.302872f, ALLY_GROUND });
    data[16].waveMobs.push_back({ NPC_ABOMI , 4914.79f, -1512.803f, 1329.918f, 4.505786f, ALLY_GROUND });
    data[16].waveMobs.push_back({ NPC_CRYPT , 4932.799f, -1517.233f, 1326.804f, 4.218163f, ALLY_GROUND });
    data[16].waveMobs.push_back({ NPC_CRYPT , 4927.063f, -1511.41f, 1328.058f, 4.27498f, ALLY_GROUND });
    data[16].waveMobs.push_back({ NPC_BANSH , 4942.199f, -1517.49f, 1326.844f, 4.009918f, ALLY_GROUND });
    data[16].waveMobs.push_back({ NPC_BANSH , 4927.272f, -1502.729f, 1329.026f, 4.333464f, ALLY_GROUND });
    data[16].waveMobs.push_back({ NPC_NECRO , 4938.311f, -1510.945f, 1327.078f, 4.180502f, ALLY_GROUND });
    data[16].waveMobs.push_back({ NPC_NECRO , 4933.119f, -1506.645f, 1328.139f, 4.214725f, ALLY_GROUND });

    // Anetheron
    data[17].waveTimer = 0;
    data[17].waveMobs.push_back({ NPC_ANETHERON , 4938.755f, -1510.624f, 1327.083f, 4.176985f, ALLY_GROUND });

    // Third boss
    // Wave 1: 6 Ghoul, 4 Abomination, 2 Banshee, 2 Necromancer
    data[18].waveTimer = 130000;
    data[18].waveMobs.push_back({ NPC_GHOUL , 5471.863f, -2392.172f, 1460.32f, 6.104537f, HORDE_GROUND });
    data[18].waveMobs.push_back({ NPC_GHOUL , 5475.722f, -2388.922f, 1460.821f, 5.319835f, HORDE_GROUND });
    data[18].waveMobs.push_back({ NPC_GHOUL , 5479.377f, -2386.201f, 1461.3f, 5.927942f, HORDE_GROUND });
    data[18].waveMobs.push_back({ NPC_GHOUL , 5482.705f, -2382.528f, 1461.503f, 4.954062f, HORDE_GROUND });
    data[18].waveMobs.push_back({ NPC_GHOUL , 5487.009f, -2379.895f, 1461.587f, 4.782725f, HORDE_GROUND });
    data[18].waveMobs.push_back({ NPC_GHOUL , 5489.937f, -2376.359f, 1461.892f, 5.575186f, HORDE_GROUND });
    data[18].waveMobs.push_back({ NPC_ABOMI , 5466.463f, -2387.134f, 1462.138f, 5.697435f, HORDE_GROUND });
    data[18].waveMobs.push_back({ NPC_ABOMI , 5473.117f, -2381.076f, 1461.292f, 5.245095f, HORDE_GROUND });
    data[18].waveMobs.push_back({ NPC_ABOMI , 5480.076f, -2375.681f, 1461.285f, 4.981046f, HORDE_GROUND });
    data[18].waveMobs.push_back({ NPC_ABOMI , 5486.467f, -2370.369f, 1461.592f, 5.523299f, HORDE_GROUND });
    data[18].waveMobs.push_back({ NPC_BANSH , 5466.94f, -2377.265f, 1463.017f, 5.533129f, HORDE_GROUND });
    data[18].waveMobs.push_back({ NPC_BANSH , 5470.159f, -2374.818f, 1461.555f, 5.228471f, HORDE_GROUND });
    data[18].waveMobs.push_back({ NPC_NECRO , 5473.245f, -2371.979f, 1460.878f, 5.124473f, HORDE_GROUND });
    data[18].waveMobs.push_back({ NPC_NECRO , 5476.542f, -2368.595f, 1460.913f, 5.286283f, HORDE_GROUND });

    // Wave 2: 4 Ghoul, 10 Gargoyle
    data[19].waveTimer = 155000;
    data[19].waveMobs.push_back({ NPC_GHOUL , 5473.612f, -2393.847f, 1460.427f, 5.538561f, HORDE_GROUND });
    data[19].waveMobs.push_back({ NPC_GHOUL , 5481.65f, -2387.077f, 1461.446f, 5.930215f, HORDE_GROUND });
    data[19].waveMobs.push_back({ NPC_GHOUL , 5484.973f, -2383.493f, 1461.744f, 5.808589f, HORDE_GROUND });
    data[19].waveMobs.push_back({ NPC_GHOUL , 5491.622f, -2377.881f, 1462.246f, 5.57801f, HORDE_GROUND });
    data[19].waveMobs.push_back({ NPC_GARGO , 5823.992f, -2908.259f, 1564.566f, 2.640309f, HORDE_GARGOYLE_1 });
    data[19].waveMobs.push_back({ NPC_GARGO , 5802.556f, -2886.877f, 1551.054f, 2.849298f, HORDE_GARGOYLE_1 });
    data[19].waveMobs.push_back({ NPC_GARGO , 5776.23f, -2872.468f, 1612.081f, 4.955018f, HORDE_GARGOYLE_1 });
    data[19].waveMobs.push_back({ NPC_GARGO , 5770.719f, -2834.339f, 1598.396f, 4.912748f, HORDE_GARGOYLE_1 });
    data[19].waveMobs.push_back({ NPC_GARGO , 5780.823f, -2822.528f, 1576.543f, 2.910838f, HORDE_GARGOYLE_1 });
    data[19].waveMobs.push_back({ NPC_GARGO , 5786.263f, -2859.778f, 1576.375f, 2.063877f, HORDE_GARGOYLE_1 });
    data[19].waveMobs.push_back({ NPC_GARGO , 5804.678f, -2901.291f, 1587.472f, 4.21586f, HORDE_GARGOYLE_1 });
    data[19].waveMobs.push_back({ NPC_GARGO , 5823.609f, -2938.187f, 1617.297f, 2.037145f, HORDE_GARGOYLE_1 });
    data[19].waveMobs.push_back({ NPC_GARGO , 5796.999f, -2912.517f, 1627.133f, 1.912279f, HORDE_GARGOYLE_1 });
    data[19].waveMobs.push_back({ NPC_GARGO , 5789.98f, -2838.952f, 1548.416f, 4.71027f, HORDE_GARGOYLE_1 });

    // Wave 3: 6 Ghoul, 6 Crypt Fiends, 2 Necromancer
    data[20].waveTimer = 130000;
    data[20].waveMobs.push_back({ NPC_GHOUL , 5473.296f, -2393.048f, 1460.443f, 5.727721f, HORDE_GROUND });
    data[20].waveMobs.push_back({ NPC_GHOUL , 5477.202f, -2389.97f, 1460.973f, 5.598216f, HORDE_GROUND });
    data[20].waveMobs.push_back({ NPC_GHOUL , 5480.104f, -2388.15f, 1461.365f, 5.133206f, HORDE_GROUND });
    data[20].waveMobs.push_back({ NPC_GHOUL , 5484.459f, -2383.279f, 1461.681f, 5.810743f, HORDE_GROUND });
    data[20].waveMobs.push_back({ NPC_GHOUL , 5487.065f, -2381.763f, 1461.635f, 4.785645f, HORDE_GROUND });
    data[20].waveMobs.push_back({ NPC_GHOUL , 5489.606f, -2377.921f, 1461.894f, 4.692288f, HORDE_GROUND });
    data[20].waveMobs.push_back({ NPC_CRYPT , 5466.04f, -2379.193f, 1463.627f, 5.572718f, HORDE_GROUND });
    data[20].waveMobs.push_back({ NPC_CRYPT , 5479.046f, -2368.428f, 1461.076f, 5.24419f, HORDE_GROUND });
    data[20].waveMobs.push_back({ NPC_CRYPT , 5467.451f, -2387.625f, 1461.341f, 5.693457f, HORDE_GROUND });
    data[20].waveMobs.push_back({ NPC_CRYPT , 5474.267f, -2382.166f, 1461.342f, 5.501296f, HORDE_GROUND });
    data[20].waveMobs.push_back({ NPC_CRYPT , 5480.889f, -2377.129f, 1461.338f, 5.308205f, HORDE_GROUND });
    data[20].waveMobs.push_back({ NPC_CRYPT , 5486.477f, -2371.984f, 1461.615f, 4.782275f, HORDE_GROUND });
    data[20].waveMobs.push_back({ NPC_NECRO , 5471.71f, -2375.949f, 1461.215f, 5.449273f, HORDE_GROUND });
    data[20].waveMobs.push_back({ NPC_NECRO , 5474.819f, -2372.617f, 1460.952f, 5.71139f, HORDE_GROUND });

    // Wave 4: 6 Crypt Fiends, 2 Necromancer, 6 Gargoyles
    data[21].waveTimer = 155000;
    data[21].waveMobs.push_back({ NPC_CRYPT , 5465.716f, -2379.399f, 1463.71f, 5.395523f, HORDE_GROUND });
    data[21].waveMobs.push_back({ NPC_CRYPT , 5478.875f, -2368.178f, 1461.07f, 5.244267f, HORDE_GROUND });
    data[21].waveMobs.push_back({ NPC_CRYPT , 5467.596f, -2387.193f, 1461.501f, 6.024768f, HORDE_GROUND });
    data[21].waveMobs.push_back({ NPC_CRYPT , 5473.881f, -2382.322f, 1461.298f, 5.24427f, HORDE_GROUND });
    data[21].waveMobs.push_back({ NPC_CRYPT , 5481.042f, -2376.943f, 1461.347f, 5.303872f, HORDE_GROUND });
    data[21].waveMobs.push_back({ NPC_CRYPT , 5487.553f, -2371.259f, 1461.737f, 5.521908f, HORDE_GROUND });
    data[21].waveMobs.push_back({ NPC_GARGO , 5793.838f, -2876.247f, 1563.766f, 4.629621f, HORDE_GARGOYLE_1 });
    data[21].waveMobs.push_back({ NPC_GARGO , 5816.996f, -2909.161f, 1582.068f, 2.111933f, HORDE_GARGOYLE_1 });
    data[21].waveMobs.push_back({ NPC_GARGO , 5777.432f, -2899.413f, 1604.322f, 1.758108f, HORDE_GARGOYLE_1 });
    data[21].waveMobs.push_back({ NPC_GARGO , 5770.996f, -2828.342f, 1594.328f, 2.268224f, HORDE_GARGOYLE_1 });
    data[21].waveMobs.push_back({ NPC_GARGO , 5781.946f, -2864.139f, 1593.854f, 4.82583f, HORDE_GARGOYLE_1 });
    data[21].waveMobs.push_back({ NPC_GARGO , 5784.716f, -2807.962f, 1567.109f, 4.379971f, HORDE_GARGOYLE_1 });
    data[21].waveMobs.push_back({ NPC_NECRO , 5471.47f, -2375.933f, 1461.281f, 5.452258f, HORDE_GROUND });
    data[21].waveMobs.push_back({ NPC_NECRO , 5474.262f, -2373.027f, 1460.936f, 5.371886f, HORDE_GROUND });

    // Wave 5: 4 Ghoul, 6 Abomination, 4 Necromancer
    data[22].waveTimer = 130000;
    data[22].waveMobs.push_back({ NPC_GHOUL , 5479.022f, -2389.681f, 1461.178f, 6.012395f, HORDE_GROUND });
    data[22].waveMobs.push_back({ NPC_GHOUL , 5482.383f, -2387.293f, 1461.526f, 5.92795f, HORDE_GROUND });
    data[22].waveMobs.push_back({ NPC_GHOUL , 5484.698f, -2385.112f, 1461.664f, 5.348127f, HORDE_GROUND });
    data[22].waveMobs.push_back({ NPC_GHOUL , 5488.692f, -2382.81f, 1462.057f, 5.213516f, HORDE_GROUND });
    data[22].waveMobs.push_back({ NPC_ABOMI , 5466.756f, -2380.586f, 1462.959f, 5.39338f, HORDE_GROUND });
    data[22].waveMobs.push_back({ NPC_ABOMI , 5479.783f, -2369.696f, 1461.108f, 5.244311f, HORDE_GROUND });
    data[22].waveMobs.push_back({ NPC_ABOMI , 5468.887f, -2388.767f, 1460.945f, 5.697079f, HORDE_GROUND });
    data[22].waveMobs.push_back({ NPC_ABOMI , 5475.438f, -2383.356f, 1461.37f, 5.501222f, HORDE_GROUND });
    data[22].waveMobs.push_back({ NPC_ABOMI , 5481.994f, -2378.48f, 1461.411f, 5.304682f, HORDE_GROUND });
    data[22].waveMobs.push_back({ NPC_ABOMI , 5486.682f, -2373.582f, 1461.661f, 4.779593f, HORDE_GROUND });
    data[22].waveMobs.push_back({ NPC_NECRO , 5472.101f, -2396.112f, 1460.305f, 6.18545f, HORDE_GROUND });
    data[22].waveMobs.push_back({ NPC_NECRO , 5475.1f, -2392.839f, 1460.541f, 6.10441f, HORDE_GROUND });
    data[22].waveMobs.push_back({ NPC_NECRO , 5491.19f, -2379.425f, 1462.319f, 5.106939f, HORDE_GROUND });
    data[22].waveMobs.push_back({ NPC_NECRO , 5493.767f, -2376.383f, 1462.851f, 5.014709f, HORDE_GROUND });

    // Wave 6: 8 Gargoyle, 1 Frost Wyrm
    data[23].waveTimer = 130000;
    data[23].waveMobs.push_back({ NPC_GARGO , 5387.046f, -2536.589f, 1561.265f, 6.042778f, HORDE_GARGOYLE_2 });
    data[23].waveMobs.push_back({ NPC_GARGO , 5384.106f, -2507.996f, 1532.068f, 5.923682f, HORDE_GARGOYLE_2 });
    data[23].waveMobs.push_back({ NPC_GARGO , 5434.01f, -2518.206f, 1504.208f, 5.396564f, HORDE_GARGOYLE_2 });
    data[23].waveMobs.push_back({ NPC_GARGO , 5477.957f, -2523.492f, 1531.497f, 4.311117f, HORDE_GARGOYLE_2 });
    data[23].waveMobs.push_back({ NPC_GARGO , 5425.072f, -2539.16f, 1527.672f, 5.892184f, HORDE_GARGOYLE_2 });
    data[23].waveMobs.push_back({ NPC_GARGO , 5454.331f, -2537.709f, 1518.542f, 5.234161f, HORDE_GARGOYLE_2 });
    data[23].waveMobs.push_back({ NPC_GARGO , 5403.739f, -2551.102f, 1545.229f, 6.21172f, HORDE_GARGOYLE_2 });
    data[23].waveMobs.push_back({ NPC_GARGO , 5453.157f, -2549.673f, 1545.859f, 0.1446965f, HORDE_GARGOYLE_2 });
    data[23].waveMobs.push_back({ NPC_FROST , 5448.235f, -2475.62f, 1468.523f, 4.756875f, HORDE_FROST_WYRM_2 });

    // Wave 7: 6 Ghoul, 4 Abomination, 1 Frost Wyrm
    data[24].waveTimer = 155000;
    data[24].waveMobs.push_back({ NPC_GHOUL , 5477.121f, -2390.861f, 1460.973f, 5.318035f, HORDE_GROUND });
    data[24].waveMobs.push_back({ NPC_GHOUL , 5480.119f, -2388.215f, 1461.374f, 5.134054f, HORDE_GROUND });
    data[24].waveMobs.push_back({ NPC_GHOUL , 5483.122f, -2384.594f, 1461.55f, 4.957835f, HORDE_GROUND });
    data[24].waveMobs.push_back({ NPC_GHOUL , 5488.021f, -2381.721f, 1461.909f, 5.215247f, HORDE_GROUND });
    data[24].waveMobs.push_back({ NPC_GHOUL , 5489.653f, -2378.407f, 1461.911f, 4.690397f, HORDE_GROUND });
    data[24].waveMobs.push_back({ NPC_GHOUL , 5494.47f, -2374.726f, 1463.068f, 5.456352f, HORDE_GROUND });
    data[24].waveMobs.push_back({ NPC_ABOMI , 5468.423f, -2387.873f, 1461.128f, 6.032522f, HORDE_GROUND });
    data[24].waveMobs.push_back({ NPC_ABOMI , 5474.42f, -2382.597f, 1461.382f, 5.506153f, HORDE_GROUND });
    data[24].waveMobs.push_back({ NPC_ABOMI , 5480.611f, -2377.919f, 1461.34f, 4.984337f, HORDE_GROUND });
    data[24].waveMobs.push_back({ NPC_ABOMI , 5488.219f, -2371.795f, 1461.817f, 5.519931f, HORDE_GROUND });
    data[24].waveMobs.push_back({ NPC_FROST , 5753.523f, -2701.919f, 1586.142f, 3.26914f, HORDE_FROST_WYRM_1 });

    // Wave 8: 4 Ghoul, 4 Abomination, 4 Crypt Fiends, 2 Banshee, 2 Necromancer
    data[25].waveTimer = 225000;
    data[25].waveMobs.push_back({ NPC_GHOUL , 5477.641f, -2389.262f, 1461.029f, 6.010409f, HORDE_GROUND });
    data[25].waveMobs.push_back({ NPC_GHOUL , 5480.969f, -2386.87f, 1461.401f, 5.930136f, HORDE_GROUND });
    data[25].waveMobs.push_back({ NPC_GHOUL , 5483.921f, -2384.053f, 1461.594f, 5.348127f, HORDE_GROUND });
    data[25].waveMobs.push_back({ NPC_GHOUL , 5486.846f, -2381.426f, 1461.63f, 4.79358f, HORDE_GROUND });
    data[25].waveMobs.push_back({ NPC_CRYPT , 5464.27f, -2379.535f, 1464.909f, 5.600142f, HORDE_GROUND });
    data[25].waveMobs.push_back({ NPC_CRYPT , 5469.777f, -2376.149f, 1461.464f, 5.479149f, HORDE_GROUND });
    data[25].waveMobs.push_back({ NPC_CRYPT , 5476.496f, -2370.759f, 1460.936f, 5.663713f, HORDE_GROUND });
    data[25].waveMobs.push_back({ NPC_CRYPT , 5481.59f, -2366.031f, 1461.301f, 5.178696f, HORDE_GROUND });
    data[25].waveMobs.push_back({ NPC_ABOMI , 5467.852f, -2387.439f, 1461.377f, 6.028671f, HORDE_GROUND });
    data[25].waveMobs.push_back({ NPC_ABOMI , 5474.733f, -2381.472f, 1461.333f, 5.869359f, HORDE_GROUND });
    data[25].waveMobs.push_back({ NPC_ABOMI , 5481.558f, -2376.589f, 1461.381f, 5.704444f, HORDE_GROUND });
    data[25].waveMobs.push_back({ NPC_ABOMI , 5486.307f, -2372.02f, 1461.601f, 4.787056f, HORDE_GROUND });
    data[25].waveMobs.push_back({ NPC_NECRO , 5470.347f, -2396.384f, 1460.145f, 5.841413f, HORDE_GROUND });
    data[25].waveMobs.push_back({ NPC_NECRO , 5492.644f, -2375.068f, 1462.704f, 4.602814f, HORDE_GROUND });
    data[25].waveMobs.push_back({ NPC_BANSH , 5473.68f, -2392.463f, 1460.439f, 6.102266f, HORDE_GROUND });
    data[25].waveMobs.push_back({ NPC_BANSH , 5489.835f, -2377.806f, 1461.933f, 4.684792f, HORDE_GROUND });

    // Kaz'rogal
    data[26].waveTimer = 0;
    data[26].waveMobs.push_back({ NPC_KAZROGAL , 5526.32f, -2449.313f, 1468.57f, 5.386355f, HORDE_GROUND });

    // Fourth boss
    // Wave 1: 6 Abomination, 6 Necromancer
    data[27].waveTimer = 130000;
    data[27].waveMobs.push_back({ NPC_ABOMI , 5466.154f, -2379.352f, 1463.502f, 5.385604f, HORDE_GROUND });
    data[27].waveMobs.push_back({ NPC_ABOMI , 5478.628f, -2368.868f, 1461.073f, 4.971114f, HORDE_GROUND });
    data[27].waveMobs.push_back({ NPC_ABOMI , 5467.629f, -2388.19f, 1461.427f, 5.534935f, HORDE_GROUND });
    data[27].waveMobs.push_back({ NPC_ABOMI , 5473.93f, -2382.598f, 1461.338f, 5.246795f, HORDE_GROUND });
    data[27].waveMobs.push_back({ NPC_ABOMI , 5480.403f, -2377.566f, 1461.326f, 4.987185f, HORDE_GROUND });
    data[27].waveMobs.push_back({ NPC_ABOMI , 5487.087f, -2372.053f, 1461.726f, 5.128849f, HORDE_GROUND });
    data[27].waveMobs.push_back({ NPC_NECRO , 5473.371f, -2393.286f, 1460.439f, 5.732042f, HORDE_GROUND });
    data[27].waveMobs.push_back({ NPC_NECRO , 5477.703f, -2389.388f, 1461.044f, 6.014513f, HORDE_GROUND });
    data[27].waveMobs.push_back({ NPC_NECRO , 5479.994f, -2387.888f, 1461.363f, 5.133204f, HORDE_GROUND });
    data[27].waveMobs.push_back({ NPC_NECRO , 5484.615f, -2383.193f, 1461.696f, 5.806313f, HORDE_GROUND });
    data[27].waveMobs.push_back({ NPC_NECRO , 5488.518f, -2380.592f, 1462.005f, 5.692625f, HORDE_GROUND });
    data[27].waveMobs.push_back({ NPC_NECRO , 5491.366f, -2377.594f, 1462.175f, 5.57586f, HORDE_GROUND });

    // Wave 2: 5 Ghoul, 8 Gargoyle, 1 Frost Wyrm
    data[28].waveTimer = 190000;
    data[28].waveMobs.push_back({ NPC_GHOUL , 5475.592f, -2388.769f, 1460.793f, 5.320674f, HORDE_GROUND });
    data[28].waveMobs.push_back({ NPC_GHOUL , 5478.963f, -2385.899f, 1461.267f, 5.925934f, HORDE_GROUND });
    data[28].waveMobs.push_back({ NPC_GHOUL , 5482.61f, -2382.277f, 1461.496f, 4.955923f, HORDE_GROUND });
    data[28].waveMobs.push_back({ NPC_GHOUL , 5486.747f, -2379.534f, 1461.571f, 5.217056f, HORDE_GROUND });
    data[28].waveMobs.push_back({ NPC_GHOUL , 5489.814f, -2375.995f, 1461.873f, 5.570403f, HORDE_GROUND });
    data[28].waveMobs.push_back({ NPC_FROST , 5423.374f, -2540.911f, 1531.116f, 5.633668f, HORDE_FROST_WYRM_2 });
    data[28].waveMobs.push_back({ NPC_GARGO , 5407.959f, -2504.334f, 1492.227f, 5.539972f, HORDE_GARGOYLE_2 });
    data[28].waveMobs.push_back({ NPC_GARGO , 5436.766f, -2518.686f, 1505.259f, 4.820778f, HORDE_GARGOYLE_2 });
    data[28].waveMobs.push_back({ NPC_GARGO , 5393.467f, -2506.886f, 1519.251f, 5.674838f, HORDE_GARGOYLE_2 });
    data[28].waveMobs.push_back({ NPC_GARGO , 5433.89f, -2540.067f, 1530.021f, 6.16795f, HORDE_GARGOYLE_2 });
    data[28].waveMobs.push_back({ NPC_GARGO , 5419.362f, -2545.74f, 1539.552f, 6.049433f, HORDE_GARGOYLE_2 });
    data[28].waveMobs.push_back({ NPC_GARGO , 5449.623f, -2546.096f, 1536.206f, 4.406961f, HORDE_GARGOYLE_2 });
    data[28].waveMobs.push_back({ NPC_GARGO , 5389.705f, -2538.281f, 1551.524f, 5.749064f, HORDE_GARGOYLE_2 });
    data[28].waveMobs.push_back({ NPC_GARGO , 5435.055f, -2556.646f, 1556.268f, 0.003030294f, HORDE_GARGOYLE_2 });

    // Wave 3: 6 Ghoul, 8 Infernal
    data[29].waveTimer = 190000;
    data[29].waveMobs.push_back({ NPC_GHOUL , 5472.351f, -2392.977f, 1460.335f, 5.548223f, HORDE_GROUND });
    data[29].waveMobs.push_back({ NPC_GHOUL , 5476.307f, -2389.896f, 1460.866f, 5.323418f, HORDE_GROUND });
    data[29].waveMobs.push_back({ NPC_GHOUL , 5479.439f, -2387.299f, 1461.308f, 5.14465f, HORDE_GROUND });
    data[29].waveMobs.push_back({ NPC_GHOUL , 5483.249f, -2383.182f, 1461.55f, 5.348069f, HORDE_GROUND });
    data[29].waveMobs.push_back({ NPC_GHOUL , 5486.856f, -2380.695f, 1461.606f, 4.79068f, HORDE_GROUND });
    data[29].waveMobs.push_back({ NPC_GHOUL , 5490.057f, -2377.032f, 1461.997f, 5.110533f, HORDE_GROUND });
    // Infernal spawns defined in SpawnWaveInfernals()

    // Wave 4: 6 Fel Stalker, 8 Infernal
    data[30].waveTimer = 190000;
    data[30].waveMobs.push_back({ NPC_STALK , 5473.774f, -2394.408f, 1460.415f, 5.552334f, HORDE_GROUND });
    data[30].waveMobs.push_back({ NPC_STALK , 5478.53f, -2389.692f, 1461.149f, 6.016419f, HORDE_GROUND });
    data[30].waveMobs.push_back({ NPC_STALK , 5482.058f, -2387.323f, 1461.471f, 5.932327f, HORDE_GROUND });
    data[30].waveMobs.push_back({ NPC_STALK , 5483.522f, -2385.227f, 1461.57f, 4.947872f, HORDE_GROUND });
    data[30].waveMobs.push_back({ NPC_STALK , 5488.204f, -2382.279f, 1461.992f, 5.21784f, HORDE_GROUND });
    data[30].waveMobs.push_back({ NPC_STALK , 5492.172f, -2378.038f, 1462.324f, 5.570403f, HORDE_GROUND });
    // Infernal spawns defined in SpawnWaveInfernals()

    // Wave 5: 4 Abomination, 6 Fel Stalker, 4 Necromancer
    data[31].waveTimer = 130000;
    data[31].waveMobs.push_back({ NPC_ABOMI , 5467.105f, -2387.462f, 1461.696f, 5.695487f, HORDE_GROUND });
    data[31].waveMobs.push_back({ NPC_ABOMI , 5473.926f, -2381.282f, 1461.319f, 5.873254f, HORDE_GROUND });
    data[31].waveMobs.push_back({ NPC_ABOMI , 5480.058f, -2376.604f, 1461.299f, 4.989628f, HORDE_GROUND });
    data[31].waveMobs.push_back({ NPC_ABOMI , 5487.268f, -2370.727f, 1461.689f, 5.517107f, HORDE_GROUND });
    data[31].waveMobs.push_back({ NPC_STALK , 5472.752f, -2392.399f, 1460.368f, 6.104362f, HORDE_GROUND });
    data[31].waveMobs.push_back({ NPC_STALK , 5476.255f, -2389.728f, 1460.871f, 5.320654f, HORDE_GROUND });
    data[31].waveMobs.push_back({ NPC_STALK , 5479.854f, -2386.684f, 1461.329f, 5.481168f, HORDE_GROUND });
    data[31].waveMobs.push_back({ NPC_STALK , 5483.469f, -2383.013f, 1461.553f, 5.341039f, HORDE_GROUND });
    data[31].waveMobs.push_back({ NPC_STALK , 5486.982f, -2380.809f, 1461.608f, 4.786619f, HORDE_GROUND });
    data[31].waveMobs.push_back({ NPC_STALK , 5490.145f, -2377.011f, 1462.003f, 5.108684f, HORDE_GROUND });
    data[31].waveMobs.push_back({ NPC_NECRO , 5463.578f, -2379.832f, 1465.341f, 5.44682f, HORDE_GROUND });
    data[31].waveMobs.push_back({ NPC_NECRO , 5469.558f, -2375.892f, 1461.789f, 5.478561f, HORDE_GROUND });
    data[31].waveMobs.push_back({ NPC_NECRO , 5475.502f, -2370.343f, 1460.9f, 5.321494f, HORDE_GROUND });
    data[31].waveMobs.push_back({ NPC_NECRO , 5480.951f, -2365.24f, 1461.213f, 4.898064f, HORDE_GROUND });

    // Wave 6: 6 Necromancer, 6 Banshee
    data[32].waveTimer = 155000;
    data[32].waveMobs.push_back({ NPC_NECRO , 5476.288f, -2389.152f, 1460.853f, 6.016577f, HORDE_GROUND });
    data[32].waveMobs.push_back({ NPC_NECRO , 5479.657f, -2386.318f, 1461.322f, 5.93025f, HORDE_GROUND });
    data[32].waveMobs.push_back({ NPC_NECRO , 5483.331f, -2383.055f, 1461.535f, 5.344517f, HORDE_GROUND });
    data[32].waveMobs.push_back({ NPC_NECRO , 5486.864f, -2380.052f, 1461.589f, 4.788543f, HORDE_GROUND });
    data[32].waveMobs.push_back({ NPC_NECRO , 5490.023f, -2376.415f, 1461.943f, 5.105319f, HORDE_GROUND });
    data[32].waveMobs.push_back({ NPC_NECRO , 5492.843f, -2373.418f, 1462.74f, 5.014687f, HORDE_GROUND });
    data[32].waveMobs.push_back({ NPC_BANSH , 5470.768f, -2381.377f, 1461.585f, 5.311578f, HORDE_GROUND });
    data[32].waveMobs.push_back({ NPC_BANSH , 5474.223f, -2378.596f, 1461.076f, 5.176897f, HORDE_GROUND });
    data[32].waveMobs.push_back({ NPC_BANSH , 5477.382f, -2375.48f, 1461.14f, 5.055127f, HORDE_GROUND });
    data[32].waveMobs.push_back({ NPC_BANSH , 5480.378f, -2372.383f, 1461.161f, 5.261695f, HORDE_GROUND });
    data[32].waveMobs.push_back({ NPC_BANSH , 5483.126f, -2369.863f, 1461.287f, 4.865324f, HORDE_GROUND });
    data[32].waveMobs.push_back({ NPC_BANSH , 5486.186f, -2366.258f, 1461.716f, 5.463794f, HORDE_GROUND });

    // Wave 7: 2 Ghoul, 2 Crypt Fiends, 2 Fel Stalker, 8 Infernal
    data[33].waveTimer = 190000;
    data[33].waveMobs.push_back({ NPC_GHOUL , 5483.201f, -2383.262f, 1461.534f, 4.941727f, HORDE_GROUND });
    data[33].waveMobs.push_back({ NPC_GHOUL , 5487.841f, -2380.242f, 1461.783f, 5.694742f, HORDE_GROUND });
    data[33].waveMobs.push_back({ NPC_CRYPT , 5473.733f, -2381.661f, 1461.289f, 5.501221f, HORDE_GROUND });
    data[33].waveMobs.push_back({ NPC_CRYPT , 5480.044f, -2376.513f, 1461.297f, 4.989067f, HORDE_GROUND });
    data[33].waveMobs.push_back({ NPC_STALK , 5467.372f, -2387.183f, 1461.729f, 6.026359f, HORDE_GROUND });
    data[33].waveMobs.push_back({ NPC_STALK , 5486.382f, -2370.961f, 1461.59f, 4.782901f, HORDE_GROUND });
    // Infernal spawns defined in SpawnWaveInfernals()

    // Wave 8: 4 Abomination, 4 Crypt Fiends, 2 Banshee, 2 Necromancer, 4 Fel Stalker
    data[34].waveTimer = 225000;
    data[34].waveMobs.push_back({ NPC_ABOMI , 5464.765f, -2379.698f, 1464.398f, 5.911384f, HORDE_GROUND });
    data[34].waveMobs.push_back({ NPC_ABOMI , 5470.104f, -2376.372f, 1461.377f, 5.478453f, HORDE_GROUND });
    data[34].waveMobs.push_back({ NPC_ABOMI , 5476.636f, -2370.652f, 1460.942f, 5.659968f, HORDE_GROUND });
    data[34].waveMobs.push_back({ NPC_ABOMI , 5481.879f, -2366.252f, 1461.323f, 5.175017f, HORDE_GROUND });
    data[34].waveMobs.push_back({ NPC_CRYPT , 5468.371f, -2387.287f, 1461.241f, 6.022331f, HORDE_GROUND });
    data[34].waveMobs.push_back({ NPC_CRYPT , 5480.369f, -2377.551f, 1461.323f, 4.98816f, HORDE_GROUND });
    data[34].waveMobs.push_back({ NPC_CRYPT , 5487.712f, -2371.494f, 1461.771f, 5.522707f, HORDE_GROUND });
    data[34].waveMobs.push_back({ NPC_CRYPT , 5474.863f, -2381.631f, 1461.36f, 5.871073f, HORDE_GROUND });
    data[34].waveMobs.push_back({ NPC_BANSH , 5479.14f, -2401.899f, 1460.762f, 5.727021f, HORDE_GROUND });
    data[34].waveMobs.push_back({ NPC_BANSH , 5496.938f, -2382.928f, 1462.721f, 5.592558f, HORDE_GROUND });
    data[34].waveMobs.push_back({ NPC_NECRO , 5470.226f, -2397.017f, 1460.11f, 5.752703f, HORDE_GROUND });
    data[34].waveMobs.push_back({ NPC_NECRO , 5494.31f, -2374.414f, 1463.051f, 5.454998f, HORDE_GROUND });
    data[34].waveMobs.push_back({ NPC_STALK , 5481.617f, -2386.691f, 1461.459f, 5.92197f, HORDE_GROUND });
    data[34].waveMobs.push_back({ NPC_STALK , 5484.139f, -2383.701f, 1461.605f, 5.337486f, HORDE_GROUND });
    data[34].waveMobs.push_back({ NPC_STALK , 5487.897f, -2381.313f, 1461.88f, 5.212841f, HORDE_GROUND });
    data[34].waveMobs.push_back({ NPC_STALK , 5489.723f, -2378.428f, 1461.897f, 4.688016f, HORDE_GROUND });

    // Azgalor
    data[35].waveTimer = 0;
    data[35].waveMobs.push_back({ NPC_AZGALOR , 5526.623f, -2449.26f, 1468.583f, 5.360693f, HORDE_GROUND });

    return data;
}

std::array<HyjalWave, MAX_WAVES> instance_mount_hyjal::m_hyjalWavesData = InitHyjalWaves();

std::array<InvasionWave, MAX_INVASION_WAVES> InitInvasionWaves()
{
    std::array<InvasionWave, MAX_INVASION_WAVES> data;
    // Alliance Invasion
    // Wave 0: Building Triggers (These creatures do not have any creation packets in sniffs (they only appear as targets of Ghouls), so coordinates are guesswork based on the Ghouls position)
    data[0].invasionTimer = 10000;
    data[0].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 4998.445f, -1774.231f, 1329.802f, 0, PATH_NONE });
    data[0].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5073.363f, -1723.617f, 1327.12f, 3.329434f, PATH_NONE });
    data[0].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5094.47f, -1770.405f, 1323.583f, 2.300560f, PATH_NONE });
    data[0].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5093.89f, -1800.052f, 1323.097f, 3.578211f, PATH_NONE });
    data[0].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 4998.343f, -1757.482f, 1331.24f, 0, PATH_NONE });
    data[0].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5115.442f, -1767.191f, 1331.234f, 1.638259f, PATH_NONE });
    data[0].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5069.346f, -1819.29f, 1328.154f, 0, PATH_NONE });
    data[0].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 4942.818f, -1887.266f, 1326.585f, 0, PATH_NONE });
    data[0].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 4997.396f, -1767.484f, 1329.677f, 0, PATH_NONE });
    data[0].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5111.535f, -1850.402f, 1333.595f, 1.273658f, PATH_NONE });
    data[0].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 4947.428f, -1893.398f, 1326.585f, 0, PATH_NONE });
    data[0].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5119.775f, -1741.85f, 1334.72f, 4.146245f, PATH_NONE });
    data[0].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5099.896f, -1804.113f, 1322.151f, 3.903546f, PATH_NONE });
    data[0].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5100.597f, -1853.859f, 1331.938f, 2.396782f, PATH_NONE });
    data[0].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5092.283f, -1777.014f, 1322.115f, 2.037452f, PATH_NONE });
    data[0].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5194.689f, -1775.331f, 1342.855f, 3.223399f, PATH_NONE });
    data[0].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5072.396f, -1839.01f, 1328.062f, 0, PATH_NONE });
    data[0].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5190.926f, -1759.95f, 1342.667f, 3.553266f, PATH_NONE });
    data[0].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5020.804f, -1843.785f, 1322.097f, 1.179404f, PATH_NONE });
    data[0].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5130.223f, -1800.01f, 1333.659f, 0, PATH_NONE });
    data[0].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 4990.336f, -1783.502f, 1329.761f, 5.228124f, PATH_NONE });
    data[0].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 4998.324f, -1779.318f, 1330.369f, 0, PATH_NONE });
    // Gargoyles (ToDo: They should cast Gargoyle Strike s.31664 on Building c.18304)
    data[0].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5188.17f, -2116.62f, 1292.18f, 0.639109f, PATH_NONE });
    data[0].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5199.75f, -2131.45f, 1283.92f, 0.116808f, PATH_NONE });
    data[0].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5183.09f, -2146.65f, 1294.17f, 4.625f, PATH_NONE });
    data[0].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5318.61f, -2195.55f, 1278.08f, 2.33164f, PATH_NONE });
    data[0].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5342.73f, -2186.79f, 1278.72f, 1.73473f, PATH_NONE });
    data[0].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5339.59f, -2106.24f, 1299.42f, 1.3813f, PATH_NONE });
    data[0].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5286.01f, -2042.93f, 1301.67f, 5.59103f, PATH_NONE });
    data[0].waveMobs.push_back({ NPC_GARGO , 5337.28f, -2162.094f, 1291.234f, 4.607669f, PATH_NONE });
    data[0].waveMobs.push_back({ NPC_GARGO , 5342.963f, -2090.72f, 1313.236f, 2.164208f, PATH_NONE });
    data[0].waveMobs.push_back({ NPC_GARGO , 5305.663f, -2182.542f, 1288.539f, 4.782202f, PATH_NONE });
    data[0].waveMobs.push_back({ NPC_GARGO , 5216.649f, -2138.837f, 1308.115f, 1.850049f, PATH_NONE });
    data[0].waveMobs.push_back({ NPC_GARGO , 5214.606f, -2097.323f, 1305.896f, 4.712389f, PATH_NONE });
    data[0].waveMobs.push_back({ NPC_GARGO , 5179.757f, -2159.528f, 1305.688f, 6.021386f, PATH_NONE });
    data[0].waveMobs.push_back({ NPC_GARGO , 5287.034f, -2051.174f, 1324.37f, 5.846853f, PATH_NONE });
    
    // Wave 1: 15 Ghouls 16:26:03.291
    data[1].invasionTimer = 1640;
    data[1].waveMobs.push_back({ NPC_GHOUL , 4959.513f, -1504.006f, 1328.48f, 3.966487f, ALLY_GROUND });
    data[1].waveMobs.push_back({ NPC_GHOUL , 4952.485f, -1498.039f, 1329.098f, 4.099744f, ALLY_GROUND });
    data[1].waveMobs.push_back({ NPC_GHOUL , 4948.812f, -1513.164f, 1327.21f, 3.996068f, ALLY_GROUND });
    data[1].waveMobs.push_back({ NPC_GHOUL , 4942.966f, -1506.395f, 1327.293f, 4.080248f, ALLY_GROUND });
    data[1].waveMobs.push_back({ NPC_GHOUL , 4918.432f, -1523.334f, 1329.023f, 4.306471f, ALLY_GROUND });
    data[1].waveMobs.push_back({ NPC_GHOUL , 4993.337f, -1480.136f, 1332.768f, 3.72974f, ALLY_GROUND });
    data[1].waveMobs.push_back({ NPC_GHOUL , 4925.223f, -1519.402f, 1327.68f, 4.222555f, ALLY_GROUND });
    data[1].waveMobs.push_back({ NPC_GHOUL , 4932.335f, -1528.784f, 1326.908f, 4.063382f, ALLY_GROUND });
    data[1].waveMobs.push_back({ NPC_GHOUL , 4927.172f, -1505.237f, 1328.761f, 4.318649f, ALLY_GROUND });
    data[1].waveMobs.push_back({ NPC_GHOUL , 4984.013f, -1500.244f, 1328.77f, 4.006137f, ALLY_GROUND });
    data[1].waveMobs.push_back({ NPC_GHOUL , 4921.589f, -1510.324f, 1329.124f, 4.342402f, ALLY_GROUND });
    data[1].waveMobs.push_back({ NPC_GHOUL , 4937.992f, -1522.938f, 1326.95f, 4.004364f, ALLY_GROUND });
    data[1].waveMobs.push_back({ NPC_GHOUL , 4952.936f, -1510.416f, 1327.974f, 3.141593f, ALLY_GROUND });
    data[1].waveMobs.push_back({ NPC_GHOUL , 4933.112f, -1513.282f, 1327.234f, 4.252195f, ALLY_GROUND });
    data[1].waveMobs.push_back({ NPC_GHOUL , 4970.478f, -1490.625f, 1331.222f, 3.141593f, ALLY_GROUND });

    // Wave 2: 1 Ghoul 16:26:04.931
    data[2].invasionTimer = 813;
    data[2].waveMobs.push_back({ NPC_GHOUL , 4953.732f, -1499.917f, 1328.826f, 4.069797f, ALLY_GROUND });

    // Wave 3: 4 Ghouls 16:26:05.744
    data[3].invasionTimer = 406;
    data[3].waveMobs.push_back({ NPC_GHOUL , 5008.219f, -1488.436f, 1329.19f, 4.164979f, ALLY_GROUND });
    data[3].waveMobs.push_back({ NPC_GHOUL , 5008.95f, -1479.773f, 1331.248f, 3.304493f, ALLY_GROUND });
    data[3].waveMobs.push_back({ NPC_GHOUL , 5004.857f, -1469.25f, 1333.713f, 3.788143f, ALLY_GROUND });
    data[3].waveMobs.push_back({ NPC_GHOUL , 4994.542f, -1483.752f, 1331.932f, 4.05802f, ALLY_GROUND });

    // Wave 4: 5 Ghouls, 3 Crypt Fiends, 3 Abominations 16:26:06.150
    data[4].invasionTimer = 813;
    data[4].waveMobs.push_back({ NPC_GHOUL , 4995.912f, -1491.933f, 1329.998f, 4.669151f, ALLY_GROUND });
    data[4].waveMobs.push_back({ NPC_GHOUL , 4986.95f, -1483.493f, 1332.586f, 3.978122f, ALLY_GROUND });
    data[4].waveMobs.push_back({ NPC_GHOUL , 4981.944f, -1494.035f, 1330.209f, 3.39514f, ALLY_GROUND });
    data[4].waveMobs.push_back({ NPC_GHOUL , 4936.42f, -1515.542f, 1326.677f, 4.097023f, ALLY_GROUND });
    data[4].waveMobs.push_back({ NPC_GHOUL , 4948.683f, -1506.477f, 1327.407f, 4.07044f, ALLY_GROUND });
    data[4].waveMobs.push_back({ NPC_CRYPT , 4958.754f, -1496.274f, 1330.116f, 3.99707f, ALLY_GROUND });
    data[4].waveMobs.push_back({ NPC_CRYPT , 5032.708f, -1465.489f, 1333.672f, 3.293118f, ALLY_GROUND });
    data[4].waveMobs.push_back({ NPC_CRYPT , 5036.025f, -1490.918f, 1333.047f, 4.211451f, ALLY_GROUND });
    data[4].waveMobs.push_back({ NPC_ABOMI , 4927.294f, -1508.408f, 1328.424f, 4.272439f, ALLY_GROUND });
    data[4].waveMobs.push_back({ NPC_ABOMI , 5008.835f, -1473.01f, 1332.708f, 3.891751f, ALLY_GROUND });
    data[4].waveMobs.push_back({ NPC_ABOMI , 5013.125f, -1492.084f, 1328.675f, 3.476192f, ALLY_GROUND });

    // Wave 5: 1 Crypt Fiend, 1 Abomination 16:26:06.963
    data[5].invasionTimer = 406;
    data[5].waveMobs.push_back({ NPC_CRYPT , 4966.799f, -1504.602f, 1329.217f, 3.994628f, ALLY_GROUND });
    data[5].waveMobs.push_back({ NPC_ABOMI , 4936.284f, -1522.261f, 1326.717f, 4.082936f, ALLY_GROUND });

    // Wave 6: 10 Ghouls 16:26:07.369
    data[6].invasionTimer = 0;
    data[6].waveMobs.push_back({ NPC_GHOUL , 4934.046f, -1520.496f, 1326.661f, 4.08217f, ALLY_GROUND });
    data[6].waveMobs.push_back({ NPC_GHOUL , 4988.24f, -1492.464f, 1330.586f, 3.743444f, ALLY_GROUND });
    data[6].waveMobs.push_back({ NPC_GHOUL , 4979.866f, -1489.607f, 1331.271f, 3.136776f, ALLY_GROUND });
    data[6].waveMobs.push_back({ NPC_GHOUL , 4962.981f, -1494.804f, 1330.432f, 4.013216f, ALLY_GROUND });
    data[6].waveMobs.push_back({ NPC_GHOUL , 4972.104f, -1500.513f, 1329.467f, 4.032326f, ALLY_GROUND });
    data[6].waveMobs.push_back({ NPC_GHOUL , 4999.812f, -1486.4f, 1330.598f, 3.856842f, ALLY_GROUND });
    data[6].waveMobs.push_back({ NPC_GHOUL , 5000.115f, -1477.628f, 1332.576f, 3.257251f, ALLY_GROUND });
    data[6].waveMobs.push_back({ NPC_GHOUL , 5018.166f, -1471.797f, 1332.521f, 4.274553f, ALLY_GROUND });
    data[6].waveMobs.push_back({ NPC_GHOUL , 5017.789f, -1482.265f, 1330.519f, 5.945532f, ALLY_GROUND });
    data[6].waveMobs.push_back({ NPC_GHOUL , 5021.02f, -1493.137f, 1329.567f, 3.286933f, ALLY_GROUND });

    // Horde Invasion
    // Wave 0: Building Triggers (These creatures do not have any creation packets in sniffs (they only appear as targets of Ghouls), so coordinates are guesswork based on the Ghouls position)
    data[7].invasionTimer = 10000;
    data[7].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5542.932f, -2687.652f, 1481.41f, 0, PATH_NONE });
    data[7].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5395.824f, -2869.261f, 1512.461f, 0, PATH_NONE });
    data[7].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5533.478f, -2650.392f, 1480.618f, 0, PATH_NONE });
    data[7].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5200.144f, -3029.1f, 1569.392f, 0, PATH_NONE });
    data[7].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5438.519f, -2687.934f, 1486.425f, 0, PATH_NONE });
    data[7].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5396.678f, -2875.943f, 1512.498f, 0, PATH_NONE });
    data[7].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5416.667f, -2750.126f, 1486.455f, 0, PATH_NONE });
    data[7].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5361.832f, -2975.182f, 1538.945f, 0, PATH_NONE });
    data[7].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5446.119f, -2704.476f, 1486.576f, 0, PATH_NONE });
    data[7].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5343.821f, -2962.491f, 1537.45f, 0, PATH_NONE });
    data[7].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5567.274f, -2768.432f, 1496.219f, 0, PATH_NONE });
    data[7].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5443.451f, -2698.953f, 1486.15f, 0, PATH_NONE });
    data[7].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5514.919f, -2805.777f, 1499.485f, 0, PATH_NONE });
    data[7].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5515.058f, -2621.353f, 1484.587f, 0, PATH_NONE });
    data[7].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5580.915f, -2624.289f, 1488.575f, 0, PATH_NONE });
    data[7].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5379.497f, -2833.334f, 1513.131f, 0, PATH_NONE });
    data[7].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5537.551f, -2807.92f, 1498.782f, 0, PATH_NONE });
    data[7].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5388.882f, -2845.648f, 1513.24f, 0, PATH_NONE });
    data[7].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5504.345f, -2960.508f, 1538.28f, 0, PATH_NONE });
    data[7].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5532.271f, -2677.553f, 1479.757f, 0, PATH_NONE });
    data[7].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5589.136f, -2739.074f, 1494.211f, 0, PATH_NONE });
    data[7].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5497.021f, -2987.404f, 1539.611f, 0, PATH_NONE });
    data[7].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5397.396f, -2864.063f, 1512.373f, 0, PATH_NONE });
    data[7].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5426.563f, -3003.045f, 1550.946f, 0, PATH_NONE });
    data[7].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5600.729f, -2694.189f, 1494.466f, 0, PATH_NONE });
    data[7].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5336.38f, -3107.861f, 1582.641f, 0, PATH_NONE });
    data[7].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5427.288f, -2732.209f, 1485.95f, 0, PATH_NONE });
    data[7].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5402.469f, -3007.309f, 1549.578f, 0, PATH_NONE });
    data[7].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5398.504f, -2882.074f, 1513.832f, 0, PATH_NONE });
    data[7].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5361.865f, -2833.854f, 1511.894f, 0, PATH_NONE });
    data[7].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5394.144f, -2847.396f, 1512.373f, 0, PATH_NONE });
    data[7].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5319.413f, -2969.343f, 1537.38f, 0, PATH_NONE });
    data[7].waveMobs.push_back({ NPC_BUILDING_TRIGGER , 5191.284f, -3018.443f, 1569.959f, 0, PATH_NONE });

    // Wave 1: 16 Ghouls 21:16:58.510
    data[8].invasionTimer = 11000;
    data[8].waveMobs.push_back({ NPC_GHOUL , 5500.83f, -2394.47f, 1464.087f, 5.946418f, HORDE_GROUND });
    data[8].waveMobs.push_back({ NPC_GHOUL , 5492.911f, -2402.083f, 1462.72f, 4.131364f, HORDE_GROUND });
    data[8].waveMobs.push_back({ NPC_GHOUL , 5485.671f, -2409.407f, 1461.317f, 0.2625881f, HORDE_GROUND });
    data[8].waveMobs.push_back({ NPC_GHOUL , 5477.696f, -2408.627f, 1459.908f, 0.1944438f, HORDE_GROUND });
    data[8].waveMobs.push_back({ NPC_GHOUL , 5485.719f, -2398.325f, 1461.7f, 6.212689f, HORDE_GROUND });
    data[8].waveMobs.push_back({ NPC_GHOUL , 5501.271f, -2382.852f, 1463.596f, 5.477748f, HORDE_GROUND });
    data[8].waveMobs.push_back({ NPC_GHOUL , 5492.362f, -2389.236f, 1462.159f, 4.535504f, HORDE_GROUND });
    data[8].waveMobs.push_back({ NPC_GHOUL , 5472.314f, -2399.513f, 1459.85f, 5.898592f, HORDE_GROUND });
    data[8].waveMobs.push_back({ NPC_GHOUL , 5466.671f, -2390.855f, 1461.081f, 6.096422f, HORDE_GROUND });
    data[8].waveMobs.push_back({ NPC_GHOUL , 5477.055f, -2385.983f, 1461.062f, 5.942765f, HORDE_GROUND });
    data[8].waveMobs.push_back({ NPC_GHOUL , 5487.516f, -2380.031f, 1461.589f, 5.205697f, HORDE_GROUND });
    data[8].waveMobs.push_back({ NPC_GHOUL , 5495.065f, -2375.436f, 1462.816f, 5.458581f, HORDE_GROUND });
    data[8].waveMobs.push_back({ NPC_GHOUL , 5490.469f, -2366.381f, 1462.752f, 5.018064f, HORDE_GROUND });
    data[8].waveMobs.push_back({ NPC_GHOUL , 5480.812f, -2372.667f, 1461.195f, 5.642497f, HORDE_GROUND });
    data[8].waveMobs.push_back({ NPC_GHOUL , 5468.748f, -2381.24f, 1462.375f, 5.911043f, HORDE_GROUND });
    data[8].waveMobs.push_back({ NPC_GHOUL , 5481.208f, -2392.753f, 1461.25f, 5.585051f, HORDE_GROUND });

    // Wave 2: 1 Infernal Target (Its purpose is unknown) 21:17:09.459
    data[9].invasionTimer = 30000;
    data[9].waveMobs.push_back({ NPC_INFERNAL_TARGET , 5507.595f, -2388.241f, 1465.027f, 5.986479f, PATH_NONE });

    // Wave 3: 4 Crypt Fiends, 1 Abominations  21:17:39.500
    data[10].invasionTimer = 400;
    data[10].waveMobs.push_back({ NPC_CRYPT , 5489.01f, -2372.279f, 1461.898f, 5.090302f, HORDE_GROUND });
    data[10].waveMobs.push_back({ NPC_CRYPT , 5474.394f, -2377.111f, 1461.038f, 5.792884f, HORDE_GROUND });
    data[10].waveMobs.push_back({ NPC_CRYPT , 5483.502f, -2354.254f, 1461.632f, 5.0666f, HORDE_GROUND });
    data[10].waveMobs.push_back({ NPC_CRYPT , 5467.474f, -2360.167f, 1460.624f, 5.334022f, HORDE_GROUND });
    data[10].waveMobs.push_back({ NPC_ABOMI , 5509.863f, -2400.571f, 1465.539f, 4.343841f, HORDE_GROUND });

    // Wave 4: 3 Abominations 21:17:39.895
    data[11].invasionTimer = 68600;
    data[11].waveMobs.push_back({ NPC_ABOMI , 5492.67f, -2419.203f, 1462.786f, 0.4728702f, HORDE_GROUND });
    data[11].waveMobs.push_back({ NPC_ABOMI , 5496.631f, -2385.819f, 1462.68f, 5.017105f, HORDE_GROUND });
    data[11].waveMobs.push_back({ NPC_ABOMI , 5476.778f, -2395.596f, 1460.487f, 5.489543f, HORDE_GROUND });

    // Wave 5: 6 Ghouls 21:18:48.484
    data[12].invasionTimer = 400;
    data[12].waveMobs.push_back({ NPC_GHOUL , 5504.538f, -2402.227f, 1464.943f, 4.768365f, HORDE_GROUND });
    data[12].waveMobs.push_back({ NPC_GHOUL , 5486.165f, -2402.342f, 1461.789f, 0.0547436f, HORDE_GROUND });
    data[12].waveMobs.push_back({ NPC_GHOUL , 5492.584f, -2360.202f, 1466.426f, 5.279465f, HORDE_GROUND });
    data[12].waveMobs.push_back({ NPC_GHOUL , 5474.823f, -2368.184f, 1460.863f, 5.05423f, HORDE_GROUND });
    data[12].waveMobs.push_back({ NPC_GHOUL , 5460.088f, -2378.418f, 1469.516f, 5.630795f, HORDE_GROUND });
    data[12].waveMobs.push_back({ NPC_GHOUL , 5463.875f, -2402.761f, 1459.793f, 6.04456f, HORDE_GROUND });

    // Wave 6: 9 Ghouls, 1 Infernal Target 21:18:48.887
    data[13].invasionTimer = 0;
    data[13].waveMobs.push_back({ NPC_GHOUL , 5487.01f, -2394.076f, 1461.719f, 5.481238f, HORDE_GROUND });
    data[13].waveMobs.push_back({ NPC_GHOUL , 5507.824f, -2379.372f, 1466.459f, 4.632452f, HORDE_GROUND });
    data[13].waveMobs.push_back({ NPC_GHOUL , 5510.295f, -2393.919f, 1465.538f, 5.587405f, HORDE_GROUND });
    data[13].waveMobs.push_back({ NPC_GHOUL , 5469.084f, -2370.722f, 1461.52f, 5.736789f, HORDE_GROUND });
    data[13].waveMobs.push_back({ NPC_GHOUL , 5483.893f, -2342.307f, 1462.35f, 4.789686f, HORDE_GROUND });
    data[13].waveMobs.push_back({ NPC_GHOUL , 5469.628f, -2350.953f, 1460.199f, 5.039412f, HORDE_GROUND });
    data[13].waveMobs.push_back({ NPC_GHOUL , 5457.844f, -2364.158f, 1473.461f, 5.330197f, HORDE_GROUND });
    data[13].waveMobs.push_back({ NPC_GHOUL , 5464.175f, -2383.035f, 1464.405f, 5.969f, HORDE_GROUND });
    data[13].waveMobs.push_back({ NPC_GHOUL , 5506.33f, -2393.534f, 1464.782f, 5.750375f, HORDE_GROUND });
    data[13].waveMobs.push_back({ NPC_INFERNAL_TARGET , 5475.502f, -2433.635f, 1468.965f, 0.3316126f, PATH_NONE });

    // Night Elf "Invasion" (Happens during Archimonde fight)
    data[14].invasionTimer = 400;
    data[14].waveMobs.push_back({ NPC_GHOUL , 5382.503f, -3283.57f, 1623.326f, 2.740167f, NELF_GROUND });
    data[14].waveMobs.push_back({ NPC_GHOUL , 5371.666f, -3289.222f, 1624.289f, 4.990488f, NELF_GROUND });
    data[14].waveMobs.push_back({ NPC_GHOUL , 5372.948f, -3280.649f, 1622.653f, 5.130647f, NELF_GROUND });

    data[15].invasionTimer = 59600;
    data[15].waveMobs.push_back({ NPC_CRYPT , 5371.828f, -3261.135f, 1617.893f, 5.014826f, NELF_GROUND });

    data[16].invasionTimer = 0;
    data[16].waveMobs.push_back({ NPC_GHOUL , 5365.365f, -3284.092f, 1622.767f, 5.217218f, NELF_GROUND });
    data[16].waveMobs.push_back({ NPC_GHOUL , 5373.57f, -3272.718f, 1620.646f, 4.991662f, NELF_GROUND });
    data[16].waveMobs.push_back({ NPC_GHOUL , 5365.245f, -3274.99f, 1620.959f, 5.226511f, NELF_GROUND });
    data[16].waveMobs.push_back({ NPC_ABOMI , 5361.223f, -3266.657f, 1619.216f, 5.216855f, NELF_GROUND });
    return data; 
}

uint32 GetInfernalsPerWave(uint32 index)
{
    switch (index) // Infernals
    {
        case 29:
        case 30:
        case 33: return 8; // Horde waves
        default: return 0;
    }
}

uint8 GetRandomWavePath(WavePaths path)
{
    switch (path)
    {
        case ALLY_GROUND:
            return urand(1, 3);
        case HORDE_GROUND:
            return urand(7, 9);
        case HORDE_GARGOYLE_1:
            return urand(1, 3);
        case HORDE_GARGOYLE_2:
            return urand(7, 9);
        case HORDE_FROST_WYRM_1:
            return 1;
        case HORDE_FROST_WYRM_2:
            return 2;
        case NELF_GROUND:
            return urand(13, 15);
        case PATH_NONE:
        default:
            return 0;
    }
}

uint32 GetRunningPerWaveMobEntry(uint32 entry, uint32 waveIndex)
{
    switch (entry)
    {
        case NPC_FROST:
            if (waveIndex == 6) // this wave runs
                return true;
        case NPC_KAZROGAL:
        case NPC_AZGALOR:
            return false;
        default: return true;
    }
}

std::array<InvasionWave, MAX_INVASION_WAVES> instance_mount_hyjal::m_invasionWavesData = InitInvasionWaves();

void instance_mount_hyjal::Initialize()
{
    memset(&m_encounter, 0, sizeof(m_encounter));
    instance->GetVariableManager().SetVariableData(WORLD_STATE_MOUNT_HYJAL_ENABLE, true, 0, 0);
    instance->GetVariableManager().SetVariableData(WORLD_STATE_MOUNT_HYJAL_ENEMYCOUNT, true, 0, 0);
    instance->GetVariableManager().SetVariableData(WORLD_STATE_MOUNT_HYJAL_WAVES, true, 0, 0);
}

bool instance_mount_hyjal::IsEncounterInProgress() const
{
    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        if (m_encounter[i] == IN_PROGRESS) return true;

    return false;
}

void instance_mount_hyjal::OnPlayerEnter(Player* /*player*/)
{
    if (m_startCheck)
        return;

    m_startCheck = true;

    if (GetData(TYPE_AZGALOR) == DONE && GetData(TYPE_ARCHIMONDE) != DONE)
        SpawnArchimonde();

    if (GetData(TYPE_ANETHERON) != DONE)
    {
        Creature* jaina = GetSingleCreatureFromStorage(NPC_JAINA);
        if (!jaina || !jaina->IsAlive())
        {
            SpawnBase(BASE_ALLY, true);
            SpawnBase(BASE_HORDE, true);
        }
    }
    else if (GetData(TYPE_AZGALOR) != DONE)
    {
        Creature* thrall = GetSingleCreatureFromStorage(NPC_THRALL);
        if (!thrall || !thrall->IsAlive())
            SpawnBase(BASE_HORDE, true);
    }
}

// Workaround for possible proc spell 32044 which is even scarcely in sniffs unless sendall is done for all auras
void instance_mount_hyjal::OnPlayerDeath(Player* player)
{
    if (GetData(TYPE_ARCHIMONDE) != IN_PROGRESS)
        return;

    switch (player->getClass())
    {
        case CLASS_PRIEST:
        case CLASS_PALADIN:
        case CLASS_WARLOCK:
            player->CastSpell(nullptr, SPELL_SOUL_CHARGE_RED_CHARGE, TRIGGERED_OLD_TRIGGERED);
            break;
        case CLASS_MAGE:
        case CLASS_ROGUE:
        case CLASS_WARRIOR:
            player->CastSpell(nullptr, SPELL_SOUL_CHARGE_YELLOW_CHARGE, TRIGGERED_OLD_TRIGGERED);
            break;
        case CLASS_DRUID:
        case CLASS_SHAMAN:
        case CLASS_HUNTER:
            player->CastSpell(nullptr, SPELL_SOUL_CHARGE_GREEN_CHARGE, TRIGGERED_OLD_TRIGGERED);
            break;
    }
}

void instance_mount_hyjal::OnCreatureCreate(Creature* creature)
{
    switch (creature->GetEntry())
    {
        case NPC_JAINA:
        case NPC_THRALL:
        case NPC_ARCHIMONDE:
        case NPC_HYJAL_DESPAWN_TRIGGER:
            m_npcEntryGuidStore[creature->GetEntry()] = creature->GetObjectGuid();
            break;
        // Alliance base
        case NPC_GUARDIAN_WATER_ELEMENTAL:
        case NPC_ALLIANCE_PEASANT:
        case NPC_ALLIANCE_KNIGHT:
        case NPC_ALLIANCE_FOOTMAN:
        case NPC_ALLIANCE_RIFLEMAN:
        case NPC_ALLIANCE_PRIEST:
        case NPC_ALLIANCE_SORCERESS:
            m_baseSpawns[BASE_ALLY].push_back(creature->GetObjectGuid());
            break;
        // Horde base
        case NPC_DIRE_WOLF:
        case NPC_HORDE_HEADHUNTER:
        case NPC_HORDE_SHAMAN:
        case NPC_HORDE_GRUNT:
        case NPC_HORDE_HEALING_WARD:
        case NPC_TAUREN_WARRIOR:
        case NPC_HORDE_WITCH_DOCTOR:
        case NPC_HORDE_PEON:
            m_baseSpawns[BASE_HORDE].push_back(creature->GetObjectGuid());
            break;
        // Elf base
        case NPC_TYRANDE:
        case NPC_NELF_WISP:
        case NPC_DRUID_OF_THE_TALON:
        case NPC_DRUID_OF_THE_CLAW:
        case NPC_NELF_ANCIENT_PROT:
        case NPC_NELF_ANCIENT_OF_LORE:
        case NPC_NELF_ANCIENT_OF_WAR:
        case NPC_NELF_ARCHER:
        case NPC_NELF_HUNTRESS:
        case NPC_DRYAD:
            m_baseSpawns[BASE_ELF].push_back(creature->GetObjectGuid());
            break;
        // Dynamic Wave spawns
        case NPC_WINTERCHILL:
        case NPC_ANETHERON:
        case NPC_KAZROGAL:
        case NPC_AZGALOR:
        {
            switch (creature->GetEntry())
            {
                case NPC_WINTERCHILL: SetData(TYPE_WINTERCHILL, IN_PROGRESS); break;
                case NPC_ANETHERON: SetData(TYPE_ANETHERON, IN_PROGRESS); break;
                case NPC_KAZROGAL: SetData(TYPE_KAZROGAL, IN_PROGRESS); break;
                case NPC_AZGALOR: SetData(TYPE_AZGALOR, IN_PROGRESS); break;
            }
            // no break
        }
        case NPC_GHOUL:
        case NPC_NECRO:
        case NPC_ABOMI:
        case NPC_BANSH:
        case NPC_CRYPT:
        case NPC_GARGO:
        case NPC_FROST:
        case NPC_INFERNAL:
        case NPC_STALK:
        {
            ObjectGuid spawner = creature->GetSpawnerGuid(); // not loaded in map - guid is enough
            // Overrun mobs are not counted
            if (spawner && spawner.GetEntry() != NPC_HYJAL_DESPAWN_TRIGGER && instance->GetVariableManager().GetVariable(WORLD_STATE_MOUNT_HYJAL_ENABLE))
                m_waveSpawns.push_back(creature->GetObjectGuid());
            break;
        }
        case NPC_TOWERING_INFERNAL:
        case NPC_LESSER_DOOMGUARD:
            m_additionalSpawns.push_back(creature->GetObjectGuid());
            break;
        case NPC_INFERNAL_RELAY:
            m_infernalRelays.push_back(creature->GetObjectGuid());
            std::sort(m_infernalRelays.begin(), m_infernalRelays.end(), [](ObjectGuid const& a, ObjectGuid const& b)->bool
            { return a.GetCounter() < b.GetCounter(); });
            break;
        case NPC_INFERNAL_TARGET:
            m_infernalTargets.push_back(creature->GetObjectGuid());
            std::sort(m_infernalTargets.begin(), m_infernalTargets.end(), [](ObjectGuid const& a, ObjectGuid const& b)->bool
            { return a.GetCounter() < b.GetCounter(); });
            break;
    }
}

void instance_mount_hyjal::OnObjectCreate(GameObject* go)
{
    switch (go->GetEntry())
    {
        case GO_ANCIENT_GEM:
        {
            if (go->GetPositionY() > -2500.f)
                m_ancientGems[BASE_ALLY].push_back(go->GetObjectGuid());
            else
                m_ancientGems[BASE_HORDE].push_back(go->GetObjectGuid());
            break;
        }
        case GO_HORDE_ENCAMPMENT_PORTAL:
        {
            m_goEntryGuidStore[GO_HORDE_ENCAMPMENT_PORTAL] = go->GetObjectGuid();

            if (GetData(TYPE_ANETHERON) == DONE)
                go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
            break;
        }
        case GO_NIGHT_ELF_VILLAGE_PORTAL:
        {
            m_goEntryGuidStore[GO_NIGHT_ELF_VILLAGE_PORTAL] = go->GetObjectGuid();

            if (GetData(TYPE_AZGALOR) == DONE)
                go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
            break;
        }
        case GO_ROARING_FLAME:
        {
            if (go->GetPositionX() < 5360.f) // Alliance Base or Gargoyle Houses between ally and horde
                m_roaringFlames[BASE_ALLY].push_back(go->GetObjectGuid());
            else
                m_roaringFlames[BASE_HORDE].push_back(go->GetObjectGuid());
            break;
        }
    }
}

void instance_mount_hyjal::OnCreatureRespawn(Creature* creature)
{
    switch (creature->GetEntry())
    {
        case NPC_LESSER_DOOMGUARD: // need to despawn like wave mobs
        case NPC_GHOUL:
        case NPC_NECRO:
        case NPC_ABOMI:
        case NPC_BANSH:
        case NPC_CRYPT:
        case NPC_GARGO:
        case NPC_FROST:
        case NPC_INFERNAL:
        case NPC_STALK:
        {
            ObjectGuid spawner = creature->GetSpawnerGuid(); // not loaded in map - guid is enough
            if (!spawner) // Static spawns in Scourge base should never grant XP, loot or reputation
            {
                creature->SetNoXP(true);
                creature->SetNoLoot(true);
                creature->SetNoReputation(true);
                return;
            }
            break;
        }
        case NPC_INFERNAL_RELAY:
        case NPC_INFERNAL_TARGET:
            static_cast<CreatureAI*>(creature->AI())->SetDeathPrevention(true);
            creature->SetCanEnterCombat(false); // on retail they enter combat, likely for some guardian purposes for the infernals - on our end they bug out friendlies
            break;
        case NPC_ANCIENT_WISP:
            creature->SetCorpseDelay(5);
            break;
    }
}

void instance_mount_hyjal::OnCreatureEnterCombat(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_ARCHIMONDE:  SetData(TYPE_ARCHIMONDE, IN_PROGRESS);  break;
    }
}

void instance_mount_hyjal::OnCreatureDeath(Creature* creature)
{
    switch (creature->GetEntry())
    {
        case NPC_WINTERCHILL:
            SetData(TYPE_WINTERCHILL, DONE);
            if (Creature* jaina = GetSingleCreatureFromStorage(NPC_JAINA))
            {
                jaina->AI()->SendAIEvent(AI_EVENT_CUSTOM_D, jaina, jaina);
                jaina->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
            }
            m_waveSpawns.erase(std::remove(m_waveSpawns.begin(), m_waveSpawns.end(), creature->GetObjectGuid()), m_waveSpawns.end());
            break;
        case NPC_ANETHERON:
        {
            SetData(TYPE_ANETHERON, DONE);
            if (GameObject* orcGate = GetSingleGameObjectFromStorage(GO_HORDE_ENCAMPMENT_PORTAL))
                orcGate->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
            if (Creature* jaina = GetSingleCreatureFromStorage(NPC_JAINA))
            {
                jaina->AI()->SendAIEvent(AI_EVENT_CUSTOM_D, jaina, jaina);
                jaina->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
            }
            m_waveSpawns.erase(std::remove(m_waveSpawns.begin(), m_waveSpawns.end(), creature->GetObjectGuid()), m_waveSpawns.end());
            break;
        }
        case NPC_KAZROGAL:
            SetData(TYPE_KAZROGAL, DONE);
            if (Creature* thrall = GetSingleCreatureFromStorage(NPC_THRALL))
            {
                thrall->AI()->SendAIEvent(AI_EVENT_CUSTOM_D, thrall, thrall);
                thrall->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
            }
            m_waveSpawns.erase(std::remove(m_waveSpawns.begin(), m_waveSpawns.end(), creature->GetObjectGuid()), m_waveSpawns.end());
            break;
        case NPC_AZGALOR:
        {
            SetData(TYPE_AZGALOR, DONE);
            if (GameObject* nelfGate = GetSingleGameObjectFromStorage(GO_NIGHT_ELF_VILLAGE_PORTAL))
                nelfGate->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
            if (Creature* thrall = GetSingleCreatureFromStorage(NPC_THRALL))
            {
                thrall->AI()->SendAIEvent(AI_EVENT_CUSTOM_D, thrall, thrall);
                thrall->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
            }
            m_waveSpawns.erase(std::remove(m_waveSpawns.begin(), m_waveSpawns.end(), creature->GetObjectGuid()), m_waveSpawns.end());
            break;
        }
        case NPC_ARCHIMONDE:  SetData(TYPE_ARCHIMONDE, DONE);  break;

        // Trash Mobs summoned in waves
        case NPC_NECRO:
        case NPC_ABOMI:
        case NPC_GHOUL:
        case NPC_BANSH:
        case NPC_CRYPT:
        case NPC_GARGO:
        case NPC_FROST:
        case NPC_INFERNAL:
        case NPC_STALK:
            // Decrease counter, and update world-state
            if (creature->IsTemporarySummon()) // only for non-static spawns
            {
                int32 enemyCount = 0;
                if (enemyCount = instance->GetVariableManager().GetVariable(WORLD_STATE_MOUNT_HYJAL_ENEMYCOUNT))
                {
                    m_waveSpawns.erase(std::remove(m_waveSpawns.begin(), m_waveSpawns.end(), creature->GetObjectGuid()), m_waveSpawns.end());
                    instance->GetVariableManager().SetVariable(WORLD_STATE_MOUNT_HYJAL_ENEMYCOUNT, enemyCount - 1);
                }
                if (m_nextWaveTimer && enemyCount == 0)
                    SpawnNextWave();
            }
            break;
        case NPC_JAINA:
        case NPC_THRALL:
            FailEvent();
            break;
    }
}

uint32 instance_mount_hyjal::GetData(uint32 type) const
{
    switch (type)
    {
        case TYPE_WINTERCHILL:
        case TYPE_ANETHERON:
        case TYPE_KAZROGAL:
        case TYPE_AZGALOR:
        case TYPE_ARCHIMONDE:
            return m_encounter[type];
        default:
            return 0;
    }
}

void instance_mount_hyjal::SetData(uint32 type, uint32 data)
{
    switch (type)
    {
        case TYPE_AZGALOR:
        case TYPE_WINTERCHILL:
        case TYPE_ANETHERON:
        case TYPE_KAZROGAL:
            m_encounter[type] = data;
            if (data != IN_PROGRESS)
            {
                // HUD is cleared on death of any boss
                instance->GetVariableManager().SetVariable(WORLD_STATE_MOUNT_HYJAL_WAVES, 0);
                instance->GetVariableManager().SetVariable(WORLD_STATE_MOUNT_HYJAL_ENABLE, 0);
                instance->GetVariableManager().SetVariable(WORLD_STATE_MOUNT_HYJAL_ENEMYCOUNT, 0);
            }
            break;
        case TYPE_ARCHIMONDE:
            m_encounter[type] = data;
            if (data == IN_PROGRESS)
            {
                DespawnWaveSpawns();
                OverrunBase(BASE_ELF);
            }
            else if (data == FAIL)
            {
                DespawnOverrun(BASE_ELF);
                m_nextInvasionWaveTimer = 0;

                if (Creature* archimonde = GetSingleCreatureFromStorage(NPC_ARCHIMONDE))
                {
                    archimonde->SetRespawnDelay(30, true);
                    archimonde->ForcedDespawn();
                }
            }
            break;
        case TYPE_WIN:
        {
            switch (data)
            {
                case SPECIAL: // Spawn Ores
                {
                    if (GetData(TYPE_AZGALOR) == DONE)
                    {
                        SpawnOres(BASE_HORDE);
                    }
                    else if (GetData(TYPE_ANETHERON) == DONE)
                    {
                        SpawnOres(BASE_ALLY);
                    }
                    SetData(TYPE_WIN, NOT_STARTED);
                    break;
                }
                case DONE: // Despawn Base
                {
                    if (GetData(TYPE_AZGALOR) == DONE)
                    {
                        DespawnBase(BASE_HORDE);
                        DespawnBase(BASE_ALLY);
                    }
                    else if (GetData(TYPE_ANETHERON) == DONE)
                    {
                        DespawnBase(BASE_ALLY);
                    }
                    SetData(TYPE_WIN, NOT_STARTED);
                    break;
                }
            }
            break;
        }
    }

    debug_log("SD2: Instance Hyjal: Instance data updated for event %u (Data=%u)", type, data);

    if (data == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_encounter[0] << " " << m_encounter[1] << " " << m_encounter[2] << " "
                   << m_encounter[3] << " " << m_encounter[4];

        m_saveData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

void instance_mount_hyjal::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_encounter[0] >> m_encounter[1] >> m_encounter[2] >> m_encounter[3] >> m_encounter[4];

    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        if (m_encounter[i] == IN_PROGRESS)               // Do not load an encounter as IN_PROGRESS - reset it instead.
            m_encounter[i] = NOT_STARTED;

    if (m_encounter[TYPE_WINTERCHILL] == DONE)
        m_hyjalWaves = 8;
    if (m_encounter[TYPE_ANETHERON] == DONE)
        m_hyjalWaves = 17;
    if (m_encounter[TYPE_KAZROGAL] == DONE)
        m_hyjalWaves = 26;
    if (m_encounter[TYPE_AZGALOR] == DONE)
        m_hyjalWaves = 35;

    OUT_LOAD_INST_DATA_COMPLETE;
}

void instance_mount_hyjal::SpawnArchimonde()
{
    // Don't spawn him twice
    if (GetSingleCreatureFromStorage(NPC_ARCHIMONDE, true))
        return;

    // Summon Archimonde
    if (Player* pPlayer = GetPlayerInMap())
        pPlayer->SummonCreature(NPC_ARCHIMONDE, aArchimondeSpawnLoc[0], aArchimondeSpawnLoc[1], aArchimondeSpawnLoc[2], aArchimondeSpawnLoc[3], TEMPSPAWN_MANUAL_DESPAWN, 0);
}

void instance_mount_hyjal::SpawnNextWave()
{
    // variable is increased inside
    SpawnWave(++m_hyjalWaves, true);
}

void instance_mount_hyjal::SpawnNextInvasionWave()
{
    // variable is increased inside
    SpawnInvasionWave(++m_invasionWaves, true);
}

void instance_mount_hyjal::SpawnWave(uint32 index, bool setTimer)
{
    uint32 newSpawns = m_hyjalWavesData[index].waveMobs.size();
    newSpawns += GetInfernalsPerWave(index);
    Creature* spawner;
    spawner = GetSingleCreatureFromStorage(index > 17 ? NPC_THRALL : NPC_JAINA);
    if (!setTimer)
    {
        if (instance->GetVariableManager().GetVariable(WORLD_STATE_MOUNT_HYJAL_ENEMYCOUNT) + newSpawns >= MAX_CONCURRENT_SPAWNS)
        {
            if (UnitAI* ai = spawner->AI())
                ai->SendAIEvent(AI_EVENT_CUSTOM_C, spawner, spawner);
            FailEvent();
            return;
        }
    }
    switch (index)
    {
        case 8:
        case 17:
        case 26:
        case 35:
            instance->GetVariableManager().SetVariable(WORLD_STATE_MOUNT_HYJAL_WAVES, 0); // Boss wave doesnt show as a wave
            break;
        default:
            // Set world state for waves to 0 to disable it.
            instance->GetVariableManager().SetVariable(WORLD_STATE_MOUNT_HYJAL_WAVES, index % 9 + 1);
            instance->GetVariableManager().SetVariable(WORLD_STATE_MOUNT_HYJAL_ENABLE, 1);
            break;
    }
    instance->GetVariableManager().SetVariable(WORLD_STATE_MOUNT_HYJAL_ENEMYCOUNT, instance->GetVariableManager().GetVariable(WORLD_STATE_MOUNT_HYJAL_ENEMYCOUNT) + newSpawns);
    for (HyjalWaveMob& mob : m_hyjalWavesData[index].waveMobs)
    {
        Creature* waveSpawn = spawner->SummonCreature(mob.mobEntry, mob.x, mob.y, mob.z, mob.ori, TEMPSPAWN_DEAD_DESPAWN, 0, true, GetRunningPerWaveMobEntry(mob.mobEntry, index), GetRandomWavePath(mob.path));
        if (index == 0) // The very first wave should never grant XP, loot or reputation. ToDo: According to a wowhead comment there should be a cooldown period after you've killed one boss after which the first wave of the next boss also won't grant any XP, loot or reputation
        {
            waveSpawn->SetNoXP(true);
            waveSpawn->SetNoLoot(true);
            waveSpawn->SetNoReputation(true);
        }
    }
    SpawnWaveInfernals(index);
    if (setTimer)
    {
        m_nextWaveTimer = m_hyjalWavesData[index].waveTimer;
    }
}

void instance_mount_hyjal::SpawnWaveInfernals(uint32 index)
{
    uint32 count = GetInfernalsPerWave(index);
    if (!count)
        return;

    SpawnInfernals(count);
}

void instance_mount_hyjal::SpawnInvasionInfernals(uint32 index)
{
    uint32 count;

    switch (index) // Infernals
    {
        case 9: count = 20; break; // Horde Invasion
        default: count = 0; break;
    }

    if (!count)
        return;

    SpawnInfernals(count);
}

void instance_mount_hyjal::SpawnInfernals(uint32 count)
{
    uint32 triggerCount = m_infernalRelays.size();
    m_infernalIndices.resize(count);
    // make sure each is used at least once if bigger than trigger count
    uint32 i;
    for (i = 0; i < count && i < triggerCount; ++i)
        m_infernalIndices[i] = i;

    count = count > triggerCount ? count - triggerCount : 0; // remaining not assigned
    for (uint32 k = 0; k < count; ++k)
        m_infernalIndices[i + k] = urand(0, triggerCount - 1);

    // Randomize all indices in vector to get random sequence every time
    for (uint32 k = 0; k < m_infernalIndices.size(); ++k)
        std::swap(m_infernalIndices[k], m_infernalIndices[urand(0, m_infernalIndices.size() - 1)]);

    m_infernalsCounter = 0;
    SpawnInfernal();
}

void instance_mount_hyjal::SpawnInfernal()
{
    if (m_infernalsCounter >= m_infernalIndices.size())
    {
        m_infernalsTimer = 0; // Counter is reset above
        return;
    }
    else if (m_infernalsCounter == 0)
        m_infernalsTimer = 2000;
    else
        m_infernalsTimer = 1200;

    if (Creature* infernalRelay = instance->GetCreature(m_infernalRelays[m_infernalIndices[m_infernalsCounter]]))
        if (Creature* infernalTarget = instance->GetCreature(m_infernalTargets[m_infernalIndices[m_infernalsCounter]]))
            infernalRelay->CastSpell(infernalTarget, SPELL_SUMMON_INFERNAL, TRIGGERED_NONE);

    ++m_infernalsCounter;
}

void instance_mount_hyjal::SpawnOres(BaseArea index)
{
    // Randomize spawns
    for (uint32 k = 0; k < m_ancientGems[index].size(); ++k)
        std::swap(m_ancientGems[index][k], m_ancientGems[index][urand(0, m_ancientGems[index].size() - 1)]);

    // Spawn a maximum of 5 per base (10 in total per instance ID)
    m_ancientGems[index].resize(5);

    // Spawn the 5 remaining GUIDs
    for (ObjectGuid& guid : m_ancientGems[index])
        DoRespawnGameObject(guid, WEEK);
}

void instance_mount_hyjal::SpawnInvasionWave(uint32 index, bool setTimer)
{
    uint32 newSpawns = m_invasionWavesData[index].waveMobs.size();
    Creature* spawner;
    spawner = GetSingleCreatureFromStorage(NPC_HYJAL_DESPAWN_TRIGGER);

    if (Creature* spawner = GetSingleCreatureFromStorage(NPC_HYJAL_DESPAWN_TRIGGER))
    {
        for (HyjalWaveMob& mob : m_invasionWavesData[index].waveMobs)
        {
            TempSpawnSettings settings(spawner, mob.mobEntry, mob.x, mob.y, mob.z, mob.ori, TEMPSPAWN_DEAD_DESPAWN, 0, true, true, GetRandomWavePath(mob.path));
            if (mob.path == PATH_NONE)
                settings.movegen = IDLE_MOTION_TYPE;
            if (index < 7)
            {
                Creature* waveSpawn = WorldObject::SummonCreature(settings, spawner->GetMap(), spawner->GetPhaseMask());
                m_overrunSpawns[BASE_ALLY].push_back(waveSpawn->GetObjectGuid());
            }
            else if (index < 14)
            {
                Creature* waveSpawn = WorldObject::SummonCreature(settings, spawner->GetMap(), spawner->GetPhaseMask());
                m_overrunSpawns[BASE_HORDE].push_back(waveSpawn->GetObjectGuid());
            }
            else
            {
                Creature* waveSpawn = WorldObject::SummonCreature(settings, spawner->GetMap(), spawner->GetPhaseMask());
                waveSpawn->SetRespawnDelay(350);
                waveSpawn->SetCorpseDelay(300);
                m_overrunSpawns[BASE_ELF].push_back(waveSpawn->GetObjectGuid());
            }
        }

        SpawnInvasionInfernals(index);

        if (setTimer)
        {
            m_nextInvasionWaveTimer = m_invasionWavesData[index].invasionTimer;
        }
    }
}

void instance_mount_hyjal::StartEvent(HyjalEvents eventId)
{
    switch (eventId)
    {
        case JAINA_FIRST_BOSS:
            SpawnBase(BASE_ALLY, false); // Respawn any dead defending NPCs
            m_hyjalWaves = 0;
            SpawnWave(0, true);
            break;
        case JAINA_SECOND_BOSS:
            SpawnBase(BASE_ALLY, false); // Respawn any dead defending NPCs
            m_hyjalWaves = 9;
            SpawnWave(9, true);
            break;
        case JAINA_WIN:
            RetreatBase(BASE_ALLY);
            break;
        case THRALL_FIRST_BOSS:
            SpawnBase(BASE_HORDE, false); // Respawn any dead defending NPCs
            m_hyjalWaves = 18;
            SpawnWave(18, true);
            break;
        case THRALL_SECOND_BOSS:
            SpawnBase(BASE_HORDE, false); // Respawn any dead defending NPCs
            m_hyjalWaves = 27;
            SpawnWave(27, true);
            break;
        case THRALL_WIN:
            RetreatBase(BASE_HORDE);
            SpawnArchimonde();
            break;
    }
}

void instance_mount_hyjal::DespawnWaveSpawns()
{
    instance->GetVariableManager().SetVariable(WORLD_STATE_MOUNT_HYJAL_WAVES, 0);
    instance->GetVariableManager().SetVariable(WORLD_STATE_MOUNT_HYJAL_ENABLE, 0);
    instance->GetVariableManager().SetVariable(WORLD_STATE_MOUNT_HYJAL_ENEMYCOUNT, 0);
    DespawnGuids(m_waveSpawns);
    DespawnGuids(m_additionalSpawns);
}

void instance_mount_hyjal::DespawnBase(BaseArea index)
{
    for (ObjectGuid& guid : m_baseSpawns[index])
        if (Creature* spawn = instance->GetCreature(guid))
            spawn->ForcedDespawn();

    switch (index)
    {
        case BASE_ALLY:
            if (Creature* creature = GetSingleCreatureFromStorage(NPC_JAINA))
                creature->ForcedDespawn();
            break;
        case BASE_HORDE:
            if (Creature* creature = GetSingleCreatureFromStorage(NPC_THRALL))
                creature->ForcedDespawn();
            break;
        default: break;
    }
}

void instance_mount_hyjal::SpawnBase(BaseArea index, bool spawnLeader = true)
{
    for (ObjectGuid& guid : m_baseSpawns[index])
        if (Creature* spawn = instance->GetCreature(guid))
            spawn->Respawn();

    if (spawnLeader)
    {
        switch (index)
        {
            case BASE_ALLY:
                if (Creature* creature = GetSingleCreatureFromStorage(NPC_JAINA))
                    creature->Respawn();
                break;
            case BASE_HORDE:
                if (Creature* creature = GetSingleCreatureFromStorage(NPC_THRALL))
                    creature->Respawn();
                break;
            default: break;
        }
    }
}

void instance_mount_hyjal::RetreatBase(BaseArea index)
{
    instance->GetVariableManager().SetVariable(WORLD_STATE_MOUNT_HYJAL_WAVES, 0);
    instance->GetVariableManager().SetVariable(WORLD_STATE_MOUNT_HYJAL_ENABLE, 0);
    instance->GetVariableManager().SetVariable(WORLD_STATE_MOUNT_HYJAL_ENEMYCOUNT, 0);

    for (ObjectGuid& guid : m_baseSpawns[index])
    {
        if (Creature* spawn = instance->GetCreature(guid))
        {
            spawn->SetCanEnterCombat(false);
            spawn->AI()->SetReactState(REACT_PASSIVE);
        }
    }

    switch (index)
    {
        case BASE_ALLY:
        {
            if (Creature* jaina = GetSingleCreatureFromStorage(NPC_JAINA))
                if (UnitAI* ai = jaina->AI())
                    ai->SendAIEvent(AI_EVENT_CUSTOM_B, jaina, jaina, 1);
            break;
        }
        case BASE_HORDE:
        {
            if (Creature* thrall = GetSingleCreatureFromStorage(NPC_THRALL))
            {
                if (UnitAI* ai = thrall->AI())
                    ai->SendAIEvent(AI_EVENT_CUSTOM_B, thrall, thrall, 0);

                thrall->SummonCreature(NPC_JAINA, 5486.305f, -2777.72f, 1496.687f, 1.082104f, TEMPSPAWN_TIMED_OR_DEAD_DESPAWN, 120000, true, false, 2);
                thrall->SummonCreature(NPC_ALLIANCE_KNIGHT, 5489.709f, -2781.63f, 1497.728f, 1.32645f, TEMPSPAWN_TIMED_OR_DEAD_DESPAWN, 120000);
                thrall->SummonCreature(NPC_ALLIANCE_KNIGHT, 5480.393f, -2778.908f, 1497.069f, 1.047198f, TEMPSPAWN_TIMED_OR_DEAD_DESPAWN, 120000);
            }
            break;
        }
        default: break;
    }

    SetData(TYPE_WIN, SPECIAL);

    // Spawn fires in retreating base
    for (ObjectGuid& guid : m_roaringFlames[index])
        DoRespawnGameObject(guid, WEEK);

    OverrunBase(index);
}

void instance_mount_hyjal::OverrunBase(BaseArea index)
{
    switch (index)
    {
        case BASE_ALLY:
            m_invasionWaves = 0;
            SpawnInvasionWave(0, true);
            break;
        case BASE_HORDE:
            m_invasionWaves = 7;
            SpawnInvasionWave(7, true);
            break;
        case BASE_ELF:
            m_invasionWaves = 14;
            SpawnInvasionWave(14, true);
            break;
        default: break;
    }
}

void instance_mount_hyjal::DespawnOverrun(BaseArea index)
{
    for (ObjectGuid& guid : m_overrunSpawns[index])
    {
        if (Creature* spawn = instance->GetCreature(guid))
        {
            spawn->ForcedDespawn();
            if (index == BASE_ELF) // Have manual despawn
                static_cast<TemporarySpawn*>(spawn)->UnSummon();
        }
    }
}

void instance_mount_hyjal::FailEvent()
{
    if (m_hyjalWaves <= 17) // Jaina died
    {
        if (Creature* jaina = GetSingleCreatureFromStorage(NPC_JAINA))
            jaina->ForcedDespawn();
        DespawnBase(BASE_ALLY);
    }

    if (Creature* thrall = GetSingleCreatureFromStorage(NPC_THRALL))
        thrall->ForcedDespawn();

    DespawnBase(BASE_HORDE);
    DespawnBase(BASE_ELF);

    if (m_hyjalWaves <= 8)
        SetData(TYPE_WINTERCHILL, FAIL);
    else if (m_hyjalWaves <= 17)
        SetData(TYPE_ANETHERON, FAIL);
    else if (m_hyjalWaves <= 26)
        SetData(TYPE_KAZROGAL, FAIL);
    else if (m_hyjalWaves <= 35)
        SetData(TYPE_AZGALOR, FAIL);
    DespawnWaveSpawns();
    instance->GetVariableManager().SetVariable(WORLD_STATE_MOUNT_HYJAL_WAVES, 0);
    instance->GetVariableManager().SetVariable(WORLD_STATE_MOUNT_HYJAL_ENABLE, 0);
    instance->GetVariableManager().SetVariable(WORLD_STATE_MOUNT_HYJAL_ENEMYCOUNT, 0);
    m_nextWaveTimer = 0;
    m_failTimer = 5 * 60 * 1000; // 5 minutes
}

void instance_mount_hyjal::RestartEvent()
{
    if (GetData(TYPE_ANETHERON) != DONE) // Spawn all bases
    {
        SpawnBase(BASE_ALLY);
        SpawnBase(BASE_HORDE);
        SpawnBase(BASE_ELF);
    }
    else if (GetData(TYPE_AZGALOR) != DONE) // Horde and Elf
    {
        SpawnBase(BASE_HORDE);
        SpawnBase(BASE_ELF);
    }
    else // Only Elf
    {
        SpawnBase(BASE_ELF);
    }
    m_failTimer = 0;
}

void instance_mount_hyjal::Update(const uint32 diff)
{
    if (m_waveSpawns.size() > 0 && !GetPlayerInMap(true))
        FailEvent();

    if (m_nextWaveTimer)
    {
        if (m_nextWaveTimer <= diff)
            SpawnNextWave();
        else
            m_nextWaveTimer -= diff;
    }

    if (m_nextInvasionWaveTimer)
    {
        if (m_nextInvasionWaveTimer <= diff)
            SpawnNextInvasionWave();
        else
            m_nextInvasionWaveTimer -= diff;
    }

    if (m_failTimer)
    {
        if (m_failTimer <= diff)
            RestartEvent();
        else
            m_failTimer -= diff;
    }

    if (m_infernalsTimer)
    {
        if (m_infernalsTimer <= diff)
            SpawnInfernal();
        else
            m_infernalsTimer -= diff;
    }
}

void instance_mount_hyjal::ShowChatCommands(ChatHandler* handler)
{
    handler->SendSysMessage("This instance supports the following commands:\n despawnbase (0,1,2), spawnbase (0,1,2), spawnwave (0-35), spawnarchimonde, retreatbase (0,1), spawnoverrun (0,1,2), despawnoverrun (0,1,2), despawnwavespawns, spawninfernals (count)");
}

void instance_mount_hyjal::ExecuteChatCommand(ChatHandler* handler, char* args)
{
    char* result = handler->ExtractLiteralArg(&args);
    if (!result)
        return;
    std::string val = result;
    if (val == "despawnbase")
    {
        uint32 baseId;
        handler->ExtractUInt32(&args, baseId);

        if (baseId > MAX_BASE - 1)
        {
            handler->PSendSysMessage("Could not despawn base %u because it doesn't exist. Please specify a value between 0-2", baseId);
            return;
        }

        DespawnBase(BaseArea(baseId));
    }
    else if (val == "spawnbase")
    {
        uint32 baseId;
        handler->ExtractUInt32(&args, baseId);

        if (baseId > MAX_BASE - 1)
        {
            handler->PSendSysMessage("Could not spawn base %u because it doesn't exist. Please specify a value between 0-2", baseId);
            return;
        }

        SpawnBase(BaseArea(baseId));
    }
    else if (val == "spawnwave")
    {
        uint32 waveId;
        handler->ExtractUInt32(&args, waveId);

        if (waveId > MAX_WAVES - 1)
        {
            handler->PSendSysMessage("Could not spawn wave %u because it doesn't exist. Please specify a value between 0-35", waveId);
            return;
        }

        if (!GetSingleCreatureFromStorage(waveId > 17 ? NPC_THRALL : NPC_JAINA))
        {
            handler->PSendSysMessage("Could not spawn wave %u because %s is despawned.", waveId, waveId > 17 ? "Thrall" : "Jaina");
            return;
        }

        SpawnWave(waveId, false);
    }
    else if (val == "spawnarchimonde")
    {
        SpawnArchimonde();
    }
    else if (val == "retreatbase")
    {
        uint32 baseId;
        handler->ExtractUInt32(&args, baseId);

        if (baseId > MAX_BASE - 2)
        {
            handler->PSendSysMessage("Could not retreat base %u because it doesn't exist. Please specify a value between 0-1", baseId);
            return;
        }

        RetreatBase(BaseArea(baseId));
    }
    else if (val == "spawnoverrun")
    {
        uint32 baseId;
        handler->ExtractUInt32(&args, baseId);

        if (baseId > MAX_BASE - 1)
        {
            handler->PSendSysMessage("Could not spawn overrun waves in base %u because it doesn't exist. Please specify a value between 0-2", baseId);
            return;
        }

        OverrunBase(BaseArea(baseId));
    }
    else if (val == "despawnoverrun")
    {
        uint32 baseId;
        handler->ExtractUInt32(&args, baseId);

        if (baseId > MAX_BASE - 1)
        {
            handler->PSendSysMessage("Could not despawn overrun waves in base %u because it doesn't exist. Please specify a value between 0-2", baseId);
            return;
        }

        DespawnOverrun(BaseArea(baseId));
    }
    else if (val == "despawnwavespawns")
    {
        DespawnWaveSpawns();
    }
    else if (val == "spawninfernals")
    {
        uint32 count = 101;
        handler->ExtractUInt32(&args, count);
        if (count > 100)
        {
            handler->PSendSysMessage("Spawn Infernals received too high number. Please input a number between 1-99");
            return;
        }
        SpawnInfernals(count);
    }
}

bool instance_mount_hyjal::CheckConditionCriteriaMeet(Player const* player, uint32 instanceConditionId, WorldObject const* conditionSource, uint32 conditionSourceType) const
{
    switch (instanceConditionId)
    {
        case INSTANCE_CONDITION_ID_WINTERCHILL_STARTABLE:
            return (GetData(TYPE_WINTERCHILL) == NOT_STARTED || GetData(TYPE_WINTERCHILL) == FAIL) && m_waveSpawns.size() <= 0;
        case INSTANCE_CONDITION_ID_ANETHERON_STARTABLE:
            return GetData(TYPE_WINTERCHILL) == DONE && (GetData(TYPE_ANETHERON) == NOT_STARTED || GetData(TYPE_ANETHERON) == FAIL) && m_waveSpawns.size() <= 0;
        case INSTANCE_CONDITION_ID_ANETHERON_DONE:
            return GetData(TYPE_ANETHERON) == DONE;
        case INSTANCE_CONDITION_ID_KAZROGAL_STARTABLE:
            return GetData(TYPE_ANETHERON) == DONE && (GetData(TYPE_KAZROGAL) == NOT_STARTED || GetData(TYPE_KAZROGAL) == FAIL) && m_waveSpawns.size() <= 0;
        case INSTANCE_CONDITION_ID_AZGALOR_STARTABLE:
            return GetData(TYPE_KAZROGAL) == DONE && (GetData(TYPE_AZGALOR) == NOT_STARTED || GetData(TYPE_AZGALOR) == FAIL) && m_waveSpawns.size() <= 0;
        case INSTANCE_CONDITION_ID_AZGALOR_DONE:
            return GetData(TYPE_AZGALOR) == DONE;
        case INSTANCE_CONDITION_ID_ARCHIMONDE_STARTABLE:
            return GetData(TYPE_AZGALOR) == DONE && GetData(TYPE_ARCHIMONDE) != DONE;
        case INSTANCE_CONDITION_ID_ARCHIMONDE_DONE:
            return GetData(TYPE_ARCHIMONDE) == DONE;
        case INSTANCE_CONDITION_ID_WAVES_INPROGRESS:
            return m_waveSpawns.size() > 0;
    }

    script_error_log("instance_mount_hyjal::CheckConditionCriteriaMeet called with unsupported Id %u. Called with param plr %s, src %s, condition source type %u",
        instanceConditionId, player ? player->GetGuidStr().c_str() : "nullptr", conditionSource ? conditionSource->GetGuidStr().c_str() : "nullptr", conditionSourceType);
    return false;
}

struct EternalSilence : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply) // residue of eternity
            aura->GetTarget()->CastSpell(nullptr, 42205, TRIGGERED_OLD_TRIGGERED);
    }
};

void AddSC_instance_mount_hyjal()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "instance_hyjal";
    pNewScript->GetInstanceData = &GetNewInstanceScript<instance_mount_hyjal>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<EternalSilence>("spell_eternal_silence");
}
