#include <QListWidgetItem>
#include <QDebug>
#include <QMessageBox>
#include <QMenu>
#include <QContextMenuEvent>
#include <QSvgRenderer>
#include <QPainter>
#include <QDebug>

#include "patroladminwidget.h"
#include "ui_patroladminwidget.h"
#include "patrollistviewdelegate.h"
#include "sentinel-admin_core/qt_utils.h"
#include "sentinel-common/core/constants.h"

QString PatrolAdminWidget::copy_keyboard_ = "copy"; //This will be used to add to a copied patrol

PatrolAdminWidget::PatrolAdminWidget(core::AppInstance &instance,
                                     eos::Patrol::Type type, QWidget *parent)
    : QWidget(parent)
    , core::EventClient<PatrolAdminWidget>(instance)
    , ui(new Ui::PatrolAdminWidget)
    , icon_(":/patrol/icon")
    , type_(type)
{
    ui->setupUi(this);

    // hide up/down for all patrol type except Remapping
    ui->pushButton_up->setVisible(type == eos::Patrol::Remapping);
    ui->pushButton_down->setVisible(type == eos::Patrol::Remapping);

    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    ui->listWidget_patrols->setItemDelegate(new PatrolListViewDelegate());

    //// prepare label
    subscribe<core::PatrolsUpdate>(*this);

    updateUI();
}

void PatrolAdminWidget::receive(core::PatrolsUpdate const &)
{
  // first clear the gui
  ui->listWidget_patrols->clear();
  updateUI();
  // next load patrols
  load();
  callChangePatrol();
}

PatrolAdminWidget::~PatrolAdminWidget()
{
  delete ui;
}

void PatrolAdminWidget::load()
{
    for (int i = 0; i < instance_.read_only().patrols()->patrols().size(); ++i)
    {
        eos::Patrol const &patrol = instance_.patrols()->patrols().Get(i);
        if (patrol.type() == type_)
            addPatrolToList(patrol);
    }
}

void PatrolAdminWidget::on_pushButton_add_clicked()
{
  if (addNewPatrol())
  {
    callChangePatrol();
    updateUI();
  }
  else
    core::alert(tr("Failed to create a new patrol, check the name !"));
}

QListWidgetItem *PatrolAdminWidget::addNewPatrol()
{
    QString const &newPatrolName = ui->lineEdit_newpatrol->text();
    if (controlNameValidity(newPatrolName) == false)
    {
        return nullptr;
    }
    eos::Patrol *patrol = instance_.patrols()->mutable_patrols()->Add();
    patrol->set_name(newPatrolName.toStdString());
    patrol->set_type(type_);

    updateUI();

    return addPatrolToList(*patrol);
}

namespace
{
QString suggestName(eos::PatrolsSharedPointer patrols, QString const &base)
{
    int id = 0;
    // the pattern have to be : "string (int)"
    QRegExp valid_input(QString("^* \\d*$"));
    for (int i = 0; i < patrols->patrols().size(); ++i)
    {
        eos::Patrol const &patrol = patrols->patrols().Get(i);
        if (valid_input.indexIn(patrol.name().c_str()) == -1)
            continue;
        id = std::max(valid_input.cap(0).toInt(), id);
    }
    return QObject::tr("%1 %2").arg(base).arg(id + 1);
}
/*eos::Patrol getPatrol(eos::PatrolsSharedPointer patrols, QString const &name) // Unused method!
{
    for (int i = 0; i < patrols->patrols().size(); ++i)
    {
        eos::Patrol const &patrol = patrols->patrols().Get(i);
        if (patrol.name() == name.toStdString())
            return patrol;
    }
    return eos::Patrol();
}*/

eos::Patrol *getPointerPatrol(eos::PatrolsSharedPointer patrols, QString const &name)
{
    for (int i = 0; i < patrols->patrols().size(); ++i)
    {
        eos::Patrol *patrol = patrols->mutable_patrols()->Mutable(i);
        if (patrol->name() == name.toStdString())
            return patrol;
    }
    return nullptr;
}
int getPatrolIndex(eos::PatrolsSharedPointer patrols, QString const &name)
{
    for (int i = 0; i < patrols->patrols().size(); ++i)
    {
        eos::Patrol const &patrol = patrols->patrols().Get(i);
        if (patrol.name() == name.toStdString())
            return i;
    }
    return -1;
}
int getPatrolIndexType(eos::PatrolsSharedPointer patrols, QString const &name, eos::Patrol::Type type)
{
    int j = 0;
    for (int i = 0; i < patrols->patrols().size(); ++i)
    {
        eos::Patrol const &patrol = patrols->patrols().Get(i);
        if (patrol.name() == name.toStdString())
            return j;
        if(type == patrol.type())
            j++;
    }
    return -1;
}
int countPatrolType(eos::PatrolsSharedPointer patrols, eos::Patrol::Type type)
{
    int count = 0;
    for (int i = 0; i < patrols->patrols().size(); ++i)
    {
        eos::Patrol const &patrol = patrols->patrols().Get(i);
        if (patrol.type() == type)
            count++;
    }
    return count;
}
}

