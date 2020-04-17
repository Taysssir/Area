#include "calendarwidget.h"
#include "ui_calendarwidget.h"

#include "dialogcreateplannification.h"
#include "contants_calendar.h"
#include <QSpacerItem>
#include <QScrollBar>
#include "sentinel-admin_core/qt_utils.h"
#include <algorithm>

CalendarWidget::CalendarWidget(core::AppInstance &instance, QWidget *parent)
    : QWidget(parent)
    , core::EventClient<CalendarWidget>(instance)
    , ui(new Ui::CalendarWidget)
    , main_toolBar_(new QToolBar("Calendar Tools", this))
    , calendar_toolBar_(new QToolBar("Calendar Tools", this))
{
    ui->setupUi(this);

    current_date_ = QDate().currentDate();
    scene_ =  new CalendarPatrolScene(CalendarBackgroundScene::ALL_VISUAL,instance_,this);
    ui->graphics_view_calendar_->setScene(scene_);

    setupToolBar();


    connect(scene_,SIGNAL(signalOnGraphicsTaskScheduled(GraphicsPatrolItem::event_graphics_task,TaskScheduled)),this,SLOT(eventOnTaskScheduled(GraphicsPatrolItem::event_graphics_task,TaskScheduled)));

    week_clicked();
    connect(scene_,SIGNAL(taskSelected(bool)),edit_selected_task_,SLOT(setEnabled(bool)));
    ui->main_toolBar_layout_->addWidget(main_toolBar_);
    main_toolBar_->setMaximumHeight(40);
    ui->calendar_toolBar_layout_->addWidget(calendar_toolBar_);

    minicalendar_ = new MiniCalendarWidget(instance,this);
    ui->minicalendar_layout->insertWidget(0,minicalendar_);
    minicalendar_->setMaximumHeight(250);
    minicalendar_->setMinimumHeight(150);
    minicalendar_->setFixedWidth(200);
    connect(minicalendar_,SIGNAL(updateCalendarView(QDateTime)),this,SLOT(updateCurrentDate(QDateTime)));

    subscribe<core::PlanificationUpdate>(*this);

    loadTasksScheduled();
}

CalendarWidget::~CalendarWidget()
{

}

void CalendarWidget::receive(core::PlanificationUpdate const& )
{
    loadTasksScheduled();
    minicalendar_->display_today();
}

void CalendarWidget::loadTasksScheduled()
{
    tasks_ = linkToCalendar(instance_.read_only().tasksScheduled());
    updateCurrentDate(QDateTime(current_date_));
}

