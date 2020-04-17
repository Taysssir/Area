#ifndef EOS_EVIGILANTE_CLIENT_HPP
#define EOS_EVIGILANTE_CLIENT_HPP

#include <string>
#include <memory>
#include <thread>

#include "msgs.pb.h"
#include "eos/configuration/models.pb.h"
#include "eos/configuration/configuration.hpp"
#include "channel.hpp"
#include "log_interface.hpp"

namespace eos
{
    namespace sdk
    {

        typedef unsigned int uint;
        namespace priv
        {
            class Client;
        }

        class RobotInterface
        {
        public:
            ///! robot interface pointer
            typedef RobotInterface *Ptr;

            /**
        * @brief RobotInterface
        */
            RobotInterface();

            /**
        * @brief sendJoystickCommand send a joystick command
        * @param x [-1, 1]
        * @param y [-1, 1]
        * @return  true in case of success
        *  false mean connection pb or no good rights
        */
            bool sendJoystickCommand(float x, float y);

            /**
        * @brief send the command turn left to the robot
        * @return  true in case of success
        *  false mean connection pb or no good rights
        */
            bool turnLeft();

            /**
        * @brief send the command turn right to the robot
        * @return  true in case of success
        *  false mean connection pb or no good rights
        */
            bool turnRight();

            /**
        * @brief send the stop command to the robot
        * @return  true in case of success
        *  false mean connection pb or no good rights
        */
            bool stop();

            /**
        * @brief switch to automatic
        * @return  true in case of success
        *  false mean connection pb or no good rights
        */
            bool toggleAutomatic();

            /**
        * @brief switch to manual
        * @return  true in case of success
        *  false mean connection pb or no good rights
        */
            bool toggleManual();

            /**
        * @brief move the robot to a x, y position
        * @param x position in the map coordinates
        * @param y position in the map coordinates
        * @param theta orientation
        * @return  true in case of success
        *  false mean connection pb or no good rights
        */
            bool moveTo(uint x, uint y, double theta);

            /**
        * @brief active the hooter or not
        * @param active true active
        * @return  true in case of success
        *  false mean connection pb or no good rights
        */
            bool hooter(bool active);

            /**
        * @brief activate the charging
        * @param active true for activation
        * @return  true in case of success
        *  false mean connection pb or no good rights
        */
            bool toggleCharging(bool active);

            /**
        * @brief active the power supply of all components
        * @param active true for activation
        * @return  true in case of success
        *  false mean connection pb or no good rights
        */
            bool toggleComponentsPowerSupply(bool active);

            /**
       * @brief change threshold for mapping and localization
       * @param event two parameters :
       * 	- switch to localization
       * 	- swith to mapping
       * @return true if correctly sended
       */
            bool changeScoresThreshold(ScoresThreshold const &event);

            // ############################################################

            /**
      * @brief controlPatrolling
      * @param start : start a patrolling
      * stop : stop patrolling (docking)
      */
            bool controlPatrolling(bool start, std::string patrol_name, int duration);

            /**
       * @brief emergency Stop
       * stop the robot directly
       * @param enable
       * @return
       */
            bool emergencyStop(bool enable);

            /**
       * @brief kick the current Operator
       * ask to be the one
       * @return
       */
            bool kickOperator();

            // ############################################################
            bool sendMap(eos::MapSharedPointer map);
            bool getMap(Map::Type type);

            bool getMap(Configuration::Model type);
            virtual void onMap(eos::Map const & /*map*/)
            {
            }
            virtual void onMapDownloadingProgress(double /*percent*/)
            {
            }
            virtual void onMapUploadingProgress(double /*percent*/)
            {
            }
            virtual void onNewMapAvailable()
            {
            }

            // ############################################################
            bool getPatrols();
            bool sendPatrols(eos::PatrolsSharedPointer patrols);
            virtual void onPatrols(eos::Patrols const & /*patrols*/)
            {
            }

            // ############################################################
            bool getZones();
            bool sendZones(eos::ZonesSharedPointer zones);
            virtual void onZones(eos::Zones const & /*zones*/)
            {
            }

            // ############################################################
            bool getSensors();
            bool sendSensors(eos::SensorsSharedPointer sensors);
            virtual void onSensors(eos::Sensors const & /*sensors*/)
            {
            }

            // ############################################################
            bool getSettings();
            bool sendSettings(eos::SettingsSharedPointer sensors);
            virtual void onSettings(eos::Settings const & /*settings*/)
            {
            }

            // ############################################################
            bool getTasksScheduled();
            bool sendTasksScheduled(eos::TasksScheduledSharedPointer tasks_scheduled);
            virtual void onTaskScheduled(eos::TasksScheduled const & /*tasks_scheduled*/)
            {
            }

            // ############################################################
            bool startCarto(float resolution, unsigned int size);
            bool stopCarto(float resolution, unsigned int size);
            bool pauseCarto();
            bool abortCarto();

