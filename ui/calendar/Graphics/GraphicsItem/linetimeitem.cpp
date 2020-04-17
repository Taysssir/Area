#include "linetimeitem.h"
#include <QPen>
#include <QBrush>

#include "../../contants_calendar.h"

TimeLineItem::TimeLineItem(QGraphicsItem *parent)    : QGraphicsObject(parent)
{
    time_line_ = new QGraphicsLineItem(QLine(),this);
    is_displayed_ = false;
}

void TimeLineItem::setLine(const QLine &line)
{
    if(line == QLine())
    {
        if(is_displayed_)
        {
            emit removeFromScene(time_line_);
        }
        is_displayed_ = false;
        time_line_= new QGraphicsLineItem(line,this);
    }
    else
    {
        if(is_displayed_ == true)
        {
            emit removeFromScene(time_line_);
        }
        time_line_= new QGraphicsLineItem(line,this);
        time_line_->setPen(QPen(QBrush(constants::calendar::LINE_CURRENT_DAY_COLOR),3));
        is_displayed_ = true;
    }
}
