#include "daybackgrounditem.h"

#include <QSize>
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QPen>

DayBackgroundItem::DayBackgroundItem(core::AppInstance &instance, QGraphicsObject *parent):BackgroundItem(instance,parent)
{

}

DayBackgroundItem::~DayBackgroundItem()
{

}

void DayBackgroundItem::setBackgroundDesign(QRectF rect)
{
    removeBackground();

    column_size_ = QSize((rect.width()-time_column_size_.width()), header_size_.height() - 1);

    QGraphicsLineItem * item = new QGraphicsLineItem(time_column_size_.width(),0,
                                                     time_column_size_.width(),header_size_.height(),this);
    item->setPen(QPen(constants::calendar::LINE_STRONG_COLOR));
    background_item_.append(item);


    // background_item_.append(new QGraphicsRectItem(0,0,time_column_size_.width() + header_size_.width(),
    //                                               header_size_.height()+ time_column_size_.height()*48,this));
    QGraphicsRectItem * item2 = new QGraphicsRectItem(0,0,time_column_size_.width() + column_size_.width(),
                                                      header_size_.height()+ time_column_size_.height()*48,this);
    item2->setPen(constants::calendar::LINE_STRONG_COLOR);
    background_item_.append(item2);
    item2 = new QGraphicsRectItem(QRectF(0,header_size_.height(),time_column_size_.width(),
                                         48*time_column_size_.height()),this);
    item2->setPen(constants::calendar::LINE_STRONG_COLOR);
    background_item_.append(item2);

    item2 = new QGraphicsRectItem(QRectF(time_column_size_.width(),
                                         header_size_.height(), column_size_.width(),48*time_column_size_.height()),this);
    item2->setPen(constants::calendar::LINE_STRONG_COLOR);
    background_item_.append(item2);
    for(int i(0); i < 48; i++)
    {
        QGraphicsLineItem * item = new QGraphicsLineItem((time_column_size_.width()*(i%2)),
                                                         header_size_.height() + time_column_size_.height() * i,
                                                         time_column_size_.width()+column_size_.width(),
                                                         header_size_.height() + time_column_size_.height() * i,this);
        if(i % 2)
        {
            item->setPen(constants::calendar::LINE_LIGHT_COLOR);
        }
        else
        {
            item->setPen(constants::calendar::LINE_STRONG_COLOR);
        }
        background_item_.append(item);

    }
    QLine time_line;
    if(semantics_date_.contains(QDate::currentDate()))
    {
       /* item = new QGraphicsLineItem((time_column_size_.width()),
                                     header_size_.height() + time_column_size_.height() * ((QTime::currentTime().hour()*60+ QTime::currentTime().minute()))/30.0,
                                     time_column_size_.width()+column_size_.width(),
                                     header_size_.height() + time_column_size_.height() * ((QTime::currentTime().hour()*60+ QTime::currentTime().minute()))/30.0,this);
       */// item->setPen(QPen(QBrush(constants::calendar::LINE_CURRENT_DAY_COLOR),3));
      //  background_item_.append(item);
        time_line = QLine((time_column_size_.width()),
                                   header_size_.height() + time_column_size_.height() * ((QTime::currentTime().hour()*60+ QTime::currentTime().minute()))/30.0,
                                   time_column_size_.width()+column_size_.width(),
                                   header_size_.height() + time_column_size_.height() * ((QTime::currentTime().hour()*60+ QTime::currentTime().minute()))/30.0);
    }
    emit displayLineTime(time_line);
}

void DayBackgroundItem::setHeader(QDate date)
{
    if(date.isValid())
    {
        removeHeader();

        text_header_.append(new QGraphicsTextItem(background_instance_.getLocaleDate(date,"dddd d MMMM yyyy")));
        text_header_.first()->setPos(column_size_.width()/2,0);
        text_header_.first()->setDefaultTextColor(Qt::black);
        QFont font;
        font.setBold(true);
        text_header_.last()->setFont(font);
        text_header_.first()->setParentItem(this);

        for(int i(0); i < 24;i++)
        {
            text_header_.append(new QGraphicsTextItem(QString(QString::number(i)+":00")));//,"hh:mm")));
            text_header_.last()->setPos(5, header_size_.height() + time_column_size_.height()*i*2);
            text_header_.last()->setDefaultTextColor(Qt::black);
            text_header_.last()->setParentItem(this);
        }
        semantics_date_.clear();
        semantics_date_.append(date);
        this->update();
    }
}

QDateTime DayBackgroundItem::getDateTimeFromMousePosition(QPointF pos)
{
    if(pos.x() <= time_column_size_.width())
    {
        return QDateTime();
    }
    if(pos.y() <= header_size_.height())
    {
        return QDateTime();
    }
    QTime time(0,0);
    float second;
    second = ((pos.y() - header_size_.height() + 1)/(float)time_column_size_.height())*(60*60/2);
    if(second >= 60*60*24)
    {
        second = 60*60*24 -1;
    }
    time = time.addSecs(second);
    QDateTime date_time(semantics_date_.first(),time);
    return date_time;
}

QVector<QRectF> DayBackgroundItem::getRectFromQDateTimes(QDateTime start, QDateTime stop)
{
    QDateTime begin_date = fitinCalendarDisplayed(start,BEGIN);
    QDateTime end_date = fitinCalendarDisplayed(stop, END);
    if(begin_date.isValid() && end_date.isValid())
    {
    QVector<QRectF> rect_vector;

    float duration = (end_date.time().hour() - begin_date.time().hour()) +
            (end_date.time().minute() - begin_date.time().minute())/60.0;
    QRectF patrol_rect(time_column_size_.width()+1,
                       header_size_.height()
                       +time_column_size_.height()*(begin_date.time().minute()/60.0+begin_date.time().hour())*2,
                       column_size_.width()-1,
                       duration *time_column_size_.height()*2);
    rect_vector.append(patrol_rect);


    return rect_vector;
    }
    return QVector<QRectF>();
}
