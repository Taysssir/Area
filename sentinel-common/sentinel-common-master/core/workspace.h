#ifndef SENTINEL_WORKSPACE_H
#define SENTINEL_WORKSPACE_H

#include <QString>

namespace core
{
  class Workspace
  {
  public:
    typedef enum
    {
      // application configuration
      App,
      AppSettings,
      Data,
      SentinelData,
      SentinelAdminData,
      SentinelConfigurations,
      SentinelAdminConfigurations,
      DataForConfig,
      Configurations,
      Videos, // where videos directory are stored
      Pictures, // where videos directory are stored
      VideoDay, // the video directory for today
      PicturesDay, // the video directory for today
      Logs, // main directory for logs
      LogDay, // log for today
      Temp,
      TempPatrols,
      TempZones,
      TempMap,
      AxisVideos, // where we put videos from robot axis sd
      LocalEOSApps, // where we put eos software in local pc
      LocalSentinelApp,
      LocalSentinelAdminApp,
    } Location;

    typedef enum
    {
      SharedDir,
    } RemoteLocation;

    static QString remote(RemoteLocation location);
    static QString local(Location location);
    static void setConfigurationName(QString const &configName)
    {
      configurationName_ = configName;
    }

  private:
    static QString configurationName_;
  };

  /**
     * @brief join a path and other (dir or file)
     * @param path the root directory
     * @param last file or dir
     * @param create if true create the full subdirectory list
     * @return  the result of join
     */
  QString join(QString const &path, QString const &last, bool create = false);
}

#endif // SENTINEL_WORKSPACE_H
