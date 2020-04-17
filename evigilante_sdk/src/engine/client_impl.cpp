#include <iostream>
#include <cstring>

#include "client_impl.hpp"
#include "eos/sdk/client.hpp"
#include "tcp/logger.hpp"
#include <functional>

namespace eos
{
    namespace sdk
    {
        namespace priv
        {

            Client::Client()
                :  connected_(false), run4ever_( false )
            {
                thread_ = std::thread(&Client::run, this);
                dataSubscription_.set_all( true );
            }

            Client::~Client()
            {
                run4ever_ = false;
                disconnect();
                if(thread_.joinable())
                {
                    thread_.join();
                }
                LOG_INFO( "Client::~Client() - Joingnable thread [OK]" );
            }
            int fd_client;
            // select add ffile descriptor for close handler
            void Client::run()
            {
                run4ever_ = true;
                connected_ = false;
                socket_.setBlocking(false);

                Socket::Status status = Socket::Status::Disconnected;
                Packet packet;
                fd_set rdfs;

                while(run4ever_)
                {
                    if(connected_ == false)
                    {
                        if(robot_ip_.empty() == false)
                        {
                            status = socket_.connect(IpAddress(robot_ip_), robot_port_);
                            socket_.setBlocking(false);

                            if(status == Socket::Done)
                            {
                                onConnected();
                                connected_ = true;
                                continue;
                            }
                        }
                        std::unique_lock<std::mutex> lk(cv_m_);
                        cv_.wait_for(lk, std::chrono::milliseconds(500));
                    }
                    else
                    { // we are connected !
                        // add the socket
                        int &sock  = socket_.getHandle();
                        timeval tv= {0,10000}; // usec

                        FD_ZERO(&rdfs);
                        FD_SET(sock, &rdfs);

                        int activity = select(sock + 1, &rdfs, NULL, NULL, &tv);
                        if( activity == -1)
                        {
                            LOG_WARN("Client::run - SELECT FAILED -");
                            continue ;
                        }
                        else if(FD_ISSET(sock, &rdfs))
                        {
                            packet.clear();
                            status = socket_.receive(packet);
                            if(status == Socket::Done)
                            {
                                if(not treatMessage(packet))
                                {
                                    LOG_WARN("Client::run - FAILED to call msg type: " << packet.getType());
                                }

                            }
                            else if(status == Socket::Disconnected || status ==  Socket::Error)
                            {
                                onLostConnection();
                                continue;
                            }

                        }
                        broadcast();

                    }

                }
                LOG_DEBUG("Client::Run - END  ");

            }

            bool Client::connect(
                    std::string const &host,
                    int port)
            {
                if(connected_)
                {
                    LOG_WARN("Client::connect - connection refused client already connected");
                    return false;
                }
                else if( host.empty() || port <= 1024)
                {
                    LOG_WARN("Client::connect - connection refused host empty or port <=1024");
                    return false;
                }
                else
                {
                    LOG_WARN("Client::connect - connection accepted");

                    robot_ip_   = host;
                    robot_port_ = port;
                    cv_.notify_all();
                    std::unique_lock<std::mutex> lk(cv_mutex_connection_);
                    cv_connection_.wait_for(lk, std::chrono::seconds(1));
                    return true;
                }
            }

            bool Client::disconnect()
            {
                std::unique_lock<std::mutex> lk(cv_mutex_connection_);

                robot_ip_.clear();
                robot_port_ = 0;
                // connected_ = false;
                //char msg = 0;
                // ::write(fd_client,&msg,1);
                ::shutdown(socket_.getHandle(), SHUT_RDWR);
                cv_.notify_all();
                cv_connection_.wait_for(lk, std::chrono::seconds(1));
                LOG_DEBUG("Client::disconnect - [OK]");;
                return true; //
            }

            void Client::onConnected()
            {
                std::lock_guard<std::mutex> lock(protect_backends_);

                for (auto backend : backends_)
                {
                    backend->onConnected();
                }
                pub(dataSubscription_);
                cv_connection_.notify_all();
                LOG_WARN("Client::onConnected() - ");

            }

            bool Client::pub_msg(Packet const &packet)
            {
                std::lock_guard<std::mutex> lock(mutex_thread_);
                queue_.push( packet );
                return queue_.size()!=0;
            }

