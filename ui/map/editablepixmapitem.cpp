#include <cmath>

#include "editablepixmapitem.h"

editablePixmapItem::editablePixmapItem(qreal resolution) : QGraphicsPixmapItem()
  , isBeingEdited_(false)
  , isBeingRotated_(false)
  , rulerStartPoint_(QPointF(0,0))
  , rulerEndPoint_(QPointF(0,0))
  , resolution_(resolution)
{
}

editablePixmapItem::~editablePixmapItem()
{

}

void editablePixmapItem::addEditPoint(const QPointF &editPoint, double ratio, int groupedPoints)
{
    if (groupedPoints == 0)
    {
        mEditPoint_ = editPoint;

        editedPointsVector_.append( QPair<QPointF, QColor>(mEditPoint_, mEditColor_) );

        editedPointsRatiosVector_.append(ratio);
    }else if (groupedPoints > 0)
    {
        groupedEditPointsVector_.append(groupedPoints);
    }
}

void editablePixmapItem::clearEditedPoints()
{
    editedPointsVector_.clear();
    editedPointsRatiosVector_.clear();
    groupedEditPointsVector_.clear();

    rotation_ = 0;
}

void editablePixmapItem::setOriginImage(const QImage &orginImage)
{
    originImg_.convertFromImage(orginImage, Qt::NoFormatConversion);
    /* Qt::NoFormatConversion : Don't do any format conversions on the image.
     * Can be useful when converting a QImage to a QPixmap */

    setPixmap(originImg_);

    rotateCenter_ = QPointF(boundingRect().width() / 2, boundingRect().height() / 2);

    update();
}

void editablePixmapItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsPixmapItem::paint(painter, option, widget);

    // Details edition
    for (int i = 0; i < editedPointsVector_.size(); ++i) // Draw edited points
    {
        // Draw a point ( To draw a round point change pen cap to Qt::RoundCap )
        painter->setPen( QPen(editedPointsVector_.at(i).second, (15/editedPointsRatiosVector_.at(i)), Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin) );
        painter->drawPoint(editedPointsVector_.at(i).first);
    }

    // Rotation center
    if ( isBeingRotated_ )
    {
        QPen pen;
        pen.setColor(Qt::red);
        pen.setWidth(5.0 / scale());
        painter->setPen(pen);

        int tall = 20.0 / scale();
        painter->drawLine(rotateCenter_.x() - tall, rotateCenter_.y(), rotateCenter_.x() + tall, rotateCenter_.y());
        painter->drawLine(rotateCenter_.x(), rotateCenter_.y() - tall, rotateCenter_.x(), rotateCenter_.y() + tall);
    }

    // Distance measurement
    if ((rulerStartPoint_ == rulerEndPoint_) && (rulerStartPoint_ != QPointF(0,0))) // Draw ruler's start point
    {
        painter->setPen( QPen(QColor(240,127,16), 5/currentScale, Qt::SolidLine, Qt::FlatCap) );
        painter->drawPoint(rulerStartPoint_);
    }

    if ( (rulerStartPoint_ != rulerEndPoint_) && (rulerStartPoint_ != QPointF(0,0)) && (rulerEndPoint_ != QPointF(0,0)) )// Draw ruler
    {
        painter->setPen( QPen(Qt::black, 3/currentScale, Qt::DotLine, Qt::FlatCap) );
        painter->drawLine(rulerStartPoint_, rulerEndPoint_);//ruler's line

        painter->setPen( QPen(QColor(240,127,16), 6/currentScale, Qt::SolidLine, Qt::FlatCap) );
        painter->drawPoint(rulerStartPoint_); // Start point
        painter->drawPoint(rulerEndPoint_); // End point

        qreal distanceTextX = rulerStartPoint_.x()+5/currentScale;
        qreal distanceTextY; // Distance text coordinates

        if (rulerStartPoint_.y() < rulerEndPoint_.y())
            distanceTextY = rulerStartPoint_.y()-10/currentScale;
        else
            distanceTextY = rulerStartPoint_.y()+25/currentScale;

        qreal pDistance = QLineF(rulerStartPoint_, rulerEndPoint_).length();

        painter->setBackgroundMode(Qt::OpaqueMode);
        painter->setBackground(Qt::black);
        painter->setFont(QFont("Times", round(11/currentScale), QFont::Bold));
        painter->drawText(distanceTextX, distanceTextY, QString(" %1 m ").arg(pDistance*resolution_, 0, 'f', 2));
    }
}

