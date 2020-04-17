#ifndef CALENDARWIDGET_H
#define CALENDARWIDGET_H

#include <QWidget>
#include <QResizeEvent>
#include <QToolBar>
#include <QToolButton>
#include <QMessageBox>

#include <memory>

#include "core/appinstance.h"
#include "tasksscheduled.h"
#include "Graphics/calendarpatrolscene.h"
#include "minicalendarwidget.h"
#include "sentinel-admin_core/events.h"

namespace Ui {
class CalendarWidget;
}

namespace core
{
class AppInstance;
}
class CalendarWidget
      : public QWidget
      , public core::EventClient<CalendarWidget>
{
    Q_OBJECT
public:
    explicit CalendarWidget(core::AppInstance &instance, QWidget *parent = 0);
    ~CalendarWidget();
    void loadTasksScheduled();
    void receive(core::PlanificationUpdate const& );
signals:
    void updateTaskUsageStatus(QString const&);
    void updateTasksUsageStatus();
    void signal_closeCalendar();
private slots:
    void day_clicked();
    void week_clicked();
    void month_clicked();
    void previous_clicked();
    void next_clicked();
    void list_clicked();
    void create_clicked();
    void edit_clicked();
    void saveAndClose_clicked();
    void today_clicked();
    void eventOnTaskScheduled(GraphicsPatrolItem::event_graphics_task event, TaskScheduled task);
    void apply();
    void cancel();
    void buttonClicked(CalendarBackgroundScene::VISUAL_SELECTED visual);
public slots :
    void updateCurrentDate(QDateTime date);
    void removeTaskFromPatrol(QString const& name);
protected slots:
    void resizeEvent(QResizeEvent * event);

private:
    void setCheckButton(CalendarBackgroundScene::VISUAL_SELECTED visual);

    TasksScheduled linkToCalendar(eos::TasksScheduledSharedPointer pointer);
    void linkToEosModel(TasksScheduled const& tasks);
    void setupToolBar();
    QString getDisplayedDateText();
    void sceneFitInView();
    bool validateMaxTasksPerDay(QVector<QDateTime> const&startDates, QVector<QDateTime> const&endDates, QSet<QDate> const& ignoreDates = QSet<QDate>());

    Ui::CalendarWidget *ui;
    QToolBar * main_toolBar_, *calendar_toolBar_;
    QAction * create_task_,
            * edit_selected_task_,
            * today_date_,
            * previous_date_ ,
            * next_date_,
            * display_day_,
            * display_week_,
            * display_month_,
            * display_list_,
            * displayed_date;
    QToolButton* saveAndClose_;
    QToolButton* closeButton_;
    QDate current_date_;
    CalendarPatrolScene* scene_;
    MiniCalendarWidget* minicalendar_;
    TasksScheduled tasks_;
};

#endif // CALENDARWIDGET_H