void PatrolAdminWidget::updateUI()
{
    QListWidgetItem *current = ui->listWidget_patrols->currentItem();

    // update enable UI
    ui->pushButton_update->setEnabled(current);
    ui->lineEdit_updatename->setEnabled(current);
    ui->pushButton_remove->setEnabled(current);
    ui->pushButton_duplicate->setEnabled(current);
    ui->lineEdit_updatename->setText("");

    QString label = tr("Patrol");
    if(type_ == eos::Patrol_Type_Remapping)
      label = tr("Remapping");
    ui->lineEdit_newpatrol->setText(suggestName(instance_.read_only().patrols(), label));

    // if one selected
    if (current)
    {
        //eos::Patrol patrol = getPatrol(patrols_, current->text());
        if (type_ != eos::Patrol::Normal)
        {
            int typeIndex = getPatrolIndexType(instance_.patrols(), current->text(), type_);
            ui->pushButton_up->setEnabled(typeIndex != 0);
            ui->pushButton_down->setEnabled(typeIndex != countPatrolType(instance_.patrols(), type_) - 1);
        }
        else
        {
            QVector<std::string> patrol_use_name;
            for(auto task : instance_.tasksScheduled()->tasks())
            {
                if(!patrol_use_name.contains(task.patrol_name()))
                    patrol_use_name.append(task.patrol_name());
            }
            bool patrolIsUsed = patrol_use_name.contains(current->data(Qt::DisplayRole).toString().toStdString()); //instance_.planification()->hasPatrol(patrol.name());
            ui->pushButton_remove->setEnabled(!patrolIsUsed);
            ui->pushButton_update->setEnabled(!patrolIsUsed);
        }
        ui->lineEdit_updatename->setText(current->text());
    }
}

void PatrolAdminWidget::updateItemList(QListWidgetItem *item, eos::Patrol const &patrol)
{
    if (!item) return;
    item->setData(Qt::DisplayRole, patrol.name().c_str());
    item->setData(Qt::DecorationRole, icon_);
    QVector<std::string> patrol_use_name;
    for(auto task : instance_.tasksScheduled()->tasks())
    {
        if(!patrol_use_name.contains(task.patrol_name()))
            patrol_use_name.append(task.patrol_name());
    }
    bool patrolIsUsed = patrol_use_name.contains(patrol.name());
    QString label =
            tr("%1,  %2 points") //, ~%3 mètre(s), ~%4 min(s)") // maybe add some metrics in the future
            .arg(patrolIsUsed ? tr("active") : tr("inactive"))
            .arg(patrol.points().size());
    item->setData(Qt::UserRole + 1, label);
    item->setData(Qt::UserRole + 2, patrolIsUsed);
}

