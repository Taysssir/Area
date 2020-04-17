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

#include "eos/sdk/server.hpp"

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

class Test
    : public eos::sdk::BackendTreatment
{
public:
  Test()
  {
  }
/*
  virtual void onLogin(std::string const &name)
  {
    eos::sdk::Alarm alarm;
    alarm.set_id(3);
    alarm.set_x(100);
    alarm.set_y(300);
    alarm.set_distance(23);
    alarm.set_type(eos::sdk::Alarm::Type::Alarm_Type_HOTPOINT);
    alarm.set_status(eos::sdk::Alarm::Status::Alarm_Status_ON);
    sendAlarm(alarm);
  }
*/
  virtual void onJoystickCommand(float x, float y)
  {
  }

  virtual void onTurnLeft()
  {
  }

  virtual void onTurnRight()
  {
  }
/*
  virtual void onStop()
  {
    eos::sdk::Alarm alarm;
    static int id = 0;
    alarm.set_id(id++);
    alarm.set_x(100);
    alarm.set_y(300);
    alarm.set_distance(23);
    alarm.set_type(eos::sdk::Alarm::Type::Alarm_Type_HOTPOINT);
    alarm.set_status(eos::sdk::Alarm::Status::Alarm_Status_ON);
    sendAlarm(alarm);
  }
*/
  virtual void onSetMode(eos::sdk::SetRobotMode const &mode)
  {
  }

  virtual void onMoveTo(uint x, uint y, double theta)
  {
  }

  virtual void onHooter(bool active)
  {
  }

  virtual void onToggleCharging(bool active)
  {
  }

  virtual void onToggleComponentsPowerSupply(bool active)
  {
  }

  void updateRobotPosition(eos::sdk::RobotPosition const &pose)
  {
  }
};

//#include "server_impl.hpp"

int main()
{
#ifdef _WIN32
#else
 /*
    struct sigaction sigIntHandler;

  sigIntHandler.sa_handler = my_handler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;

  sigaction(SIGINT, &sigIntHandler, NULL);
*/
#endif
  eos::sdk::Server server;

  auto serverbk = server.newInterface<Test>();

 if(!server.listen(55499))
 {
     std::cout << "FAILEd to LISTEN " << std::endl;
 }

 std::this_thread::sleep_for(std::chrono::milliseconds(20000));
 // std::cout << " CLOSING " << std::endl;
 server.close();
 std::this_thread::sleep_for(std::chrono::milliseconds(200000));

//  std::unique_lock<std::mutex> lock(m);
//  cond_var.wait(lock);

  return 0;
}
