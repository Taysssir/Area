#ifndef LISTBACKGROUNDITEM_H
#define LISTBACKGROUNDITEM_H

#include "backgrounditem.h"

class ListBackgroundItem : public BackgroundItem
{
    Q_OBJECT
public:
    ListBackgroundItem(core::AppInstance &instance, QGraphicsObject *parent = 0);
    ~ListBackgroundItem();
    virtual QDateTime getDateTimeFromMousePosition(QPointF pos);
    virtual QVector<QRectF> getRectFromQDateTimes(QDateTime start, QDateTime stop);
    virtual int getRequiredHeight()
    {
        return 1;
    }
private :
    virtual void setBackgroundDesign(QRectF rect);
    virtual void setHeader(QDate date);
    virtual void getSemanticsDisplayed(QDate const& /*date*/){}
};

#endif // LISTBACKGROUNDITEM_H
