#ifndef EOS_EVIGILANTE_SERVER_IMPL_HPP
#define EOS_EVIGILANTE_SERVER_IMPL_HPP

#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include <mutex>
#include <condition_variable>

#include <algorithm>    // std::find
#include "tcp/listener.hpp"
#include "eos/sdk/server.hpp"
#include "message_handler.hpp"
#include "logger.hpp"

namespace eos
{
    namespace sdk
    {
        namespace priv
        {
            class Server
            {
                public:
                    Server();
                    ~Server();

                    bool listen(unsigned short port);

                    void close();

                    void setRobotType(std::string const& robot_type);

                    void setCamera(const Camera::Type &type, std::string const& url, int &port, int id);

                    template <typename T>
                    bool pub(T const &msg)
                    {
                        LOG_INFO("Server::pub -"<< msg.GetTypeName());
                        Packet packet;
                        return toPacket(msg, packet) && pub_msg(packet);
                    }

                    void treatMessage(Packet & packet );

                    void subscribe(BackendTreatment::Ptr listener);

                    void sendBatteryState(BatteryState const &state);

                    void sendRobotInfo(RobotInfo robot);

                private :
                    /**
                     * @brief run application sender reactor and msg handler
                     */
                    void run();

                    bool pub_msg(Packet const &packet);
                    void sendAllPacket();
                    void broadcast(Packet const& packet);
                    void onConnected();

                    bool run4ever_;
                    std::thread thread_;
                    Listener listener_;
                    std::vector<Socket::Ptr> consumers_;
                    // std::vector<Consumer::Ptr> consumers_new;
                    Socket::Ptr socket_;
                    std::vector<BackendTreatment::Ptr> backends_;
                    std::vector<Packet> packets_;
                    eos::sdk::RobotInfo robot_info_;
                    std::mutex mutex_thread_;
                    std::mutex protect_backends_;

                    std::condition_variable cv_;
                    std::mutex cv_m_;
                    // std::mutex mutex_run_;
                    // int interrupt_pipe[2];
                    int should_exit = 0;
            };


            class Consumer
            {
                public :
                    Consumer(Socket::Ptr socket) : socket_(socket)
                    {
                        subscribe_.set_all(true);
                    }

                    /**
                     * @brief isSubscribed return true if is subbscribed to a given type
                     * @param type
                     * @return
                     */
                    bool isSubscribed(int& type) const
                    {
                           return  subscribe_.all()
                                || type == RobotInfo::descriptor()->index()
                                || type == RobotDataSubscription::descriptor()->index()
                                || std::find( subscribe_.dataindex().begin(),
                                              subscribe_.dataindex().end(),
                                              type) !=  subscribe_.dataindex().end();
                    }
                    /**
                     * @brief subscribe to all data or to an array list of at least one datum
                     * @param subscriptiondata
                     */
                    void subscribe(RobotDataSubscription subscriptiondata)
                    {
                        LOG_INFO(" subscribe - ");
                        subscribe_ = subscriptiondata;
                    }

                    /**
                     * @brief Ptr Type def shared pointer of consumer
                     */
                    typedef std::shared_ptr<Consumer> Ptr;

                private :
                    Socket::Ptr socket_;
                    RobotDataSubscription subscribe_;

            };


        } // namespace priv
    } // namespace sdk
} // namespace eos

#endif // EOS_EVIGILANTE_SERVER_IMPL_HPP
