#ifndef GRAPHICSPATROLITEM_H
#define GRAPHICSPATROLITEM_H

#include <QGraphicsItem>
#include <QObject>

#include "ui/calendar/taskscheduled.h"
#include "core/appinstance.h"

class GraphicsPatrolItem : public QGraphicsObject
{
    Q_OBJECT
public:
    GraphicsPatrolItem(core::AppInstance &instance, QGraphicsItem *parent  = 0);
    ~GraphicsPatrolItem();
    typedef enum {NORMAL, SELECTED, OVER_TASK} color_enum;
    typedef enum {MODIFY_TASK, REMOVE_TASK, REMOVE_GROUP_TASK, REMOVE_SCENE_TASK, ADD_TASK} event_graphics_task;

    QVector<QGraphicsItem*> getPatrols_rect()
    {
        QVector<QGraphicsItem*> item;
        for(int i(0);i < patrol_rect_.size();++i)
        {
            item.append(patrol_rect_[i]);
        }
        return item;
    }

    void addPatrol(QVector<QRectF> pos, TaskScheduled task, color_enum status);
    void addMonthPatrol(QVector<QRectF> pos, TaskScheduled task, color_enum status);

    void setTaskScheduled(TaskScheduled task)
    {
        task_ = task;
    }

    TaskScheduled getTaskScheduled()
    {
        return task_;
    }
    virtual void removeHeader();
    virtual void removeRect();
signals :
    void removeFromScene(QGraphicsItem * item);
    void actionOnTaskRequired(GraphicsPatrolItem::event_graphics_task event,TaskScheduled task);
public:
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
    void updateGraphicsItem(color_enum is_selected);

protected :
    virtual QRectF boundingRect() const;
    virtual void paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private :
    QVector<QGraphicsRectItem*> patrol_rect_;
    QVector<QGraphicsTextItem*> patrol_name_header_;
    QImage current_color_;
    TaskScheduled task_;
    core::AppInstance &patrol_instance_;
    QRectF bounding_rect_;

    QColor getTextColor(QColor color);
    QColor getColor(color_enum is_selected);
};

#endif // GRAPHICSPATROLITEM_H
