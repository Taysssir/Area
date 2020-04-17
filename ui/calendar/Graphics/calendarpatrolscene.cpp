#include "calendarpatrolscene.h"

#include <QGraphicsSceneMouseEvent>

CalendarPatrolScene::CalendarPatrolScene(VISUAL_NEEDED visual_needed, core::AppInstance &instance, QObject *parent):CalendarBackgroundScene(visual_needed,instance,parent)
{
    selected_patrol_index = -1;
    over_patrol_index = -1;
    current_used_process_ = NORMAL_PROCESS;
    temporary_task = new GraphicsPatrolItem(instance);
    temporary_task->setTaskScheduled(TaskScheduled());
    emit taskSelected(false);
}

CalendarPatrolScene::~CalendarPatrolScene()
{

}

void CalendarPatrolScene::switchVisual(VISUAL_SELECTED visual)
{
    clearPatrols();
    CalendarBackgroundScene::switchVisual(visual);
}

void CalendarPatrolScene::shiftDate(SHIFT shift)
{
    clearPatrols();
    CalendarBackgroundScene::shiftDate(shift);
}

void CalendarPatrolScene::clearPatrols()
{
    for(auto a_task : graphic_patrols_)
    {
        if(a_task->scene() == this)
            this->removeItem(a_task);
        a_task->removeHeader();
        a_task->removeRect();
    }
    current_scene_item_->clearPatrolOffset();
    graphic_patrols_.clear();
}

void CalendarPatrolScene::clearGraphicsTask(GraphicsPatrolItem *task)
{
    task->removeHeader();
    task->removeRect();
}

void CalendarPatrolScene::updatePatrols(QVector<TaskScheduled> const& task)
{
    over_patrol_index =-1;
    selected_patrol_index= -1;
    emit taskSelected(false);
    clearPatrols();
    current_scene_item_->clearPatrolOffset();
    addPatrols(task);
}

void CalendarPatrolScene::addPatrols(QVector<TaskScheduled> const& task)
{
    current_scene_item_->clearPatrolOffset();
    for (auto a_task : task)
    {
        graphic_patrols_.append(new GraphicsPatrolItem(instance_));

        if ( this->getVisual() == CalendarPatrolScene::MONTH ) // Special patrol items when on month display
            graphic_patrols_.last()->addMonthPatrol(current_scene_item_->getRectFromQDateTimes(a_task.getStartingTime(),a_task.getEndingTime()),a_task,GraphicsPatrolItem::NORMAL);
        else
            graphic_patrols_.last()->addPatrol(current_scene_item_->getRectFromQDateTimes(a_task.getStartingTime(),a_task.getEndingTime()),a_task,GraphicsPatrolItem::NORMAL);

        connect(graphic_patrols_.last(),SIGNAL(removeFromScene(QGraphicsItem*)),this,SLOT(removeSceneItem(QGraphicsItem*)));
        connect(graphic_patrols_.last(),SIGNAL(actionOnTaskRequired(GraphicsPatrolItem::event_graphics_task,TaskScheduled)),
                this,SLOT(actionOnGraphicsPatrolItem(GraphicsPatrolItem::event_graphics_task,TaskScheduled)));
        this->addItem(graphic_patrols_.last());
    }
    time_line_->setZValue(99);
}

void CalendarPatrolScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    current_scene_item_->clearPatrolOffset();
    if(mouseEvent->button() != Qt::RightButton)
    {
        int patrol_index = verifyMousePosition(mouseEvent);

        if(patrol_index >= 0 )
        {
            graphic_patrols_[patrol_index]->mouseDoubleClickEvent(mouseEvent);
        }
    }
}

