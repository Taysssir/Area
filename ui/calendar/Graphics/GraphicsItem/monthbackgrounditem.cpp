#include "monthbackgrounditem.h"


#include <QSize>
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QPen>

MonthBackgroundItem::MonthBackgroundItem(core::AppInstance &instance, QGraphicsObject *parent):BackgroundItem(instance,parent)
{
    user_selected_date_ = QDate();
}

MonthBackgroundItem::~MonthBackgroundItem()
{

}
void MonthBackgroundItem::setBackgound(QRectF rect, QDate date)
{
    getSemanticsDisplayed(date);
    BackgroundItem::setBackgound(rect,date);
}

void MonthBackgroundItem::setBackgroundDesign(QRectF rect)
{
    removeBackground();
    column_size_ = QSize(rect.width()/constants::calendar::DAYS_IN_WEEK_,
                         (rect.height()-header_size_.height())/(semantics_date_.size()/constants::calendar::DAYS_IN_WEEK_));

    for(int j(0);j < constants::calendar::DAYS_IN_WEEK_;j++)
    {
        QGraphicsRectItem * item = new QGraphicsRectItem(QRectF(j*column_size_.width(),0,column_size_.width(),header_size_.height()),this);
        item->setPen(constants::calendar::LINE_STRONG_COLOR);
        background_item_.append(item);
    }

    for(int j(0);j < constants::calendar::DAYS_IN_WEEK_;j++)
    {
        for(int i(0);i < (semantics_date_.size()/constants::calendar::DAYS_IN_WEEK_);i++)
        {
            if(semantics_date_[(i)*constants::calendar::DAYS_IN_WEEK_ + j] == QDate::currentDate())
            {
                QGraphicsRectItem * item = new QGraphicsRectItem(QRectF(column_size_.width()*j+1,
                                                                        header_size_.height()+ column_size_.height() * i+1,
                                                                        column_size_.width()-2,
                                                                        column_size_.height()-2),this);
                item->setPen(constants::calendar::LINE_CURRENT_DAY_COLOR);
                item->setBrush(constants::calendar::BACKGROUND_CURRENT_DAY_COLOR);
                background_item_.append(item);
            }
            else if(semantics_date_[(i)*constants::calendar::DAYS_IN_WEEK_ + j] == user_selected_date_)
            {
                QGraphicsRectItem * item = new QGraphicsRectItem(QRectF(column_size_.width()*j+1,
                                                                        header_size_.height()+ column_size_.height() * i+1,
                                                                        column_size_.width()-2,
                                                                        column_size_.height()-2),this);
                item->setPen(constants::calendar::LINE_SELECTED_DAY_COLOR);
                item->setBrush(constants::calendar::BACKGROUND_SELECTED_DAY_COLOR);
                background_item_.append(item);
            }
            else
            {
                QGraphicsRectItem * item = new QGraphicsRectItem(QRectF(column_size_.width()*j,header_size_.height()+ column_size_.height() * i,
                                                                        column_size_.width(), column_size_.height()),this);
                item->setPen(constants::calendar::LINE_STRONG_COLOR);
                background_item_.append(item);
            }
        }
    }
    emit displayLineTime(QLine());
}

void MonthBackgroundItem::setBackgroundOnSelectedDate(QVector<QDate> date_vector)
{
    if(date_vector.size() <= 0)
        return;
    if(date_vector.contains(QDate()))
        return;

    int first_semantics_index = -1, last_semantics_index = -1;
    for(auto date : date_vector)
    {
        if(first_semantics_index == -1)
        {
            first_semantics_index = semantics_date_.indexOf(date);
        }
        if(semantics_date_.indexOf(date) > last_semantics_index || last_semantics_index == -1)
        {
            last_semantics_index = semantics_date_.indexOf(date);
        }
    }

    for(auto item : display_date_graphics_)
    {
        emit removeFromScene(item);
    }
    display_date_graphics_.clear();

    for(int j(0);j < constants::calendar::DAYS_IN_WEEK_;j++)
    {
        for(int i(0);i < (semantics_date_.size()/constants::calendar::DAYS_IN_WEEK_);i++)
        {
            if( first_semantics_index <= (i*constants::calendar::DAYS_IN_WEEK_)+j && (i*constants::calendar::DAYS_IN_WEEK_)+j <= last_semantics_index)
            {
                QGraphicsRectItem * item = new QGraphicsRectItem(QRectF(column_size_.width()*j,header_size_.height()+ column_size_.height() * i,
                                                                        column_size_.width(), column_size_.height()),this);
                item->setPen(constants::calendar::LINE_STRONG_COLOR);
                item->setBrush(constants::calendar::BACKGROUND_CURRENT_DAY_COLOR);
                display_date_graphics_.append(item);
            }
            if(semantics_date_[(i)*constants::calendar::DAYS_IN_WEEK_ + j] == QDate::currentDate())
            {
                QGraphicsRectItem * item = new QGraphicsRectItem(QRectF(column_size_.width()*j,header_size_.height()+ column_size_.height() * i,
                                                                        column_size_.width()-1, column_size_.height()-1),this);
                item->setPen(constants::calendar::LINE_CURRENT_DAY_COLOR);
                item->setBrush(constants::calendar::BACKGROUND_CURRENT_DAY_COLOR);
                display_date_graphics_.append(item);
            }
        }
    }
}
void MonthBackgroundItem::setHeader(QDate date)
{
    if(date.isValid())
    {
        QDate cursor_date = QDate(date.year(),date.month(),1);
        cursor_date = cursor_date.addDays(-cursor_date.dayOfWeek() +1);

        removeHeader();

        for (int i(0);i< constants::calendar::DAYS_IN_WEEK_;++i)
        {

            if(column_size_.width() < 40)
            {
                text_header_.append(new QGraphicsTextItem(background_instance_.getLocaleDate(QDateTime(semantics_date_[i]),"dddd").left(1)));
            }
            else if( column_size_.width() > 70)
            {
                text_header_.append(new QGraphicsTextItem(background_instance_.getLocaleDate(QDateTime(semantics_date_[i]),"dddd")));
            }
            else
                text_header_.append(new QGraphicsTextItem(background_instance_.getLocaleDate(QDateTime(semantics_date_[i]),"ddd.")));
            text_header_.last()->setPos((column_size_.width()-text_header_.last()->boundingRect().width())/2 + (column_size_.width())*i,0);
            text_header_.last()->setDefaultTextColor(Qt::black);
            QFont font;
            font.setBold(true);
            text_header_.last()->setFont(font);
            text_header_.last()->setParentItem(this);
        }

        for(auto display_date : semantics_date_)
        {
            int week_count =  (semantics_date_.indexOf(display_date))/constants::calendar::DAYS_IN_WEEK_;
            if(display_date.day() != 1 || column_size_.width() < 40)
                text_header_.append(new QGraphicsTextItem(background_instance_.getLocaleDate(QDateTime(display_date),"d")));
            else
                text_header_.append(new QGraphicsTextItem(QString::number(display_date.day())
                                                          + " " + background_instance_.getLocaleDate(QDateTime(display_date),"MMM. yyyy")));
            text_header_.last()->setPos(5 + (display_date.dayOfWeek()-1)* column_size_.width() ,
                                        header_size_.height() + column_size_.height()*week_count);
            if(display_date.month() == date.month())
                text_header_.last()->setDefaultTextColor(Qt::black);
            else
                text_header_.last()->setDefaultTextColor(QColor(125,125,125));
            text_header_.last()->setParentItem(this);
        }

        this->update();
    }
}

