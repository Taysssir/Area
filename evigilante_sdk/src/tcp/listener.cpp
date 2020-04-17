#include <iostream>

#include "listener.hpp"

namespace eos
{
  namespace sdk
  {
    Listener::Listener()
        : Socket()
    {
    }

    Socket::Status Listener::listen(Port port, size_t nbMax)
    {
      create();

      if (socket_ == invalid())
        return Socket::Error;

      sockaddr_in address = Socket::createAddress(INADDR_ANY, port);

      // int z;     /* Status code */
      //int s;   /* Socket number */
      int so_reuseaddr = 1;

      if (setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR,
                     (const char *)&so_reuseaddr,
                     sizeof so_reuseaddr) == -1)
      {
        std::cerr << "Failed to set socket options ." << std::endl;
        throw std::string("Failed to set socket options.");
      }

      if (::bind(socket_, reinterpret_cast<sockaddr *>(&address), sizeof(address)) == -1)
      {
        // Not likely to happen, but...
        //std::cerr << "Failed to bind listener socket to port " << port << std::endl;
        return Error;
      }

      // Socket::Param on = 0;
      // ////// ignore "socket already in use" EOS_Errors
      // if (setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, (const char *)&on, sizeof(on)) == -1)
      //     throw std::string("Failed to set socket options.");

      if (::listen(socket_, nbMax) == -1)
      {
        // Oops, socket is deaf
        //std::cerr << "Failed to listen to port " << port << std::endl;
        return Error;
      }

      return Done;
    }

    Socket::Status Listener::accept(Socket &socket)
    {
      if (socket_ == invalid())
      {
        //std::cerr << "Failed to accept a new connection, the socket is not listening" << std::endl;
        return Error;
      }

      // Accept a new connection
      sockaddr_in address;
      Socket::AddrLength length = sizeof(address);

      setBlocking(false);

      Socket::Handle remote = ::accept(socket_, reinterpret_cast<sockaddr *>(&address), &length);

      if (remote == invalid())
      {
        return Socket::getErrorStatus();
      }

      socket.close();

      socket.create(remote);

      return Done;
    }
  }
}
