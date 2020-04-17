#include "server_callsystem.hpp"

namespace eos
{
  namespace sdk
  {
    namespace priv
    {
      bool Caller::treatMessage(std::vector<BackendTreatment::Ptr> &backends, Packet &packet)
      {
        int32_t type = 0;
        packet >> type;
        return false
               || call<DynamicObstacle>(backends, packet,type)
               || call<JoystickCommand>(backends, packet,type)
               || call<Turn>(backends, packet,type)
               || call<Stop>(backends, packet,type)
               || call<SetRobotMode>(backends, packet,type)
               || call<MoveTo>(backends, packet,type)
               || call<Hooter>(backends, packet,type)
               || call<ToggleCharging>(backends, packet,type)
               || call<ToggleComponentsPowerSupply>(backends, packet,type)
               || call<ScoresThreshold>(backends, packet,type)
               || call<ControlPatrolling>(backends, packet,type)
               || call<EmergencyStop>(backends, packet,type)
               || call<ClearDynamicObstacles>(backends, packet,type)
               || call<Reboot>(backends, packet,type)
               || call<CartoOrder>(backends, packet,type);

      }

      void Caller::call(Reboot const &event, BackendTreatment &backend)
      {
        std::function<void(void)>  f = std::bind(&BackendTreatment::onReboot, &backend);
        f();

      }

      void Caller::call(JoystickCommand const &event, BackendTreatment &backend)
      {
        std::function<void(float, float)> f = std::bind(&BackendTreatment::onJoystickCommand,
                                                        &backend,
                                                        std::placeholders::_1,
                                                        std::placeholders::_2);
        f(event.x(), event.y());
      }

      void Caller::call(Turn const &event, BackendTreatment &backend)
      {
        std::function<void(void)> f;
        if (event.sens() == Turn::Sens::Turn_Sens_LEFT)
          f = std::bind(&BackendTreatment::onTurnLeft, &backend);
        else
          f = std::bind(&BackendTreatment::onTurnRight, &backend);
        f();
      }

      void Caller::call(Stop const &, BackendTreatment &backend)
      {
        std::function<void(void)> f = std::bind(&BackendTreatment::onStop, &backend);
        f();
      }

      void Caller::call(SetRobotMode const &event, BackendTreatment &backend)
      {
        std::function<void(SetRobotMode const &)> f = std::bind(&BackendTreatment::onSetMode,
                                                                &backend,
                                                                std::placeholders::_1);
        f(event);
      }

      void Caller::call(MoveTo const &event, BackendTreatment &backend)
      {
        std::function<void(uint, uint, double)> f = std::bind(&BackendTreatment::onMoveTo,
                                                              &backend,
                                                              std::placeholders::_1,
                                                              std::placeholders::_2,
                                                              std::placeholders::_3);
        f(event.x(), event.y(), event.theta());
      }

      void Caller::call(Hooter const &event, BackendTreatment &backend)
      {
        std::function<void(bool)> f = std::bind(&BackendTreatment::onHooter,
                                                &backend,
                                                std::placeholders::_1);
        f(event.active());
      }

      void Caller::call(ToggleCharging const &event, BackendTreatment &backend)
      {
        std::function<void(bool)> f = std::bind(&BackendTreatment::onToggleCharging,
                                                &backend,
                                                std::placeholders::_1);
        f(event.active());
      }

      void Caller::call(ToggleComponentsPowerSupply const &event, BackendTreatment &backend)
      {
        std::function<void(bool)> f = std::bind(&BackendTreatment::onToggleComponentsPowerSupply,
                                                &backend,
                                                std::placeholders::_1);
        f(event.active());
      }

      void Caller::call(CartoOrder const &event, BackendTreatment &backend)
      {
        std::function<void(CartoOrder const &)> f = std::bind(&BackendTreatment::onCartoOrder,
                                                              &backend,
                                                              std::placeholders::_1);
        f(event);
      }

      void Caller::call(ScoresThreshold const &event, BackendTreatment &backend)
      {
        std::function<void(ScoresThreshold const &)> f = std::bind(&BackendTreatment::onNewScoresThreshold,
                                                                   &backend,
                                                                   std::placeholders::_1);
        f(event);
      }

      void Caller::call(ControlPatrolling const &event, BackendTreatment &backend)
      {
        std::function<void(eos::sdk::ControlPatrolling)> f = std::bind(&BackendTreatment::onControlPatrolling,
                                                                   &backend,
                                                                   std::placeholders::_1);
        f(event);
      }

      void Caller::call(EmergencyStop const &event, BackendTreatment &backend)
      {
        std::function<void(bool)> f = std::bind(&BackendTreatment::onEmergencyStop,
                                                                   &backend,
                                                                   std::placeholders::_1);
        f(event.active());
      }

      void Caller::call(DynamicObstacle const &event, BackendTreatment &backend)
      {
        std::function<void(DynamicObstacle)> f = std::bind(&BackendTreatment::onDynamicObstacle,
                                                                   &backend,
                                                                   std::placeholders::_1);
        f(event);
      }

      void Caller::call(ClearDynamicObstacles const &, BackendTreatment &backend)
      {
        std::function<void()> f = std::bind(&BackendTreatment::onClearDynamicObstacles,
                                                                   &backend);
        f();
      }

    }
  }
}
