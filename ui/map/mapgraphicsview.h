#ifndef MAPGRAPHICSVIEW_H
#define MAPGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QVector>

#include "core/constants.h"
#include "core/appinstance.h"
#include "sentinel-admin_core/events.h"
#include "sentinel-admin_core/constants.h"
#include "sensorpointgraphicsitem.h"
#include "editablepixmapitem.h"

class QGraphicsRectItem;
class ZoneGraphicsItem;
class PatrolGraphicsItem;

class MapGraphicsView
        : public QGraphicsView
        , public core::EventClient<MapGraphicsView>
{
    Q_OBJECT
public:

    MapGraphicsView(core::AppInstance & instance, QGraphicsScene * scene, QWidget * parent);
    ~MapGraphicsView();

    void scaleView(qreal scaleFactor);
    void translateView(qreal x, qreal y);
    /**
     * @brief setMode change the map interaction mode
     * @param mode : create zone or normal mode
     */
    void setMode(constants::Mode mode);
    inline PatrolGraphicsItem * patrolsItem()
    {
        return patrolsItem_;
    }

    void resize(QSize const& size);

    void loadSensors();

    void loadZones();
    void clearZones();
    void receive(core::DeleteZone const& event);
    void receive(core::ZonesUpdate const& );
    void receive(core::SensorsUpdate const& );
    void receive(core::MapUpdate const& );
    void updateSensorsScale();

    editablePixmapItem *map();
    void setMapEditColor(const QColor &editColor)
    {
        mMapEditColor_ = editColor;
        map_->setEditColor(mMapEditColor_);
    }
    void ApplyEditChangesToMap();
    void CancelEditChangesToMap();
    void undoLastEditToMap();
    bool isMapEdited();
    void rotateMap(int iDegree);

    void setMap(QImage const &img, const qreal &resolution);

public slots:
    /*!
     * \brief remove the sensor attached to the given graphic item.
     * <br> This also remove the graphic item from the scene
     * \param item the graphic item
     */
    void removeSensor(SensorPointGraphicsItem* item);
    /*!
     * \brief activate the update_ state of the sensor attached to this item.
     * \param item the graphic item with the attached sensor
     */
    void updateSensor(SensorPointGraphicsItem* item);

    void redrawZones();

protected:
    void contextMenuEvent(QContextMenuEvent *event);

    virtual void wheelEvent(QWheelEvent* event);
    virtual void keyReleaseEvent(QKeyEvent *);
public:
    virtual void keyPressEvent(QKeyEvent* event);
protected:
    void mouseReleaseEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *event);

private:
    void createPoint(QPoint const& point);
    void addSensor(QMouseEvent *event);

    void showHideItemsForEditMode(bool bVisibility);

    QGraphicsScene * scene_;


    ///! map interaction mode
    constants::Mode mode_;
    constants::Mode previousMode_;

    QGraphicsRectItem * dummyZone_;
    QPointF startCreateZone_;
    PatrolGraphicsItem * patrolsItem_;

    SensorModifier * mSensorModifier_;
    QSize size_;
    bool creating_ = false;
    bool space_ = false;
    double currentScale = 1.0;

    editablePixmapItem * map_;

    QColor mMapEditColor_;

    QImage originalMap_;

    int iGroupedEditPointsCounter;
};

inline editablePixmapItem* MapGraphicsView::map()
{
    return map_;
}

#endif // MAPGRAPHICSVIEW_H
