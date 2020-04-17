#include "tasksscheduled.h"
#include <QtAlgorithms>

TasksScheduled::TasksScheduled()
{

}

TasksScheduled::~TasksScheduled()
{

}

int TasksScheduled::addTasks(QString const& name, int patrol_number, QVector<QDateTime> const& time_start, QVector<QDateTime> const& time_end)
{
    if(time_start.size() == 1)
    {
        if(!addTask(name,patrol_number,time_start.first(),time_end.first()))
        {
            return 0;
        }
    }
    else
    {
        for(int i(0);i< time_start.size();++i)
        {
            if(!addTask(name,patrol_number,time_start[i],time_end[i],time_start.first()))
            {
                return i;
            }
        }
    }
    return time_start.size();
}

bool TasksScheduled::addTask(QString const& name, int patrol_number, QDateTime const& time_start, QDateTime const& time_end)
{
    if(isFree(time_start,time_end))
    {
        tasks_vector_.append(TaskScheduled(name,patrol_number,time_start,time_end));
        sort();
        return true;
    }
    return false;
}

bool TasksScheduled::addTask(QString const& name, int patrol_number, QDateTime const& time_start, QDateTime const& time_end, QDateTime const& first)
{
    if(isFree(time_start,time_end))
    {
        tasks_vector_.append(TaskScheduled(name,patrol_number,time_start,time_end,first));
        sort();
        return true;
    }
    return false;
}

bool TasksScheduled::isFree(QDateTime const& time_start, QDateTime const& time_end, int index_non_used) const
{
    if(tasks_vector_.isEmpty())
        return true;
    if(scheduledPatrol(time_start,time_end,index_non_used).isEmpty())
        return true;
    return false;
}

QVector<TaskScheduled> TasksScheduled::scheduledPatrol(QDateTime const& time_start, QDateTime const& time_end, int index_non_verified) const
{
    QVector<TaskScheduled> temp;
    for(int i(0); i<tasks_vector_.size();++i)
    {
        if(i != index_non_verified)
        {
            if(tasks_vector_[i].getStartingTime() <= time_start && time_start <= tasks_vector_[i].getEndingTime())
            {
                temp.append(tasks_vector_[i]);
            }
            else if(time_start <= tasks_vector_[i].getStartingTime() &&  tasks_vector_[i].getEndingTime() <= time_end)
            {
                temp.append(tasks_vector_[i]);
            }
            else if(tasks_vector_[i].getStartingTime() <= time_end && time_end <= tasks_vector_[i].getEndingTime())
            {
                temp.append(tasks_vector_[i]);
            }
            else if(tasks_vector_[i].getStartingTime() <= time_start && time_end <= tasks_vector_[i].getEndingTime())
            {
                temp.append(tasks_vector_[i]);
            }
        }
    }
    return temp;
}

void TasksScheduled::sort()
{
    qSort(tasks_vector_);
}

void TasksScheduled::removeTask(TaskScheduled const& task)
{
    for(int i(0);i < tasks_vector_.size();++i)
    {
        if(tasks_vector_[i].getStartingTime() == task.getStartingTime() && tasks_vector_[i].getEndingTime() == task.getEndingTime())
        {
            tasks_vector_.remove(i);
            return;
        }
    }
}

void TasksScheduled::removeGroupTask(TaskScheduled const& task)
{
    if(!tasks_vector_.size())
    {
        return;
    }
    int i(0);
    while(i < tasks_vector_.size())
    {
        if(tasks_vector_[i].getRepetionGroupNumber() == task.getRepetionGroupNumber())
        {
            tasks_vector_.remove(i);
        }
        else
            ++i;
    }
}


void TasksScheduled::removeAllTask()
{
    tasks_vector_.clear();
}

bool TasksScheduled::updateTask(TaskScheduled const& task, QString const& name, int patrol_number, QDateTime const& time_start, QDateTime const& time_end)
{
    for(int i(0);i < tasks_vector_.size();++i)
    {
        if(tasks_vector_[i].getStartingTime() == task.getStartingTime() && tasks_vector_[i].getEndingTime() == task.getEndingTime())
        {
            if(isFree(time_start,time_end,i))
            {
                tasks_vector_[i].setPatrolName(name);
                tasks_vector_[i].setPatrolNumber(patrol_number);
                tasks_vector_[i].setStartingTime(time_start);
                tasks_vector_[i].setEndingTime(time_end);
                tasks_vector_[i].setRepetionGroupNumber(QDateTime());
                return true;
            }
            return false;
        }
    }
    return false;
}
