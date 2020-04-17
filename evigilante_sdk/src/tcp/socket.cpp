#include <memory.h>
#include <cstring>
#include <algorithm>
#include <iostream>

#ifndef WIN32
#include <errno.h>
#include <fcntl.h>
#endif

#include "socket.hpp"

namespace eos
{
    namespace sdk
    {
        namespace
        {
            // Define the low-level send/receive flags, which depend on the OS
#ifdef WIN32
            const int flags = 0;
#else
            const int flags = MSG_NOSIGNAL;
#endif
            struct HexCharStruct
            {
                    unsigned char c;
                    HexCharStruct(unsigned char _c) : c(_c) { }
            };

            inline std::ostream& operator<<(std::ostream& o, const HexCharStruct& hs)
            {
                return (o << std::hex << (int)hs.c);
            }

            inline HexCharStruct hex(unsigned char _c)
            {
                return HexCharStruct(_c);
            }

        }

        Socket::Socket()
            : socket_(invalid())
        {
        }

        Socket::~Socket()
        {
            close();
        }

        bool Socket::valid() const
        {
            return socket_ != invalid();
        }

        Socket::Handle Socket::invalid()
        {
#ifdef WIN32
            return INVALID_SOCKET;
#else
            return -1;
#endif
        }

        void Socket::create(Handle handle)
        {
            if (socket_ == invalid())
            {
                socket_ = handle;
                int on = 1;
                setBlocking(true);
                if (setsockopt(socket_, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char *>(&on), sizeof(on)) == -1)
                {
                    std::cerr << "Failed to set socket option \"TCP_NODELAY\" ; "
                              << "all your TCP packets will be buffered" << std::endl;
                }
            }
        }

        bool Socket::setTimeout(long seconds)
        {
            struct timeval tv;
            tv.tv_sec = seconds;
            tv.tv_usec = 0;
            setBlocking(true);
            return setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char *>(&tv), sizeof(struct timeval)) >= 0;
            //&& setsockopt(socket_, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<char *>(&tv), sizeof(struct timeval)) >= 0;
        }

        void Socket::create()
        {
            if (socket_ == invalid())
            {
                Socket::Handle handle = socket(PF_INET, SOCK_STREAM, 0);
                create(handle);
            }
        }

        void Socket::close(Socket::Handle socket)
        {
            if (socket != invalid())
            {
#ifdef WIN32
                closesocket(socket);
#else
                ::close(socket);
#endif
            }
        }

        void Socket::close()
        {
            this->close(socket_);
            socket_ = invalid();
        }

        Socket::Status Socket::getErrorStatus()
        {
#ifdef WIN32
            switch (WSAGetLastError())
            {
            case WSAEWOULDBLOCK:
                return Socket::NotReady;
            case WSAECONNABORTED:
                return Socket::Disconnected;
            case WSAECONNRESET:
                return Socket::Disconnected;
            case WSAETIMEDOUT:
                return Socket::Disconnected;
            case WSAENETRESET:
                return Socket::Disconnected;
            case WSAENOTCONN:
                return Socket::Disconnected;
            default:
                return Socket::Error;
            }
#else
            if ((errno == EAGAIN) || (errno == EINPROGRESS))
                return Socket::NotReady;

            switch (errno)
            {
            case EWOULDBLOCK:
                return Socket::NotReady;
            case ECONNABORTED:
                return Socket::Disconnected;
            case ECONNRESET:
                return Socket::Disconnected;
            case ETIMEDOUT:
                return Socket::Disconnected;
            case ENETRESET:
                return Socket::Disconnected;
            case ENOTCONN:
                return Socket::Disconnected;
            default:
                return Socket::Error;
            }
#endif
            return Socket::Error;
        }

        sockaddr_in Socket::createAddress(unsigned int address, unsigned short port)
        {
            sockaddr_in addr;
            std::memset(&addr, 0, sizeof(addr));
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = htonl(address);
            addr.sin_port = htons(port);

            return addr;
        }

        Socket::Status Socket::connect(IpAddress const &remoteAddress, Port port)
        {
            create();

            sockaddr_in address = createAddress(remoteAddress.toInteger(), port);

            setBlocking(false);

            if (::connect(socket_, reinterpret_cast<sockaddr *>(&address), sizeof(address)) >= 0)
            {
                setBlocking(true);
                return Done;
            }

            //return getEOS_ErrorStatus();
            Status status = getErrorStatus();

            if (status == Socket::NotReady)
            {
                fd_set selector;
                FD_ZERO(&selector);
                FD_SET(socket_, &selector);

                timeval time;
                time.tv_sec = static_cast<long>(500000 / 1000000);
                time.tv_usec = static_cast<long>(500000 % 1000000);

                // Wait for something to write on our socket (which means that the connection request has returned)
                if (select(static_cast<int>(socket_ + 1), NULL, &selector, NULL, &time) > 0)
                {
                    if (getRemoteAddress() != IpAddress::None)
                    {
                        status = Done;
                        //status = getErrorStatus();
                    }
                    else
                    {
                        status = getErrorStatus();
                    }
                }
                else
                {
                    status = getErrorStatus();
                }
            }

            setBlocking(true);

            return status;
        }

