#ifndef EOS_CONFIGURATION_PIMPL_CONFIGURATION_HPP
#define EOS_CONFIGURATION_PIMPL_CONFIGURATION_HPP

#include <memory>
#include <string>
#include <chrono>

#include "resource_manager/resource_manager.hpp"

namespace eos
{
  uint64_t timestamp();

  class Patrols;
  typedef std::shared_ptr<Patrols> PatrolsSharedPointer;

  class Zones;
  typedef std::shared_ptr<Zones> ZonesSharedPointer;

  class Sensors;
  typedef std::shared_ptr<Sensors> SensorsSharedPointer;

  class Settings;
  typedef std::shared_ptr<Settings> SettingsSharedPointer;

  class TasksScheduled;
  typedef std::shared_ptr<TasksScheduled> TasksScheduledSharedPointer;

  class Map;
  typedef std::shared_ptr<Map> MapSharedPointer;

  namespace eosconfig
  {
    class Configuration
    {
    public:
      Configuration();

      bool open(std::string const &filepath);

      bool save();

      bool save(std::string const& filepath);

      bool isOpened() const;

      void reset();

      void clear();

      PatrolsSharedPointer patrols();
      PatrolsSharedPointer const patrols() const;
      bool save_patrols();
      bool export_patrols(std::string const& filepath);
      bool import_patrols(std::string const& filepath);

      ZonesSharedPointer zones();
      ZonesSharedPointer const zones() const;
      bool save_zones();
      bool export_zones(std::string const& filepath);
      bool import_zones(std::string const& filepath);

      SensorsSharedPointer sensors();
      SensorsSharedPointer const sensors() const;
      bool save_sensors();
      bool export_sensors(std::string const& filepath);
      bool import_sensors(std::string const& filepath);

      SettingsSharedPointer settings();
      SettingsSharedPointer const settings() const;
      bool save_settings();
      bool export_settings(std::string const& filepath);
      bool import_settings(std::string const& filepath);

      TasksScheduledSharedPointer tasksScheduled();
      TasksScheduledSharedPointer const tasksScheduled() const;
      bool save_tasks_scheduled();
      bool export_tasks_scheduled(std::string const& filepath);
      bool import_tasks_scheduled(std::string const& filepath);

      MapSharedPointer base_map();
      MapSharedPointer const base_map() const;
      bool save_base_map();
      bool export_base_map(std::string const& filepath);
      bool import_base_map(std::string const& filepath);

      MapSharedPointer static_map();
      MapSharedPointer const static_map() const;
      bool save_static_map();
      bool export_static_map(std::string const& filepath);
      bool import_static_map(std::string const& filepath);

      MapSharedPointer display_map();
      MapSharedPointer const display_map() const;
      bool save_display_map();
      bool export_display_map(std::string const& filepath);
      bool import_display_map(std::string const& filepath);

    private:
      template <typename Loader, typename Rsc>
      std::shared_ptr<Rsc> resource(std::string const &filename, bool force = false)
      {
        std::shared_ptr<Rsc> rsc = resourceManager_.load<Loader, Resource<Rsc>>(filepath_, filename, force);
        if (!rsc)
        {
          // allocate resource !
          std::shared_ptr<Resource<Rsc>> rsc = Resource<Rsc>::alloc();
          if (!rsc)
          {
            throw std::runtime_error("impossible to alloc new ressource !");
            return nullptr;
          }
          if (!resourceManager_.save<Loader, Resource<Rsc>>(filepath_, filename, rsc))
          {
            throw std::runtime_error("impossible to save ressource !");
            return nullptr;
          }
          return resourceManager_.load<Loader, Resource<Rsc>>(filepath_, filename, force);
        }
        return rsc;
      }

      template <typename Loader, typename Rsc>
      std::shared_ptr<Rsc> resource_update_timestamp(std::string const &filename)
      {
        std::shared_ptr<Rsc> rsc = resource<Loader, Rsc>(filename);
        rsc->set_timestamp(timestamp());
        return rsc;
      }

      template <typename Loader, typename Rsc>
      std::shared_ptr<Rsc> const resource(std::string const &filename) const
      {
        return std::const_pointer_cast<Rsc>(resource<Loader, Rsc>(filename));
      }

      ResourceManager resourceManager_;
      std::string filepath_;
    };
  }
}

#endif // EOS_CONFIGURATION_PIMPL_CONFIGURATION_HPP
