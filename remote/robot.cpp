#include <QDebug>
#include <QTimer>

#include "robot.h"
#include "core/sentinel_logger.h"

Robot::Robot(core::AppInstance &appInstance)
    : core::EventClient<Robot>(appInstance)
{
  client_.subscribe(this);

  connect(this, SIGNAL(signal_newMapAvailable()), this, SLOT(slot_newMapAvailable()));
  connect(this, SIGNAL(robotConnected()), this, SLOT(slot_robotConnected()));
  connect(this, SIGNAL(robotDisconnected()), this, SLOT(slot_robotDisconnected()));
  connect(this, SIGNAL(robotStandby()),this, SLOT(slot_robotStandby()));
  subscribe<core::IPChanged>(*this);
  subscribe<core::DisconnectRobot>(*this);
}

Robot::~Robot()
{
  client_.unsubscribe(this);
  client_.disconnect();
}

void Robot::initConnection()
{
    connectToHost(instance_.robotIP());
}

void Robot::connectToHost(QString const &host)
{
  client_.connect(host.toStdString(), 55499);
}

void Robot::close()
{
  client_.disconnect();
}

void Robot::onConnected()
{
  instance_.setConnected(true);
  emit robotConnected();
}

void Robot::onLostConnection()
{
  instance_.setConnected(false);
  emit robotDisconnected();
}

void Robot::onWaitingForConnection()
{
    instance_.setConnected(false);
    emit robotStandby();
}

void Robot::onOperator()
{
  instance_.setModeClientOperator(true);
  //qDebug() << "operator !!!";
}

void Robot::onViewer()
{
  instance_.setModeClientOperator(false);
  //qDebug() << "viewer !!!";
}

void Robot::onMap(eos::Map const &map)
{
  EOS_Info() << "Received map !";
  *instance_.configuration().base_map() = map;
  //instance_.configuration().save(eos::Configuration::BaseMap); // Irrelevant save operation
  emit mapChanged();
}

void Robot::onPatrols(eos::Patrols const &patrols)
{
  EOS_Info() << "Received patrols !";
  *instance_.configuration().patrols() = patrols;
  //instance_.configuration().save(eos::Configuration::Patrols);
  emit patrolsChanged();
}

void Robot::onZones(eos::Zones const &zones)
{
  EOS_Info() << "Received zones !";
  *instance_.configuration().zones() = zones;
  //instance_.configuration().save(eos::Configuration::Zones);
  emit zonesChanged();
}

void Robot::onSensors(eos::Sensors const &sensors)
{
  EOS_Info() << "Received sensors !";
  *instance_.configuration().sensors() = sensors;
  //instance_.configuration().save(eos::Configuration::Sensors);
  emit sensorsChanged();
}

void Robot::onTaskScheduled(eos::TasksScheduled const &tasks_scheduled)
{
  EOS_Info() << "Received tasks schedulded !";
  eos::TasksScheduledSharedPointer tasks = instance_.configuration().tasksScheduled();
  if(tasks)
  {
      tasks->mutable_tasks()->Clear();
      tasks->set_timestamp(tasks_scheduled.timestamp());
      for(int i=0;i<tasks_scheduled.tasks_size();i++)
          *tasks->mutable_tasks()->Add() = tasks_scheduled.tasks().Get(i);
  }
  //instance_.configuration().save(eos::Configuration::TasksScheduled);
  emit tasksScheduldedChanged();
}

void Robot::onNewMapAvailable()
{
    emit signal_newMapAvailable();
}

void Robot::slot_newMapAvailable()
{
    emitEvent<core::NewMapAvailable>();
}

void Robot::slot_robotConnected()
{
    emitEvent<core::RobotConnected>();
}

void Robot::slot_robotDisconnected()
{
    emitEvent<core::RobotDisconnected>();
}

void Robot::slot_robotStandby()
{
    emitEvent<core::RobotStandby>();
}

void Robot::receive(core::IPChanged const &)
{
  initConnection();
}

void Robot::receive(core::DisconnectRobot const &)
{
    close();
}
