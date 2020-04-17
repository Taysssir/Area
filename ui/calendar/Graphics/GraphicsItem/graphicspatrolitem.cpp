#include "graphicspatrolitem.h"

#include <QPen>
#include <QBrush>
#include <QAction>
#include <QMenu>
#include <QGraphicsSceneMouseEvent>
#include <QRgb>
#include "../../contants_calendar.h"

GraphicsPatrolItem::GraphicsPatrolItem(core::AppInstance &instance, QGraphicsItem *parent)
    : QGraphicsObject(parent)
    , patrol_instance_(instance)
{

}

GraphicsPatrolItem::~GraphicsPatrolItem()
{
    removeHeader();
    removeRect();
}

QColor GraphicsPatrolItem::getColor(color_enum is_selected)
{
    QImage image;
    QPoint pixel(0,0);
    switch(is_selected)
    {
    case OVER_TASK:
        image = current_color_;
    case NORMAL :
        if(is_selected == NORMAL)
            image = QImage(":/calendar/Resources/calendar/patrol_color.png");
        current_color_ = image;
        if(task_.getPatrolNumber() > 0)

            pixel = QPoint(((task_.getPatrolNumber()-1)%50)*image.width()/50 , image.height()/2);
        if(task_.getPatrolNumber() == 0)
        {
            image = QImage(":/calendar/Resources/calendar/patrol_selected_color.png");
            pixel = QPoint(300,0);
        }
        break;
    case SELECTED :
        image = QImage(":/calendar/Resources/calendar/patrol_selected_color.png");
        current_color_ = image;
        break;
    }

    QColor color(image.pixel(pixel));
    switch(is_selected)
    {
    case NORMAL :
        color.setAlpha(150);
        break;
    case OVER_TASK:
        color.setAlpha(210);
        break;
    case SELECTED :
        color.setAlpha(150);
        break;
    }

    return color;
}

QColor GraphicsPatrolItem::getTextColor(QColor color)
{
    QColor text_color(Qt::black);
    if(qGray(color.rgb()) < constants::calendar::GRAY_THRESHOLD)
        text_color = QColor(Qt::white);
    return text_color;
}

void GraphicsPatrolItem::addPatrol(QVector<QRectF> pos, TaskScheduled task,color_enum status )
{
    if(pos.isEmpty())
        return;
    task_ = task;
    QColor color = getColor(status);

    QPointF top_left(pos.first().topLeft()), bottom_right(pos.last().bottomRight());
    for(auto rect : pos)
    {
        if(rect.topLeft().x() < top_left.x())
            top_left.setX(rect.topLeft().x());
        if(rect.topLeft().y() < top_left.y())
            top_left.setY(rect.topLeft().y());
        if(rect.bottomRight().x() > bottom_right.x())
            bottom_right.setX(rect.bottomRight().x());
        if(rect.bottomRight().y() > bottom_right.y())
            bottom_right.setY(rect.bottomRight().y());
        QGraphicsRectItem* item = new QGraphicsRectItem(rect,this);
        item->setPen(QPen(color));
        item->setBrush(QBrush(color));
        patrol_rect_.append(item);
        QGraphicsTextItem* text_item = new QGraphicsTextItem(task_.getPatrolName(),this);
        text_item->setDefaultTextColor(getTextColor(color));
        text_item->setPos(rect.x() + 5 , rect.y() + 5);
        QFont font;
        font.setBold(true);
        text_item->setFont(font);
        patrol_name_header_.append(text_item);
    }
    bounding_rect_ = QRectF(top_left,bottom_right);
}

void GraphicsPatrolItem::addMonthPatrol(QVector<QRectF> pos, TaskScheduled task, GraphicsPatrolItem::color_enum status)
{
    if(pos.isEmpty())
        return;
    task_ = task;
    QColor color = getColor(status);

    QPointF top_left(pos.first().topLeft()), bottom_right(pos.last().bottomRight());
    for(auto rect : pos)
    {
        if(rect.topLeft().x() < top_left.x())
            top_left.setX(rect.topLeft().x());
        if(rect.topLeft().y() < top_left.y())
            top_left.setY(rect.topLeft().y());
        if(rect.bottomRight().x() > bottom_right.x())
            bottom_right.setX(rect.bottomRight().x());
        if(rect.bottomRight().y() > bottom_right.y())
            bottom_right.setY(rect.bottomRight().y());
        QGraphicsRectItem* item = new QGraphicsRectItem(rect,this);
        item->setPen(QPen(color));
        item->setBrush(QBrush(color));
        item->setToolTip(task_.getPatrolName());
        patrol_rect_.append(item);
    }
    bounding_rect_ = QRectF(top_left,bottom_right);
}

QRectF GraphicsPatrolItem::boundingRect() const
{
    return bounding_rect_;
}

void GraphicsPatrolItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    for(auto item : patrol_rect_)
    {
        item->paint(painter,option,widget);
    }
    for(auto item : patrol_name_header_)
    {
        item->paint(painter,option,widget);
    }
}

void GraphicsPatrolItem::removeHeader()
{
    for(auto item : patrol_name_header_)
    {
        item->setParentItem(NULL);
        emit removeFromScene(item);
    }
    patrol_name_header_.clear();
}

void GraphicsPatrolItem::removeRect()
{
    for(auto item : patrol_rect_)
    {
        item->setParentItem(NULL);
        emit removeFromScene(item);
    }
    patrol_rect_.clear();
}

void GraphicsPatrolItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *)
{
    emit actionOnTaskRequired(MODIFY_TASK, task_);
}

void GraphicsPatrolItem::mouseMoveEvent(QGraphicsSceneMouseEvent *)
{
    updateGraphicsItem(OVER_TASK);
}

void GraphicsPatrolItem::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    updateGraphicsItem(SELECTED);
    if(mouseEvent->button() == Qt::RightButton)
    {
        QMenu menu;
        QAction action1(menu.parent());
        QAction action2(menu.parent());
        QAction action3(menu.parent());

        action1.setText(tr("Remove this task"));
        menu.addAction(&action1);
        if(task_.getRepetionGroupNumber().isValid())
        {
            action2.setText(tr("Remove the whole group of task"));
            menu.addAction(&action2);
        }
        action3.setText(tr("Remove all the scheduled tasks"));
        menu.addAction(&action3);
        menu.setGeometry(mouseEvent->screenPos().x(),mouseEvent->screenPos().y(),menu.geometry().width(),menu.geometry().height());
        QAction* choosen_action = menu.exec();
        if(choosen_action != nullptr)
        {
            if(choosen_action->text() == action1.text())
            {
                emit actionOnTaskRequired(REMOVE_TASK, task_);
            }
            else if(choosen_action->text() == action2.text())
            {
                emit actionOnTaskRequired(REMOVE_GROUP_TASK, task_);
            }
            else if(choosen_action->text() == action3.text())
            {
                emit actionOnTaskRequired(REMOVE_SCENE_TASK, task_);
            }
        }
    }
}

void GraphicsPatrolItem::mouseReleaseEvent(QGraphicsSceneMouseEvent * )
{

}

void GraphicsPatrolItem::updateGraphicsItem(color_enum is_selected)
{
    QColor color;
    color = getColor(is_selected);

    for(auto rect : patrol_rect_)
    {
        rect->setBrush(color);
        rect->setPen(color);
    }

}
