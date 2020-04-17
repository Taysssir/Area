#include "resource_manager.hpp"
#include "loader.hpp"
#include "resource_manager/loaders/patrols_loader.hpp"
#include "ziparchive.hpp"
#include "file.hpp"

namespace eos
{
  namespace eosconfig
  {

    BaseResource::Ptr ResourceManager::load(const std::string &media, const std::string &filename, Loader const &loader, bool force)
    {
      if (resourcesPool_.find(filename) != resourcesPool_.end() && force == false)
      {
        return resourcesPool_[filename];
      }

      BaseResource::Ptr rsc;
      RawData data;
      if (openFile(media, filename, data) == false)
      {
        return BaseResource::Ptr();
      }

      if (data.size() == 0)
      {
        return BaseResource::Ptr();
      }

      rsc = loader.load(data);
      if (rsc)
      {
        resourcesPool_[filename] = rsc;
      }
      return rsc;
    }


    bool ResourceManager::save(const std::string &media, const std::string &filename, Loader const &loader)
    {
      bool result = false;
      std::map<std::string, BaseResource::Ptr>::const_iterator it = resourcesPool_.find(filename);
      if (it != resourcesPool_.end())
      {
        result = save(media, filename, it->second, loader);
      }
      return result;
    }

    bool ResourceManager::save(const std::string &media, const std::string &filename, BaseResource::Ptr const rsc, Loader const &loader)
    {
      resourcesPool_[filename] = rsc;
      // if we have a filepath, save the instance
      if(media.size())
      {
        std::string data;
        return loader.save(data, rsc) && saveFile(media, filename, data);
      }
      return true;
    }

    bool ResourceManager::exportModel(std::string const &filepath, std::string const& filename, Loader const &loader)
    {
      bool result = false;
      std::map<std::string, BaseResource::Ptr>::const_iterator it = resourcesPool_.find(filename);
      if (it != resourcesPool_.end())
      {
        RawData data;
        result = loader.save(data, it->second) && exportModel(filepath, data);
      }
      return result;
    }

    bool ResourceManager::exportModel(std::string const& filepath, RawData & data)
    {
        std::fstream output_file;
        output_file.open(filepath, std::fstream::out | std::fstream::binary);
        if(output_file.is_open())
        {
            output_file.write(&data[0], data.size());
            return true;
        }
        return false;
    }

    bool ResourceManager::importModel(std::string const &filepath, std::string const& filename, Loader const &loader)
    {
        BaseResource::Ptr rsc;
        RawData data;
        if (importModel(filepath, data) == false)
        {
          return false;
        }

        if (data.size() == 0)
        {
          return false;
        }

        rsc = loader.load(data);
        if(rsc)
        {
            resourcesPool_[filename] = rsc;
            return true;
        }
        return false;
    }

    bool ResourceManager::importModel(std::string const& filepath, RawData & data)
    {
        std::fstream input_file;
        input_file.open(filepath, std::fstream::in | std::fstream::binary);
        if(input_file.is_open())
        {
            input_file.seekg(0, std::ios::end);
            data.reserve(input_file.tellg());
            input_file.seekg(0, std::ios::beg);

            data.assign((std::istreambuf_iterator<char>(input_file)),
                        std::istreambuf_iterator<char>());
            return true;
        }
        return false;
    }

    bool ResourceManager::openFile(std::string const &media, std::string const &filename, RawData &data)
    {
      ZipArchive za(media);
      za.open(ZipArchive::ReadOnly);
      if (za.hasEntry(filename))
      {
        ZipArchive::Entry entry = za.getEntry(filename);
        data = entry.readAsText();
        za.close();
        return true;
      }
      return false;
    }

    bool ResourceManager::saveFile(std::string const &media, std::string const &filename, RawData const &data)
    {
      ZipArchive za(media);
      za.open(eos::file::exist(media) ? ZipArchive::Write : ZipArchive::New);
      za.addData(filename, &data[0], data.size());
      za.close();
      return true;
    }

    void ResourceManager::reset()
    {
      resourcesPool_.clear();
    }

    namespace
    {
      static constexpr char FILENAME_INFOS[] = "infos";
    }

    bool ResourceManager::addExtraInfo(std::string const &media, std::string const &infos)
    {
      ZipArchive za(media);
      if (za.open(eos::file::exist(media) ? ZipArchive::Write : ZipArchive::New))
      {
        return za.addData(FILENAME_INFOS, &infos[0], infos.size());
      }
      return false;
    }

    bool ResourceManager::hasExtraInfo(std::string const &media)
    {
      ZipArchive za(media);
      za.open(ZipArchive::ReadOnly);
      return za.hasEntry(FILENAME_INFOS);
    }
  }
}
