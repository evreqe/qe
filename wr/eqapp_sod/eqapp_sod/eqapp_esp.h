#ifndef EQAPP_ESP_H
#define EQAPP_ESP_H

bool g_espIsEnabled            = true;
bool g_espSpawnIsEnabled       = true;
bool g_espGroundSpawnIsEnabled = true;
bool g_espDoorIsEnabled        = false;
bool g_espSkeletonIsEnabled    = false;
bool g_espZoneObjectIsEnabled  = false;
bool g_espWaypointIsEnabled    = true;

float g_espSpawnDistance       = 400.0f;
float g_espGroundSpawnDistance = 200.0f;
float g_espDoorDistance        = 100.0f;
float g_espSkeletonDistance    = 100.0f;
float g_espZoneObjectDistance  = 200.0f;
float g_espWaypointDistance    = 1000.0f;

DWORD g_espGroundSpawnColor = 0xFFFFFFFF;
DWORD g_espDoorColor        = 0xFFFFFFFF;
DWORD g_espZoneObjectColor  = 0xFFFFFFFF;
DWORD g_espWaypointColor    = 0xFFC0C0C0;
DWORD g_espLocatorColor     = 0xFFFFFFFF;
DWORD g_espFindColor        = 0xFF00FF80;

bool g_espLocatorIsEnabled = false;
float g_espLocatorY = 0.0f;
float g_espLocatorX = 0.0f;
float g_espLocatorZ = 0.0f;

bool g_espFindIsEnabled = false;
std::string g_espFindSpawnName;

unsigned int g_espNumDrawText = 0;
unsigned int g_espNumNpcCorpseMax = 10;

#endif // EQAPP_ESP_H