void CalendarWidget::setupToolBar()
{
    main_toolBar_->setOrientation(Qt::Horizontal);

    create_task_ = new QAction(this);
    create_task_->setText(tr("Schedule a task"));
    main_toolBar_->addAction(create_task_);
    connect(create_task_,SIGNAL(triggered()),this,SLOT(create_clicked()));
    edit_selected_task_ = new QAction(this);
    edit_selected_task_->setText(tr("Modify the selected task"));
    edit_selected_task_->setEnabled(false);
    main_toolBar_->addAction(edit_selected_task_);
    connect(edit_selected_task_,SIGNAL(triggered()),this,SLOT(edit_clicked()));

    QWidget* spacerWidget = new QWidget(main_toolBar_);
    spacerWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
    main_toolBar_->addWidget(spacerWidget);

    saveAndClose_ = new QToolButton(this);
    saveAndClose_->setIcon(QIcon(":/icons/toolbar/ok_t"));
    saveAndClose_->setText(tr("Save and close"));
    saveAndClose_->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    main_toolBar_->addWidget(saveAndClose_);
    connect(saveAndClose_, SIGNAL(clicked()), this, SLOT(saveAndClose_clicked()));

    closeButton_ = new QToolButton(this);
    closeButton_->setIcon(QIcon(":/icons/toolbar/fail_t"));
    closeButton_->setText(tr("Close"));
    closeButton_->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    main_toolBar_->addWidget(closeButton_);
    connect(closeButton_, SIGNAL(clicked()), this, SIGNAL(signal_closeCalendar())); // Yes, you can connect a signal to another signal in Qt!

    main_toolBar_->setIconSize(QSize(18,18));

    today_date_ = new QAction(this);
    today_date_->setText(tr("Today"));
    calendar_toolBar_->addAction(today_date_);
    connect(today_date_,SIGNAL(triggered()),this,SLOT(today_clicked()));

    previous_date_ = new QAction(this);
    previous_date_->setIcon(QIcon(":/calendar/Resources/calendar/previous.png"));
    calendar_toolBar_->addAction(previous_date_);
    connect(previous_date_,SIGNAL(triggered()),this,SLOT(previous_clicked()));

    next_date_ = new QAction(this);
    next_date_->setIcon(QIcon(":/calendar/Resources/calendar/next.png"));
    calendar_toolBar_->addAction(next_date_);
    connect(next_date_,SIGNAL(triggered()),this,SLOT(next_clicked()));

    calendar_toolBar_->addSeparator();

    displayed_date = new QAction(this);
    displayed_date->setText("");
    displayed_date->setEnabled(false);
    calendar_toolBar_->addAction(displayed_date);


    calendar_toolBar_->addSeparator();

    display_day_ = new QAction(this);
    display_day_->setIcon(QIcon(":/calendar/Resources/calendar/day.png"));
    display_day_->setCheckable(true);
    calendar_toolBar_->addAction(display_day_);
    connect(display_day_,SIGNAL(triggered()),this,SLOT(day_clicked()));

    display_week_ = new QAction(this);
    display_week_->setIcon(QIcon(":/calendar/Resources/calendar/week.png"));
    display_week_->setCheckable(true);
    calendar_toolBar_->addAction(display_week_);
    connect(display_week_,SIGNAL(triggered()),this,SLOT(week_clicked()));

    display_month_ = new QAction(this);
    display_month_->setIcon(QIcon(":/calendar/Resources/calendar/month.png"));
    display_month_->setCheckable(true);
    calendar_toolBar_->addAction(display_month_);
    connect(display_month_,SIGNAL(triggered()),this,SLOT(month_clicked()));


}

void CalendarWidget::day_clicked()
{
    buttonClicked(CalendarBackgroundScene::DAY);
}

void CalendarWidget::week_clicked()
{
    buttonClicked(CalendarBackgroundScene::WEEK);
}

void CalendarWidget::month_clicked()
{
    buttonClicked(CalendarBackgroundScene::MONTH);
}

void CalendarWidget::list_clicked()
{
    buttonClicked(CalendarBackgroundScene::LIST);
}

void CalendarWidget::previous_clicked()
{
    scene_->shiftDate(CalendarBackgroundScene::PREVIOUS);
    scene_->updatePatrols(tasks_.scheduledPatrol(scene_->getFirstDisplayedDate(), scene_->getLastDisplayedDate()));
    displayed_date->setText(getDisplayedDateText());
}

void CalendarWidget::next_clicked()
{
    scene_->shiftDate(CalendarBackgroundScene::NEXT);
    scene_->updatePatrols(tasks_.scheduledPatrol(scene_->getFirstDisplayedDate(), scene_->getLastDisplayedDate()));
    displayed_date->setText(getDisplayedDateText());
}

void CalendarWidget::buttonClicked(CalendarBackgroundScene::VISUAL_SELECTED visual)
{

    scene_->switchVisual(visual);
    setCheckButton(visual);
//    if(visual == CalendarBackgroundScene::MONTH)
//        ui->graphics_view_calendar_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    else
//        ui->graphics_view_calendar_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    sceneFitInView();
    displayed_date->setText(getDisplayedDateText());
}

void CalendarWidget::resizeEvent(QResizeEvent * )
{
    sceneFitInView();
}

void CalendarWidget::sceneFitInView()
{
    QRectF rect = ui->graphics_view_calendar_->rect();
    int width= rect.width();
    if(ui->graphics_view_calendar_->verticalScrollBar()->isVisible())
    {
        width -= ui->graphics_view_calendar_->verticalScrollBar()->width();
    }
    rect = QRectF(rect.x(),rect.y(),
                  width,
                  scene_->getRequiredHeight());
    scene_->setSceneRect(rect);
    scene_->resize(rect);
    ui->graphics_view_calendar_->centerOn(0,0);
    scene_->updatePatrols(tasks_.scheduledPatrol(scene_->getFirstDisplayedDate(), scene_->getLastDisplayedDate()));
}

