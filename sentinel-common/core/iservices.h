#ifndef SENTINEL_COMMON_ISERVICES_H
#define SENTINEL_COMMON_ISERVICES_H

#include <map>
#include <QStringList>

#include "eventmanager.h"
#include "appinstance.h"

namespace core
{
  class IServices
  {
  public:
    ~IServices()
    {
      for (auto instance : instances_)
      {
        delete instance.second;
      }
      instances_.clear();
    }

    template <typename T, typename App>
    T *launchService(QString const &name)
    {
      T *inst = new T(*static_cast<App *>(this));
      instances_[name] = inst;
      return inst;
    }

    template <typename T>
    T *getService()
    {
      for (std::map<QString, core::BaseReceiver *>::iterator it = instances_.begin();
           it != instances_.end(); ++it)
      {
        if (typeid(T) == typeid(*(*it).second))
          return static_cast<T *>((*it).second);
      }
      return 0;
    }

    core::BaseReceiver *getService(QString const &name)
    {
      for (std::map<QString, core::BaseReceiver *>::iterator it = instances_.begin();
           it != instances_.end(); ++it)
      {
        if (it->first == name)
          return (*it).second;
      }
      return 0;
    }

    QStringList getServicesName() const
    {
      QStringList list;
      for (auto const &instance : instances_)
        list << instance.first;
      return list;
    }

  private:
    std::map<QString, core::BaseReceiver *> instances_;
  };
}

#endif // SENTINEL_COMMON_ISERVICES_H
