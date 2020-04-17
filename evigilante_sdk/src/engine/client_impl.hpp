#ifndef EOS_EVIGILANTE_ROBOT_SSL_IMPL_HPP
#define EOS_EVIGILANTE_ROBOT_SSL_IMPL_HPP

#include <thread>
#include <condition_variable>
#include <iostream>
#include <queue>
#include <mutex>

//#include "tcp/socket.hpp"
// #include "tcp/listener.hpp"
#include "message_handler.hpp"
#include "eos/sdk/client.hpp"
#include "eos/configuration/models.pb.h"
#include "channel.hpp"
#include "tcp/socket.hpp"

namespace eos
{
  namespace sdk
  {
    namespace priv
    {

      /**
     * @brief Client::Client
     * Implement API :
     * - Subscribe to a CMD or Data channel type messagess
     * Manage communication connection :
     * - contains IPAddress and port connection
     * - shall at start up Connect to a remote server
     * - shall manage reconnection to the server
     * - Shall wait to the remote connection for a determined timerfd_create
     * - Shall allow to send and receive data type it suscribe for
     * @param io_service
     */

      // MANAGE TEMPLATE OF
      class Client
      {
        public:
          Client();
          virtual ~Client();
          /**
         * @brief connect
         * Manage communication connection wiht remote
         * if host args IP and host are compliant
         * - if the server is not launched launch the service
         * - else do nothing but return false connection is false
         * else if host args are not compliant
         * - if the service is launched, stop the service
         * @param host
         * @param port
         * @return
         */
          bool connect( std::string const &host, int port);

          /**
         * @brief disconnect
         * This is an order to disconnect the client and stop service
         * clear all internal datas
         * @return
         */
          bool disconnect();

          template <typename T>
          bool pub(T const &msg)
          {
            LOG_INFO("Client::pub -"<< msg.GetTypeName());
            Packet packet;
            toPacket(msg, packet) && pub_msg(packet);
            return true;
          }
          void broadcast() ;
          bool pub_msg(Packet const &packet);

          void  update(RobotDataSubscription const& dataSubscription)
          {
            dataSubscription_ = dataSubscription;
            pub(dataSubscription_);
          }
          /**
         * @brief pub_msg_data
         * Anable User API to receive data msg type
         * @param packet
         * @return
         */

          void subscribe(RobotInterface::Ptr listener);
          void unsubscribe(RobotInterface::Ptr listener);

        private:
          // timeout for robot connection in seconds
          // after that the connexion will be reset
          void onLostConnection();
          void onConnected();
          void run();
          /**
         * @brief treatMessage
         * @param packet
         * @param type
         * @return
         */
          bool treatMessage(Packet &packet);

          std::string robot_ip_;
          int robot_port_;
          bool connected_;
          std::thread thread_;

          std::vector<RobotInterface::Ptr> backends_;
          RobotDataSubscription dataSubscription_;

          std::mutex protect_backends_;
          bool run4ever_;
          Socket socket_;
          std::queue<Packet> queue_;
          std::mutex mutex_thread_;

          std::condition_variable cv_;
          std::condition_variable cv_connection_;
          std::mutex cv_m_;
          std::mutex cv_mutex_connection_;

          // ssh::Transfert transfert_;
      };
      // MOVE to callerClient.cpp !!!
      struct Caller
      {
          void call(RobotInfo const &event, RobotInterface::Ptr backend);
          void call(RobotPosition const &event, RobotInterface::Ptr backend);
          void call(RobotState const &event, RobotInterface::Ptr backend);
          void call(HardwareIssue const &event, RobotInterface::Ptr backend);
          void call(Alarm const &event, RobotInterface::Ptr backend);
          void call(AlarmUpdate const &event, RobotInterface::Ptr backend);
          void call(eos::Patrols const &event, RobotInterface::Ptr backend);
          void call(eos::Zones const &event, RobotInterface::Ptr backend);
          void call(eos::Sensors const &event, RobotInterface::Ptr backend);
          void call(eos::Settings const &event, RobotInterface::Ptr backend);
          void call(BatteryState const &event, RobotInterface::Ptr backend);
          void call(RobotMonitoring const &event, RobotInterface::Ptr backend);
          void call(eos::sdk::NewMapAvailable const &event, RobotInterface::Ptr backend);
          void call(eos::TasksScheduled const &event, RobotInterface::Ptr backend);
          void call(RobotSensors const &event, RobotInterface::Ptr backend);
          void call(sdk::CartoState const &event, RobotInterface::Ptr backend);
          void call(sdk::RobotVelocity const &event, RobotInterface::Ptr backend);

          template <typename T>
          bool call(std::vector<RobotInterface::Ptr> &listeners, Packet &packet, int msg_type)
          {
            if (T::descriptor()->index() == msg_type)
            {
              T msg;
              LOG_INFO("Caller::call Client - " <<msg. GetTypeName() );
              if (toMessage<T>(packet, msg))
              {
                for (auto listener : listeners)
                {
                  call(msg, listener);
                }
                return true;
              }
            }
            return false;
          }

      };
    }
  }
}

#endif // EOS_EVIGILANTE_ROBOT_SSL_IMPL_HPP