            void Client::broadcast()
            {
                std::lock_guard<std::mutex> lock(mutex_thread_);
                while(  queue_.size() > 0 )
                {
                    socket_.send(queue_.front());
                    queue_.pop();

                }
            }

            void Client::onLostConnection()
            {
                socket_.close();
                socket_ = Socket();
                connected_ = false ;
//                ::close(fd_client);

                cv_connection_.notify_all();

                std::lock_guard<std::mutex> lock(protect_backends_);
                for (auto backend : backends_)
                {
                    backend->onLostConnection();
                }
                LOG_WARN("Client::onLostConnection() - ");

            }

            void Client::unsubscribe(RobotInterface::Ptr listener)
            {
                std::lock_guard<std::mutex> lock(protect_backends_);
                // have to protect backends
                listener->setClient(0);
                int i = 0;
                for (auto const b : backends_)
                {
                    if (b == listener)
                    {
                        backends_.erase(backends_.begin() + i);
                        return;
                    }
                    i++;
                }
            }

            void Client::subscribe(RobotInterface::Ptr listener)
            {
                std::lock_guard<std::mutex> lock(protect_backends_);
                // have to protect backends
                listener->setClient(this);
                backends_.push_back(listener);
            }

            bool Client::treatMessage(Packet &packet)
            {
                std::lock_guard<std::mutex> lock(protect_backends_);
                uint32_t type;
                packet >> type;
                static Caller caller;
                return false
                        || caller.call<RobotInfo>(backends_, packet, type)
                        || caller.call<RobotPosition>(backends_, packet, type)
                        || caller.call<RobotState>(backends_, packet, type)
                        || caller.call<HardwareIssue>(backends_, packet, type)
                        || caller.call<Alarm>(backends_, packet, type)
                        || caller.call<AlarmUpdate>(backends_, packet, type)
                        || caller.call<BatteryState>(backends_, packet, type)
                        || caller.call<RobotMonitoring>(backends_, packet, type)
                        || caller.call<RobotVelocity>(backends_, packet, type)
                        || caller.call<NewMapAvailable>(backends_, packet, type)
                        || caller.call<RobotSensors>(backends_, packet, type);
            }

            void Caller::call(BatteryState const &event, RobotInterface::Ptr backend)
            {
                std::function<void(BatteryState const &state)> f = std::bind(&RobotInterface::onBatteryState,
                                                                             backend,
                                                                             std::placeholders::_1);
                f(event);
            }

            void Caller::call(RobotPosition const &event, RobotInterface::Ptr backend)
            {
                std::function<void(uint, uint, double)> f = std::bind(&RobotInterface::onRobotPosition,
                                                                      backend,
                                                                      std::placeholders::_1,
                                                                      std::placeholders::_2,
                                                                      std::placeholders::_3);
                f(event.x(), event.y(), event.theta());
            }

            void Caller::call(RobotState const &event, RobotInterface::Ptr backend)
            {
                std::function<void(RobotState::Type const&)> f = std::bind(&RobotInterface::onChangeState,
                                                                           backend,
                                                                           std::placeholders::_1);
                f(event.type());
            }

            void Caller::call(HardwareIssue const &event, RobotInterface::Ptr backend)
            {
                std::function<void(HardwareIssue::Issue)> f = std::bind(&RobotInterface::onHardwareIssue,
                                                                        backend,
                                                                        std::placeholders::_1);
                f(event.issue());
            }

            void Caller::call(Alarm const &event, RobotInterface::Ptr backend)
            {
                std::function<void(Alarm const &)> f = std::bind(&RobotInterface::onAlarm,
                                                                 backend,
                                                                 std::placeholders::_1);
                f(event);
            }

            void Caller::call(AlarmUpdate const &event, RobotInterface::Ptr backend)
            {
                std::function<void(AlarmUpdate const &)> f = std::bind(&RobotInterface::onAlarmUpdate,
                                                                       backend,
                                                                       std::placeholders::_1);
                f(event);
            }

            void Caller::call(eos::Patrols const &event, RobotInterface::Ptr backend)
            {
                std::function<void(eos::Patrols const &)> f = std::bind(&RobotInterface::onPatrols,
                                                                        backend,
                                                                        std::placeholders::_1);
                f(event);
            }

            void Caller::call(eos::Zones const &event, RobotInterface::Ptr backend)
            {
                std::function<void(eos::Zones const &)> f = std::bind(&RobotInterface::onZones,
                                                                      backend,
                                                                      std::placeholders::_1);
                f(event);
            }

