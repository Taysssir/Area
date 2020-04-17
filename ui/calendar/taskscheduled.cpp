#include "taskscheduled.h"

TaskScheduled::TaskScheduled(const QString &name, int number, const QDateTime &start_time, const QDateTime &end_time)
{
    patrol_number_ = number;
    patrol_name_ = name;
    starting_time_ = start_time;
    ending_time_ = end_time;
}

TaskScheduled::TaskScheduled(QString const& name, int number, QDateTime const& start_time, QDateTime const& end_time, QDateTime const& repetition_is_on)
{
    patrol_number_ = number;
    patrol_name_ = name;
    starting_time_ = start_time;
    ending_time_ = end_time;
    repetition_group_number_ = repetition_is_on;
}

TaskScheduled::~TaskScheduled()
{
    patrol_number_ = 0;
}

