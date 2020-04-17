#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <iostream>
#include <queue>
#include <chrono>
#include <iostream>

#include "eos/sdk/client.hpp"

std::condition_variable cond_var;
std::mutex m;

#ifdef _WIN32
#else
void my_handler(int s)
{
  printf("Caught signal %d\n", s);
  cond_var.notify_one();
}
#endif
class TestRobotEvents
    : public eos::sdk::RobotInterface
{
public:
  TestRobotEvents()
      : eos::sdk::RobotInterface()
  {
  }

  virtual void onLostConnection()
  {
    std::cout << "Connexion lost !!!" << std::endl;
  }

  virtual void onRobotInfo(std::string const &name, int uptime)
  {
    std::cout << "Robot Name : " << name << std::endl;
    std::cout << "Robot uptime : " << uptime << std::endl;
  }

  virtual void onOperator()
  {
    std::cout << " I am an operator :) " << std::endl;
  }

  virtual void onViewer()
  {
    std::cout << " I am a viewer :) " << std::endl;
  }

  virtual void onRobotPosition(uint x, uint y, double theta)
  {
    std::cout << "Robot Position " << x << " " << y << " " << theta << std::endl;
  }

  virtual void onChangeState(eos::sdk::RobotState::Type state)
  {
  }

  virtual void onHardwareIssue(eos::sdk::HardwareIssue::Issue issue)
  {
    std::cout << "On Hardware Issue : " << issue << std::endl;
  }

  virtual void onAlarm(eos::sdk::Alarm const &alarm)
  {
    std::cout << this << " New alarm !" << std::endl;
    std::cout << "id : " << alarm.id() << std::endl;
    std::cout << "position : " << alarm.x() << " " << alarm.y() << " " << alarm.distance() << std::endl;
    stop();
  }

  virtual void onAlarmUpdate(eos::sdk::AlarmUpdate const &alarm)
  {
    std::cout << "new alarm update !" << std::endl;
  }
};

int main()
{
#ifdef _WIN32
#else
  /*
   * struct sigaction sigIntHandler;

  sigIntHandler.sa_handler = my_handler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;

  sigaction(SIGINT, &sigIntHandler, NULL);
*/

#endif
  eos::sdk::Client robot;
  if (robot.connect( "localhost",  55499)) // 172.16.12.10
  {
    auto bk = robot.newInterface<TestRobotEvents>();
    // robot.newInterface<TestRobotEvents>();
    std::cout << "bk->sendRobotReboot" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    bk->sendRobotReboot();
//   while(true)
//   {
//    bk->turnLeft();
    // bk->sendJoystickCommand(1,0);
//    std::this_thread::sleep_for(std::chrono::milliseconds(100));
//   }
//    robot.disconnect();
//    if (robot.connect( "localhost",  55897))
//    {
//        std::cout << "RECONNECT client  " << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
//    }
   // bk->sendJoystickCommand(1,0);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    //robot.disconnect();
    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
//    std::unique_lock<std::mutex> lock(m);
  //  cond_var.wait(lock);
  }
  else
  {
    std::cerr << "Impossible to connect to the server !" << std::endl;
  }

  return 0;
}
