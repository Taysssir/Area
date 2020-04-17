#include "backgrounditem.h"

BackgroundItem::BackgroundItem(core::AppInstance &instance, QGraphicsItem *parent)
    : QGraphicsObject(parent)
    , time_column_size_(60,15)
    , header_size_(200,30)
    , background_instance_(instance)
{
}

BackgroundItem::~BackgroundItem()
{

}

void BackgroundItem::setBackgound(QRectF rect, QDate date)
{
    if(rect != QRect())
    {
        bounding_rect_= rect;
    }
    getSemanticsDisplayed(date);
    setBackgroundDesign(bounding_rect_);
    setHeader(date);
    emit newDisplayedDate(semantics_date_);
}

void BackgroundItem::removeHeader()
{
    for(auto item : text_header_)
    {
        emit removeFromScene(item);
    }
    text_header_.clear();
}

void BackgroundItem::removeBackground()
{
    for(auto item : background_item_)
    {
        emit removeFromScene(item);
    }
    background_item_.clear();
}

QRectF BackgroundItem::boundingRect() const
{
    return bounding_rect_;
}

QDateTime BackgroundItem::fitinCalendarDisplayed(QDateTime date, FITIN_OPTION option, QDateTime special_selected_day)
{
    if(date.isValid())
    {

        QDateTime displayed_date = date;
        switch (option) {
        case BEGIN:
            if(!special_selected_day.isValid())
                special_selected_day = QDateTime(semantics_date_.first(),QTime(0,0));
            if(date < special_selected_day)
                displayed_date = special_selected_day;
            break;
        case END:
            if(!special_selected_day.isValid())
                special_selected_day = QDateTime(semantics_date_.last(),QTime(23,59));
            if(special_selected_day < date)
                displayed_date = special_selected_day;
            break;
        default:
            break;
        }
        return displayed_date;
    }
    return QDateTime();
}