void PatrolAdminWidget::editPatrolFromList(eos::Patrol const &patrol)
{
    QList<QListWidgetItem*> item = ui->listWidget_patrols->findItems(QString::fromStdString(patrol.name()),Qt::MatchExactly);

    if (!item.size()) return;

    int index =ui->listWidget_patrols->row(item[0]);
    item[0]->setData(Qt::DisplayRole, patrol.name().c_str());
    item[0]->setData(Qt::DecorationRole, icon_);
    QVector<std::string> patrol_use_name;
    for(auto task : instance_.tasksScheduled()->tasks())
    {
        if(!patrol_use_name.contains(task.patrol_name()))
            patrol_use_name.append(task.patrol_name());
    }
    if(ui->listWidget_patrols->currentItem()->data(Qt::DisplayRole) == patrol.name().c_str())
    {
        if(patrol_use_name.contains(patrol.name()))
        {
            ui->pushButton_remove->setEnabled(false);
            ui->pushButton_update->setEnabled(false);
        }
        else
        {
            ui->pushButton_remove->setEnabled(true);
            ui->pushButton_update->setEnabled(true);
        }
    }
    bool patrolIsUsed = patrol_use_name.contains(patrol.name());
    QString label =
            tr("%1,  %2 points")
            .arg(patrolIsUsed ? tr("active") : tr("inactive"))
            .arg(patrol.points().size());

    item[0]->setData(Qt::UserRole + 1, label);
    item[0]->setData(Qt::UserRole + 2, patrolIsUsed);
    ui->listWidget_patrols->insertItem(index,item[0]);
    ui->listWidget_patrols->removeItemWidget(ui->listWidget_patrols->item(++index));
}

QListWidgetItem *PatrolAdminWidget::addPatrolToList(eos::Patrol const &patrol)
{
    QListWidgetItem *item = new QListWidgetItem();
    if(item)
    {
        updateItemList(item, patrol);
        ui->listWidget_patrols->addItem(item);
        // assume that the order of Patrol::Type is correct : 0, 1, 2, 3 ...
        ui->listWidget_patrols->setCurrentItem(item);
        return item;
    }
    return 0;
}

void PatrolAdminWidget::on_pushButton_remove_clicked()
{
    //callChangePatrol();

    QListWidgetItem *current = ui->listWidget_patrols->currentItem();
    if (current)
    {
        const QString &s = current->text();
        int patrol_index = getPatrolIndex(instance_.patrols(), s);

        if (patrol_index >= 0)
        {
            instance_.patrols()->mutable_patrols()->SwapElements(patrol_index, instance_.patrols()->patrols().size() - 1);
            instance_.patrols()->mutable_patrols()->RemoveLast();
            QListWidgetItem *item = ui->listWidget_patrols->takeItem(ui->listWidget_patrols->row(current));
            delete item;
            //int index = 0;
            //for (; index < instance_.patrols()->patrols().size(); ++index)
            //{
            //  eos::Patrol const &patrol = instance_.patrols()->patrols().Get(index);
            //  if (patrol.type() == type_)
            //    updateItemList(ui->listWidget_patrols->item(index++), patrol);
            //}
            emit removePatrol();
            //if (type_ == eos::Patrol::Normal)
            //  emitEvent<core::NormalPatrolsUpdate>();
        }
        else
            core::alert(tr("Impossible to delete the patrol : %1").arg(s));
    }
    else
        core::alert(tr("No Patrol Selected !"));

    callChangePatrol();
}

void PatrolAdminWidget::on_pushButton_update_clicked()
{
    QListWidgetItem *current = ui->listWidget_patrols->currentItem();
    if (current)
    {
        const QString &s = current->text();
        eos::Patrol *patrol = getPointerPatrol(instance_.patrols(), s);

        QString const &newPatrolName = ui->lineEdit_updatename->text();

        // check the new input if it's change
        if (s != newPatrolName)
        {
            if (controlNameValidity(newPatrolName) == true)
            {
                //instance_.planification()->changePatrolName(patrol.name(), newPatrolName);
                if (patrol)
                    patrol->set_name(newPatrolName.toStdString());
            }
        }

        updateItemList(current, *patrol);
    }
    updateUI();
}

