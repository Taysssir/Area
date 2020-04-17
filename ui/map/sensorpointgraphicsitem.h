#ifndef SENSORPOINTGRAPHICITEM_H
#define SENSORPOINTGRAPHICITEM_H

#include <QGraphicsSceneHoverEvent>
#include <QMouseEvent>
#include <QGraphicsItem>
#include <QMenu>
#include "qgraphicssvgs.h"
#include "evigilante_configuration/include/eos/configuration/models.pb.h"

class SensorModifier;

/*! SensorPointGraphicItem: a graphic representation of a sensor */
class SensorPointGraphicsItem
        : public QGraphicsSvgs
{
    Q_OBJECT
public:
    enum { Type = UserType + 2 };
    int type() const
    {
        return Type;
    }
    SensorPointGraphicsItem(eos::Sensor const& sensor, SensorModifier * modifPane);

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual QRectF boundingRect() const;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    /*!
     * \brief check if this item is in editing mode (i.e. selected)
     * \return true if editing, false otherwise
     */
    bool getSelected() const;
    /*!
     * \brief set the editing mode for this item.
     * \param value: true if activate editing, false otherwise
     */
    void setSelected(bool value);

    /*!
     * @brief construct and return a const sensor in order to add to the sensor list
     * @return
     */
    eos::Sensor getConstSensor() const;


    bool isChangeName() const;
    void setIsChangeName(bool isChangeName);

    int getMSensorIdx() const;
    void setMSensorIdx(int value);
    void setCurrentScale(double value);

    eos::Sensor::Type sensor_type() const;

    QString sensor_name() const;

private:
    /*!
     * \brief update the relative position of the *Pos_ variable according to the position of the sensor.
     */
    void updateElementPosition(eos::Sensor const& sensor);

    bool selected_;               /*!< state if this item is currently in editing mode */
    int mSensorIdx;               /*!< the index of the sensor attached to this item */
    QImage mIconImage_;           /*!< the image/icon of the sensor */
    int mType_;                   /*!< the resource filename of the icon */
    QSizeF isize_;                /*!< size of the sensor icon */
    SensorModifier * mModifier_;  /*!< the modifier that this item is attached to */
    QPoint mDelPos_;              /*!< delete sign position */
    QPoint mRoboPos_;             /*!< position of robotPoint */
    double mCurrentScale_ = 1.0;  /*!< scale of the current view */
    QRectF mNameRect_;            /*!< position of the name's text */
    QString mName_;               /*!< Name of the sensor */
    bool isMovingIcon_;           /*!< state if user is dragging the sensor icon */
    bool isChangeName_;           /*!< state if user clicks on the icon to change sensor's name&type */
    bool isMovingRobPos_;         /*!< state if user is dragging the robotPoint */

};

/*! SensorModifier: a class to be associated to the sensor that is currently in editing mode */
class SensorModifier : public QObject
{
    Q_OBJECT
public:
    SensorModifier();

    /*!
     * \brief check if any sensor graphic item is in editing mode
     * \return true if in editing mode, false otherwise
     */
    bool getIsActive() const;
    /*!
     * \brief activate the editing mode, usually called when the mouse hovers in a sensor graphic item
     * \param value: true if activate editing mode, false otherwise
     */
    void setIsActive(bool value);
    /*!
     * \brief add a sensor graphics item
     */
    void appendGItem(SensorPointGraphicsItem *);

    /*!
     * \brief remove sensor graphic item at index idx
     * \param idx index of the item to be remove
     * \return true if success, false otherwise
     */
    bool removeGItem(int idx);
    /*!
     * \brief toggle the modifier used when editing mode change in MapGraphicView
     */
    void toggleModifer(bool isEditing);
    /*!
     * \brief processing the mouse move event for the currently selected item
     * \param pos
     */
    void setMouseMovePosition(QPointF pos);

    /*!
     * \brief check if the mouse is hovering on any item or not
     * \return true if a item is hovered, false otherwise
     */
    bool isHovered();

    /*!
     * \brief check if the given position is contained inside any existing item
     * \param pos
     * \return true if yes, false otherwise
     */
    bool contains(QPointF const pos);

    QVector<SensorPointGraphicsItem *> *mSensorGItem() const;
    void setMSensorGItem(QVector<SensorPointGraphicsItem *> *mSensorGItem);

signals:
    /*!
     * \brief when a sensor graphic item asks to be delete, this modifier will sends a signal<br>
     * to the mapgraphicview to delete the sensor attached to this modifier and update the view accordingly.
     *
     */
    void deleteSensor(SensorPointGraphicsItem *);

    /*!
     * \brief when a sensor attached to a graphic item is modified, this modifier will sends a signal<br>
     * to the mapgraphicview to update the sensor attached to the concerned item.
     *
     */
    void updateSensor(SensorPointGraphicsItem *);

public slots:

private:
    bool isActive_;                                  /*!< Is there some sensor in editing mode? */
    QVector<SensorPointGraphicsItem*> * mSensorGItem_;/*!< List of all visible/available sensor items */

};

#endif // SENSORPOINTGRAPHICITEM_H