void CalendarPatrolScene::mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    current_scene_item_->clearPatrolOffset();
    switch(current_used_process_)
    {
    case NORMAL_PROCESS:
    {
        int patrol_index = verifyMousePosition(mouseEvent);

        if(patrol_index >= 0 )
        {
            graphic_patrols_[patrol_index]->mouseMoveEvent(mouseEvent);
        }
        if(over_patrol_index != -1 && patrol_index != over_patrol_index)
        {
            if(selected_patrol_index == over_patrol_index)
            {
                editItemColor(GraphicsPatrolItem::SELECTED,graphic_patrols_[over_patrol_index]);
            }
            else
            {
                editItemColor(GraphicsPatrolItem::NORMAL,graphic_patrols_[over_patrol_index]);
            }
        }
        patrol_index = verifyMousePosition(mouseEvent);
        over_patrol_index = patrol_index;
        break;
    }
    case CREATION_PROCESS:
    {
        mouse_create_stop = getDateTimeFromMousePosition(mouseEvent->scenePos());
        if(mouse_create_stop.isValid() && mouse_create_start != mouse_create_stop)
        {
            TaskScheduled task(temporary_task->getTaskScheduled());
            if(mouse_create_start< mouse_create_stop)
            {
                task.setStartingTime(mouse_create_start);
                task.setEndingTime(mouse_create_stop);
            }
            else
            {
                task.setStartingTime(mouse_create_stop);
                task.setEndingTime(mouse_create_start);
            }
            if(temporary_task->scene() == this)
                this->removeItem(temporary_task);
            clearGraphicsTask(temporary_task);
            temporary_task->addPatrol(current_scene_item_->getRectFromQDateTimes(task.getStartingTime(),task.getEndingTime()),task,GraphicsPatrolItem::NORMAL);
            this->addItem(temporary_task);
        }
        break;
    }
    case DRAG_DROP_PROCESS :
    {
        break;
    }
    }
}

void CalendarPatrolScene::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    int patrol_index = verifyMousePosition(mouseEvent);
    current_scene_item_->clearPatrolOffset();
    if(patrol_index >= 0 )
    {

        graphic_patrols_[patrol_index]->mousePressEvent(mouseEvent);
    }
    else
    {
        mouse_create_start = getDateTimeFromMousePosition(mouseEvent->scenePos());
        current_used_process_ =  CREATION_PROCESS;
    }
    if(patrol_index != selected_patrol_index)
    {
        int last = selected_patrol_index;
        selected_patrol_index = patrol_index;
        if(last >= 0 && graphic_patrols_.size() > 0 && last < graphic_patrols_.size())
        {
            editItemColor(GraphicsPatrolItem::NORMAL,graphic_patrols_[last]);
        }
        if(selected_patrol_index >= 0)
            emit taskSelected(true);
    }
    selected_patrol_index = patrol_index;
    if(patrol_index == -1)
        emit taskSelected(false);
}

void CalendarPatrolScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    current_scene_item_->clearPatrolOffset();
    switch(current_used_process_)
    {
    case NORMAL_PROCESS :
        break;
    case CREATION_PROCESS :
    {
        current_used_process_= NORMAL_PROCESS;
        mouse_create_stop = getDateTimeFromMousePosition(mouseEvent->scenePos());
        if(mouse_create_start.isValid() && mouse_create_stop.isValid())
            actionOnGraphicsPatrolItem(GraphicsPatrolItem::ADD_TASK,temporary_task->getTaskScheduled());
        if(temporary_task->scene() == this)
            this->removeItem(temporary_task);
        clearGraphicsTask(temporary_task);
        mouse_create_start = QDateTime();
        mouse_create_stop = QDateTime();
        temporary_task->setTaskScheduled(TaskScheduled());
        this->addItem(temporary_task);
        break;
    }
    case DRAG_DROP_PROCESS :
    {
        break;
    }
    }
}

int CalendarPatrolScene::verifyMousePosition(QGraphicsSceneMouseEvent * mouseEvent)
{
    QList<QGraphicsItem*> list = this->items(mouseEvent->scenePos());
    if(list.size() > 0 && !graphic_patrols_.isEmpty())
    {
        while(!list.isEmpty())
        {
            for(int i(0);i< graphic_patrols_.size();++i)
            {
                if(graphic_patrols_[i]->getPatrols_rect().contains(list.first()))
                {
                    return i;
                }
            }
            list.takeFirst();
        }
    }
    return -1;
}

void CalendarPatrolScene::editItemColor(GraphicsPatrolItem::color_enum color_enum, GraphicsPatrolItem *patrol)
{
    patrol->updateGraphicsItem(color_enum);
}

void CalendarPatrolScene::actionOnGraphicsPatrolItem(GraphicsPatrolItem::event_graphics_task event,TaskScheduled task)
{
    emit signalOnGraphicsTaskScheduled(event, task);
    if(event != GraphicsPatrolItem::ADD_TASK && event != GraphicsPatrolItem::MODIFY_TASK)
    {
        selected_patrol_index = -1;
        over_patrol_index = -1;
    }
}
