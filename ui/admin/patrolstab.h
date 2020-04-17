#ifndef PATROLSTAB_H
#define PATROLSTAB_H

#include <QWidget>
#include <QVector>

#include "patroladminwidget.h"

QT_BEGIN_NAMESPACE
class QTabWidget;
QT_END_NAMESPACE

class PatrolsTab
    : public QWidget
{
  Q_OBJECT
public:
  PatrolsTab(core::AppInstance &instance, QWidget *parent = 0);
  void linkWithMapWidget(QObject * map_widget)
  {
      patrol_graphics_item_ = map_widget;
  }
signals :
  void removePatrolTasks(QString const&);
public slots:
  void updatePatrolStatus(QString const&);
  void updateAllPatrolStatus();
  void currentChanged(int index);
  void removePatrolTasks(std::string const&);
private:
  QTabWidget *tab_;
  QObject * patrol_graphics_item_ = 0;
};

#endif // PATROLSTAB_H