//void PatrolAdminWidget::on_listWidget_patrols_currentRowChanged(int)
//{
//    callChangePatrol();
//}

namespace
{
/**
 * @brief indexForAType : return the real index from the complete patrol list
 * @param patrols list
 * @param type current type
 * @param index_type the index patrol for this type
 * @return
 */
int indexForAType(eos::PatrolsSharedPointer patrols, eos::Patrol::Type type, int index_type)
{
  int found = -1;
  int cpt = 0;
  for (int index = 0; index < patrols->patrols().size(); ++index)
  {
      eos::Patrol const &patrol = patrols->patrols().Get(index);
      if (patrol.type() == type) found++;
      if (found == index_type)
          return cpt;
      cpt++;
  }
  return cpt;
}
} // ano namespace

void PatrolAdminWidget::callChangePatrol()
{
    QListWidgetItem *current = ui->listWidget_patrols->currentItem();
    if (current)
        emit changePatrol(indexForAType(instance_.patrols(), type_, ui->listWidget_patrols->row(current)));
    else
        emit removePatrol();
    updateUI();
}

bool PatrolAdminWidget::controlNameValidity(QString const &newName)
{
    if(newName.isEmpty()) return false;
    for (int index = 0; index < instance_.patrols()->patrols().size(); ++index)
    {
        eos::Patrol const &patrol = instance_.patrols()->patrols().Get(index);
        if (patrol.name() == newName.toStdString())
        {
            core::alert(tr("A patrol with same name already exist : %1").arg(newName));
            return false;
        }
    }
    return true;
}

void PatrolAdminWidget::newPoint(QPoint const &point)
{
    // new point, have i a select patrol ?
    QListWidgetItem *current = ui->listWidget_patrols->currentItem();
    if (!current)
    {
        addNewPatrol();
        current = ui->listWidget_patrols->currentItem();
    }

    if (!current)
    {
        core::alert(tr("impossible_to_create_patrol"));
        return;
    }

    const QString &s = current->text();
    eos::Patrol *patrol = getPointerPatrol(instance_.patrols(), s);
    eos::Patrol::Point *patrol_point = patrol->mutable_points()->Add();
    patrol_point->mutable_position()->set_x(point.x());
    patrol_point->mutable_position()->set_y(point.y());
    patrol_point->set_priority(eos::Patrol::Priority_MIN);
    updateItemList(current, *patrol);
}

void PatrolAdminWidget::updatePoint(int index, QPoint const &point)
{
    QListWidgetItem *current = ui->listWidget_patrols->currentItem();
    if (current)
    {
        const QString &s = current->text();
        eos::Patrol *patrol = getPointerPatrol(instance_.patrols(), s);
        if (patrol)
        {
            eos::Patrol::Point *patrol_point = patrol->mutable_points()->Mutable(index);
            if (patrol_point)
            {
                patrol_point->mutable_position()->set_x(point.x());
                patrol_point->mutable_position()->set_y(point.y());
                updateItemList(current, *patrol);
            }
        }
    }
}

void PatrolAdminWidget::removePoint(int index)
{
    QListWidgetItem *current = ui->listWidget_patrols->currentItem();
    if (current)
    {
        const QString &s = current->text();
        eos::Patrol *patrol = getPointerPatrol(instance_.patrols(), s);
        if (patrol && index < patrol->points_size() && patrol->points_size() > 0)
        {
            for (int i = index; i < patrol->points().size()-1; ++i) // Swap elements until the last one to maintain the right order
            {
                patrol->mutable_points()->SwapElements(i, i+1);
            }
            patrol->mutable_points()->RemoveLast();
            updateItemList(current, *patrol);
            callChangePatrol();
        }
    }
}

void PatrolAdminWidget::changeIndex(int old, int index)
{
    QListWidgetItem *current = ui->listWidget_patrols->currentItem();
    if (current)
    {
        if (old != -1 && index != -1)
        {
            eos::Patrol *patrol = getPointerPatrol(instance_.patrols(), current->text());
            patrol->mutable_points()->SwapElements(old, index);
            updateItemList(current, *patrol);
            callChangePatrol();
        }
    }
}

