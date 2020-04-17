#ifndef EOS_CONFIGURATION_TASKSSCHEDULED_LOADER_HPP
#define EOS_CONFIGURATION_TASKSSCHEDULED_LOADER_HPP

#include "resource_manager/loader.hpp"

namespace eos
{
  namespace eosconfig
  {
    class TasksScheduledLoader
        : public Loader
    {
    public:
      BaseResource::Ptr load(RawData const &data) const;
      bool save(RawData &data, BaseResource::Ptr const &rsc) const;
    };
  }
}

#endif // EOS_CONFIGURATION_TASKSSCHEDULED_LOADER_HPP
