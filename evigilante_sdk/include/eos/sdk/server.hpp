#ifndef EOS_EVIGILANTE_SERVER_HPP
#define EOS_EVIGILANTE_SERVER_HPP

#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <functional>
#include <eos/configuration/models.pb.h>

#include "msgs.pb.h"
#include "channel.hpp"
#include "log_interface.hpp"
#include <stdlib.h>

namespace eos
{
  namespace sdk
  {
    typedef unsigned int uint;
    namespace priv
    {
      class Server;
    }

    class BackendTreatment
    {
    public:
      typedef std::shared_ptr<BackendTreatment> Ptr;

      BackendTreatment();

      virtual void onNewClient()
      {
      }

      virtual void onJoystickCommand(float /*x*/, float /*y*/)
      {
      }

      virtual void onTurnLeft()
      {
      }

      virtual void onTurnRight()
      {
      }

      virtual void onStop()
      {
      }

      virtual void onSetMode(SetRobotMode const & /*mode*/)
      {
      }

      virtual void onMoveTo(uint /*x*/, uint /*y*/, double /*theta*/)
      {
      }

      virtual void onHooter(bool /*active*/)
      {
      }

      virtual void onToggleCharging(bool /*active*/)
      {
      }

      virtual void onToggleComponentsPowerSupply(bool /*active*/)
      {
      }

      virtual void onCartoOrder(CartoOrder const & /*order*/)
      {
      }

      // ############################################################
      virtual void onNewScoresThreshold(ScoresThreshold const & /*scores*/)
      {
      }

      // ############################################################
      void sendUsersInformation();

      // ############################################################
      void sendRobotInfo();

      // ############################################################
      void sendAlarm(Alarm const &alarm);

      // ############################################################
      void sendAlarmUpdate(AlarmUpdate const &alarm);

      // ############################################################
      void sendRobotPosition(RobotPosition const &position);

      // ############################################################
      void sendRobotState(RobotState::Type type);

      // ############################################################
      void sendRobotHardwareIssue(HardwareIssue::Issue issue);

      // ############################################################
      bool sendBatteryState(BatteryState const &state);

      // ############################################################
      bool sendMonitoringMessage(RobotMonitoring const &monitoring);

      // ############################################################
      bool sendRobotSensors(RobotSensors const &sensors);

      bool sendNewMapAvailable();

      bool sendRobotVelocity(sdk::RobotVelocity  const &msg);

      virtual void onControlPatrolling(eos::sdk::ControlPatrolling /*control_patrolling*/)
      {

      }

      virtual void onEmergencyStop(bool /*enable*/)
      {

      }

      /**
       * @brief event receive from client : ignore a zone
       */
      virtual void onDynamicObstacle(DynamicObstacle const& /*dyno*/)
      {

      }

      /**
       * @brief clear all previous obstacles
       */
      virtual void onClearDynamicObstacles()
      {

      }

      void onReboot()
      {
          system("shutdown -r now");
      }

      /**
        * @brief internal function, set the server pointer
        * @param server
        */
      void setServer(priv::Server *server)
      {
        server_ = server;
      }

    private:
      priv::Server *server_;
      eos::Map map_;
      int map_part_number_;
      int map_current_part_number_;
    protected:
      bool able_to_treat_map_ = false;
    };

    class Server
            : public LogInterface
    {
    public:
      Server();
      ~Server();

      template <typename T, typename... Args>
      std::shared_ptr<T> newInterface(Args &&... args)
      {
        std::shared_ptr<T> ptr = std::make_shared<T>(args...);
        subscribe(ptr);
        return ptr;
      }

      bool listen(unsigned short port);

      /**
       * @brief define the robot type, evigilante or evolve for the momentt :)
       * @param robot_type
       */
      void setRobotType(std::string const& robot_type);

      /**
       * @brief define the url or each camera (F44 based or axis)
       * @param type
       * @param url
       */
      void setCamera(eos::sdk::Camera::Type const &type, std::string const& ipAdress, int &port, int id = 0);

      /**
        * @brief stop listening
        */
      void close();

      /**
        * @brief add new listener
        * @param item
        */
      void subscribe(BackendTreatment::Ptr item);

    private:
      friend class BackendTreatment;
      std::shared_ptr<priv::Server> server_;
      std::thread thread_;
    };
  }
}

#endif // EOS_EVIGILANTE_SERVER_HPP