void PatrolAdminWidget::changePriority(int index, int priority)
{
    QListWidgetItem *current = ui->listWidget_patrols->currentItem();
    if (current)
    {
        const QString &s = current->text();
        eos::Patrol *patrol = getPointerPatrol(instance_.patrols(), s);
        eos::Patrol::Point *point = patrol->mutable_points()->Mutable(index);
        point->set_priority(static_cast<eos::Patrol::Priority>(priority));
    }
}

void PatrolAdminWidget::updateUsageStatus(QString previous_name)
{
    for(auto const& patrol : instance_.patrols()->patrols())
    {
        std::for_each(instance_.patrols()->patrols().begin()
                      ,instance_.patrols()->patrols().end(),
                      [&](eos::Patrol patrol){if(previous_name.toStdString() == patrol.name() && previous_name != "")
                                              {editPatrolFromList(patrol);}}
        );

        (void)patrol; // Just to silence unused warning !
    }
}

void PatrolAdminWidget::updateAllUsageStatus()
{
    for(auto patrol : instance_.patrols()->patrols())
    {
        editPatrolFromList(patrol);
    }
}

void PatrolAdminWidget::on_pushButton_duplicate_clicked()
{
    QListWidgetItem *current = ui->listWidget_patrols->currentItem();
    if (current)
    {
        const QString &s = current->text();
        //  /// @@@ HA check the keyword copie
        //  if (s.contains("copy"))
        //  {
        //    core::alert(tr("forbidden_duplicate_patrol_with_keyword_copy_inside"));
        //    return;
        //  }

        eos::Patrol *patrol = getPointerPatrol(instance_.patrols(), s);
        eos::Patrol *new_patrol = instance_.patrols()->mutable_patrols()->Add();
        *new_patrol = *patrol;
        new_patrol->set_name(suggestCopyName(s).toStdString());
        addPatrolToList(*new_patrol);

        updateUI();
    }
}

QString PatrolAdminWidget::suggestCopyName(QString const& base)
{
    int id = 0;
    QRegExp valid_input(QString("^* \\d*$"));
    for (int i = 0; i < ui->listWidget_patrols->count(); ++i)
    {
        QListWidgetItem *item = ui->listWidget_patrols->item(i);
        if (item->text().contains(base))
        {
            int copyIdx = item->text().indexOf(tr("( %1 ").arg(copy_keyboard_));
            int valIdx = item->text().length() - 1;
            if (valid_input.indexIn(item->text().mid(copyIdx, valIdx - copyIdx)) != -1)
                id = std::max(valid_input.cap(0).toInt(), id);
        }
    }
    return QObject::tr("%1 (%2 %3)").arg(base).arg(copy_keyboard_).arg(id + 1);
}

void PatrolAdminWidget::on_pushButton_down_clicked()
{
    QListWidgetItem *current = ui->listWidget_patrols->currentItem();
    int list_current_index = ui->listWidget_patrols->row(current);
    QListWidgetItem *next = ui->listWidget_patrols->item(list_current_index + 1);
    if (current && next)
    {
        int current_index = getPatrolIndex(instance_.patrols(), current->text());
        int next_index = getPatrolIndex(instance_.patrols(), next->text());
        if (current_index != -1 && next_index != -1)
        {
            //swap the current & next patrol
            instance_.patrols()->mutable_patrols()->SwapElements(current_index, next_index);

            //update the displayed List
            int list_index = 0;
            for (auto const &patrol : instance_.patrols()->patrols())
            {
                if (patrol.type() == type_)
                {
                    updateItemList(ui->listWidget_patrols->item(list_index), patrol);
                    ++list_index;
                }
            }
            ui->listWidget_patrols->setCurrentRow(list_current_index + 1);
        }
    }
    callChangePatrol();
}

