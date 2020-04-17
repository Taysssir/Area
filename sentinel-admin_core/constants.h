#ifndef EADMIN_CONSTANTS_H
#define EADMIN_CONSTANTS_H

#include "core/constants.h"

namespace constants
{
    const char APP_NAME[] = "sentinel-admin";

    const unsigned short MAJOR = 2;
    const unsigned short MINOR = 1;
    const unsigned short PATCH = 0;

    typedef enum Mode
    {
        Normal ,
        CreateZone,
        CreatePoint,
        CreateSensors,
        AddDetails,
        RotateMap,
        DistanceMeasurement
    } Type;

    typedef enum
    {
        Map = 0x0001,
        Zones = 0x0002,
        Patrols = 0x0004,
        Sensors = 0x0008
    } DataType;

    constexpr char FILE_MAP[] = "obstacle_map.zip";
    constexpr char FILE_MAP_REMAPPING_IN[] = "obstacle_map_remapping_in.pgm";
    constexpr char FILE_MAP_IN_ZIP[] = "obstacle_map.pgm";

    constexpr qreal ZoneZValue = 12;
    constexpr qreal PatrolZValue = 125;
    constexpr qreal MapZValue = -3;
    constexpr qreal CostmapZValue = -2;
    constexpr qreal SensorZValue = 120;
    constexpr qreal PositionStringZValue = 200;
    //constexpr qreal PatrolPointZValue = 12;

    constexpr char LASER_CARTO[] = "lasercarto";
    constexpr char CARTO_FROM_FILE[] = "cartofromfile";
    constexpr char TERMINATOR[] = "terminator";

    const int kMaxPatrolPoint = 100;

    inline std::string login()
    {
        return obfuscation<'e','-','v','i','g','i','l','a','n','t','e'>();
    }

    inline std::string password()
    {
        return obfuscation<'R','S','d','4','9','g','m','9'>();
    }
}

#endif // EADMIN_CONSTANTS_H
