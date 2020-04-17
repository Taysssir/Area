#ifndef CALENDARPATROLSCENE_H
#define CALENDARPATROLSCENE_H

#include <QObject>

#include "calendarbackgroundscene.h"
#include "GraphicsItem/graphicspatrolitem.h"
#include "../tasksscheduled.h"

class CalendarPatrolScene : public CalendarBackgroundScene
{
    Q_OBJECT
public:
    CalendarPatrolScene(VISUAL_NEEDED visual_needed, core::AppInstance &instance, QObject *parent = 0);
    ~CalendarPatrolScene();

    typedef enum {
        CREATION_PROCESS,
        NORMAL_PROCESS,
        DRAG_DROP_PROCESS
    } scene_usage_process;

    void clearPatrols();
    void clearGraphicsTask(GraphicsPatrolItem *task);
    void updatePatrols(QVector<TaskScheduled> const& task);
    void addPatrols(QVector<TaskScheduled> const& task);

    virtual void switchVisual(VISUAL_SELECTED visual);
    virtual void shiftDate(SHIFT shift);
    TaskScheduled getSelectedPatrol()
    {
        if(selected_patrol_index != -1)
        {
            return graphic_patrols_[selected_patrol_index]->getTaskScheduled();
        }
        return TaskScheduled();
    }

signals :
    void signalOnGraphicsTaskScheduled(GraphicsPatrolItem::event_graphics_task event, TaskScheduled task);
    void taskSelected(bool);
public slots :
    virtual void actionOnGraphicsPatrolItem(GraphicsPatrolItem::event_graphics_task event,TaskScheduled task);

protected:
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * mouseEvent);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent);

private :
    // usage of the scene

    scene_usage_process current_used_process_;
    QDateTime mouse_create_start, mouse_create_stop;
    int selected_patrol_index, over_patrol_index;
    GraphicsPatrolItem* temporary_task;

    // patrol
    QVector<GraphicsPatrolItem*> graphic_patrols_;

    virtual int verifyMousePosition(QGraphicsSceneMouseEvent * mouseEvent);
    void editItemColor(GraphicsPatrolItem::color_enum color_enum, GraphicsPatrolItem *patrol);
};

#endif // CALENDARPATROLSCENE_H
