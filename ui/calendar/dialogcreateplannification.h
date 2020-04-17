#ifndef DIALOGCREATEPLANNIFICATION_H
#define DIALOGCREATEPLANNIFICATION_H

#include <QDialog>
#include "taskscheduled.h"
#include "core/appinstance.h"
#include <QMutex>

namespace Ui {
class DialogCreatePlannification;
}

class DialogCreatePlannification : public QDialog
{
    Q_OBJECT

public:
    typedef enum {NO_REPETITION, DAY_REPETITION, PSEUDO_DAY_REPETITION, WEEK_REPETITION, MONTH_REPETITION, YEAR_REPETITION}repetition_type;
    explicit DialogCreatePlannification(core::AppInstance &instance, QVector<QString> string, QWidget *parent = 0);
    DialogCreatePlannification(core::AppInstance &instance, QVector<QString> string, TaskScheduled task, QWidget *parent = 0);
    DialogCreatePlannification(core::AppInstance &instance, QVector<QString> string, QDateTime start, QDateTime end, QWidget *parent = 0);
    ~DialogCreatePlannification();
    QString getPatrolName();
    int getPatrolNumber();
    QVector<QDateTime> getStartDates();
    QVector<QDateTime> getEndDates();
    QVector<QString> timeSpan(QDateTime time_left, QDateTime time_right);

private slots:
    void on_radioButton_repeted_no__clicked();
    void on_radioButton_repeted__clicked();
    void on_radioButton_every_day__clicked();
    void on_radioButton_every_week__clicked();
    void on_radioButton_every_month__clicked();
    void on_radioButton_every_year__clicked();
    void on_dateEdit_start__dateChanged(const QDate &date);
    void on_timeEdit_start__timeChanged(const QTime &time);
    void on_dateEdit_end__dateChanged(const QDate &date);
    void on_timeEdit_end__timeChanged(const QTime &time);
    void updateWhatsUpText();
private:
    void setupLabel();
    void manageCheckBoxWeekUsage();
    void manageCheckBoxWeekDisplay();
    void setMinimumTime();

    core::AppInstance &instance_;
    QMutex date_time_schedule_mutex;
    Ui::DialogCreatePlannification *ui;
    bool start_, editing_;
    QDate last_day_selected_ ;
    repetition_type current_repetition_;
};

#endif // DIALOGCREATEPLANNIFICATION_H
