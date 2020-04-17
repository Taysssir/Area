
#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this
// in one cpp file
#include "catch.hpp"
#include <thread>
#include <chrono>
#include "../src/tcp/logger.hpp"
#include "eos/sdk/client.hpp"
#include "eos/sdk/server.hpp"
#include "../src/engine/message_handler.hpp"

int serviceTimeValue(100);

TEST_CASE("TC_EVIGILANTE_SDK_00001 - connection, Server disconnection then recpnnection with the same Client")
{
    LOG_DEBUG("TC_EVIGILANTE_SDK_00001 ");
    eos::sdk::Server server;
    eos::sdk::Client client;

    REQUIRE(server.listen(55897));
    REQUIRE(client.connect("127.0.0.1", 55897));

    std::this_thread::sleep_for(std::chrono::milliseconds(10*serviceTimeValue));
    server.close();
    std::this_thread::sleep_for(std::chrono::milliseconds(2*serviceTimeValue));
    LOG_DEBUG("TC_EVIGILANTE_SDK_00001 - END ");
}


TEST_CASE("TC_EVIGILANTE_SDK_00002 - connection, disconnect client then reconnect client")
{
  LOG_DEBUG("========================================================= TC_EVIGILANTE_SDK_00002 - wrong connection - extern");
  eos::sdk::Server server;
  eos::sdk::Client client;
  //eos::sdk::Client client2;
  //eos::sdk::Client client3;
  REQUIRE(server.listen(55892));
  std::this_thread::sleep_for(std::chrono::milliseconds(serviceTimeValue));
  client.connect("127.0.0.1", 55892);
  //client2.connect("127.0.0.1", 55897);
  LOG_DEBUG("------------- WAIT CONNECTED");
  std::this_thread::sleep_for(std::chrono::milliseconds(serviceTimeValue));
  client.disconnect();
  LOG_DEBUG("------------- WAIT DISCONNECTED");
  std::this_thread::sleep_for(std::chrono::milliseconds(serviceTimeValue));
  client.connect("127.0.0.1", 55892);
  LOG_DEBUG("--------------- WAIT RECONNECTED");
  std::this_thread::sleep_for(std::chrono::milliseconds(serviceTimeValue));
  LOG_DEBUG("TC_EVIGILANTE_SDK_00002 - END");

}
/*
TEST_CASE("TC_EVIGILANTE_SDK_00003 - [Robustness] test closing while writting")
{//  @brief TEST_CASE while Server is connected and is sending msgs, client is destrioyed and created

    LOG_INFO("TC_EVIGILANTE_SDK_00003 - BEGIN ");
    struct TestRobot
        : public eos::sdk::RobotInterface
    {
      void onRobotPosition(uint x, uint y, double theta)
      {
        // LOG_INFO("TC_EVIGILANTE_SDK_00003 - RobotInterface::onRobotPosition - [OK] ");
        x_ = x;
        y_ = y;
        theta_ = theta;
        size++;
      }
      uint x_ = 0;
      uint y_ = 0;
      double theta_ = 0;
      int size = 0;
    };

    struct FakeServer
        : public eos::sdk::BackendTreatment
    {
      FakeServer()
      {
      }
      void send_position()
      {
        eos::sdk::RobotPosition position;
        position.set_x(10);
        position.set_y(20);
        position.set_theta(1.23);
        sendRobotPosition(position);
      }
    };

    eos::sdk::Server server;
    server.listen(55895);

    // wait for a moment to promote the client
    // send new position !
    auto fake = server.newInterface<FakeServer>();

    for(int i = 0; i< 10; i++)
    {
        eos::sdk::Client client;
        client.connect("localhost", 55895);
        auto test = client.newInterface<TestRobot>();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        for(int j = 0; j < 2 ; j++)
        {
            fake->send_position();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        REQUIRE(test->size == 50);
        // client is destroyed
    }
}


TEST_CASE("TC_EVIGILANTE_SDK_00004 - test client forced connection disconnection")
{
 struct TestRobot
      : public eos::sdk::RobotInterface
  {

    void onConnected()
    {
      LOG_INFO("Test Robot - onConnected ");
      connected = true;
    }
    void onLostConnection()
    {
      LOG_INFO("Test Robot - onLostConnection ");
      connected = false;
    }

    bool connected = false;
  };
  LOG_DEBUG("TC_EVIGILANTE_SDK_00004 - ");


  eos::sdk::Server server;
  eos::sdk::Client client;



  auto test = client.newInterface<TestRobot>();
  REQUIRE(test->connected == false);
  REQUIRE(server.listen(55895));

  client.connect("localhost", 55895);
  for(int i =0; i < 10; i++)
  {
      std::this_thread::sleep_for(std::chrono::milliseconds(serviceTimeValue));
      LOG_DEBUG("TC_EVIGILANTE_SDK_00004 - BEGIN");
      REQUIRE(test->connected == true);
      std::this_thread::sleep_for(std::chrono::milliseconds(serviceTimeValue));
      client.disconnect();
      std::this_thread::sleep_for(std::chrono::milliseconds(serviceTimeValue));
      LOG_DEBUG("TC_EVIGILANTE_SDK_00004 - DISCONNECT");

      REQUIRE(test->connected == false);
      client.connect("localhost", 55895);
      LOG_DEBUG("TC_EVIGILANTE_SDK_00004 - RE-CONNECT");
      std::this_thread::sleep_for(std::chrono::milliseconds(serviceTimeValue));
      REQUIRE(test->connected == true);
  }
      LOG_DEBUG("TC_EVIGILANTE_SDK_00004 - END");

}

TEST_CASE("TC_EVIGILANTE_SDK_00005 - SUBSCRIPTION : test robot subscription")
{
  // do not subcribe     send position -> SENT
  // subscribe to alarm  send position -> NOT SENT
  //                     send alarm    -> SENT
  // subscribe to Pos    send alarm    -> NOT SENT
  //                     send position -> SENT
  // subscribe to ALL    send position -> NOT SENT
  //                     send alarm    -> SENT

  struct TestRobot
      : public eos::sdk::RobotInterface
  {
          void onAlarmUpdate(eos::sdk::AlarmUpdate const &alarm)
          {
            alarm_ = alarm;
          }
          eos::sdk::AlarmUpdate alarm_;

          void onRobotPosition(uint x, uint y, double theta)
          {
            x_ = x;
            y_ = y;
            theta_ = theta;
          }
          uint x_ = 0;
          uint y_ = 0;
          double theta_ = 0;
  };

  struct FakeServer
      : public eos::sdk::BackendTreatment
  {

    FakeServer()
    {
    }
    void send_position()
    {
      eos::sdk::RobotPosition position;
      position.set_x(10);
      position.set_y(20);
      position.set_theta(1.23);
      sendRobotPosition(position);
    }
    void send_position2()
    {
      eos::sdk::RobotPosition position;
      position.set_x(100);
      position.set_y(200);
      position.set_theta(12.3);
      sendRobotPosition(position);
    }
    void senAlarm()
    {
        eos::sdk::AlarmUpdate alarm;
        alarm.set_id(4);
        alarm.set_x(100);
        alarm.set_y(300);
        alarm.set_distance(23);
        sendAlarmUpdate(alarm);
    }
    void senAlarm2()
    {
        eos::sdk::AlarmUpdate alarm;
        alarm.set_id(4);
        alarm.set_x(1000);
        alarm.set_y(3000);
        alarm.set_distance(230);
        sendAlarmUpdate(alarm);
    }

  };

  eos::sdk::Server server;
  REQUIRE(server.listen(55895));
  auto fake = server.newInterface<FakeServer>();

  eos::sdk::Client client;
  auto test = client.newInterface<TestRobot>();
  client.connect("localhost", 55895);

  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  fake->send_position();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  REQUIRE(test->x_ == 10);
  REQUIRE(test->y_ == 20);
  REQUIRE(test->theta_ == 1.23);

  client.subscribe< eos::sdk::AlarmUpdate>();
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  fake->send_position2(); // not update
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  REQUIRE(test->x_ == 10);
  REQUIRE(test->y_ == 20);
  REQUIRE(test->theta_ == 1.23);

  fake->senAlarm();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  REQUIRE(test->alarm_.id() == 4);
  REQUIRE(test->alarm_.x() == 100);
  REQUIRE(test->alarm_.y() == 300);
  REQUIRE(test->alarm_.distance() == 23);

  client.subscribe< eos::sdk::RobotPosition>();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  fake->senAlarm2();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  REQUIRE(test->alarm_.id() == 4);
  REQUIRE(test->alarm_.x() == 100);
  REQUIRE(test->alarm_.y() == 300);
  REQUIRE(test->alarm_.distance() == 23);

  fake->send_position2(); // update
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  REQUIRE(test->x_ == 100);
  REQUIRE(test->y_ == 200);
  REQUIRE(test->theta_ == 12.3);

  client.subscribe<>();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  fake->senAlarm2();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  REQUIRE(test->alarm_.id() == 4);
  REQUIRE(test->alarm_.x() == 1000);
  REQUIRE(test->alarm_.y() == 3000);
  REQUIRE(test->alarm_.distance() == 230);

}

TEST_CASE("TC_EVIGILANTE_SDK_00006 - test robot position : 2 Client")
{
  LOG_INFO("TC_EVIGILANTE_SDK_00006 - BEGIN 5sec waited");
  struct TestRobot
      : public eos::sdk::RobotInterface
  {
    void onRobotPosition(uint x, uint y, double theta)
    {
      LOG_INFO("TC_EVIGILANTE_SDK_00006 - RobotInterface::onRobotPosition - [OK] ");

      x_ = x;
      y_ = y;
      theta_ = theta;
    }
    uint x_ = 0;
    uint y_ = 0;
    double theta_ = 0;
  };

  struct FakeServer
      : public eos::sdk::BackendTreatment
  {
    FakeServer()
    {
    }
    void send_position()
    {
      eos::sdk::RobotPosition position;
      position.set_x(10);
      position.set_y(20);
      position.set_theta(1.23);
      sendRobotPosition(position);
    }
  };

  eos::sdk::Server server;
  server.listen(55895);

  eos::sdk::Client client;
  client.connect("localhost", 55895);
  auto test = client.newInterface<TestRobot>();

  eos::sdk::Client client2;
  client2.connect("localhost", 55895);
  auto test2 = client2.newInterface<TestRobot>();

  // wait for a moment to promote the client
  // send new position !
  auto fake = server.newInterface<FakeServer>();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  fake->send_position();

  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  REQUIRE(test->x_ == 10);
  REQUIRE(test->y_ == 20);
  REQUIRE(test->theta_ == 1.23);

  // sentinel-admin  receive anything !
  REQUIRE(test2->x_ == 10);
  REQUIRE(test2->y_ == 20);
  REQUIRE(test2->theta_ == 1.23);

}

TEST_CASE("TC_EVIGILANTE_SDK_00007 - test robot position : sentinel & reporter")
{
    // do not subcribe     send position -> SENT
    // subscribe to alarm position and sensors

    struct TestRobot
        : public eos::sdk::RobotInterface
    {
            void onAlarmUpdate(eos::sdk::AlarmUpdate const &alarm)
            {
              alarm_ = alarm;
            }
            eos::sdk::AlarmUpdate alarm_;

            void onRobotPosition(uint x, uint y, double theta)
            {
              x_ = x;
              y_ = y;
              theta_ = theta;
            }
            uint x_ = 0;
            uint y_ = 0;
            double theta_ = 0;

            void onRobotSensors(eos::sdk::RobotSensors const& sensors)
            {
                size = sensors.sensors().size();
            }
            size_t size = 0;
    };

    struct FakeServer
        : public eos::sdk::BackendTreatment
    {

      FakeServer()
      {
      }
      void send_position()
      {
        eos::sdk::RobotPosition position;
        position.set_x(10);
        position.set_y(20);
        position.set_theta(1.23);
        sendRobotPosition(position);
      }
      void senAlarm()
      {
          eos::sdk::AlarmUpdate alarm;
          alarm.set_id(4);
          alarm.set_x(100);
          alarm.set_y(300);
          alarm.set_distance(23);
          sendAlarmUpdate(alarm);
      }

    };

    eos::sdk::Server server;
    REQUIRE(server.listen(55895));
    auto fake = server.newInterface<FakeServer>();

    eos::sdk::Client client;
    auto test = client.newInterface<TestRobot>();
    client.connect("localhost", 55895);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    client.subscribe< eos::sdk::AlarmUpdate, eos::sdk::RobotSensors,  eos::sdk::RobotPosition>();

    std::this_thread::sleep_for(std::chrono::milliseconds(serviceTimeValue));
    fake->send_position(); // not update
    std::this_thread::sleep_for(std::chrono::milliseconds(serviceTimeValue));
    REQUIRE(test->x_ == 10);
    REQUIRE(test->y_ == 20);
    REQUIRE(test->theta_ == 1.23);

    fake->senAlarm();
    std::this_thread::sleep_for(std::chrono::milliseconds(serviceTimeValue));
    REQUIRE(test->alarm_.id() == 4);
    REQUIRE(test->alarm_.x() == 100);
    REQUIRE(test->alarm_.y() == 300);
    REQUIRE(test->alarm_.distance() == 23);

}

TEST_CASE("TC_EVIGILANTE_SDK_00008_a - test robot change state")
{
  LOG_DEBUG("TC_EVIGILANTE_SDK_00008_a - test robot change state");
  struct TestRobot
      : public eos::sdk::RobotInterface
  {
    void onChangeState(eos::sdk::RobotState::Type const& state)
    {
      state_ = state;
    }
    eos::sdk::RobotState::Type state_;
  };

  eos::sdk::Server server;
  server.listen(55777);

  eos::sdk::Client client;
  client.connect("localhost", 55777);
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  auto test = client.newInterface<TestRobot>();
  auto fake = server.newInterface<eos::sdk::BackendTreatment>();
  fake->sendRobotState(eos::sdk::RobotState_Type_CHARGING);
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  REQUIRE(test->state_ == eos::sdk::RobotState_Type_CHARGING);
}

TEST_CASE("TC_EVIGILANTE_SDK_00009_a test robot hardware issue")
{
  struct TestRobot
      : public eos::sdk::RobotInterface
  {
    void onHardwareIssue(eos::sdk::HardwareIssue::Issue issue)
    {
      issue_ = issue;
    }
    eos::sdk::HardwareIssue::Issue issue_;
  };

  eos::sdk::Server server;
  server.listen(58784);

  eos::sdk::Client client;
  client.connect("localhost", 58784);
  auto test = client.newInterface<TestRobot>();
  auto fake = server.newInterface<eos::sdk::BackendTreatment>();

  fake->sendRobotHardwareIssue(eos::sdk::HardwareIssue::OBSTRUCTED_LASER);
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  REQUIRE(test->issue_ == eos::sdk::HardwareIssue::OBSTRUCTED_LASER);

  fake->sendRobotHardwareIssue(eos::sdk::HardwareIssue::LOW_BATTERY);
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  REQUIRE(test->issue_ == eos::sdk::HardwareIssue::LOW_BATTERY);
}

TEST_CASE("TC_EVIGILANTE_SDK_00010_a - Test Alarm")
{
  LOG_DEBUG("TC_EVIGILANTE_SDK_00010_a - Test Alarm");
  struct TestRobot
      : public eos::sdk::RobotInterface
  {
    void onAlarm(eos::sdk::Alarm const &alarm)
    {
      alarm_ = alarm;
      count_++;
      LOG_DEBUG("onAlarm - "<< count_);

    }
    eos::sdk::Alarm alarm_;
    int count_ = 0;
  };

  eos::sdk::Server server;
  eos::sdk::Client client;

  auto test = client.newInterface<TestRobot>();
  auto test2 = client.newInterface<TestRobot>();
  auto fake = server.newInterface<eos::sdk::BackendTreatment>();

  REQUIRE(server.listen(58784));
  client.connect("localhost", 58784);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  eos::sdk::Alarm alarm;
  alarm.set_id(3);
  alarm.set_x(100);
  alarm.set_y(300);
  alarm.set_distance(23);
  alarm.set_type(eos::sdk::Alarm::Type::Alarm_Type_HOTPOINT);
  alarm.set_status(eos::sdk::Alarm::Status::Alarm_Status_ON);
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  fake->sendAlarm(alarm);
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  REQUIRE(test->alarm_.id() == 3);
  REQUIRE(test->alarm_.x() == 100);
  REQUIRE(test->alarm_.y() == 300);
  REQUIRE(test->alarm_.distance() == 23);
  REQUIRE(test->alarm_.type() == eos::sdk::Alarm::HOTPOINT);
  REQUIRE(test->alarm_.status() == eos::sdk::Alarm::ON);

  REQUIRE(test2->alarm_.id() == 3);
  REQUIRE(test2->alarm_.x() == 100);
  REQUIRE(test2->alarm_.y() == 300);
  REQUIRE(test2->alarm_.distance() == 23);
  REQUIRE(test2->alarm_.type() == eos::sdk::Alarm::HOTPOINT);
  REQUIRE(test2->alarm_.status() == eos::sdk::Alarm::ON);

  for (int i = 0; i < 100; ++i)
    fake->sendAlarm(alarm);


  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  REQUIRE(test->count_ == 101);
}

TEST_CASE("TC_EVIGILANTE_SDK_00011_a - test alarm update")
{
  struct TestRobot
      : public eos::sdk::RobotInterface
  {
    void onAlarmUpdate(eos::sdk::AlarmUpdate const &alarm)
    {
      alarm_ = alarm;
    }
    eos::sdk::AlarmUpdate alarm_;
  };

  eos::sdk::Server server;
  eos::sdk::Client robot;

  auto test = robot.newInterface<TestRobot>();
  auto fake = server.newInterface<eos::sdk::BackendTreatment>();

  REQUIRE(server.listen(48563));
  robot.connect("localhost", 48563);
  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  eos::sdk::AlarmUpdate alarm;
  alarm.set_id(4);
  alarm.set_x(100);
  alarm.set_y(300);
  alarm.set_distance(23);
  fake->sendAlarmUpdate(alarm);
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  REQUIRE(test->alarm_.id() == 4);
  REQUIRE(test->alarm_.x() == 100);
  REQUIRE(test->alarm_.y() == 300);
  REQUIRE(test->alarm_.distance() == 23);
}

TEST_CASE("TC_EVIGILANTE_SDK_00012_a test receive basic commands")
{
   LOG_DEBUG("TC_EVIGILANTE_SDK_00012_a - ");
  struct FakeServer
      : public eos::sdk::BackendTreatment
  {
    virtual void onTurnLeft()
    {
      received_++;
    }

    virtual void onTurnRight()
    {
      received_++;
    }

    virtual void onStop()
    {
      received_++;
    }
    void onJoystickCommand(float x, float y)
    {
      x_ = x;
      y_ = y;
    }

    void onSetMode(eos::sdk::SetRobotMode const &mode)
    {
      mode_ = mode.type();
    }

    int received_ = 0;
    float x_;
    float y_;
    eos::sdk::SetRobotMode::Type mode_;
  };

  eos::sdk::Server server;
  eos::sdk::Client robot;

  auto test = robot.newInterface<eos::sdk::RobotInterface>();
  auto fake = server.newInterface<FakeServer>();

  REQUIRE(server.listen(49655));
  robot.connect("localhost", 49655);
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  REQUIRE(test->turnLeft());
  REQUIRE(test->turnRight());
  REQUIRE(test->stop());
  REQUIRE(test->sendJoystickCommand(10, 20));
  REQUIRE(test->toggleAutomatic());
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  REQUIRE(fake->received_ == 3);
  REQUIRE(fake->x_ == 10);
  REQUIRE(fake->y_ == 20);
  REQUIRE(fake->mode_ == eos::sdk::SetRobotMode::AUTOMATIC);
  LOG_DEBUG("TC_EVIGILANTE_SDK_00012_a - END");

}

TEST_CASE("TC_EVIGILANTE_SDK_00014_a - reconnect to a dummy Robot")
{
    struct TestRobot
         : public eos::sdk::RobotInterface
     {

       void onConnected()
       {
         LOG_INFO("Test Robot - onConnected ");
         connected = true;
       }
       void onLostConnection()
       {
         LOG_INFO("Test Robot - onLostConnection ");
         connected = false;
       }

       bool connected = false;
     };
     LOG_DEBUG("TC_EVIGILANTE_SDK_00004 - ");


     eos::sdk::Server server;
     eos::sdk::Client client;



     auto test = client.newInterface<TestRobot>();
     REQUIRE(test->connected == false);
     REQUIRE(server.listen(55895));

     client.connect("localhost", 55895);
     std::this_thread::sleep_for(std::chrono::milliseconds(serviceTimeValue));
     REQUIRE(test->connected == true);

     for(int i = 0; i < 10 ; i++)
     {
         std::this_thread::sleep_for(std::chrono::milliseconds(serviceTimeValue));
         client.disconnect();
         std::this_thread::sleep_for(std::chrono::milliseconds(serviceTimeValue));
         LOG_DEBUG("TC_EVIGILANTE_SDK_00014_a - DISCONNECTED");
         REQUIRE(test->connected ==  false );
         client.connect("localhost", 42);
         std::this_thread::sleep_for(std::chrono::milliseconds(10*serviceTimeValue));
         LOG_DEBUG("TC_EVIGILANTE_SDK_00014_a - RECONNECTED FAILED");
         REQUIRE(test->connected == false);
         client.disconnect();
         std::this_thread::sleep_for(std::chrono::milliseconds(serviceTimeValue));
         LOG_DEBUG("TC_EVIGILANTE_SDK_00014_a - RECONNECT");
         client.connect("localhost", 55895);
         std::this_thread::sleep_for(std::chrono::milliseconds(serviceTimeValue));
         REQUIRE(test->connected == true);
     }
     LOG_DEBUG("TC_EVIGILANTE_SDK_00014_a - END");
}

TEST_CASE("TC_EVIGILANTE_SDK_00015_a - Destroy and reconstruiuct server")
{
    struct TestRobot
         : public eos::sdk::RobotInterface
     {

       void onConnected()
       {
         LOG_INFO("Test Robot - onConnected ");
         connected = true;
       }
       void onLostConnection()
       {
         LOG_INFO("Test Robot - onLostConnection ");
         connected = false;
       }

       bool connected = false;
     };
     LOG_DEBUG("TC_EVIGILANTE_SDK_00015_a - ");


     eos::sdk::Server server;
     eos::sdk::Client client;



     auto test = client.newInterface<TestRobot>();
     client.connect("localhost", 55815);
     std::this_thread::sleep_for(std::chrono::milliseconds(serviceTimeValue));

     for(int i = 0; i < 10 ; i++)
     {
         LOG_DEBUG("TC_EVIGILANTE_SDK_00015_a - DICONNECTED ");

         std::this_thread::sleep_for(std::chrono::milliseconds(3*serviceTimeValue));
         REQUIRE(test->connected == false);
         LOG_DEBUG("TC_EVIGILANTE_SDK_00015_a - CONNECTED ");

         eos::sdk::Server server;
         REQUIRE(server.listen(55815));
         std::this_thread::sleep_for(std::chrono::milliseconds(4*serviceTimeValue));
         REQUIRE(test->connected == true);
     }
     LOG_DEBUG("TC_EVIGILANTE_SDK_00015_a - END");
}

TEST_CASE("TC_EVIGILANTE_SDK_00017_a - test control patrolling")
{
  LOG_DEBUG("TC_EVIGILANTE_SDK_00017_a -");
  struct FakeServer
      : public eos::sdk::BackendTreatment
  {
      void onControlPatrolling(eos::sdk::ControlPatrolling control_patrolling)
      {
          LOG_DEBUG("FakeServer::onControlPatrolling - TC_EVIGILANTE_SDK_00017_a -control patrolling " << control_patrolling.action() );

          static int i = 0;
          if(i == 0)
              REQUIRE(control_patrolling.action() == eos::sdk::ControlPatrolling::ACTION::ControlPatrolling_ACTION_START);
          else
              REQUIRE(control_patrolling.action() == eos::sdk::ControlPatrolling::ACTION::ControlPatrolling_ACTION_STOP);
          i++;

      }
  };

  eos::sdk::Server server;
  eos::sdk::Client robot;

  auto test = robot.newInterface<eos::sdk::RobotInterface>();
  auto fake = server.newInterface<FakeServer>();

  REQUIRE(server.listen(49878));
  robot.connect("localhost", 49878);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  test->controlPatrolling(true, "Patrol1", 300);
  std::this_thread::sleep_for(std::chrono::milliseconds(400));
  test->controlPatrolling(false, "", 0);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
}


TEST_CASE("TC_EVIGILANTE_SDK_00018_a - test emergency stop")
{
  struct FakeServer
      : public eos::sdk::BackendTreatment
  {
      void onEmergencyStop(bool active)
      {
          LOG_DEBUG("FakeServer::onEmergencyStop - TC_EVIGILANTE_SDK_00018_a");
          static int i = 0;
          if(i == 0)
              REQUIRE(active);
          else
              REQUIRE_FALSE(active);
          i++;
          std::cout<<"emergency stop " << active << std::endl;
      }
  };

  eos::sdk::Server server;
  eos::sdk::Client robot;

  auto test = robot.newInterface<eos::sdk::RobotInterface>();
  auto fake = server.newInterface<FakeServer>();

  REQUIRE(server.listen(49878));
  robot.connect("localhost", 49878);

  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  test->emergencyStop(true);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  test->emergencyStop(false);

  std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST_CASE("TC_EVIGILANTE_SDK_00019_a - test carto order")
{
  struct FakeServer
      : public eos::sdk::BackendTreatment
  {
      virtual void onCartoOrder(eos::sdk::CartoOrder const & order)
      {
          if(order.action() == eos::sdk::CartoOrder::Action::CartoOrder_Action_START)
          {
            resolution = order.resolution();
            size = order.size();
          }
          else if(order.action() == eos::sdk::CartoOrder::Action::CartoOrder_Action_STOP)
          {
            stop++;
          }
      }
      float resolution;
      unsigned int size;
      int stop = 0;
  };

  eos::sdk::Server server;
  eos::sdk::Client robot;

  auto test = robot.newInterface<eos::sdk::RobotInterface>();
  auto fake = server.newInterface<FakeServer>();

  REQUIRE(server.listen(49878));
  robot.connect("localhost", 49878);
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  // start a carto with a specific resolution and size
  test->startCarto(0.1, 1000);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  // test the reception
  REQUIRE(fake->resolution == 0.1f);
  REQUIRE(fake->size == 1000);

  // same test with differents values
  test->startCarto(0.03, 8192);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  REQUIRE(fake->resolution == 0.03f);
  REQUIRE(fake->size == 8192);

  // test the stop carto order
  test->stopCarto(0.03, 8192);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  REQUIRE(fake->stop == 1);
}



TEST_CASE("TC_EVIGILANTE_SDK_00020_a - test dynamic obstacles")
{
  LOG_DEBUG("TC_EVIGILANTE_SDK_00020_a - ");

  struct FakeServer
      : public eos::sdk::BackendTreatment
  {
      virtual void onDynamicObstacle(eos::sdk::DynamicObstacle const& dyno)
      {
          LOG_DEBUG("TC_EVIGILANTE_SDK_00020_a - FakeServer::onDynamicObstacle - ");
          doo = dyno;
      }

      virtual void onClearDynamicObstacles()
      {
          clear++;
          LOG_DEBUG("TC_EVIGILANTE_SDK_00020_a - FakeServer::onClearDynamicObstacles - "<< clear);
      }
      eos::sdk::DynamicObstacle doo;
      int clear = 0;
  };


  eos::sdk::Client robot;
  eos::sdk::Server server;

  auto test = robot.newInterface<eos::sdk::RobotInterface>();
  auto fake = server.newInterface<FakeServer>();

  REQUIRE(server.listen(49878));
  robot.connect("localhost", 49878);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // send an obstacle
  test->addDynamicObstacle(10, 11, 23);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  REQUIRE(fake->doo.x() == 10);
  REQUIRE(fake->doo.y() == 11);
  REQUIRE(fake->doo.radius() == 23);
  REQUIRE(fake->doo.active());

  // remove an obstacle
  test->removeDynamicObstacle(10, 11, 23);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  REQUIRE(fake->doo.x() == 10);
  REQUIRE(fake->doo.y() == 11);
  REQUIRE(fake->doo.radius() == 23);
  REQUIRE_FALSE(fake->doo.active());

  // remove all obstacles
  test->cleanDynamicObstacles();
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  REQUIRE(fake->clear == 1);

  LOG_DEBUG("TC_EVIGILANTE_SDK_00020_a ***** END");

}
TEST_CASE("TC_EVIGILANTE_SDK_00021_a - test receive robot sensors")
{
  struct FakeClient
      : public eos::sdk::RobotInterface
  {
    void onRobotSensors(eos::sdk::RobotSensors const& sensors)
    {
        size = sensors.sensors().size();
    }
    int size = 0;
  };

  eos::sdk::Server server;
  eos::sdk::Client robot;

  auto test = robot.newInterface<FakeClient>();
  auto fake = server.newInterface<eos::sdk::BackendTreatment>();

  REQUIRE(server.listen(49878));
  std::this_thread::sleep_for(std::chrono::milliseconds(4));
  REQUIRE(robot.connect("localhost", 49878));
  eos::sdk::RobotSensors rs;
  eos::sdk::RobotSensor * sensor = rs.add_sensors();
  sensor->set_path("/sensors/ok/other");
  sensor->set_type(eos::sdk::RobotSensor::US);
  sensor->set_unit("meter");
  sensor->set_value(1);

  for(int i=0;i<1;i++)
    *rs.add_sensors() = *sensor;

}
*/

