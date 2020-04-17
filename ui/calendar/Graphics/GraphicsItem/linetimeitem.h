#ifndef TIMELINEITEM_H
#define TIMELINEITEM_H

#include <QGraphicsObject>
#include <QGraphicsLineItem>

class TimeLineItem : public QGraphicsObject
{
    Q_OBJECT
public:
    TimeLineItem(QGraphicsItem *parent = 0);
    ~TimeLineItem(){}
    virtual QRectF boundingRect() const {return time_line_->boundingRect();}
    virtual void paint (QPainter */*painter*/, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/) {}

public slots :
    void setLine(QLine const& line = QLine());
signals :
    void removeFromScene(QGraphicsItem*);
private :
    QGraphicsLineItem* time_line_;
    int is_displayed_;
};

#endif // TIMELINEITEM_H
