#include <cstring>

#include "server.hpp"
//#include "backend_treatment.hpp"
#include "server_impl.hpp"

namespace eos
{
  namespace sdk
  {
    BackendTreatment::BackendTreatment()
        : server_(0)
    {
    }
    void BackendTreatment::sendRobotInfo()
    {
      RobotInfo infos;
      infos.set_name("Agent K");
      infos.set_uptime(100);
      if (server_)
        server_->pub(infos);
    }

    void BackendTreatment::sendAlarm(Alarm const &alarm)
    {
      if (server_)
        server_->pub(alarm);
    }

    void BackendTreatment::sendAlarmUpdate(AlarmUpdate const &alarm)
    {
      if (server_)
        server_->pub(alarm);
    }

    void BackendTreatment::sendRobotPosition(RobotPosition const &position)
    {
      if (server_)
        server_->pub(position);
    }

    void BackendTreatment::sendRobotHardwareIssue(HardwareIssue::Issue issue)
    {
      if (server_)
      {
        HardwareIssue issue_msg;
        issue_msg.set_issue(issue);
        server_->pub(issue_msg);
      }
    }

    void BackendTreatment::sendRobotState(RobotState::Type type)
    {
      if (server_)
      {
        RobotState state;
        state.set_type(type);
        server_->pub(state);
      }
    }

    bool BackendTreatment::sendNewMapAvailable()
    {
      return server_ && server_->pub(NewMapAvailable());
    }

    bool BackendTreatment::sendBatteryState(BatteryState const &state)
    {
      return server_ && server_->pub(state);
    }

    bool BackendTreatment::sendMonitoringMessage(RobotMonitoring const &monitoring)
    {
        return server_ && server_->pub(monitoring);
    }

    bool BackendTreatment::sendRobotSensors(RobotSensors const &sensors)
    {
        return server_ &&  server_->pub(sensors);
    }
    bool BackendTreatment::sendRobotVelocity(sdk::RobotVelocity const &msg)
    {
        if(server_ == nullptr)
        {
            LOG_WARN("server is null - ");
        }
        else
        {
            LOG_WARN("server is NOT null - ");

        }

        return server_ && server_->pub(msg);
    }

    void onReboot()
    {

    }

  } // sdk
} // eos