void editablePixmapItem::setEditionStatus(bool editStatus)
{
    isBeingEdited_ = editStatus;
}

void editablePixmapItem::setEditColor(QColor &color)
{
    mEditColor_ = color;
}

QPointF editablePixmapItem::rotateCenter() const
{
    return rotateCenter_;
}

void editablePixmapItem::setRotateCenter(const QPointF &rotateCenter)
{
    rotateCenter_ = rotateCenter;
}

bool editablePixmapItem::isBeingRotated() const
{
    return isBeingRotated_;
}

void editablePixmapItem::setIsBeingRotated(bool isBeingRotated)
{
    isBeingRotated_ = isBeingRotated;
}

void editablePixmapItem::rotate(int iDegree)
{
    setTransformOriginPoint(rotateCenter_);
    setRotation(iDegree);

    rotation_ = iDegree;
}

void editablePixmapItem::writeChanges()
{
    if ( editedPointsVector_.isEmpty() && rotation_ == 0 )
        return;

    QImage img(originImg_.size(), QImage::Format_Grayscale8);
    img.fill(Qt::transparent);

    QPainter p(&img);

    p.drawImage(0, 0, originImg_.toImage());
    p.end();

    p.begin(&img);
    for (int i = 0; i < editedPointsVector_.size(); ++i)
    {
        // Draw a point ( To draw a round point change pen cap to Qt::RoundCap )
        p.setPen( QPen(editedPointsVector_.at(i).second, (15/editedPointsRatiosVector_.at(i)), Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin) );
        p.drawPoint(editedPointsVector_.at(i).first);
    }
    p.end();

    if (rotation_ != 0)
    {
        img = getRotatedMap(img);
        rotate(0);
    }

    setOriginImage(img);

    clearEditedPoints();
}

QImage editablePixmapItem::getRotatedMap(const QImage &origImg) const
{
    QSize size = origImg.size();

    QImage img(size, QImage::Format_Grayscale8);

    img.fill( Qt::darkGray ); //Set transparent background qRgb(128,128,128)

    QPainter p;
    p.begin(&img);
    p.translate(img.width() / 2, img.height() / 2);
    p.rotate(rotation_);
    p.translate(-(img.width() / 2), -(img.height() / 2));
    p.drawImage(0, 0, origImg);
    p.end();

    return img;
}

void editablePixmapItem::setRulerStartPoint(const QPointF &rulerStartPoint)
{
    rulerStartPoint_ = rulerStartPoint;
}

void editablePixmapItem::setRulerEndPoint(const QPointF &rulerStartPoint)
{
    rulerEndPoint_ = rulerStartPoint;
}

QPointF editablePixmapItem::getRulerStartPoint() const
{
    return rulerStartPoint_;
}

QPointF editablePixmapItem::getRulerEndPoint() const
{
    return rulerEndPoint_;
}

double editablePixmapItem::getCurrentScale() const
{
    return currentScale;
}

void editablePixmapItem::setCurrentScale(double value)
{
    currentScale = value;
}

qreal editablePixmapItem::getResolution() const
{
    return resolution_;
}

void editablePixmapItem::setResolution(const qreal &resolution)
{
    resolution_ = resolution;
}

void editablePixmapItem::resetChanges()
{
    if ( editedPointsVector_.isEmpty() && (rotation() == 0.0) )
        return;

    rotate(0);

    clearEditedPoints();
    update();
}

void editablePixmapItem::undoLastEdit()
{
    if ( groupedEditPointsVector_.isEmpty() )
        return;

    int pointsToRemove = groupedEditPointsVector_.last();

    editedPointsVector_.remove(editedPointsVector_.size()-pointsToRemove, pointsToRemove);
    editedPointsRatiosVector_.remove(editedPointsRatiosVector_.size()-pointsToRemove, pointsToRemove);
    groupedEditPointsVector_.removeLast();

    update();
}

bool editablePixmapItem::isEdited()
{
    return ( !editedPointsVector_.isEmpty() || (rotation_ != 0) );
}
