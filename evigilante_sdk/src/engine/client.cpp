#include "eos/sdk/client.hpp"
#include "client_impl.hpp"

namespace eos
{
  namespace sdk
  {
    Client::Client()
    {
      robot_.reset(new priv::Client());
    }

    Client::~Client()
    {
    }

    bool Client::connect(std::string const &host, int port)
    {
      return robot_->connect(host,port);
    }

    bool Client::disconnect()
    {
      return robot_->disconnect();
    }

    void Client::subscribe(RobotInterface::Ptr item)
    {
      robot_->subscribe(item);
    }

    void Client::unsubscribe(RobotInterface::Ptr item)
    {
      robot_->unsubscribe(item);
    }

    void Client::update(RobotDataSubscription const& dataSubscription)
    {
      robot_->update(dataSubscription);
      robot_->pub(dataSubscription);
    }

  } // sdk
} // eos
