#include "pimpl_configuration.hpp"

#include "eos/configuration/models.pb.h"
#include "resource_manager/loaders/patrols_loader.hpp"
#include "resource_manager/loaders/zones_loader.hpp"
#include "resource_manager/loaders/sensors_loader.hpp"
#include "resource_manager/loaders/settings_loader.hpp"
#include "resource_manager/loaders/users_loader.hpp"
#include "resource_manager/loaders/map_loader.hpp"
#include "resource_manager/loaders/tasksscheduled_loader.hpp"

#include "resource_manager/file.hpp"

#include "eos/configuration/configuration.hpp"

namespace eos
{
  namespace eosconfig
  {
    namespace
    {
      constexpr char PATROLS_FILENAME[] = "patrols";
      constexpr char ZONES_FILENAME[] = "zones";
      constexpr char BASE_MAP_FILENAME[] = "base_map";
      constexpr char STATIC_MAP_FILENAME[] = "static_map";
      constexpr char DISPLAY_MAP_FILENAME[] = "display_map";
      constexpr char SENSORS_FILENAME[] = "sensors";
      constexpr char SETTINGS_FILENAME[] = "settings";
      constexpr char TASKS_SCHEDULED_FILENAME[] = "tasks_scheduled";
    }

    Configuration::Configuration()
    {
        reset();
    }

    bool Configuration::open(std::string const &filepath)
    {
      filepath_ = filepath;
      if (!eos::file::exist(filepath) || !resourceManager_.hasExtraInfo(filepath))
      {
        std::stringstream infos;
        infos << "EOS Innovation Configuration file Version [" << eos::Configuration::version() << "]" << std::endl;
        infos << "Compiled at ";
        infos << __DATE__ << " " << __TIME__ << std::endl;
        resourceManager_.addExtraInfo(filepath, infos.str());
      }
      reset();
      return true;
    }

    void Configuration::clear()
    {
       filepath_ = "";
       reset();
    }

    bool Configuration::save()
    {
       return true
               && save_patrols()
               && save_zones()
               && save_sensors()
               && save_settings()
               && save_base_map()
               && save_static_map()
               && save_display_map()
               && save_tasks_scheduled();
    }

    bool Configuration::save(std::string const& filepath)
    {
      filepath_ = filepath;
      if (eos::file::exist(filepath) == false || resourceManager_.hasExtraInfo(filepath) == false)
      {
        std::stringstream infos;
        infos << "EOS Innovation Configuration file Version [" << eos::Configuration::version() << "]" << std::endl;
        infos << "Compiled at ";
        infos << __DATE__ << " " << __TIME__ << std::endl;
        resourceManager_.addExtraInfo(filepath, infos.str());
      }
      return save();
    }

    bool Configuration::save_patrols()
    {
      return isOpened()
              && resourceManager_.save<PatrolsLoader, Resource<Patrols>>(filepath_, PATROLS_FILENAME);
    }

    bool Configuration::export_patrols(std::string const& filepath)
    {
      return resourceManager_.exportModel<PatrolsLoader, Resource<Patrols>>(filepath, PATROLS_FILENAME);
    }

    bool Configuration::import_patrols(std::string const& filepath)
    {
      return resourceManager_.importModel<PatrolsLoader, Resource<Patrols>>(filepath, PATROLS_FILENAME);
    }

    bool Configuration::save_zones()
    {
      return isOpened()
              && resourceManager_.save<ZonesLoader, Resource<Zones>>(filepath_, ZONES_FILENAME);
    }

    bool Configuration::export_zones(std::string const& filepath)
    {
      return resourceManager_.exportModel<ZonesLoader, Resource<Zones>>(filepath, ZONES_FILENAME);
    }

    bool Configuration::import_zones(std::string const& filepath)
    {
      return resourceManager_.importModel<ZonesLoader, Resource<Zones>>(filepath, ZONES_FILENAME);
    }

    bool Configuration::save_sensors()
    {
      return resourceManager_.save<SensorsLoader, Resource<Sensors>>(filepath_, SENSORS_FILENAME);
    }

    bool Configuration::export_sensors(std::string const& filepath)
    {
      return resourceManager_.exportModel<SensorsLoader, Resource<Sensors>>(filepath, SENSORS_FILENAME);
    }

    bool Configuration::import_sensors(std::string const& filepath)
    {
      return resourceManager_.importModel<SensorsLoader, Resource<Sensors>>(filepath, SENSORS_FILENAME);
    }

    bool Configuration::save_settings()
    {
      return isOpened()
              && resourceManager_.save<SettingsLoader, Resource<Settings>>(filepath_, SETTINGS_FILENAME);
    }

    bool Configuration::export_settings(std::string const& filepath)
    {
      return resourceManager_.exportModel<SettingsLoader, Resource<Settings>>(filepath, SETTINGS_FILENAME);
    }

    bool Configuration::import_settings(std::string const& filepath)
    {
      return resourceManager_.importModel<SettingsLoader, Resource<Settings>>(filepath, SETTINGS_FILENAME);
    }

    bool Configuration::save_base_map()
    {
      return isOpened()
              && resourceManager_.save<MapLoader, Resource<Map>>(filepath_, BASE_MAP_FILENAME);
    }

    bool Configuration::export_base_map(std::string const& filepath)
    {
      return resourceManager_.exportModel<MapLoader, Resource<Map>>(filepath, BASE_MAP_FILENAME);
    }