            void Caller::call(eos::Sensors const &event, RobotInterface::Ptr backend)
            {
                std::function<void(eos::Sensors const &)> f = std::bind(&RobotInterface::onSensors,
                                                                        backend,
                                                                        std::placeholders::_1);
                f(event);
            }

            void Caller::call(eos::Settings const &event, RobotInterface::Ptr backend)
            {
                std::function<void(eos::Settings const &)> f = std::bind(&RobotInterface::onSettings,
                                                                         backend,
                                                                         std::placeholders::_1);
                f(event);
            }

            void Caller::call(eos::TasksScheduled const &event, RobotInterface::Ptr backend)
            {
                std::function<void(eos::TasksScheduled const &)> f = std::bind(&RobotInterface::onTaskScheduled,
                                                                               backend,
                                                                               std::placeholders::_1);
                f(event);
            }

            void Caller::call(RobotSensors const &event, RobotInterface::Ptr backend)
            {
                std::function<void(RobotSensors const &)> f = std::bind(&RobotInterface::onRobotSensors,
                                                                        backend,
                                                                        std::placeholders::_1);
                f(event);
            }

            void Caller::call(sdk::CartoState const &event, RobotInterface::Ptr backend)
            {
                std::function<void(eos::sdk::CartoState const &)> f = std::bind(&RobotInterface::onCartoState,
                                                                                backend,
                                                                                std::placeholders::_1);
                f(event);
            }

            void Caller::call(RobotMonitoring const &event, RobotInterface::Ptr backend)
            {
                std::function<void(RobotMonitoring const &)> f = std::bind(&RobotInterface::onRobotMonitoring,
                                                                           backend,
                                                                           std::placeholders::_1);
                f(event);
            }

            void Caller::call(eos::sdk::NewMapAvailable const &/*event*/, RobotInterface::Ptr backend)
            {
                std::function<void(void)> f = std::bind(&RobotInterface::onNewMapAvailable,
                                                        backend);
                f();
            }
            void Caller::call(sdk::RobotInfo const &event, RobotInterface::Ptr backend)
            {
                std::function<void(eos::sdk::RobotInfo const &)> f = std::bind(&RobotInterface::onRobotInfo,
                                                                               backend,
                                                                               std::placeholders::_1);
                f(event);
            }
            void Caller::call(sdk::RobotVelocity const &event, RobotInterface::Ptr backend)
            {
                std::function<void(eos::sdk::RobotVelocity const &)> f = std::bind(&RobotInterface::onRobotVelocity,
                                                                                   backend,
                                                                                   std::placeholders::_1);
                f(event);
            }

        }


        RobotInterface::RobotInterface()
            : client_(0)
        {
        }

        bool RobotInterface::sendJoystickCommand(float x, float y)
        {
            //if (user_rights_ == UserRights::Mode::UserRights_Mode_VIEWER)
            //  return false;
            if (!client_)
                return false;
            JoystickCommand joy;
            joy.set_x(x);
            joy.set_y(y);
            return client_->pub(joy);
        }

        bool RobotInterface::turnLeft()
        {
            //if (user_rights_ == UserRights::Mode::UserRights_Mode_VIEWER)
            //  return false;
            if (!client_)
                return false;
            Turn turn;
            turn.set_sens(Turn_Sens_LEFT);
            return client_->pub(turn);
        }

        bool RobotInterface::turnRight()
        {
            //if (user_rights_ == UserRights::Mode::UserRights_Mode_VIEWER)
            //  return false;
            if (!client_)
                return false;
            Turn turn;
            turn.set_sens(Turn_Sens_RIGHT);
            return client_->pub(turn);
        }

        bool RobotInterface::stop()
        {
            //if (user_rights_ == UserRights::Mode::UserRights_Mode_VIEWER)
            //  return false;
            if (!client_)
                return false;
            Stop stop;
            return client_->pub(stop);
        }

        bool RobotInterface::toggleAutomatic()
        {
            //if (user_rights_ == UserRights::Mode::UserRights_Mode_VIEWER)
            //  return false;
            if (!client_)
                return false;
            SetRobotMode robot_mode;
            robot_mode.set_type(SetRobotMode_Type_AUTOMATIC);
            return client_->pub(robot_mode);
        }

