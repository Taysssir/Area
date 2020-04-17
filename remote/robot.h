#ifndef SENTINEL_ADMIN_REMOTE_H
#define SENTINEL_ADMIN_REMOTE_H

#include <string>
#include <QTime>
#include <QProcess>

#include "core/appinstance.h"
#include "sentinel-admin_core/events.h"
#include "eos/sdk/client.hpp"
#include "evigilante_sdk/include/eos/sdk/msgs.pb.h"

class QTimer;

class Robot
    : public QObject,
      public core::EventClient<Robot>,
      public eos::sdk::RobotInterface
{
  Q_OBJECT

public:
  Robot(core::AppInstance &appInstance);
  ~Robot();

  void connectToHost(QString const &host);

  void close();

  void onConnected();
  void onLostConnection();
  void onWaitingForConnection();
  void onOperator();
  void onViewer();

  void onMap(eos::Map const &map);
  void onPatrols(eos::Patrols const &patrols);
  void onZones(eos::Zones const &zones);
  void onSensors(eos::Sensors const &sensors);
  void onTaskScheduled(eos::TasksScheduled const &tasks_scheduled);
  void onNewMapAvailable();

  void receive(core::IPChanged const &event);
  //void receive(core::ConnectRobot const &event);
  void receive(core::DisconnectRobot const &event);
private slots:
  void slot_newMapAvailable();
  void slot_robotConnected();
  void slot_robotDisconnected();
  void slot_robotStandby();
signals:
  void signal_newMapAvailable();
  void mapChanged();
  void patrolsChanged();
  void sensorsChanged();
  void tasksScheduldedChanged();
  void zonesChanged();
  void robotConnected();
  void robotDisconnected();
  void robotStandby();

private slots:
  void initConnection();

private:
  eos::sdk::Client client_;
};

#endif // SENTINEL_ADMIN_REMOTE_H
