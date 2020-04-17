#include "eos/sdk/server.hpp"
#include "server_impl.hpp"

namespace eos
{
  namespace sdk
  {
    Server::Server()
    {
       // _io_service.reset(new asio::io_service());
       server_.reset(new priv::Server );
       // io_service_work_.reset(new asio::io_service::work(*_io_service));
       // thread_ = std::thread([this](){ _io_service->run(); });
    }

    Server::~Server()
    {
      server_->close();
    }

    bool Server::listen(unsigned short port)
    {
      if(server_)
        return server_->listen(port);
      return false;
    }

    void Server::setRobotType(std::string const& robot_type)
    {
      if(server_)
        server_->setRobotType(robot_type);
    }

    void Server::setCamera(eos::sdk::Camera::Type const &type, std::string const& url, int &port, int id)
    {
      if(server_)
        server_->setCamera(type, url,port,id);
    }

    void Server::close()
    {
        server_->close();
    }

    void Server::subscribe(BackendTreatment::Ptr item)
    {
      server_->subscribe(item);
    }
  }
}
