#ifndef EOS_CONFIGURATION_CONFIGURATION_HPP
#define EOS_CONFIGURATION_CONFIGURATION_HPP

#include <string>
#include <memory>

#include "eos/configuration/models.pb.h"

namespace eos
{
  namespace constants
  {
    // name of file for shared dir output
    namespace shared
    {
      constexpr char BASE_MAP[] = "base_map";
      constexpr char STATIC_MAP[] = "static_map";
      constexpr char DISPLAY_MAP[] = "display_map";
      constexpr char PATROLS[] = "patrols";
      constexpr char ZONES[] = "zones";
      constexpr char SETTINGS[] = "settings";
      constexpr char TASKS_SCHEDULED[] = "tasks";
      constexpr char SENSORS[] = "sensors";
    }
  }

  typedef std::shared_ptr<Patrols> PatrolsSharedPointer;

  typedef std::shared_ptr<Zones> ZonesSharedPointer;

  typedef std::shared_ptr<Sensors> SensorsSharedPointer;

  typedef std::shared_ptr<Settings> SettingsSharedPointer;

  typedef std::shared_ptr<Map> MapSharedPointer;

  typedef std::shared_ptr<TasksScheduled> TasksScheduledSharedPointer;

  uint64_t timestamp();

  namespace eosconfig
  {
    class Configuration;
  }

  class Configuration
  {
  public:
    constexpr static int kMajor = 1;
    constexpr static int kMinor = 0;
    constexpr static int kRelease = 0;

    typedef enum
    {
      Patrols = 0,
      Zones,
      Sensors,
      Settings,
      TasksScheduled,
      BaseMap, // map with just amers
      StaticMap, // map remmaped
      DisplayMap, // map for dislay
      All
    } Model;

    Configuration();

    ~Configuration();

    /**
      * @brief open an eos configuration file
      * @param filepath the absolute path of the file
      * @return true if the file is successfully opended
      */
    bool open(std::string const &filepath);

    /**
     * @brief save the configuration to a filepath
     * @param filepath
     * @return  true in case of success
     */
    bool save(std::string const &filepath);

    /**
      * @brief save the current memory state in eos configuration file
      */
    bool save();

    bool save(Model model);

    void reset();

    void clear();

    /**
     * @brief export a model to a file
     * @param model
     * @param filepath
     * @return
     */
    bool export_model(Model model, std::string const& filepath);

    /**
     * @brief import a model from a file
     * @param model
     * @param filepath
     * @return
     */
    bool import_model(Model model, std::string const& filepath);

    /**
      * @brief isOpened ?
      * @return true if an eos configuration file is opended
      */
    bool isOpened() const;

    /**
         * @brief version
         * @return current version of this library
         */
    static std::string version();

    PatrolsSharedPointer patrols();
    PatrolsSharedPointer const patrols() const;

    ZonesSharedPointer zones();
    ZonesSharedPointer const zones() const;

    SensorsSharedPointer sensors();
    SensorsSharedPointer const sensors() const;

    SettingsSharedPointer settings();
    SettingsSharedPointer const settings() const;

    TasksScheduledSharedPointer tasksScheduled();
    TasksScheduledSharedPointer const tasksScheduled() const;

    MapSharedPointer base_map();
    MapSharedPointer const base_map() const;

    MapSharedPointer static_map();
    MapSharedPointer const static_map() const;

    MapSharedPointer display_map();
    MapSharedPointer const display_map() const;

    bool isModified();
    void reset_modification_status();

  private:
    std::unique_ptr<eosconfig::Configuration> pConfig_;
    bool modification_flag_ = false; // Config modification flag : true = modified / false = unmodified
  };
}

#endif // EOS_CONFIGURATION_CONFIGURATION_HPP
