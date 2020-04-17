#ifndef EDITABLEPIXMAPITEM_H
#define EDITABLEPIXMAPITEM_H

#include <QGraphicsPixmapItem>
#include <QPainter>
#include <QMessageBox>

class editablePixmapItem : public QGraphicsPixmapItem
{
public:
    editablePixmapItem(qreal resolution = 0.03);

    ~editablePixmapItem();

    void addEditPoint(const QPointF &editPoint, double ratio, int groupedPoints);
    void clearEditedPoints();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setOriginImage(const QImage &orginImage );
    void setEditionStatus(bool editStatus);
    void setEditColor(QColor &color);

    QPointF rotateCenter() const;
    void setRotateCenter(const QPointF &rotateCenter);
    QImage getRotatedMap(const QImage &origImg) const;
    bool isBeingRotated() const;
    void setIsBeingRotated(bool isBeingRotated);
    void rotate(int iDegree);
    void writeChanges();

    void resetChanges();
    void undoLastEdit();
    bool isEdited();

    void setRulerStartPoint(const QPointF &rulerStartPoint);
    void setRulerEndPoint(const QPointF &rulerStartPoint);
    QPointF getRulerStartPoint() const;
    QPointF getRulerEndPoint() const;

    double getCurrentScale() const;
    void setCurrentScale(double value);

    qreal getResolution() const;
    void setResolution(const qreal &resolution);

private:
    QVector< QPair<QPointF, QColor> > editedPointsVector_;
    QVector<double> editedPointsRatiosVector_;
    QVector<int> groupedEditPointsVector_;

    QPixmap originImg_;
    QPointF mEditPoint_;
    QColor mEditColor_;
    bool isBeingEdited_;
    bool isBeingRotated_;
    QPointF rotateCenter_;
    QPointF rulerStartPoint_, rulerEndPoint_;
    double currentScale = 1.0;
    qreal rotation_;
    qreal resolution_;
};

#endif // EDITABLEPIXMAPITEM_H
