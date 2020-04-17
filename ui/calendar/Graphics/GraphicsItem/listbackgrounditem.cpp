#include "listbackgrounditem.h"

ListBackgroundItem::ListBackgroundItem(core::AppInstance &instance, QGraphicsObject *parent):BackgroundItem(instance,parent)
{

}

ListBackgroundItem::~ListBackgroundItem()
{

}

void ListBackgroundItem::setBackgroundDesign(QRectF )
{

}

void ListBackgroundItem::setHeader(QDate )
{

}

QDateTime ListBackgroundItem::getDateTimeFromMousePosition(QPointF )
{
   return QDateTime();
}

QVector<QRectF> ListBackgroundItem::getRectFromQDateTimes(QDateTime , QDateTime )
{
    return QVector<QRectF>();
}
