#include <QDir>
#include <QDate>
#include <QStandardPaths>

#include "workspace.h"
#include "constants.h"

namespace core
{
  QString Workspace::configurationName_;

  namespace
  {
    constexpr char EOS_DATA[] = "EOS-Innovation";
    constexpr char EOS_SENTINEL_DIR[] = "sentinel";
    constexpr char EOS_SENTINEL_ADMIN_DIR[] = "sentinel_admin";
    constexpr char EOS_SENTINEL_CONFIGURATIONS[] = "configs";
    constexpr char VIDEOS_DIRECTORY[] = "videos";
    constexpr char PICTURES_DIRECTORY[] = "pictures";
    constexpr char LOGS_DIRECTORY[] = "logs";
    constexpr char AXIS_VIDEOS_DIRECTORY[] = "robot";
    constexpr char CONFIGURATIONS_DIRECTORY[] = "Configurations";
    constexpr char EOS_APPS[] = "eos";

    QString getFirstEntry(QStandardPaths::StandardLocation type)
    {
      QStringList dirs = QStandardPaths::standardLocations(type);
      Q_ASSERT(dirs.size());
      return dirs[0];
    }
  }

  QString Workspace::local(Location location)
  {
    if (location == App)
    {
      return QDir::currentPath();
    }
    //else if (location == Temp)
    //{
    //  return core::join(QDir::tempPath(), EOS_DATA, true);
    //}
    //else if (location == TempPatrols)
    //{
    //  return core::join(local(Temp), PATROLS_DIR, true);
    //}
    //else if (location == TempMap)
    //{
    //  return core::join(local(Temp), MAP_DIR, true);
    //}
    //else if (location == TempZones)
    //{
    //  return core::join(local(Temp), ZONES_DIR, true);
    //}
    else if (location == Data)
    {
      return join(getFirstEntry(QStandardPaths::GenericDataLocation), EOS_DATA, true);
    }
    else if (location == SentinelData)
    {
      return join(local(Data), EOS_SENTINEL_DIR, true);
    }
    else if (location == SentinelAdminData)
    {
      return join(local(Data), EOS_SENTINEL_ADMIN_DIR, true);
    }
    else if (location == SentinelConfigurations)
    {
      return join(local(SentinelData), EOS_SENTINEL_CONFIGURATIONS, true);
    }
    else if (location == SentinelAdminConfigurations)
    {
      return join(local(SentinelAdminData), EOS_SENTINEL_CONFIGURATIONS, true);
    }
    else if (location == DataForConfig)
    {
      return join(local(Data),
                  configurationName_, true);
    }
    else if (location == Configurations)
    {
      return join(local(Data),
                  CONFIGURATIONS_DIRECTORY, true);
    }
    else if (location == Videos)
      return join(local(SentinelData), VIDEOS_DIRECTORY, true);
    else if (location == Pictures)
      return join(local(SentinelData), PICTURES_DIRECTORY, true);
    else if (location == VideoDay)
      return join(local(Videos), QDate::currentDate().toString(Qt::ISODate), true);
    else if (location == PicturesDay)
      return join(local(Pictures), QDate::currentDate().toString(Qt::ISODate), true);
    else if (location == Logs)
      return join(local(SentinelData), LOGS_DIRECTORY, true);
    else if (location == LogDay)
      return join(local(Logs), QDate::currentDate().toString(Qt::ISODate), true);
    else if (location == AxisVideos)
      return join(local(VideoDay), AXIS_VIDEOS_DIRECTORY, true);
    else if (location == LocalEOSApps)
    {
      return join(getFirstEntry(QStandardPaths::ApplicationsLocation), EOS_APPS, true);
    }
    else if (location == LocalSentinelApp)
    {
      return join(local(LocalEOSApps), EOS_SENTINEL_DIR, true);
    }
    else if (location == LocalSentinelAdminApp)
    {
      return join(local(LocalEOSApps), EOS_SENTINEL_ADMIN_DIR, true);
    }
    return local(Data);
  }

  QString Workspace::remote(RemoteLocation location)
  {
    if (location == SharedDir)
      return ".config/eos/shared/";
    return "";
  }

  QString join(QString const &path, QString const &last, bool create)
  {
    QDir dir(path + QDir::separator() + last);
    if (create)
      if (dir.exists() == false)
        dir.mkpath(dir.path()); // alternative to mkdir -p
    return dir.path();
  }
}
