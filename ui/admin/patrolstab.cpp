#include <QDebug>
#include <QTabWidget>
#include "ui/map/patrolgraphicsitem.h"

#include "patrolstab.h"

PatrolsTab::PatrolsTab(core::AppInstance &instance, QWidget *)
    : tab_(new QTabWidget(this))
{
  tab_->addTab(new PatrolAdminWidget(instance, eos::Patrol_Type_Normal, this), QIcon(), tr("Patrol"));
  tab_->addTab(new PatrolAdminWidget(instance, eos::Patrol_Type_Remapping, this), QIcon(), tr("Remapping"));

  tab_->setMinimumWidth(800);
  tab_->setMinimumHeight(800);

  tab_->setStyleSheet("QTabBar::tab{width: 220px; height: 40px; border: 0px solid #f07f10;border-top-left-radius: 0px;border-top-right-radius: 0px;color: white;} "
                      "QTabBar::tab:selected {font: bold;background: solid #f07f10; } "
                      "QTabBar::tab:!selected {background: rgb(38,38,38);}"
                      "QTabBar::tab {border-bottom:4px solid #f07f10;} "
                      "QTabBar::tab:hover:!selected {color:#f07f10;} "
                      "QTabWidget::pane{border: 0px solid #020202;} ");
  connect(tab_, SIGNAL(currentChanged(int)), this, SLOT(currentChanged(int)));
}

void PatrolsTab::currentChanged(int index)
{
  if(!patrol_graphics_item_) return ;

  PatrolGraphicsItem * patrol_graphics = reinterpret_cast<PatrolGraphicsItem*>(patrol_graphics_item_);
  PatrolAdminWidget * patrolController = static_cast<PatrolAdminWidget*>(tab_->widget(index));
  PatrolAdminWidget * patrolControllerPast = static_cast<PatrolAdminWidget*>(tab_->widget(!index));

  disconnect(patrol_graphics, SIGNAL(sig_newPoint(QPoint)), patrolControllerPast, SLOT(newPoint(QPoint)));
  disconnect(patrol_graphics, SIGNAL(sig_updatePoint(int, QPoint)), patrolControllerPast, SLOT(updatePoint(int, QPoint)));
  disconnect(patrol_graphics, SIGNAL(sig_removePoint(int)), patrolControllerPast, SLOT(removePoint(int)));
  disconnect(patrol_graphics, SIGNAL(sig_changeIndex(int, int)), patrolControllerPast, SLOT(changeIndex(int, int)));
  disconnect(patrol_graphics, SIGNAL(sig_changePriority(int, int)), patrolControllerPast, SLOT(changePriority(int, int)));
  disconnect(patrolControllerPast, SIGNAL(removePatrol()), patrol_graphics, SLOT(removePatrol()));
  disconnect(patrolControllerPast, SIGNAL(changePatrol(int)), patrol_graphics, SLOT(changePatrol(int)));
  disconnect(patrolControllerPast, SIGNAL(removeTasksFromPatrol(std::string)),this,SLOT(removePatrolTasks(std::string)));

  connect(patrol_graphics, SIGNAL(sig_newPoint(QPoint)), patrolController, SLOT(newPoint(QPoint)));
  connect(patrol_graphics, SIGNAL(sig_updatePoint(int, QPoint)), patrolController, SLOT(updatePoint(int, QPoint)));
  connect(patrol_graphics, SIGNAL(sig_removePoint(int)), patrolController, SLOT(removePoint(int)));
  connect(patrol_graphics, SIGNAL(sig_changeIndex(int, int)), patrolController, SLOT(changeIndex(int, int)));
  connect(patrol_graphics, SIGNAL(sig_changePriority(int, int)), patrolController, SLOT(changePriority(int, int)));
  connect(patrolController, SIGNAL(removePatrol()), patrol_graphics, SLOT(removePatrol()));
  connect(patrolController, SIGNAL(changePatrol(int)), patrol_graphics, SLOT(changePatrol(int)));
  connect(patrolController, SIGNAL(removeTasksFromPatrol(std::string)),this,SLOT(removePatrolTasks(std::string)));

  patrolController->callChangePatrol();
}

void PatrolsTab::updatePatrolStatus(QString const& name)
{
    PatrolAdminWidget * patrolController = static_cast<PatrolAdminWidget*>(tab_->widget(tab_->currentIndex()));
    patrolController->updateUsageStatus(name);
}

void PatrolsTab::updateAllPatrolStatus()
{
    PatrolAdminWidget * patrolController = static_cast<PatrolAdminWidget*>(tab_->widget(tab_->currentIndex()));
    patrolController->updateAllUsageStatus();
}

void PatrolsTab::removePatrolTasks(const std::string &patrol_name)
{
    emit removePatrolTasks(QString::fromStdString(patrol_name));
}