/*


TEST_CASE("TC_EVIGILANTE_SDK_00016_a - test send tasks scheduled")
{
  struct FakeClient
      : public eos::sdk::RobotInterface
  {
      void onTaskScheduled(eos::TasksScheduled const & tasks_scheduled)
      {
          std::cout<<"fake client receive tasks "<<tasks_scheduled.tasks_size()<<std::endl;
          REQUIRE(tasks_scheduled.tasks_size() == 100099);
      }
  };

  struct FakeServer
      : public eos::sdk::BackendTreatment
  {

      void onTasksScheduled(eos::TasksScheduled const & tasks_scheduled)
      {
          std::cout<<"fake server receive tasks "<<tasks_scheduled.tasks_size()<<std::endl;
          REQUIRE(tasks_scheduled.tasks_size() == 100099);
      }
      void onGetTasksScheduled()
      {
          std::cout<<"get tasks"<<std::endl;
          eos::TasksScheduledSharedPointer tasks = std::make_shared<eos::TasksScheduled>();
          eos::TaskScheduled task;
          task.set_start(100);
          task.set_stop(100);
          task.set_patrol_name("Patrol 2");
          task.set_group_id(0);
          tasks->set_timestamp(0);
          for(int i=0;i<100099;++i)
            *tasks->mutable_tasks()->Add() = task;
          sendTasksScheduled(*tasks);
      }
  };

  eos::sdk::Server server;
  eos::sdk::Client robot;

  auto test = robot.newInterface<FakeClient>();
  auto fake = server.newInterface<FakeServer>();

  REQUIRE(server.listen(49877));
  robot.connect("localhost", 49877);
  eos::TasksScheduledSharedPointer tasks = std::make_shared<eos::TasksScheduled>();
  eos::TaskScheduled task;
  task.set_start(100);
  task.set_stop(100);
  task.set_patrol_name("Patrol 1");
  task.set_group_id(0);
  tasks->set_timestamp(0);
  for(int i=0;i<100099;++i)
    *tasks->mutable_tasks()->Add() = task;

  std::cout << std::chrono::high_resolution_clock::period::den << std::endl;
  auto start_time = std::chrono::high_resolution_clock::now();
  test->sendTasksScheduled(tasks);
  auto end_time = std::chrono::high_resolution_clock::now();
  std::cout << "elapsed : ";
  std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << ":";
  std::cout << std::endl;

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  test->getTasksScheduled();
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}



*/
