#ifndef MONTHBACKGROUNDITEM_H
#define MONTHBACKGROUNDITEM_H

#include "backgrounditem.h"

class MonthBackgroundItem : public BackgroundItem
{
    Q_OBJECT
public:
    MonthBackgroundItem(core::AppInstance &instance, QGraphicsObject *parent = 0);
    ~MonthBackgroundItem();
    virtual void setBackgound(QRectF rect = QRect(), QDate date = QDate());
    virtual QDateTime getDateTimeFromMousePosition(QPointF pos);
    virtual QVector<QRectF> getRectFromQDateTimes(QDateTime start, QDateTime stop);
    virtual int getRequiredHeight();
    void clearPatrolOffset();
    virtual void setBackgroundOnSelectedDate(QVector<QDate> date_vector);
    QDate getSelectedDate() const;
    void setSelectedDate(const QDate &selectedDate);

private :
    virtual void setBackgroundDesign(QRectF rect);
    virtual void setHeader(QDate date);
    virtual void getSemanticsDisplayed(QDate const& date);
    QVector<int> patrol_offset_;
    QVector<QGraphicsRectItem*> display_date_graphics_;
    QDate user_selected_date_;
};

#endif // MONTHBACKGROUNDITEM_H