bool CalendarWidget::validateMaxTasksPerDay(const QVector<QDateTime> &startDates, const QVector<QDateTime> &endDates, QSet<QDate> const& ignoreDates)
{
    QString errorDates = "";
    QSet<QDate> dates_set; // Using QSet will prevent from having duplicate dates
    for (int i = 0; i < startDates.size(); ++i) // Populate with start dates
    {
        dates_set.insert(startDates.at(i).date());
    }
    for (int i = 0; i < endDates.size(); ++i) // Populate with end dates
    {
        dates_set.insert(endDates.at(i).date());
    }
    foreach (const QDate &ignDate, ignoreDates) // Remove dates to be ignored
    {
        dates_set.remove(ignDate);
    }
    foreach (const QDate &date, dates_set) // Check each date for max number of tasks
    {
        if ( tasks_.scheduledPatrol(date, date).size() == constants::tasks::MAX_TASKS_PER_DAY_ )
            errorDates += date.toString("dd/MM/yyyy") + "\n";
    }
    if ( !errorDates.isEmpty() )
    {
        core::alert( tr("Cannot have more than %1 scheduled tasks on the following dates :\n\n").arg(constants::tasks::MAX_TASKS_PER_DAY_)
                     + errorDates );
        return false;
    }
    return true;
}

void CalendarWidget::create_clicked()
{
    QVector<QString> patrol_name_vector;
    for(auto patrol : instance_.read_only().patrols()->patrols())
    {
        if(patrol.type() == eos::Patrol::Type::Patrol_Type_Normal)
            patrol_name_vector.append(QString::fromStdString(patrol.name()));
    }

    if ( patrol_name_vector.isEmpty() ) // If no patrols, do not run task schedule dialog
    {
        QMessageBox::critical( parentWidget(),
                               tr("Schedule a task"),
                               tr("Cannot schedule a task when no patrols are created !") );
        return;
    }

    DialogCreatePlannification dialog(instance_, patrol_name_vector);
    if(dialog.exec())
    {
        if ( !validateMaxTasksPerDay(dialog.getStartDates(), dialog.getEndDates()) )
            return;

        int add_tasks_result = tasks_.addTasks(dialog.getPatrolName(),dialog.getPatrolNumber(),dialog.getStartDates(),dialog.getEndDates());
        if(add_tasks_result == 0 && dialog.getStartDates().size() == 1)
        {
            core::alert(tr("Overlap schedulation is detected. This task will not be scheduled."));
        }
        else if (add_tasks_result != dialog.getStartDates().size())
        {
            core::alert(tr("Overlap schedulation is detected. A part of the tasks will not be scheduled."));
        }
        apply();
        emit updateTaskUsageStatus(dialog.getPatrolName());
        scene_->addPatrols(tasks_.scheduledPatrol(scene_->getFirstDisplayedDate(), scene_->getLastDisplayedDate()));
    }

}

void CalendarWidget::edit_clicked()
{
    eventOnTaskScheduled(GraphicsPatrolItem::MODIFY_TASK,scene_->getSelectedPatrol());
}

void CalendarWidget::saveAndClose_clicked()
{
    instance_.configuration().save(eos::Configuration::TasksScheduled);
    emit signal_closeCalendar();
}

void CalendarWidget::today_clicked()
{
    scene_->shiftDate(CalendarBackgroundScene::TODAY);
    scene_->updatePatrols(tasks_.scheduledPatrol(scene_->getFirstDisplayedDate(), scene_->getLastDisplayedDate()));
    displayed_date->setText(getDisplayedDateText());
    minicalendar_->display_today();
}

