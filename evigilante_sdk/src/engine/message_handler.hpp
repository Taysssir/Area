#ifndef EOS_EVIGILANTE_MESSAGE_HANDLER_HPP
#define EOS_EVIGILANTE_MESSAGE_HANDLER_HPP

#include <string>
#include "tcp/packet.hpp"
#include "channel.hpp"

namespace eos
{
    namespace sdk
    {
        template <typename T>
        inline bool toPacket(T const &msg, Packet &packet)
        {
            packet.clear();
            packet << msg.GetDescriptor()->index();

            std::vector<char> data(msg.ByteSize());
            if(msg.SerializeToArray(&data[0], data.size()) == false)
            {
                LOG_WARN(" toPacket - SerializeToArray Failed for " << msg.GetTypeName() );
                return false;
            }
            packet << data;
            return true;
        }

        template <typename T>
        static bool toMessage(Packet &packet, T &msg)
        {
            std::vector<char> data;
            packet >> data;
            return msg.ParseFromArray(&data[0], data.size());
        }

    } // namespace sdk
} // namespace eos

#endif // EOS_EVIGILANTE_MESSAGE_HANDLER_HPP
