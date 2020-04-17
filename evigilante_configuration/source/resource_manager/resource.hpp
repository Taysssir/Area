#ifndef EOS_CONFIGURATION_RESOURCE_HPP
#define EOS_CONFIGURATION_RESOURCE_HPP

#include <memory>
#include <string>

namespace eos
{
  namespace eosconfig
  {
    class Loader;

    class BaseResource
    {
    public:
      typedef std::shared_ptr<BaseResource> Ptr;

      template <typename T>
      static std::shared_ptr<T> as(BaseResource::Ptr const rsc)
      {
        return std::static_pointer_cast<T>(rsc);
      }
    };

    template <typename U>
    class Resource
        : public BaseResource,
          public U
    {
    public:
      typedef std::shared_ptr<Resource> Ptr;

      static Ptr alloc()
      {
        return std::make_shared<Resource>();
      }
    };

    typedef std::string RawData;

    // sugar syntax:
    // auto zones = rsc::as<Zones>(rsc);
    // auto zones = rsc::alloc<Zones>();
    namespace rsc
    {
      template <typename T>
      inline std::shared_ptr<Resource<T>> as(BaseResource::Ptr const &rsc)
      {
        return BaseResource::as<Resource<T>>(rsc);
      }

      template <typename T>
      inline std::shared_ptr<Resource<T>> alloc()
      {
        return Resource<T>::alloc();
      }
    }
  }
}

#endif // EOS_CONFIGURATION_RESOURCE_HPP
