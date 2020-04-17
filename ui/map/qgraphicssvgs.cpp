#include <QPainter>
#include <QGraphicsScene>
#include <QSvgRenderer>
#include <QDebug>

#include "qgraphicssvgs.h"
#include "core/constants.h"

QGraphicsSvgs::QGraphicsSvgs(QString const &fileName, QGraphicsItem *parent)
    : QGraphicsSvgItem(fileName, parent)
    , past_item_svg_(0)
{
    changePicture(fileName);
    setZValue(10);
}

QRectF QGraphicsSvgs::boundingRect() const
{
    return QRectF(-size_.width() / 2, -size_.height() / 2, size_.width(), size_.height());
}

void QGraphicsSvgs::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QRect r = QRect(-size_.width() / 2, -size_.height() / 2, size_.width(), size_.height());
    painter->translate(-r.width() / 2, -r.height() / 2);
    QGraphicsSvgItem::paint(painter, option, widget);
}

void QGraphicsSvgs::setPosition(QPointF const &position, qreal orientation)
{
    qreal x = position.x();
    qreal y = constants::map::height() - position.y();

    setPos(x, y);
    setRotation(-orientation);
}

void QGraphicsSvgs::changePicture(QString const &fileName)
{
    image_ = QImage(fileName);

    size_ = image_.size();
    if (past_item_svg_)
        delete past_item_svg_;
    past_item_svg_ = new QSvgRenderer(fileName, this);
    setSharedRenderer(past_item_svg_);
}

void QGraphicsSvgs::active()
{
    show();
}

void QGraphicsSvgs::release()
{
    hide();
}