void CalendarWidget::removeTaskFromPatrol(const QString &name)
{
    if(core::question(tr("Do you want to remove the task from patrol : %1?").arg(name)))
    {
        tasks_.mutable_tasks().erase(std::remove_if(
                                         tasks_.mutable_tasks().begin(),
                                         tasks_.mutable_tasks().end(),
                                         [&](const TaskScheduled& task){ return task.getPatrolName() == name; })
                                     ,tasks_.mutable_tasks().end());
        apply();
        emit updateTaskUsageStatus(name);
        scene_->updatePatrols(tasks_.scheduledPatrol(scene_->getFirstDisplayedDate(), scene_->getLastDisplayedDate()));
    }
}

void CalendarWidget::eventOnTaskScheduled(GraphicsPatrolItem::event_graphics_task event, TaskScheduled task)
{
    QVector<QString> patrol_name_vector;
    for(auto patrol : instance_.read_only().patrols()->patrols())
    {
        if(patrol.type() == eos::Patrol::Type::Patrol_Type_Normal)
            patrol_name_vector.append(QString::fromStdString(patrol.name()));
    }
    switch(event)
    {
    case GraphicsPatrolItem::REMOVE_TASK:
    {
        if(core::question(tr("Do you want to remove the selected task ?")))
        {
            tasks_.removeTask(task);
            apply();
            emit updateTaskUsageStatus(task.getPatrolName());
        }
        break;
    }
    case GraphicsPatrolItem::MODIFY_TASK :
    {
        DialogCreatePlannification dialog(instance_, patrol_name_vector,task);
        if(dialog.exec())
        {
            QSet<QDate> ignoreDates; // These dates will be ignored since the task already exists on them
            ignoreDates.insert(task.getStartingTime().date());
            ignoreDates.insert(task.getEndingTime().date());

            if ( !validateMaxTasksPerDay(dialog.getStartDates(), dialog.getEndDates(), ignoreDates) )
                return;

            bool result = tasks_.updateTask(task,dialog.getPatrolName(),dialog.getPatrolNumber(),dialog.getStartDates().first(),dialog.getEndDates().first());
            if(!result)
            {
                core::alert(tr("Overlap schedulation detected. Modification will not be accepted"));
            }
            apply();
            emit updateTaskUsageStatus(task.getPatrolName());
            emit updateTaskUsageStatus(dialog.getPatrolName());
        }
        break;
    }
    case GraphicsPatrolItem::REMOVE_GROUP_TASK:
        if(core::question(tr("Do you want to remove the selected task and it group of task ?")))
        {
            tasks_.removeGroupTask(task);
            apply();
            emit updateTasksUsageStatus();
        }
        break;
    case GraphicsPatrolItem::REMOVE_SCENE_TASK :
        if(core::question(tr("Do you want to remove ALL the different task ?")))
        {
            tasks_.removeAllTask();
            apply();
            emit updateTasksUsageStatus();
        }
        break;
    case GraphicsPatrolItem::ADD_TASK:
    {
        if(patrol_name_vector.size() > 0)
        {
            if(task.getStartingTime() < task.getEndingTime().addSecs(-60*60))
            {
                DialogCreatePlannification dialog(instance_, patrol_name_vector, task.getStartingTime(),task.getEndingTime());
                if(dialog.exec())
                {
                    if ( !validateMaxTasksPerDay(dialog.getStartDates(), dialog.getEndDates()) )
                        return;

                    int add_tasks_result = tasks_.addTasks(dialog.getPatrolName(),dialog.getPatrolNumber(),dialog.getStartDates(),dialog.getEndDates());
                    if(add_tasks_result == 0 && dialog.getStartDates().size() == 1)
                    {
                        core::alert(tr("Overlap schedulation is detected. This task will not be scheduled."));
                    }
                    else if (add_tasks_result != dialog.getStartDates().size())
                    {
                        core::alert(tr("Overlap schedulation is detected. A part of the tasks will not be scheduled."));
                    }
                    apply();
                    emit updateTaskUsageStatus(task.getPatrolName());
                    emit updateTaskUsageStatus(dialog.getPatrolName());
                }
            }
        }
        break;
    }
    }
    scene_->updatePatrols(tasks_.scheduledPatrol(scene_->getFirstDisplayedDate(), scene_->getLastDisplayedDate()));
}

