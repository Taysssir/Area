#ifndef TASKSCHEDULED_H
#define TASKSCHEDULED_H

#include <QDateTime>
#include <QString>

class TaskScheduled
{
public:
    TaskScheduled(QString const& name, int number, QDateTime const& start_time, QDateTime const& end_time);
    TaskScheduled(QString const& name, int number, QDateTime const& start_time, QDateTime const& end_time, QDateTime const& repetition_is_on);
    TaskScheduled(){
        patrol_number_ = 0;
    }
    ~TaskScheduled();

    QString const& getPatrolName() const
    {
        return patrol_name_;
    }
    int getPatrolNumber()
    {
        return patrol_number_;
    }
    void setPatrolNumber(int value)
    {
        patrol_number_ = value;
    }

    void setPatrolName(QString const& patrol_name)
    {
        patrol_name_ = patrol_name;
    }
    QDateTime const& getStartingTime() const
    {
        return starting_time_;
    }
    void setStartingTime(QDateTime const& starting_time)
    {
        starting_time_ = starting_time;
    }
    QDateTime const& getEndingTime() const
    {
        return ending_time_;
    }
    void setEndingTime(QDateTime const& ending_time)
    {
        ending_time_ = ending_time;
    }

    QDateTime const getRepetionGroupNumber() const
    {
        return repetition_group_number_;
    }
    void setRepetionGroupNumber(QDateTime const& value)
    {
        repetition_group_number_ = value;
    }


private:
    QString patrol_name_;
    int patrol_number_;
    QDateTime starting_time_, ending_time_;
    QDateTime repetition_group_number_;
};

inline bool operator<(TaskScheduled const& task1, TaskScheduled const& task2)
{
    return task1.getStartingTime() < task2.getStartingTime();
}
#endif // TASKSCHEDULED_H