void MonthBackgroundItem::getSemanticsDisplayed(QDate const& date)
{
    semantics_date_.clear();
    patrol_offset_.clear();
    QDate cursor_date = QDate(date.year(),date.month(),1);
    cursor_date = cursor_date.addDays(-cursor_date.dayOfWeek() +1);
    int daysInMonth;
    if(cursor_date.month() != date.month())
        daysInMonth = date.daysInMonth() + (cursor_date.daysInMonth() - cursor_date.day() + 1);
    else
        daysInMonth = date.daysInMonth();

    while(semantics_date_.size()%constants::calendar::DAYS_IN_WEEK_ != 0 || semantics_date_.size() < daysInMonth)
    {
        semantics_date_.append(cursor_date);
        patrol_offset_.append(0);
        cursor_date = cursor_date.addDays(1);
    }
}

QDate MonthBackgroundItem::getSelectedDate() const
{
    return user_selected_date_;
}

void MonthBackgroundItem::setSelectedDate(const QDate &selectedDate)
{
    user_selected_date_ = selectedDate;
}

QDateTime MonthBackgroundItem::getDateTimeFromMousePosition(QPointF pos)
{
    if(pos.y() <= header_size_.height())
    {
        return QDateTime();
    }
    QTime time(0,0);
    int week_count = ((pos.y() - header_size_.height())/(column_size_.height()));
    QDate date;
    int week_day = ((pos.x())/ (float)column_size_.width());
    int week = (week_count*constants::calendar::DAYS_IN_WEEK_) + week_day;
    if(week >= semantics_date_.size())
        week = semantics_date_.size()- 1;
    date = semantics_date_[week];
    QDateTime date_time(date,time);
    return date_time;
}

QVector<QRectF> MonthBackgroundItem::getRectFromQDateTimes(QDateTime start, QDateTime stop)
{
    QDateTime begin_date = fitinCalendarDisplayed(start,BEGIN);
    QDateTime end_date = fitinCalendarDisplayed(stop, END);
    if(begin_date.isValid() && end_date.isValid())
    {
        QDateTime day_starting_time, day_ending_time;

        QVector<QRectF> rect_vector;

        QDate selected_day = begin_date.date();
        int i(0);

        do
        {
            day_starting_time = fitinCalendarDisplayed(start,BEGIN,QDateTime(selected_day.addDays(i),QTime(0,0)));
            day_ending_time = fitinCalendarDisplayed(stop,END,QDateTime(selected_day.addDays(i),QTime(23,59)));
            if(day_starting_time.isValid() && day_starting_time.isValid())
            {
                int number_day = semantics_date_.indexOf(day_starting_time.date());
                if(number_day < 0 || number_day >= semantics_date_.size())
                {
                    break;
                }
                ++patrol_offset_[number_day];
                int number_week = number_day/constants::calendar::DAYS_IN_WEEK_;
                QPoint xy;
                int iOffset = (patrol_offset_[number_day] > 1)?(patrol_offset_[number_day]-1)*20:0;

                xy = QPoint(5 + column_size_.width()*(day_ending_time.date().dayOfWeek()-1) + iOffset,
                            header_size_.height() + number_week * column_size_.height() + time_column_size_.height()*2);

                QRectF patrol_rect(xy.x(),xy.y(),15,15);

                rect_vector.append(patrol_rect);
            }
            ++i;
        }
        while(i <= begin_date.date().daysTo(end_date.date()));
        return rect_vector;
    }
    return QVector<QRectF>();
}

int MonthBackgroundItem::getRequiredHeight()
{
    return header_size_.height() + (semantics_date_.size()/constants::calendar::DAYS_IN_WEEK_)*column_size_.height();
}

void MonthBackgroundItem::clearPatrolOffset()
{
    for(int i(0); i < patrol_offset_.size();++i)
    {
        patrol_offset_[i] = 0;
    }
}
