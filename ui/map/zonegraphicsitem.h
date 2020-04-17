#ifndef ZONEGRAPHICSITEM_H
#define ZONEGRAPHICSITEM_H

#include <QGraphicsItem>
#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QColor>
#include <QPainter>
#include <QPen>
#include <QPointF>
#include <QGraphicsSvgItem>

#include "core/appinstance.h"


class Snap 
        : public QGraphicsItem
{

public:
    typedef enum
    {
        TopLeft,
        TopRight,
        BottomRight,
        BottomLeft
    } Type;

    static const int kSize = 10;

    explicit Snap(QGraphicsItem *parent = 0, Type type=TopLeft);

    Type  getType() const;

private:

    virtual QRectF boundingRect() const;
    virtual void paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual void mousePressEvent (QGraphicsSceneMouseEvent * event );
    virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );

    virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent * event );

    Type type_;
    QPointF mouseDown_;
};

class MapGraphicsView;

class ZoneGraphicsItem
        : public QGraphicsItem
        , public core::EventClient<ZoneGraphicsItem>
{
public:
    enum { Type = UserType + 1 };
    int type() const
    {
        // Enable the use of qgraphicsitem_cast with this item.
        return Type;
    }
    ZoneGraphicsItem(core::AppInstance & instance, int zone, const QSize &mapSize, QGraphicsItem * parent);

    /**
     * @brief mouseMove mouse move event (callback from Snap)
     * not the well way to do that, normaly use an eventFilter method ...
     * @param point diff
     * @param type which one
     * @param mouseDown start position
     */
    void mouseMove(QPointF const& point, Snap::Type type, const QPointF &mouseDown);

    /**
     * @brief adjustSize add offset to size
     * @param x new value have to add to width
     * @param y new value have to add to height
     */
    void adjustSize(int x, int y);

    void setSize(int x, int y);

    int zoneID() const
    {
        return zone_;
    }
    static void edit(bool ed)
    {
        edit_ = ed;
    }
    static bool const& hover()
    {
        return hover_;
    }
    static void setHover(bool h)
    {
        hover_ = h;
    }
    static int const& selectedZone()
    {
        return selectedZone_;
    }
private:
    static constexpr int kMinSizeSide = 5;
    static constexpr int kPadding = 10;
    void updateZone();

    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

    virtual QRectF boundingRect() const;
    virtual void paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent * event );
    virtual void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event );

    virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );
    virtual void mousePressEvent (QGraphicsSceneMouseEvent * event );
    virtual void mouseReleaseEvent (QGraphicsSceneMouseEvent * event );

    void setCornerPositions();

    QPointF dragStart_;
    qreal   width_;
    qreal   height_;
    Snap*  corners_[4];
    QSize mapSize_;
    int zone_;
    static bool edit_;
    static bool hover_;
    static int selectedZone_;

    QGraphicsSvgItem nomovement_;
    QGraphicsSvgItem nodetection_;
    QGraphicsSvgItem noremapping_;
    QGraphicsSvgItem datacenter_;
    QGraphicsSvgItem map_;
};

#endif // ZONEGRAPHICSITEM_H
