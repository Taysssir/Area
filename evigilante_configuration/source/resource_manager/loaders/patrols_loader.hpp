#ifndef EOS_CONFIGURATION_PATROLS_LOADER_HPP
#define EOS_CONFIGURATION_PATROLS_LOADER_HPP

#include "resource_manager/loader.hpp"

namespace eos
{
  namespace eosconfig
  {
    class PatrolsLoader
        : public Loader
    {
    public:

      BaseResource::Ptr load(RawData const &data) const;
      BaseResource::Ptr loadJson(const RawData &data) const;
      BaseResource::Ptr loadXml(const RawData &data) const;

      bool save(RawData &data, BaseResource::Ptr const &rsc) const;
      bool saveJson(RawData &data, BaseResource::Ptr const &rsc) const;
      bool saveXml(RawData &data, BaseResource::Ptr const &rsc) const;

    protected:
      bool m_load_json; // new version true by default
    };
  }
}

#endif // EOS_CONFIGURATION_PATROLS_LOADER_HPP
