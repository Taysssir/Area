#ifndef EOS_CONFIGURATION_RESOURCE_MANAGER_H
#define EOS_CONFIGURATION_RESOURCE_MANAGER_H

#include <string>
#include <memory>
#include <map>

#include "resource.hpp"
#include "loader.hpp"

namespace eos
{
  namespace eosconfig
  {
    typedef unsigned int uint;
    class Loader;

    class ResourceManager
    {
    public:
      template <typename L, typename T>
      std::shared_ptr<T> load(std::string const &media, std::string const &filename, bool force);

      template <typename L, typename T>
      std::shared_ptr<T> load(std::string const &media, std::string const &filename, bool force) const;

      template <typename T>
      std::shared_ptr<T> reload(std::string const &media, std::string const &filename);

      template <typename L, typename T>
      bool save(std::string const &media, std::string const &filename);

      template <typename L, typename T>
      bool exportModel(std::string const& filepath, std::string const& filename);

      template <typename L, typename T>
      bool importModel(std::string const& filepath, std::string const& filename);

      template <typename L, typename T>
      bool save(std::string const &media, std::string const &filename, std::shared_ptr<T> const rsc);

      void reset();

      bool addExtraInfo(std::string const &media, std::string const &infos);

      bool hasExtraInfo(std::string const &media);

    private:
      typedef uint Key;

      BaseResource::Ptr load(std::string const &media, std::string const &filename, Loader const &loader, bool force = false);

      bool save(std::string const &media, std::string const &filename, BaseResource::Ptr const rsc, Loader const &loader);

      bool save(std::string const &media, std::string const &filename, Loader const &loader);

      bool exportModel(std::string const &filepath, std::string const& filename, Loader const &loader);

      bool exportModel(std::string const& filepath, RawData & data);

      bool importModel(std::string const &filepath, std::string const& filename, Loader const &loader);

      bool importModel(std::string const& filepath, RawData & data);

      bool openFile(std::string const &media, std::string const &filename, RawData &data);

      bool saveFile(std::string const &media, std::string const &filename, RawData const &data);

      ///! cache of resources
      std::map<std::string, BaseResource::Ptr> resourcesPool_;
    };

    template <typename L, typename T>
    inline std::shared_ptr<T> ResourceManager::load(const std::string &media, std::string const &filename, bool force)
    {
      return std::static_pointer_cast<T>(load(media, filename, L(), force));
    }

    template <typename L, typename T>
    inline std::shared_ptr<T> ResourceManager::load(const std::string &media, std::string const &filename, bool force) const
    {
      return std::const_pointer_cast<T>(load<L, T>(media, filename, force));
    }

    template <typename T>
    inline std::shared_ptr<T> ResourceManager::reload(const std::string &media, std::string const &filename)
    {
      //return std::dynamic_pointer_cast<T>(load(media, filename, T::loader(), true));
      return nullptr;
    }

    template <typename L, typename T>
    inline bool ResourceManager::save(const std::string &media, std::string const &filename, std::shared_ptr<T> const rsc)
    {
      return save(media, filename, rsc, L());
    }

    template <typename L, typename T>
    bool ResourceManager::save(std::string const &media, std::string const &filename)
    {
      return save(media, filename, L());
    }

    template <typename L, typename T>
    bool ResourceManager::exportModel(std::string const& filepath, std::string const& filename)
    {
      return exportModel(filepath, filename, L());
    }

    template <typename L, typename T>
    bool ResourceManager::importModel(std::string const& filepath, std::string const& filename)
    {
      return importModel(filepath, filename, L());
    }
  }
}

#endif // EOS_CONFIGURATION_RESOURCE_MANAGER_H