            virtual void onCartoState(eos::sdk::CartoState const &)
            {
            }

            // ############################################################
            // receive robots sensors status from the robot
            virtual void onRobotSensors(RobotSensors const&)
            {
            }

            /**
        * @brief receive some informations from the robot
        * @param robot name
        * @param robot uptime
        */
            virtual void onRobotInfo(RobotInfo const &)
            {

            }
            /**
       * @brief onRobotVelocity Handle robot velocity
       * @param data
       */
            virtual void onRobotVelocity(RobotVelocity const &)
            {

            }

            /**
        * @brief indicate that we are in viewer mode
        */
            virtual void onViewer()
            {
            }

            /**
        * @brief receive a new robot position
        * @param x position
        * @param y position
        * @param theta orientation
        */
            virtual void onRobotPosition(uint /*x*/, uint /*y*/, double /*theta*/)
            {
            }

            /**
        * @brief the robot change it's state
        * @param new state
        */
            virtual void onChangeState(RobotState::Type const&/*state*/)
            {
            }

            /**
        * @brief new hardware issue
        * @param issue type
        */
            virtual void onHardwareIssue(HardwareIssue::Issue /*issue*/)
            {
            }

            /**
        * @brief receive new alarm
        * @param alarm content
        */
            virtual void onAlarm(Alarm const & /*alarm*/)
            {
            }

            /**
        * @brief receive an alarm updated
        * @param new alarm information
        */
            virtual void onAlarmUpdate(AlarmUpdate const & /*alarm*/)
            {
            }

            virtual void onBatteryState(BatteryState const & /*state*/)
            {
            }

            /**
        * @brief connextion lost
        */
            virtual void onLostConnection()
            {
            }

            /**
        * @brief connected !
        */
            virtual void onConnected()
            {
            }

            /**
        * try reconnection !
        */
            virtual void onWaitingForConnection()
            {
            }

            /**
        * @brief on robot monitoring information
        */
            virtual void onRobotMonitoring(RobotMonitoring const & /*event*/)
            {
            }

            /**
       * @brief add a new dynamic obstacle
       * @param x map coor [pixels]
       * @param y map coor [pixels]
       * @param radius size of the obstacle
       */
            bool addDynamicObstacle(uint x, uint y, uint radius);

            /**
       * @brief remove dynamic obstacle
       * @param x map coor [pixels]
       * @param y map coor [pixels]
       * @param radius size of the obstacle
       */
            bool removeDynamicObstacle(uint x, uint y, uint radius);

            /**
       * @brief delete all dynamic obstacles
       */
            bool cleanDynamicObstacles();

            /**
       * @brief sendRobotDataSubscription
       * @param data
       * @return
       */
            bool sendRobotDataSubscription(sdk::RobotDataSubscription const & data);

            bool sendRobotReboot();


            void setClient(priv::Client *client);
        private:

            //void onTransfertCompleted(ssh::Job& job, ssh::Result status );
            // void onTransfertStatus(ssh::Job& job, double status );
            priv::Client *client_;

            eos::Map map_;
            int map_current_part_number_;
            int map_part_number_;
        };

        class Client
                : public LogInterface
        {
        public:
            Client();
            ~Client();

            /**
       * @brief connect the client to a robot
       * @param channel which channel to plug (command or data)
       * @param host
       * @param user
       * @param port
       * @return
       */
            bool connect(
                    std::string const &host,
                    int port);

            /**
       * @brief disconnect form the robot
       */
            bool disconnect();

            template <typename T>
            T *newInterface()
            {
                T *ptr = new T();
                subscribe(ptr);
                return ptr;
            }

            template <typename T>
            void addInterface(T *ptr)
            {
                subscribe(ptr);
            }

            template <typename T>
            void subscribe_once()
            {
                subscriptionData_.add_dataindex(T::descriptor()->index());
            }

            template<typename...>
            struct typelist_event{};

            template<typename E, typename ... Rest>
            void call_event( typelist_event<E,Rest...>)
            {
                subscribe_once<E>();
                call_event(typelist_event<Rest...>());
            }

            void call_event(typelist_event<>) { }
            /**
             * Subscribe : if variadic list is empty subscribes to all datas
             * can subscribe to a list of one data at least
             */
            template<typename...classes>  void subscribe()
            {
                subscriptionData_.Clear();
                call_event(typelist_event<classes...>());
                subscriptionData_.set_all(0 == subscriptionData_.dataindex().size());
                update(subscriptionData_);
            }

            /**
       * @brief subscribes a new interface
       * @param item
       */
            void subscribe(RobotInterface *item);
            void unsubscribe(RobotInterface *item);

        private:
            RobotDataSubscription subscriptionData_;
            void update(sdk::RobotDataSubscription const& dataSubscription);
            friend class RobotInterface;
            std::shared_ptr<priv::Client> robot_;
        };

    } // namespace sdk
} // namespace eos

#endif // EOS_EVIGILANTE_ROBOT_HPP
