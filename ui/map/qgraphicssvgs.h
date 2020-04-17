#ifndef SENTINEL_QGRAPHICSSVGS_H
#define SENTINEL_QGRAPHICSSVGS_H

#include <QGraphicsSvgItem>

class QGraphicsSvgs
    : public QGraphicsSvgItem
{
public:
    QGraphicsSvgs(QString const &fileName, QGraphicsItem *parent = 0);
    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setPosition(QPointF const &position, qreal orientation = 0);
    void changePicture(QString const &fileName);
    void active();
    void release();

private:
    QRectF geometry_;
    QSvgRenderer *past_item_svg_;

protected:
    QSizeF size_;
    QImage image_;
};

#endif // SENTINEL_QGRAPHICSSVGS_H