        void Socket::setBlocking(bool block)
        {
#ifdef WIN32
            u_long blocking = block ? 0 : 1;
            ioctlsocket(socket_, FIONBIO, &blocking);
#else
            int status = fcntl(socket_, F_GETFL);
            if (block)
                fcntl(socket_, F_SETFL, status & ~O_NONBLOCK);
            else
                fcntl(socket_, F_SETFL, status | O_NONBLOCK);
#endif
        }

        IpAddress Socket::getRemoteAddress() const
        {
            if (socket_ != invalid())
            {
                sockaddr_in address;
                Socket::AddrLength size = sizeof(address);
                if (getpeername(socket_, reinterpret_cast<sockaddr *>(&address), &size) != -1)
                {
                    return IpAddress(ntohl(address.sin_addr.s_addr));
                }
            }

            return IpAddress::None;
        }

        bool Socket::hasDataReady(unsigned int time_usec)
        {
            struct timeval tv = { 0, 0 };
            fd_set rdfs;

            FD_ZERO(&rdfs);
            FD_SET(socket_, &rdfs);
            tv.tv_sec = 0;
            tv.tv_usec = time_usec;

            int ret = select(socket_ + 1, &rdfs, NULL, NULL, &tv);
            if (ret)
                return true;
            return false;
        }

        Socket::Status Socket::receive(void *data, std::size_t size, std::size_t &received)
        {
            received = 0;

            if (!data)
            {
                return Error;
            }

            int sizeReceived = recv(socket_, static_cast<char *>(data), static_cast<int>(size), flags);

            if (sizeReceived > 0)
            {
                received = static_cast<std::size_t>(sizeReceived);
                return Done;
            }
            else if (sizeReceived == 0)
                return Disconnected;
            return getErrorStatus();
        }

        Socket::Status Socket::send(const void *data, std::size_t size)
        {
            if (!data || (size == 0))
            {
                //LOG_DEBUG(" data "<< data << " size " << size);
                return Error;
            }
            int sent = 0;
            int sizeToSend = static_cast<int>(size);
            // LOG_DEBUG(" sizeToSend "<< sizeToSend );

            for (int length = 0; length < sizeToSend; length += sent)
            {
                sent = ::send(socket_, static_cast<const char *>(data) + length, sizeToSend - length, flags);

                if (sent < 0)
                {
                    LOG_WARN("Socket::send - < 0");
                    return getErrorStatus();
                }
            }
            return Done;
        }


        Socket::Status Socket::receive(Packet &packet)
        {
            packet.clear();

            unsigned int packetSize = 0;
            std::size_t received = 0;
            if (pending_.size_received_ < sizeof(pending_.size_))
            {
                while (pending_.size_received_ < sizeof(pending_.size_))
                {
                    char *data = reinterpret_cast<char *>(&pending_.size_) + pending_.size_received_;
                    Status status = receive(data, sizeof(pending_.size_) - pending_.size_received_, received);
                    pending_.size_received_ += received;

                    if (status != Done)
                        return status;
                }

                packetSize = ntohl(pending_.size_);
            }
            else
            {
                packetSize = ntohl(pending_.size_);
            }
            char dummy[1024];
            while (pending_.data_.size() < packetSize)
            {
                std::size_t sizeToGet = std::min(static_cast<std::size_t>(packetSize - pending_.data_.size()), sizeof(dummy));

                Status status = receive(dummy, sizeToGet, received);
                if (status != Done)
                    return status;
                if (received > 0)
                {
                    pending_.data_.resize(pending_.data_.size() + received);
                    char *begin = &pending_.data_[0] + pending_.data_.size() - received;
                    std::memcpy(begin, dummy, received);
                }
                else
                    return Status::Error;
            }

            if (!pending_.data_.empty())
                packet.append(&pending_.data_[0], pending_.data_.size());

            pending_ = PendingPacket();
            return Done;
        }

        void Socket::resetPendingPacket()
        {
            pending_ = PendingPacket();
        }

        Socket::Status Socket::send(Packet const &packet)
        {
            std::size_t size = packet.getDataSize();
            if (size == 0)
                return Error;

            unsigned int packet_size = htonl(static_cast<unsigned int>(size));

            Socket::Status status = send(&packet_size, sizeof(packet_size));

            if(status == Status::Done)
            {
                status = send(packet.getData(), size) ;
            }
            return status; //  send(&data[0], data.size());
        }


#ifdef WIN32
        namespace
        {
            struct WindowsSpecific
            {
                    WindowsSpecific()
                    {
                        WSADATA init;
                        WSAStartup(MAKEWORD(2, 2), &init);
                    }

                    ~WindowsSpecific()
                    {
                        WSACleanup();
                    }
            };
            WindowsSpecific WindowsSocketRequired;
        }
#endif
    }
}
