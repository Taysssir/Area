#ifndef SENTINEL_ADMIN_EVENTS_H
#define SENTINEL_ADMIN_EVENTS_H

#include <QString>

namespace core
{
  struct IPChanged : public Event<IPChanged>
  {
  };

  struct DownloadFileOk : public Event<DownloadFileOk>
  {
  };

  struct DeleteZone
      : public Event<DeleteZone>
  {
    explicit DeleteZone(int index)
        : index(index)
    {
    }
    int index;
  };

  struct RobotConnected : public Event<RobotConnected>
  {
  };

  struct RobotDisconnected : public Event<RobotDisconnected>
  {
  };

  struct RobotStandby : public Event<RobotStandby>
  {
  };

  struct ConfigurationUpdate : public Event<ConfigurationUpdate>
  {
  };

  struct PatrolsUpdate : public Event<PatrolsUpdate>
  {
  };

  struct ZonesUpdate : public Event<ZonesUpdate>
  {
  };

  struct SensorsUpdate : public Event<SensorsUpdate>
  {
  };

  struct MapUpdate : public Event<MapUpdate>
  {
  };

  struct PlanificationUpdate : public Event<PlanificationUpdate>
  {
  };

  struct NewMapAvailable : public Event<NewMapAvailable>
  {
  };

  struct MapDownloading : public Event<MapDownloading>
  {
    MapDownloading(double percent)
        : percent(percent)
    {
    }
    double percent = 0;
  };

  struct DisconnectRobot : public Event<DisconnectRobot>
  {};
}

#endif // SENTINEL_ADMIN_EVENTS_H
