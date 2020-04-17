#include <cstring>

#include "ipaddress.hpp"
#include "socket.hpp"

namespace eos
{
  namespace sdk
  {
    IpAddress IpAddress::None;

    namespace
    {
      unsigned int resolve(std::string const &address)
      {
        if (address == "255.255.255.255")
        {
          // The broadcast address needs to be handled explicitely,
          // because it is also the value returned by inet_addr on EOS_Error
          return INADDR_BROADCAST;
        }
        else
        {
          // Try to convert the address as a byte representation ("xxx.xxx.xxx.xxx")
          unsigned int ip = inet_addr(address.c_str());
          if (ip != INADDR_NONE)
            return ip;
          // Warning("Use gethostbyname !!!!!!");
          // host = gethostbyname(address.c_str());
          // if(host)
          //     _address = reinterpret_cast<in_addr*>(host->h_addr)->s_addr;
          // Not a valid address, try to convert it as a host name
          addrinfo hints;
          std::memset(&hints, 0, sizeof(hints));
          hints.ai_family = AF_INET;
          addrinfo *result = NULL;
          if (getaddrinfo(address.c_str(), NULL, &hints, &result) == 0)
          {
            if (result)
            {
              ip = reinterpret_cast<sockaddr_in *>(result->ai_addr)->sin_addr.s_addr;
              freeaddrinfo(result);
              return ip;
            }
          }

          // Not a valid address nor a host name
          return 0;
        }
      }
    }

    IpAddress::IpAddress()
        : address_(0)
    {
    }

    IpAddress::IpAddress(std::string const &address)
        : address_(resolve(address))
    {
    }

    IpAddress::IpAddress(unsigned int address)
        : address_(htonl(address))
    {
    }

    std::string IpAddress::toString() const
    {
      in_addr address;
      address.s_addr = address_;
      return inet_ntoa(address);
    }

    unsigned int IpAddress::toInteger() const
    {
      return ntohl(address_);
    }

    IpAddress IpAddress::getLocalAddress()
    {
      IpAddress localAddress;

      Socket::Handle sock = socket(PF_INET, SOCK_DGRAM, 0);
      if (sock == Socket::invalid())
        return localAddress;

      sockaddr_in address = Socket::createAddress(ntohl(INADDR_LOOPBACK), 9);
      if (connect(sock, reinterpret_cast<sockaddr *>(&address), sizeof(address)) == -1)
      {
        Socket::close(sock);
        return localAddress;
      }

      Socket::AddrLength size = sizeof(address);
      if (getsockname(sock, reinterpret_cast<sockaddr *>(&address), &size) == -1)
      {
        Socket::close(sock);
        return localAddress;
      }

      Socket::close(sock);

      localAddress = IpAddress(ntohl(address.sin_addr.s_addr));

      return localAddress;
    }

    IpAddress IpAddress::getPublicAddress()
    {
      return IpAddress();
    }

    bool operator==(const IpAddress &left, const IpAddress &right)
    {
      return left.toInteger() == right.toInteger();
    }

    bool operator!=(const IpAddress &left, const IpAddress &right)
    {
      return !(left == right);
    }
  }
}
