#include <sstream>
#include <chrono>

#include "eos/configuration/configuration.hpp"
#include "resource_manager/resource_manager.hpp"
#include "pimpl_configuration.hpp"

namespace eos
{
  uint64_t timestamp()
  {
    using namespace std::chrono;
    return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
  }

  Configuration::Configuration()
  {
    pConfig_.reset(new eosconfig::Configuration);
    if (!pConfig_)
      throw std::bad_alloc();
  }

  Configuration::~Configuration()
  {
  }

  bool Configuration::open(std::string const &filepath)
  {
    return pConfig_->open(filepath);
  }

  bool Configuration::isOpened() const
  {
    return pConfig_->isOpened();
  }

  PatrolsSharedPointer Configuration::patrols()
  {
    modification_flag_= true;
    return pConfig_->patrols();
  }

  PatrolsSharedPointer const Configuration::patrols() const
  {
    return pConfig_->patrols();
  }

  ZonesSharedPointer Configuration::zones()
  {
    modification_flag_= true;
    return pConfig_->zones();
  }

  ZonesSharedPointer const Configuration::zones() const
  {
    return pConfig_->zones();
  }

  SensorsSharedPointer Configuration::sensors()
  {
    modification_flag_= true;
    return pConfig_->sensors();
  }

  SensorsSharedPointer const Configuration::sensors() const
  {
    return pConfig_->sensors();
  }

  SettingsSharedPointer Configuration::settings()
  {
    modification_flag_= true;
    return pConfig_->settings();
  }

  SettingsSharedPointer const Configuration::settings() const
  {
    return pConfig_->settings();
  }

  TasksScheduledSharedPointer Configuration::tasksScheduled()
  {
    modification_flag_= true;
    return pConfig_->tasksScheduled();
  }

  TasksScheduledSharedPointer const Configuration::tasksScheduled() const
  {
    return pConfig_->tasksScheduled();
  }

  MapSharedPointer Configuration::base_map()
  {
    modification_flag_= true;
    return pConfig_->base_map();
  }

  MapSharedPointer const Configuration::base_map() const
  {
    return pConfig_->base_map();
  }

  MapSharedPointer Configuration::static_map()
  {
    modification_flag_= true;
    return pConfig_->static_map();
  }

  MapSharedPointer const Configuration::static_map() const
  {
    return pConfig_->static_map();
  }

  MapSharedPointer Configuration::display_map()
  {
    modification_flag_= true;
    return pConfig_->display_map();
  }

  MapSharedPointer const Configuration::display_map() const
  {
    return pConfig_->display_map();
  }

  std::string Configuration::version()
  {
    std::stringstream ss;
    ss << "v" << kMajor << "." << kMinor << "." << kRelease;
    return ss.str();
  }

  bool Configuration::save(std::string const &filepath)
  {
    modification_flag_ = false;
    return pConfig_->save(filepath);
  }

  void Configuration::reset()
  {
    modification_flag_ = false;
    return pConfig_->reset();
  }

  void Configuration::clear()
  {
    modification_flag_ = false;
    return pConfig_->clear();
  }

  bool Configuration::save()
  {
    modification_flag_ = false;
    return pConfig_->save();
  }

  bool Configuration::export_model(Configuration::Model model, std::string const& filepath)
  {
    if (model == Configuration::Patrols)
      return pConfig_->export_patrols(filepath);
    else if (model == Configuration::Zones)
      return pConfig_->export_zones(filepath);
    else if (model == Configuration::Sensors)
      return pConfig_->export_sensors(filepath);
    else if (model == Configuration::Settings)
      return pConfig_->export_settings(filepath);
    else if (model == Configuration::BaseMap)
      return pConfig_->export_base_map(filepath);
    else if (model == Configuration::StaticMap)
      return pConfig_->export_static_map(filepath);
    else if (model == Configuration::DisplayMap)
      return pConfig_->export_display_map(filepath);
    else if (model == Configuration::TasksScheduled)
      return pConfig_->export_tasks_scheduled(filepath);
    return false;
  }

  bool Configuration::import_model(Configuration::Model model, std::string const& filepath)
  {
    if (model == Configuration::Patrols)
      return pConfig_->import_patrols(filepath);
    else if (model == Configuration::Zones)
      return pConfig_->import_zones(filepath);
    else if (model == Configuration::Sensors)
      return pConfig_->import_sensors(filepath);
    else if (model == Configuration::Settings)
      return pConfig_->import_settings(filepath);
    else if (model == Configuration::BaseMap)
      return pConfig_->import_base_map(filepath);
    else if (model == Configuration::StaticMap)
      return pConfig_->import_static_map(filepath);
    else if (model == Configuration::DisplayMap)
      return pConfig_->import_display_map(filepath);
    else if (model == Configuration::TasksScheduled)
      return pConfig_->import_tasks_scheduled(filepath);
    return false;
  }

  bool Configuration::save(Configuration::Model model)
  {
    if (model == Configuration::Patrols)
      return pConfig_->save_patrols();
    else if (model == Configuration::Zones)
      return pConfig_->save_zones();
    else if (model == Configuration::Sensors)
      return pConfig_->save_sensors();
    else if (model == Configuration::Settings)
      return pConfig_->save_settings();
    else if (model == Configuration::BaseMap)
      return pConfig_->save_base_map();
    else if (model == Configuration::StaticMap)
      return pConfig_->save_static_map();
    else if (model == Configuration::DisplayMap)
      return pConfig_->save_display_map();
    else if (model == Configuration::TasksScheduled)
      return pConfig_->save_tasks_scheduled();
    return false;
  }

  bool Configuration::isModified()
  {
      return modification_flag_;
  }

  void Configuration::reset_modification_status()
  {
      modification_flag_ = false;
  }
}
