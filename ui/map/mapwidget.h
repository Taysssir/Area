#ifndef MAP_WIDGET_H
#define MAP_WIDGET_H

#include <QWidget>
#include <limits>
#include <QToolBar>

#include "evigilante_configuration/include/eos/configuration/configuration.hpp"
#include "sentinel-admin_core/constants.h"
#include "sentinel-common/core/appinstance.h"
#include "sentinel-admin_core/events.h"
#include "sentinel-admin_core/constants.h"
#include "patrolgraphicsitem.h"

#include <QMouseEvent>
#include <QGraphicsView>
#include <QSlider>
#include <QSpinBox>
#include <QPropertyAnimation>
#include <QApplication>
#include <QAction>

class QGraphicsScene;
class MapGraphicsView;
class PatrolController;
class QGLWidget;

class MapWidget
        : public QWidget
        , public core::EventClient<MapWidget>
{
    Q_OBJECT
public:

    MapWidget(core::AppInstance &instance, QWidget * parent=0,
              bool showToolBar=true);

    /**
     * @brief setMap
     * @param map
     */
    void setMap(eos::MapSharedPointer const& map);

    /**
     * @brief setMode change the map interaction mode
     * @param mode : create zone or normal mode
     */
    void setMode(constants::Mode mode);

    void receive(core::MapUpdate const& );
    PatrolGraphicsItem * patrolsItem();

    void resetRotationControls();

    bool isEditing() {return editModeStatus;}
    void triggerEditMode() { mapEdit_->trigger(); }
    void triggerCalendar() { calendar->trigger(); }

signals:
    void activateZoneTool(bool);
    void activatePointTool(bool);
    void activateSensorsTool(bool);
    void activateCalendarTool(bool);
    void activateWhitePencilTool(bool);
    void activateBlackPencilTool(bool);
    void activateGrayPencilTool(bool);
    void activateMapRotationTool(bool);
    void editModeTriggered(bool);
    void activateRulerTool(bool);

private slots:
    void zoneEdition();
    void pointEdition();
    void sensorsEdition();
    void calendarEdition();
    void whitePencilEdition();
    void blackPencilEdition();
    void grayPencilEdition();
    void undoPencilEdition();
    void rotateMapEdition();
    void doMapRotation(int);
    void activateEditMode();
    void activateDistanceMeasurementMode();

protected:
    /**
     * @brief resizeEvent receive event form the parent
     */
    virtual void resizeEvent(QResizeEvent *);
    virtual void keyPressEvent(QKeyEvent *);
private:
    void setupToolBar();
    void addToolBarSpacer();
    //! scene pointer
    QGraphicsScene * scene_;
    //! view pointer
    MapGraphicsView * view_;
    QToolBar * toolBar_;
    QAction * zone;
    QAction * point;
    QAction * sensors;
    QAction * calendar;
    QAction * mapEdit_;
    QAction * whitePencil;
    QAction * blackPencil;
    QAction * grayPencil;
    QAction * undoPencil;
    QAction * rotateMap;
    QAction * rulerTool;
    QPoint centerOfMap_;

    QSlider * rotationVerticalSlider;
    QSpinBox * rotationSpinBox;
    QAction * rotationVerticalSliderAction;
    QAction * rotationSpinBoxAction;

    bool editModeStatus;
};

#endif // MAP_WIDGET_H