void PatrolAdminWidget::on_pushButton_up_clicked()
{
    QListWidgetItem *current = ui->listWidget_patrols->currentItem();
    int list_current_index = ui->listWidget_patrols->row(current);

    QListWidgetItem *previous = ui->listWidget_patrols->item(list_current_index - 1);
    if (current && previous)
    {
        int current_index = getPatrolIndex(instance_.patrols(), current->text());
        int previous_index = getPatrolIndex(instance_.patrols(), previous->text());
        if (current_index != -1 && previous_index != -1)
        {
            //swap the current & next patrol
            instance_.patrols()->mutable_patrols()->SwapElements((current_index),(previous_index));//remove(current->text());

            //update the displayed List
            int list_index = 0;
            for (auto const &patrol : instance_.patrols()->patrols())
            {
                if (patrol.type() == type_)
                {
                    updateItemList(ui->listWidget_patrols->item(list_index), patrol);
                    ++list_index;
                }
            }
            ui->listWidget_patrols->setCurrentRow(list_current_index - 1);
        }
    }
    callChangePatrol();
}

void PatrolAdminWidget::update()
{
    ui->listWidget_patrols->clear();
    load();
    updateUI();
}

void PatrolAdminWidget::contextMenuEvent(QContextMenuEvent *event)
{
    // No need for context menu when no patrols
    if (ui->listWidget_patrols->count() == 0)
        return;

    std::shared_ptr<QMenu> menu(new QMenu(0));

    QAction *duplicate = menu->addAction(tr("Duplicate"));

    std::unique_ptr<QAction> changeType;
    changeType.reset(new QAction(tr("Change type"),this));
    std::unique_ptr<QAction> remove;
    remove.reset(new QAction(tr("Delete"),this));
    std::unique_ptr<QAction> removeTasks;
    removeTasks.reset(new QAction(tr("Remove the scheduled tasks associated"),this));

    // Qt::UserRole+2 is the patrol status (active - inactive).
    //So if the selected item in the ui->listWidget_patrols is a patrol not scheduled we display the permitted action
    if(ui->listWidget_patrols->currentItem()->data(Qt::UserRole+2) == false)
    {
        menu->addAction(changeType.get());
        menu->addAction(remove.get());
    }
    else
        menu->addAction(removeTasks.get());

    // and run the menu !
    QAction *selectedAction = menu->exec(event->globalPos());

    if (selectedAction != 0)
    {
        if (selectedAction == removeTasks.get())
        {
            QListWidgetItem const *current = ui->listWidget_patrols->currentItem();
            if (current)
            {
                const QString &s = current->text();
                eos::Patrol const *patrol = getPointerPatrol(instance_.patrols(), s);
                emit removeTasksFromPatrol(patrol->name());
            }
        }
        else if (selectedAction == remove.get())
        {
            if (core::question(tr("Please confirm delete this patrol")))
            {
                on_pushButton_remove_clicked();
            }
        }
        else if (selectedAction == duplicate)
        {
            on_pushButton_duplicate_clicked();
        }
        else if(selectedAction == changeType.get())
        {
            QListWidgetItem *current = ui->listWidget_patrols->currentItem();
            if (current)
            {
                // Qt::UserRole+2 is the patrol status (active - inactive).
                //So if the selected item in the ui->listWidget_patrols is a patrol not scheduled we display the permitted action
                if(ui->listWidget_patrols->currentItem()->data(Qt::UserRole+2) == false)
                {
                    const QString &s = current->text();
                    eos::Patrol *patrol = getPointerPatrol(instance_.patrols(), s);
                    if(type_ == eos::Patrol_Type_Normal)
                        patrol->set_type(eos::Patrol_Type_Remapping);
                    else
                        patrol->set_type(eos::Patrol_Type_Normal);
                    emitEvent<core::PatrolsUpdate>();
                }
                else
                    core::alert(tr("You can't change the type of an activ patrol. Try to remove the associated tasks before or duplicate your patrol before changing the type."));
            }
        }
    }
}

void PatrolAdminWidget::on_listWidget_patrols_clicked(const QModelIndex &/*index*/)
{
  callChangePatrol();
}
