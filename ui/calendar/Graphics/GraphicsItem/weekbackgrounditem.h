#ifndef WEEKBACKGROUNDITEM_H
#define WEEKBACKGROUNDITEM_H

#include "backgrounditem.h"

class WeekBackgroundItem : public BackgroundItem
{
    Q_OBJECT
public:
    WeekBackgroundItem(core::AppInstance &instance, QGraphicsObject *parent = 0);
    ~WeekBackgroundItem();
    virtual QDateTime getDateTimeFromMousePosition(QPointF pos);
    virtual QVector<QRectF> getRectFromQDateTimes(QDateTime start, QDateTime stop);
    virtual int getRequiredHeight()
    {
        return 48*time_column_size_.height() + header_size_.height();
    }

private :
    virtual void setBackgroundDesign(QRectF rect);
    virtual void setHeader(QDate date);
    virtual void getSemanticsDisplayed(QDate const& date);
};

#endif // WEEKBACKGROUNDITEM_H
