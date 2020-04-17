#ifndef PATROLADMINWIDGET_H
#define PATROLADMINWIDGET_H

#include <QWidget>
#include <QMap>
#include <QPixmap>

#include "evigilante_configuration/include/eos/configuration/models.pb.h"
#include "sentinel-common/core/appinstance.h"
#include "sentinel-admin_core/events.h"

namespace Ui
{
  class PatrolAdminWidget;
}

class QListWidgetItem;

class PatrolAdminWidget
    : public QWidget
    , public core::EventClient<PatrolAdminWidget>
{
  Q_OBJECT

public:
  explicit PatrolAdminWidget(core::AppInstance &instance,
                             eos::Patrol::Type type,
                             QWidget *parent = 0);

  ~PatrolAdminWidget();

  /**
   * @brief load all patrol with the good type
   */
  void load();

  void callChangePatrol();
  void update();
  void receive(core::PatrolsUpdate const &);
public slots:
  void newPoint(QPoint const &point);
  void updatePoint(int index, QPoint const &point);
  void removePoint(int index);
  void changeIndex(int old, int index);
  void changePriority(int index, int priority);
  void updateUsageStatus(QString previous_name);
  void updateAllUsageStatus();

private slots:
  void on_pushButton_add_clicked();

  void on_pushButton_remove_clicked();

  void on_pushButton_update_clicked();

  //void on_listWidget_patrols_currentRowChanged(int currentRow);

  void on_pushButton_duplicate_clicked();

  void on_pushButton_down_clicked();

  void on_pushButton_up_clicked();

  // click on the list
  void on_listWidget_patrols_clicked(const QModelIndex &index);

signals:
  void removePatrol();
  void changePatrol(int index);
  void removeTasksFromPatrol(std::string);

protected:
  void contextMenuEvent(QContextMenuEvent *event);

private:
  QListWidgetItem *addNewPatrol();
  void editPatrolFromList(eos::Patrol const &patrol);

  // add a new patrol to the list
  QListWidgetItem *addPatrolToList(eos::Patrol const &patrol);

  // update a particular list entry with the corresponding patrol
  void updateItemList(QListWidgetItem *item, eos::Patrol const& patrol);

  // refresh the ui
  void updateUI();

  // check the validity of the entered patrol name
  bool controlNameValidity(QString const &newName);

  // this method suggest a good name for a new evenual patrol name
  QString suggestCopyName(QString const& base);

  // ui, just qt ui
  Ui::PatrolAdminWidget *ui;

  // icon used in the list
  QImage icon_;

  // reserved keyword for patrol suggestion name
  static QString copy_keyboard_;

  // this the the current type of patrol manage by this instance
  eos::Patrol::Type type_;
};

#endif // PATROLADMINWIDGET_H
