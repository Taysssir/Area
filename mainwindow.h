#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QActionGroup>
#include <QTimer>

#include "core/appinstance.h"
#include "ui/map/mapwidget.h"
#include "core/constants.h"

#include "remote/robot.h"

#include "evigilante_configuration/include/eos/configuration/configuration.hpp"

#include "ui/calendar/calendarwidget.h"
#include "ui/map/mapcreationwizard.h"

#include "sentinel-common/sftp/transfer.h"

namespace Ui
{
  class MainWindow;
}

class QLabel;
class QProgressBar;
class PatrolsTab;

class MainWindow
    : public QMainWindow,
      public core::EventClient<MainWindow>
{
  Q_OBJECT

public:
  static const int EXIT_CODE_REBOOT = -123456789;
  explicit MainWindow(core::AppInstance &instance, QWidget *parent = 0);
  ~MainWindow();

  void receive(core::RobotConnected const &event);
  void receive(core::RobotDisconnected const &event);
  void receive(core::RobotStandby const &event);
  void receive(core::MapDownloading const &event);
  void receive(core::NewMapAvailable const &event);

protected:
  void closeEvent(QCloseEvent *event);

private:
  typedef enum
  {
    Pull,
    Push
  } Sens;

public slots:
  void updateMap();
  void updatePatrols();
  void updateSensors();
  void updateZones();
  void updateTasksScheduled();
private slots:
  void activateZone(bool checked);
  void activatePoint(bool checked);
  void activateSensors(bool checked);
  void activateWhitePencil(bool checked);
  void activateBlackPencil(bool checked);
  void activateGrayPencil(bool checked);
  void activateRuler(bool checked);
  void activateMapRotation(bool checked);
  void about();
  void connectRobot();
  void showSettings();

  void startMapCreationWizard();
  void abortCarto();
  void createMap(bool create, float resolution = 0.03, unsigned int size = 8192);
  void newMapAvailable();

  void pushPatrols();
  void pushZones();
  void pushMap();
  void pushTasksScheduled();
  void pushSensors();
  void pushSettings();
  void pushEverything();

  void pullPatrols();
  void pullZones();
  void pullMap();
  void pullTasksScheduled();
  void pullSensors();
  void pullSettings();
  void pullEverything();

  void on_actionA_propos_triggered();
  void on_actionQuitter_triggered();
  void on_actionEnregistrer_triggered();
  //void on_actionSaveAs_triggered();
  void on_actionNew_Configurations_triggered();
  void on_actionSettings_triggered();
  void on_actionReboot_the_robot_triggered();
  void displayCalendar(bool checked);
  void on_actionImport_Map_triggered();
  void on_actionMapEdit_triggered(bool activated);
  void on_actionHelp_triggered();
  //void on_connectRobot_triggered();
  //void on_actionStop_Robot_triggered();
  //void on_actionReboot_Robot_triggered();
  //void on_actionComboBox_ip_robot_edited(QString ip);
  void on_actionSave_As_triggered();
  void on_actionOpen_triggered();
  void slot_triggerCalendar() { mapWidget_->triggerCalendar(); }

signals:
  int push(int datatype);
  int pull(int datatype);

public slots:
  void downloading(int model, double progression);
  void uploading(int model, double progression);
  void downloaded(int model, bool ok);
  void uploaded(int model, bool ok);
private:
  //    Dealing * dealer_;
private:
  void setupWidgets();
  void setupToolBar();
  void setupStatusBar();

  bool saveCurrentConfigAndContinue();

  QString getModelName(int iModel);

  Ui::MainWindow *ui;
  QLabel *statusLabel;
  QLabel *information_status_;
  QLabel *filepath_;
  QProgressBar *statusProgressBar;
  eos::PatrolsSharedPointer patrols;
  //eos::PlanningSharedPointer planification;
  MapWidget *mapWidget_;
  CalendarWidget *calendar_widget_;
  QAction *zone;
  PatrolsTab *patrolsTab_;
  QAction *pushAction_;
  QAction *pullAction_;
  QAction *connectRobot_;
  bool is_trying_connection_;
  QAction *createMap_;
  Robot *robot_service_;
  QComboBox *comboBox_ip_;
  float map_resolution_;
  int map_size_;
  MapCreationWizard *newMapWizard_;

  QAction *new_config_;
  QAction *open_config_;
  QAction *save_config_;
  QAction *saveas_config_;
  QAction *settings_;

  bool bEditModeFlag;
  eos::sftp::Transfer transfer_;
};

#endif // MAINWINDOW_H