void CalendarWidget::setCheckButton(CalendarBackgroundScene::VISUAL_SELECTED visual)
{

    /* if(visual != CalendarBackgroundScene::LIST)
        display_list_->setChecked(false);
    else
        display_list_->setChecked(true);*/
    if(visual != CalendarBackgroundScene::MONTH)
        display_month_->setChecked(false);
    else
        display_month_->setChecked(true);
    if(visual != CalendarBackgroundScene::DAY)
        display_day_->setChecked(false);
    else
        display_day_->setChecked(true);
    if(visual != CalendarBackgroundScene::WEEK)
        display_week_->setChecked(false);
    else
        display_week_->setChecked(true);

}

TasksScheduled CalendarWidget::linkToCalendar(eos::TasksScheduledSharedPointer pointer)
{
    TasksScheduled temporary;

    for(auto pointer_task : pointer->tasks())
    {
        QString name = QString::fromStdString(pointer_task.patrol_name());
        int patrol_number(0);
        eos::PatrolsSharedPointer patrols = instance_.read_only().patrols();
        for(int i(0);i< patrols->patrols_size();++i)
        {
            if(name.toStdString() == patrols->patrols(i).name())
            {
                patrol_number = i+1;
                break;
            }
        }
        if(pointer_task.group_id())
            temporary.addTask(name, patrol_number,QDateTime::fromMSecsSinceEpoch(pointer_task.start()),QDateTime::fromMSecsSinceEpoch(pointer_task.stop()),
                              QDateTime::fromMSecsSinceEpoch(pointer_task.group_id()));
        else
            temporary.addTask(name, patrol_number,QDateTime::fromMSecsSinceEpoch(pointer_task.start()),QDateTime::fromMSecsSinceEpoch(pointer_task.stop()));
    }
    return temporary;
}

void CalendarWidget::linkToEosModel(TasksScheduled const& tasks)
{
    instance_.tasksScheduled()->mutable_tasks()->Clear();
    for(auto const& task : tasks.tasks())
    {
        eos::TaskScheduled *eos_task = instance_.tasksScheduled()->mutable_tasks()->Add();
        eos_task->set_patrol_name(task.getPatrolName().toStdString());
        eos_task->set_start(task.getStartingTime().toMSecsSinceEpoch());
        eos_task->set_stop(task.getEndingTime().toMSecsSinceEpoch());
        if(task.getRepetionGroupNumber().isValid())
            eos_task->set_group_id(task.getRepetionGroupNumber().toMSecsSinceEpoch());
        else
            eos_task->set_group_id(0);
    }
}

void CalendarWidget::apply()
{
    linkToEosModel(tasks_);
    // instance_.configuration().save(eos::Configuration::TasksScheduled);
}

void CalendarWidget::cancel()
{
    tasks_ = linkToCalendar(instance_.tasksScheduled());

    scene_->updatePatrols(tasks_.scheduledPatrol(scene_->getFirstDisplayedDate(), scene_->getLastDisplayedDate()));
    sceneFitInView();
}

QString CalendarWidget::getDisplayedDateText()
{
    switch(scene_->getVisual())
    {
    case CalendarBackgroundScene::NOTHING:
        break;
    case CalendarBackgroundScene::DAY:
        return instance_.getLocaleDate(scene_->getFirstDisplayedDate(),"dddd, d MMMM yyyy");
        break;
    case CalendarBackgroundScene::MONTH:
        return instance_.getLocaleDate(scene_->getFirstDisplayedDate().addDays(7),"MMMM yyyy");
        break;
    case CalendarBackgroundScene::WEEK:
    case CalendarBackgroundScene::LIST:
        QString date;
        date = instance_.getLocaleDate(scene_->getFirstDisplayedDate(),"d MMM. ");
        date += instance_.getLocaleDate(scene_->getLastDisplayedDate(),"- d MMM. yyyy");
        return date;
        break;
    }
    return "";
}

void CalendarWidget::updateCurrentDate(QDateTime date)
{
    scene_->changeCurrentDate(date.date());
    scene_->updatePatrols(tasks_.scheduledPatrol(scene_->getFirstDisplayedDate(), scene_->getLastDisplayedDate()));
    displayed_date->setText(getDisplayedDateText());
}