        bool RobotInterface::toggleManual()
        {
            //if (user_rights_ == UserRights::Mode::UserRights_Mode_VIEWER)
            //  return false;
            if (!client_)
                return false;
            SetRobotMode robot_mode;
            robot_mode.set_type(SetRobotMode_Type_MANUAL);
            return client_->pub(robot_mode);
        }

        bool RobotInterface::moveTo(uint x, uint y, double theta)
        {
            //if (user_rights_ == UserRights::Mode::UserRights_Mode_VIEWER)
            //  return false;
            if (!client_)
                return false;
            MoveTo move;
            move.set_x(x);
            move.set_y(y);
            move.set_theta(theta);
            return client_->pub(move);
        }

        bool RobotInterface::hooter(bool active)
        {
            //if (user_rights_ == UserRights::Mode::UserRights_Mode_VIEWER)
            //  return false;
            if (!client_)
                return false;
            Hooter hooter;
            hooter.set_active(active);
            return client_->pub(hooter);
        }

        bool RobotInterface::toggleCharging(bool active)
        {
            //if (user_rights_ == UserRights::Mode::UserRights_Mode_VIEWER)
            //  return false;
            if (!client_)
                return false;
            ToggleCharging charging;
            charging.set_active(active);
            return client_->pub(charging);
        }

        bool RobotInterface::toggleComponentsPowerSupply(bool active)
        {
            //if (user_rights_ == UserRights::Mode::UserRights_Mode_VIEWER)
            //  return false;
            if (!client_)
                return false;
            ToggleComponentsPowerSupply ps;
            ps.set_active(active);
            return client_->pub(ps);
        }

        bool RobotInterface::changeScoresThreshold(ScoresThreshold const &event)
        {
            return client_ && client_->pub(event);
        }

        void RobotInterface::setClient(priv::Client *client)
        {
            client_ = client;
        }

        bool RobotInterface::startCarto(float resolution, unsigned int size)
        {
            CartoOrder order;
            order.set_action(CartoOrder::Action::CartoOrder_Action_START);
            order.set_resolution(resolution);
            order.set_size(size);
            return client_ && client_->pub(order);
        }

        bool RobotInterface::stopCarto(float resolution, unsigned int size)
        {
            if (!client_) return false;
            CartoOrder order;
            order.set_action(CartoOrder::Action::CartoOrder_Action_STOP);
            order.set_resolution(resolution);
            order.set_size(size);
            return client_->pub(order);
        }
        bool RobotInterface::abortCarto()
        {
            CartoOrder order;
            order.set_action(CartoOrder::Action::CartoOrder_Action_ABORT);
            return client_->pub(order);
        }

        bool RobotInterface::sendRobotReboot()
        {
            return client_ && client_->pub(Reboot());
        }

        bool RobotInterface::pauseCarto()
        {
            if (!client_) return false;
            CartoOrder order;
            order.set_action(CartoOrder::Action::CartoOrder_Action_PAUSE);
            return client_->pub(order);
        }

        bool RobotInterface::controlPatrolling(bool start, std::string patrol_name, int duration)
        {
            ControlPatrolling control;
            control.set_action( start ? ControlPatrolling::START : ControlPatrolling::STOP);
            control.set_patrol_name(patrol_name);
            control.set_duration(duration);
            return client_ && client_->pub(control);
        }

        bool RobotInterface::emergencyStop(bool enable)
        {
            EmergencyStop stop;
            stop.set_active(enable);
            return client_ && client_->pub(stop);
        }

        bool RobotInterface::addDynamicObstacle(uint x, uint y, uint radius)
        {
            DynamicObstacle dyno;
            dyno.set_active(true);
            dyno.set_x(x);
            dyno.set_y(y);
            dyno.set_radius(radius);
            return client_ && client_->pub(dyno);
        }

        bool RobotInterface::removeDynamicObstacle(uint x, uint y, uint radius)
        {
            DynamicObstacle dyno;
            dyno.set_active(false);
            dyno.set_x(x);
            dyno.set_y(y);
            dyno.set_radius(radius);
            return client_ && client_->pub(dyno);
        }

        bool RobotInterface::cleanDynamicObstacles()
        {
            return client_ && client_->pub(ClearDynamicObstacles());
        }

        bool RobotInterface::sendRobotDataSubscription(sdk::RobotDataSubscription const& data)
        {
            return client_ && client_->pub(data);
        }

    } // sdk
} // eos
