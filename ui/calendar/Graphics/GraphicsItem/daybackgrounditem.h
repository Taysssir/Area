#ifndef DAYBACKGROUNDITEM_H
#define DAYBACKGROUNDITEM_H

#include "backgrounditem.h"

class DayBackgroundItem : public BackgroundItem
{
    Q_OBJECT
public:
    DayBackgroundItem(core::AppInstance &instance, QGraphicsObject *parent = 0);
    ~DayBackgroundItem();

    virtual QDateTime getDateTimeFromMousePosition(QPointF pos);
    virtual QVector<QRectF> getRectFromQDateTimes(QDateTime start, QDateTime stop);

    virtual int getRequiredHeight()
    {
        return 48*time_column_size_.height() + header_size_.height();
    }

private :
    virtual void setBackgroundDesign(QRectF rect);
    virtual void setHeader(QDate date);
virtual void getSemanticsDisplayed(QDate const& date)
    {
        semantics_date_.clear();
        semantics_date_.append(date);
    }
};

#endif // DAYBACKGROUNDITEM_H
