#include "msgs.pb.h"
#include "server.hpp"
#include "packet.hpp"
#include "message_handler.hpp"

namespace eos
{
    namespace sdk
    {
        namespace priv
        {

            struct Caller
            {
            public:
                static bool treatMessage(std::vector<BackendTreatment::Ptr> &backends, Packet &packet);

            private:
                static void call(JoystickCommand const &event, BackendTreatment &backend);
                static void call(Turn const &event, BackendTreatment &backend);
                static void call(Stop const &event, BackendTreatment &backend);
                static void call(SetRobotMode const &event, BackendTreatment &backend);
                static void call(MoveTo const &event, BackendTreatment &backend);
                static void call(Hooter const &event, BackendTreatment &backend);
                static void call(ToggleCharging const &event, BackendTreatment &backend);
                static void call(ToggleComponentsPowerSupply const &event, BackendTreatment &backend);
                static void call(CartoOrder const &event, BackendTreatment &backend);
                static void call(TasksScheduled const &event, BackendTreatment &backend);
                static void call(ScoresThreshold const &event, BackendTreatment &backend);
                static void call(ControlPatrolling const &event, BackendTreatment &backend);
                static void call(EmergencyStop const &event, BackendTreatment &backend);
                static void call(DynamicObstacle const &event, BackendTreatment &backend);
                static void call(ClearDynamicObstacles const &event, BackendTreatment &backend);
                static void call(Reboot const &event, BackendTreatment &backend);

                template <typename T>
                static bool call(std::vector<BackendTreatment::Ptr> &listeners, Packet &packet, int32_t type)
                {
                    if( T::descriptor()->index() == type )
                    {
                        T msg;
                        if (toMessage<T>(packet, msg))
                        {
                            LOG_INFO("Caller::call Server - " << msg. GetTypeName() );
                            for (auto listener : listeners)
                            {
                                call(msg, *listener);
                            }
                            return true;
                        }
                    }

                    return false;
                }
            };
        } // namespace priv
    } // namespace sdk
} // namespace eos
