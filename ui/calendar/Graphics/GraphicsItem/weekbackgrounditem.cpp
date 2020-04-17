#include "weekbackgrounditem.h"


#include <QSize>
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QPen>

WeekBackgroundItem::WeekBackgroundItem(core::AppInstance &instance, QGraphicsObject *parent):BackgroundItem(instance,parent)
{

}

WeekBackgroundItem::~WeekBackgroundItem()
{

}

void WeekBackgroundItem::setBackgroundDesign(QRectF rect)
{
    removeBackground();

    column_size_ = QSize((rect.width()-time_column_size_.width())/(float)constants::calendar::DAYS_IN_WEEK_, header_size_.height() - 1);

    for(int j(0);j < constants::calendar::DAYS_IN_WEEK_;j++)
    {
        QGraphicsLineItem * item = new QGraphicsLineItem(j*column_size_.width() + time_column_size_.width(),0,
                                                         j*column_size_.width() + time_column_size_.width(),header_size_.height(),this);
        item->setPen(QPen(constants::calendar::LINE_STRONG_COLOR));
        background_item_.append(item);

    }
    background_item_.append(new QGraphicsRectItem(0,0,time_column_size_.width() + column_size_.width()*constants::calendar::DAYS_IN_WEEK_,
                                                  header_size_.height()+ time_column_size_.height()*48,this));
    background_item_.append(new QGraphicsRectItem(QRectF(0,header_size_.height(),time_column_size_.width(),
                                                         48*time_column_size_.height()),this));
    for(int j(0);j < constants::calendar::DAYS_IN_WEEK_;j++)
    {
        QGraphicsRectItem * item = new QGraphicsRectItem(QRectF(time_column_size_.width()+ column_size_.width()*j,
                                                                header_size_.height(), column_size_.width(),48*time_column_size_.height()),this);
        item->setPen(constants::calendar::LINE_STRONG_COLOR);
        background_item_.append(item);
        for(int i(0); i < 48; i++)
        {
            QGraphicsLineItem * item = new QGraphicsLineItem((time_column_size_.width()*(i%2)) + column_size_.width()*j ,
                                                             header_size_.height() + time_column_size_.height() * i,
                                                             column_size_.width()*(j+1) + time_column_size_.width(),
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
    }
    QLine time_line;
    if(semantics_date_.contains(QDate::currentDate()))
    {
        QGraphicsRectItem * item_rect = new QGraphicsRectItem(QRectF((time_column_size_.width())+column_size_.width()*(QDate::currentDate().dayOfWeek()-1)+1,
                                                                     header_size_.height()+ 1,
                                                                     column_size_.width()-2,
                                                                     time_column_size_.height() * ((QTime::currentTime().hour()*60+ QTime::currentTime().minute())-1)-2),this);
        item_rect->setPen(constants::calendar::LINE_CURRENT_DAY_COLOR);
        item_rect->setBrush(constants::calendar::BACKGROUND_CURRENT_DAY_COLOR);
        background_item_.append(item_rect);
        //QGraphicsLineItem * item = new QGraphicsLineItem(this);
        //item->setPen(QPen(QBrush(constants::calendar::LINE_CURRENT_DAY_COLOR),3));
        //item->setZValue(9999);
        time_line = QLine((time_column_size_.width())+column_size_.width()*(QDate::currentDate().dayOfWeek()-1),
                             header_size_.height() + time_column_size_.height() * ((QTime::currentTime().hour()*60+ QTime::currentTime().minute()))/30.0,
                             time_column_size_.width()+column_size_.width()*(QDate::currentDate().dayOfWeek()),
                             header_size_.height() + time_column_size_.height() * ((QTime::currentTime().hour()*60+ QTime::currentTime().minute()))/30.0);
       // background_item_.append(item);
       // background_item_.last()->setZValue(9999);
    }
    emit displayLineTime(time_line);
}

void WeekBackgroundItem::setHeader(QDate date)
{
    if(date.isValid())
    {
        semantics_date_.clear();

        QDate cursor_date(date);
        cursor_date = cursor_date.addDays(-date.dayOfWeek()+1);

        removeHeader();

        for (int i(0);i< constants::calendar::DAYS_IN_WEEK_;++i)
        {
            semantics_date_.append(cursor_date);
            text_header_.append(new QGraphicsTextItem(background_instance_.getLocaleDate(cursor_date,"ddd. d/M")));
            text_header_.last()->setPos(time_column_size_.width() + (column_size_.width()-text_header_.last()->boundingRect().width())/2 + (column_size_.width())*i,0);
            text_header_.last()->setDefaultTextColor(Qt::black);
            QFont font;
            font.setBold(true);
            text_header_.last()->setFont(font);
            cursor_date = cursor_date.addDays(1);
            text_header_.last()->setParentItem(this);
        }

        for(int i(0); i < 24;i++)
        {
            text_header_.append(new QGraphicsTextItem(QString(QString::number(i)+":00")));//background_instance_.getLocaleDate(QTime(i,0),"hh:mm")));
            text_header_.last()->setPos(5, header_size_.height() + time_column_size_.height()*i*2);
            text_header_.last()->setDefaultTextColor(Qt::black);
            text_header_.last()->setParentItem(this);
        }
    }
}

QDateTime WeekBackgroundItem::getDateTimeFromMousePosition(QPointF pos)
{
    if(pos.x() <= time_column_size_.width())
    {
        return QDateTime(semantics_date_[0],QTime(0,0));
    }
    if(pos.y() <= header_size_.height())
    {
        return QDateTime(semantics_date_[0],QTime(0,0));
    }
    QTime time(0,0);
    float second;
    second = ((pos.y() - header_size_.height() + 1)/(float)time_column_size_.height())*(60*60/2);
    if(second >= 60*60*24)
    {
        second = 60*60*24 -1;
    }
    time = time.addSecs(second);
    QDate date;
    int week_day = ((pos.x() - time_column_size_.width() + 1)/ (float)column_size_.width());
    if( week_day >= constants::calendar::DAYS_IN_WEEK_ )
        week_day = 6;
    date = semantics_date_[week_day];
    QDateTime date_time(date,time);
    return date_time;
}

QVector<QRectF> WeekBackgroundItem::getRectFromQDateTimes(QDateTime start, QDateTime stop)
{
    QDateTime begin_date = fitinCalendarDisplayed(start,BEGIN);
    QDateTime end_date = fitinCalendarDisplayed(stop, END);
    if(begin_date.isValid() && end_date.isValid())
    {
        QDateTime day_starting_time, day_ending_time;
        float duration;
        QVector<QRectF> rect_vector;

        QDate selected_day = begin_date.date();
        int i(0);

        do
        {
            day_starting_time = fitinCalendarDisplayed(begin_date ,BEGIN,QDateTime(selected_day.addDays(i),QTime(0,0)));
            day_ending_time = fitinCalendarDisplayed(end_date,END,QDateTime(selected_day.addDays(i),QTime(23,59)));
            if(day_starting_time.isValid() && day_starting_time.isValid())
            {
                duration = (day_ending_time.time().hour() - day_starting_time.time().hour()) +
                        (day_ending_time.time().minute() - day_starting_time.time().minute())/60.0;
                QRectF patrol_rect(time_column_size_.width()+1+column_size_.width()*(day_ending_time.date().dayOfWeek()-1),
                                   header_size_.height() + time_column_size_.height()*(day_starting_time.time().minute()/60.0+day_starting_time.time().hour())*2,
                                   column_size_.width()-1,
                                   duration *time_column_size_.height()*2);
                rect_vector.append(patrol_rect);
            }
            ++i;
        }
        while(i <= begin_date.date().daysTo(end_date.date()));
        return rect_vector;
    }
    return QVector<QRectF>();
}

void WeekBackgroundItem::getSemanticsDisplayed(QDate const& date)
{
    if(date.isValid())
    {
        semantics_date_.clear();

        QDate cursor_date(date);
        cursor_date = cursor_date.addDays(-date.dayOfWeek()+1);

        for (int i(0);i< constants::calendar::DAYS_IN_WEEK_;++i)
        {
            semantics_date_.append(cursor_date);
            cursor_date = cursor_date.addDays(1);
        }
    }
}
