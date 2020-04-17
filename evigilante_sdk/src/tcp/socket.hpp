#ifndef EOS_EVIGILANTE_SOCKET_HPP
#define EOS_EVIGILANTE_SOCKET_HPP

#include <vector>

#ifdef _WIN32
#include <basetsd.h>
#ifdef _WIN32_WINDOWS
#undef _WIN32_WINDOWS
#endif
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define _WIN32_WINDOWS 0x0501
#define _WIN32_WINNT 0x0501
#ifndef SHUT_RDWR
#define SHUT_RDWR		2
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#endif
#include <memory>

#include "packet.hpp"
#include "ipaddress.hpp"

namespace eos
{
  namespace sdk
  {
    typedef unsigned short Port;

    class Socket
    {
    public:
      typedef std::shared_ptr<Socket> Ptr;
// handle for each target os
#if _WIN32
      typedef int Handle;
      typedef int AddrLength;
      typedef int Param;
#else
      typedef int Handle;
      typedef socklen_t AddrLength;
      typedef char Param;
#endif

      /**
             * @brief The Status enum
             * state result of socket action
             */
      enum Status
      {
          Done, // ok
          NotReady, // socket not ready
          Disconnected, // socket disconnected
          NoData, // receive no data on the socket
          TimeOut, // timeout on the socket
          Error // something bad happened
      };

      Socket();

      /**
     * @brief ~Socket close the socket
     */
      virtual ~Socket();

      /**
     * @brief setBlocking enable/disable wait the end of socket operation
     * like connect, accept, receive
     * @param block true wait the end of operation, false clockwise
     */
      void setBlocking(bool block);

      bool setTimeout(long seconds);

      /**
     * @brief return a static invalid value
     * @return  invalid socket
     */
      static Handle invalid();

      /**
     * @brief test if the socket is valid
     * @return  true, it's valid
     */
      bool valid() const;

      /**
     * @brief getEOS_ErrorStatus
     * @return  the common EOS_Error status
     */
      static Status getErrorStatus();

      /**
     * @brief createAddress create a new address based on address and port
     * @param address the ip (on unsigned int way)
     * @param port the port
     * @return  the structue updated
     */
      static sockaddr_in createAddress(unsigned int address, Port port);

      /**
     * @brief getRemoteAddress return the remote address with IpAddress type
     * @return  the remote address
     */
      IpAddress getRemoteAddress() const;

      /**
     * @brief connect the socket on a server
     * @param address the ip
     * @param port the port
     * @return the result of the operation
     */
      Status connect(IpAddress const &address, Port port);

      /**
     * @brief send a packet throw the socket
     * @param packet
     * @return return result of the operation
     */
      Status send(Packet const &packet);

      /**
     * @brief receive a packet
     * @param packet the one
     * @return operation result
     */
      Status receive(Packet &packet);

      /**
     * @brief send data to the socket
     * @param data raw data
     * @param size
     * @return  operation result
     */
      Status send(void const *data, std::size_t size);

      /**
     * @brief receive raw data
     * @param data
     * @param size
     * @param received how many bytes i received ?
     * @return operation result
     */
      Status receive(void *data, std::size_t size, std::size_t &received);

      /**
     * @brief close the socket
     */
      void close();

      /**
     * @brief close extern socket
     * @param socket
     */
      static void close(Handle socket);

      /**
     * @brief create a socket
     * @param handle and set the handle
     */
      void create(Handle handle);

      /**
     * @brief hasDataReady
     * @param time_usec
     * @return have a data ready to read ?
     */
      bool hasDataReady(unsigned int time_usec);

      void resetPendingPacket();

      Handle& getHandle() {return socket_;}
    protected:
      /**
     * @brief create the socket
     */
      void create();

    private:
      /**
     * @brief The PendingPacket struct
     * used to have a buffer
     */
      struct PendingPacket
      {
        PendingPacket()
            : size_(0)
            , size_received_(0)
            , data_()
        {
        }
        unsigned int size_;
        std::size_t size_received_;
        std::vector<char> data_;
      };

      ///! the real buffer
      PendingPacket pending_;

    protected:
      ///! the socket
      Handle socket_;
    };
  }
}

#endif // EOS_EVIGILANTE_SOCKET_HPP