    bool Configuration::import_base_map(std::string const& filepath)
    {
      return resourceManager_.importModel<MapLoader, Resource<Map>>(filepath, BASE_MAP_FILENAME);
    }

    bool Configuration::save_static_map()
    {
      return isOpened()
              && resourceManager_.save<MapLoader, Resource<Map>>(filepath_, STATIC_MAP_FILENAME);
    }

    bool Configuration::export_static_map(std::string const& filepath)
    {
      return resourceManager_.exportModel<MapLoader, Resource<Map>>(filepath, STATIC_MAP_FILENAME);
    }

    bool Configuration::import_static_map(std::string const& filepath)
    {
      return resourceManager_.importModel<MapLoader, Resource<Map>>(filepath, STATIC_MAP_FILENAME);
    }

    bool Configuration::save_display_map()
    {
      return isOpened()
              && resourceManager_.save<MapLoader, Resource<Map>>(filepath_, DISPLAY_MAP_FILENAME);
    }

    bool Configuration::export_display_map(std::string const& filepath)
    {
      return resourceManager_.exportModel<MapLoader, Resource<Map>>(filepath, DISPLAY_MAP_FILENAME);
    }

    bool Configuration::import_display_map(std::string const& filepath)
    {
      return resourceManager_.importModel<MapLoader, Resource<Map>>(filepath, DISPLAY_MAP_FILENAME);
    }

    bool Configuration::save_tasks_scheduled()
    {
      return isOpened()
              && resourceManager_.save<TasksScheduledLoader, Resource<TasksScheduled>>(filepath_, TASKS_SCHEDULED_FILENAME);
    }

    bool Configuration::export_tasks_scheduled(std::string const& filepath)
    {
      return resourceManager_.exportModel<TasksScheduledLoader, Resource<TasksScheduled>>(filepath, TASKS_SCHEDULED_FILENAME);
    }

    bool Configuration::import_tasks_scheduled(std::string const& filepath)
    {
      return resourceManager_.importModel<TasksScheduledLoader, Resource<TasksScheduled>>(filepath, TASKS_SCHEDULED_FILENAME);
    }

    void Configuration::reset()
    {
       resourceManager_.reset();
       resource<PatrolsLoader,  Patrols>             (PATROLS_FILENAME,         true);
       resource<ZonesLoader,    Zones>               (ZONES_FILENAME,           true);
       resource<SensorsLoader,  Sensors>             (SENSORS_FILENAME,         true);
       resource<SettingsLoader, Settings>            (SETTINGS_FILENAME,        true);
       resource<TasksScheduledLoader, TaskScheduled> (TASKS_SCHEDULED_FILENAME, true);
       resource<MapLoader,      Map>                 (BASE_MAP_FILENAME,        true);
       resource<MapLoader,      Map>                 (STATIC_MAP_FILENAME,      true);
       resource<MapLoader,      Map>                 (DISPLAY_MAP_FILENAME,     true);
    }

    bool Configuration::isOpened() const
    {
      return filepath_.size() && eos::file::exist(filepath_);
    }

    PatrolsSharedPointer Configuration::patrols()
    {
      return resource<PatrolsLoader, Patrols>(PATROLS_FILENAME);
    }

    PatrolsSharedPointer const Configuration::patrols() const
    {
      return resource<PatrolsLoader, Patrols>(PATROLS_FILENAME);
    }

    ZonesSharedPointer Configuration::zones()
    {
      return resource<ZonesLoader, Zones>(ZONES_FILENAME);
    }

    ZonesSharedPointer const Configuration::zones() const
    {
      return resource<ZonesLoader, Zones>(ZONES_FILENAME);
    }

    SensorsSharedPointer Configuration::sensors()
    {
      return resource<SensorsLoader, Sensors>(SENSORS_FILENAME);
    }

    SensorsSharedPointer const Configuration::sensors() const
    {
      return resource<SensorsLoader, Sensors>(SENSORS_FILENAME);
    }

    SettingsSharedPointer Configuration::settings()
    {
      return resource<SettingsLoader, Settings>(SETTINGS_FILENAME);
    }

    SettingsSharedPointer const Configuration::settings() const
    {
      return resource<SettingsLoader, Settings>(SETTINGS_FILENAME);
    }

    TasksScheduledSharedPointer Configuration::tasksScheduled()
    {
      return resource<TasksScheduledLoader, TasksScheduled>(TASKS_SCHEDULED_FILENAME);
    }

    TasksScheduledSharedPointer const Configuration::tasksScheduled() const
    {
      return resource<TasksScheduledLoader, TasksScheduled>(TASKS_SCHEDULED_FILENAME);
    }

    MapSharedPointer Configuration::base_map()
    {
      return resource<MapLoader, Map>(BASE_MAP_FILENAME);
    }

    MapSharedPointer const Configuration::base_map() const
    {
      return resource<MapLoader, Map>(BASE_MAP_FILENAME);
    }

    MapSharedPointer Configuration::static_map()
    {
      return resource<MapLoader, Map>(STATIC_MAP_FILENAME);
    }

    MapSharedPointer const Configuration::static_map() const
    {
      return resource<MapLoader, Map>(STATIC_MAP_FILENAME);
    }

    MapSharedPointer Configuration::display_map()
    {
      return resource<MapLoader, Map>(DISPLAY_MAP_FILENAME);
    }

    MapSharedPointer const Configuration::display_map() const
    {
      return resource<MapLoader, Map>(DISPLAY_MAP_FILENAME);
    }
  }
}
