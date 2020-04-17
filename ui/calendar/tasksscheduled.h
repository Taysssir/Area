#ifndef CALENDAR_PLANNIFICATION_H
#define CALENDAR_PLANNIFICATION_H

#include <QVector>

#include "taskscheduled.h"

class TasksScheduled
{
public:
    TasksScheduled();
    ~TasksScheduled();

    int addTasks(QString const& name, int patrol_number, QVector<QDateTime> const& time_start, QVector<QDateTime> const& time_end);
    bool addTask(QString const& name, int patrol_number, QDateTime const& time_start, QDateTime const& time_end);
    bool addTask(QString const& name, int patrol_number, QDateTime const& time_start, QDateTime const& time_end, QDateTime const& first);
    bool isFree(QDateTime const& time_start, QDateTime const& time_end, int index_non_used = -1) const;

    QVector<TaskScheduled> scheduledPatrol(QDateTime const& time_start, QDateTime const& time_end, int index_non_verified = -1 ) const;
    QVector<TaskScheduled> scheduledPatrol(QDate const& time_start, QDate const& time_end) const
    {
        return scheduledPatrol(QDateTime(time_start,QTime(0,0)), QDateTime(time_end,QTime(23,59)));
    }

    /**
     * @brief removeTask
     * @param task
     */
    void removeTask(TaskScheduled const& task);

    void removeGroupTask(TaskScheduled const& task);

    void removeAllTask();

    bool updateTask(TaskScheduled const& task, QString const& name, int patrol_number, QDateTime const& time_start, QDateTime const& time_end);

    QVector<TaskScheduled> const& tasks() const
    {
        return tasks_vector_;
    }

    QVector<TaskScheduled> & mutable_tasks()
    {
        return tasks_vector_;
    }
private :
    void sort();

    QVector<TaskScheduled> tasks_vector_;
};

#endif // CALENDAR_PLANNIFICATION_H
