#include <QGraphicsPixmapItem>
#include <QResizeEvent>
#include <QHBoxLayout>
#include <QAction>
#include <QDebug>
#include <iostream>

#include "ui/admin/patroladminwidget.h"
#include "ui/admin/patrolstab.h"
#include "mapwidget.h"
#include "mapgraphicsview.h"
#include "patrolgraphicsitem.h"
#include "sentinel-common/helper/map.h"

MapWidget::MapWidget(core::AppInstance &instance,
                     QWidget *parent,
                     bool showToolBar)
    : QWidget(parent)
    , core::EventClient<MapWidget>(instance)
    , toolBar_(new QToolBar("MapTools", this))
    , editModeStatus(false)
{
  QHBoxLayout *layout = new QHBoxLayout(this);

  QWidget *empty = new QWidget(this);

  scene_ = new QGraphicsScene(empty);
  /* No index is applied. Item location is of linear complexity, as all items on the scene are searched.
   * Adding, moving and removing items, however, is done in constant time.
   * This approach is ideal for dynamic scenes, where many items are added, moved or removed continuously.
   * (Fixed Random SegFault when saving//loading configs)*/
  scene_->setItemIndexMethod(QGraphicsScene::NoIndex);
  scene_->setBackgroundBrush(Qt::gray);

  layout->addWidget(empty);
  layout->addWidget(toolBar_);
  toolBar_->setMaximumWidth(40);
  setLayout(layout);

  if (showToolBar == false)
    toolBar_->hide();

  setupToolBar();

  view_ = new MapGraphicsView(instance, scene_, empty);

  subscribe<core::MapUpdate>(*this);
}

PatrolGraphicsItem * MapWidget::patrolsItem()
{
    return view_->patrolsItem();
}

void MapWidget::setupToolBar()
{
  toolBar_->setOrientation(Qt::Vertical);

  calendar = new QAction(this);
  calendar->setIcon(QPixmap(":/icons/toolbar/calendarf"));
  calendar->setToolTip(tr("Show/hide tasks calendar"));
  calendar->setCheckable(true);
  toolBar_->addAction(calendar);
  connect(calendar, SIGNAL(triggered()), this, SLOT(calendarEdition()));

  toolBar_->addSeparator();

  zone = new QAction(this);
  zone->setIcon(QPixmap(":/icons/toolbar/zone"));
  zone->setToolTip(tr("Create zones"));
  zone->setCheckable(true);
  toolBar_->addAction(zone);
  connect(zone, SIGNAL(triggered()), this, SLOT(zoneEdition()));

  point = new QAction(this);
  point->setIcon(QPixmap(":/icons/toolbar/addpoint"));
  point->setToolTip(tr("Create patrol points"));
  point->setCheckable(true);
  toolBar_->addAction(point);
  connect(point, SIGNAL(triggered()), this, SLOT(pointEdition()));

  //Add the button of Sensors Editors to the toolbar
  sensors = new QAction(this);
  sensors->setIcon(QPixmap(":/icons/toolbar/sensors_editor"));
  sensors->setToolTip(tr("Create sensors"));
  sensors->setCheckable(true);
  toolBar_->addAction(sensors);
  connect(sensors, SIGNAL(triggered()), this, SLOT(sensorsEdition()));

  toolBar_->addSeparator();

  rulerTool = new QAction(this);
  rulerTool->setIcon(QPixmap(":/icons/toolbar/ruler"));
  rulerTool->setToolTip(tr("Trigger distance measurement mode"));
  rulerTool->setCheckable(true);
  if ( instance_.getExpertMode() ) // Will be available only in EXPERT mode
      toolBar_->addAction(rulerTool);
  connect(rulerTool, SIGNAL(triggered()), this, SLOT(activateDistanceMeasurementMode()));

  mapEdit_ = new QAction(this);
  mapEdit_->setIcon(QPixmap(":/icons/toolbar/map_edit"));
  mapEdit_->setToolTip(tr("Trigger map edition mode"));
  mapEdit_->setCheckable(true);
  if ( instance_.getExpertMode() ) // Will be available only in EXPERT mode
      toolBar_->addAction(mapEdit_);
  connect(mapEdit_, SIGNAL(triggered()), this, SLOT(activateEditMode()));

  addToolBarSpacer();

  whitePencil = new QAction(this);
  whitePencil->setIcon(QPixmap(":/icons/toolbar/pencil_white"));
  whitePencil->setToolTip(tr("Add white details"));
  whitePencil->setCheckable(true);
  toolBar_->addAction(whitePencil);
  QObject::connect(whitePencil, SIGNAL(triggered()), this, SLOT(whitePencilEdition()));

  blackPencil = new QAction(this);
  blackPencil->setIcon(QPixmap(":/icons/toolbar/pencil_black"));
  blackPencil->setToolTip(tr("Add black details"));
  blackPencil->setCheckable(true);
  toolBar_->addAction(blackPencil);
  QObject::connect(blackPencil, SIGNAL(triggered()), this, SLOT(blackPencilEdition()));

  grayPencil = new QAction(this);
  grayPencil->setIcon(QPixmap(":/icons/toolbar/pencil_gray"));
  grayPencil->setToolTip(tr("Add gray details"));
  grayPencil->setCheckable(true);
  toolBar_->addAction(grayPencil);
  QObject::connect(grayPencil, SIGNAL(triggered()), this, SLOT(grayPencilEdition()));

  undoPencil = new QAction(this);
  undoPencil->setIcon(QPixmap(":/icons/toolbar/undo"));
  undoPencil->setToolTip(tr("Undo last pencil edition"));
  toolBar_->addAction(undoPencil);
  QObject::connect(undoPencil, SIGNAL(triggered()), this, SLOT(undoPencilEdition()));

  addToolBarSpacer();

  rotateMap = new QAction(this);
  rotateMap->setIcon(QPixmap(":/icons/toolbar/map_rotation"));
  rotateMap->setToolTip(tr("Rotate map"));
  rotateMap->setCheckable(true);
  toolBar_->addAction(rotateMap);
  QObject::connect(rotateMap, SIGNAL(triggered()), this, SLOT(rotateMapEdition()));

  addToolBarSpacer();

  rotationVerticalSlider = new QSlider(Qt::Orientation::Vertical, this);
  rotationVerticalSlider->setMinimum(-180);
  rotationVerticalSlider->setMaximum(180);
  rotationVerticalSlider->setValue(0);
  rotationVerticalSlider->setTickPosition(QSlider::TicksBothSides);
  rotationVerticalSlider->setMinimumHeight(100);
  rotationVerticalSlider->setMaximumHeight(200);
  rotationVerticalSlider->setMaximumWidth(35);
  rotationVerticalSlider->setStyleSheet( "QSlider { border: 1px solid grey; border-radius: 3px;} "
                                         "QSlider::handle:vertical { border: 1px solid grey; border-radius: 2px; background: rgb(240,127,16); }" );

  rotationSpinBox = new QSpinBox(this);
  rotationSpinBox->setMinimum(-180);
  rotationSpinBox->setMaximum(180);
  rotationSpinBox->setAlignment(Qt::AlignCenter);
  rotationSpinBox->setMaximumWidth(35);
  QFont spinFont;
  spinFont.setBold(true);
  rotationSpinBox->setFont(spinFont);
  rotationSpinBox->setStyleSheet( "QSpinBox { border: 1px solid grey; border-radius: 3px; color: rgb(90,90,90); }"
                                  "QSpinBox { padding-left: 0px; padding-right: 0px; padding-top: 10px; padding-bottom: 10px; margin-top: 1px; margin-bottom: 1px; }"
                                  "QSpinBox::up-button { subcontrol-origin: padding; subcontrol-position: top; width: 25px; height: 10px;}"
                                  "QSpinBox::down-button { subcontrol-origin: padding; subcontrol-position: bottom; width: 25px; height: 10px;}" );

  QObject::connect(rotationVerticalSlider, SIGNAL(valueChanged(int)), rotationSpinBox, SLOT(setValue(int)), Qt::UniqueConnection);
  QObject::connect(rotationSpinBox, SIGNAL(valueChanged(int)), this, SLOT(doMapRotation(int)), Qt::UniqueConnection);

  rotationVerticalSliderAction = toolBar_->addWidget(rotationVerticalSlider);
  rotationSpinBoxAction = toolBar_->addWidget(rotationSpinBox);

  rotationVerticalSliderAction->setVisible(false);
  rotationSpinBoxAction->setVisible(false);

  whitePencil->setVisible(false);
  blackPencil->setVisible(false);
  grayPencil->setVisible(false);
  undoPencil->setVisible(false);
  rotateMap->setVisible(false);
}

void MapWidget::addToolBarSpacer()
{
    QWidget* spacerWidget = new QWidget(toolBar_);
    spacerWidget->setMinimumHeight(10);
    spacerWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
    toolBar_->addWidget(spacerWidget);
}

void MapWidget::setMap(eos::MapSharedPointer const &map)
{
    qApp->setOverrideCursor(Qt::WaitCursor);
    qApp->processEvents();
  if (!map) return;
  constants::map::width() = map->width();
  constants::map::height() = map->height();

  qDebug() << "map content size : " << map->content().size();
  if(!map->content().size())
    return ;

  //map_->setZValue(constants::MapZValue);
  //view_->map()->setPixmap(QPixmap::fromImage(eos::helper::convert(*map)));
  //QImage  imap = QImage::fromData((const uchar*)(map->content()[0]), map->content().size(), "PNG");

  QImage imap = eos::helper::fromMap(*map);

  if(!imap.size().width())
    return ;
  //QImage imap = eos::helper::convert(*map);
  qreal resolution = map->resolution();
  view_->setMap(imap, resolution);
  view_->resize(view_->map()->pixmap().size());
  view_->setFocus();

  if(view_ && view_->patrolsItem())
    view_->patrolsItem()->resize(QSize(map->width(), map->height()));

  centerOfMap_ = QPoint(map->width() / 2, map->height() / 2);
  view_->centerOn(centerOfMap_.x(), centerOfMap_.y());
  qApp->restoreOverrideCursor();
}

void MapWidget::resizeEvent(QResizeEvent *event)
{
  view_->setFixedSize(event->size().width(), event->size().height());
  QWidget::resizeEvent(event);
 // view_->setFocus();
}

void MapWidget::setMode(constants::Mode mode)
{
  view_->setMode(mode);
}

void MapWidget::receive(core::MapUpdate const &)
{
  setMap(instance_.read_only().configuration().base_map());
  emitEvent<core::PatrolsUpdate>();
  emitEvent<core::ZonesUpdate>();
  emitEvent<core::SensorsUpdate>();

  if ( rotationSpinBox->value() != 0 )
      resetRotationControls();
}

void MapWidget::zoneEdition()
{
  point->setChecked(false);
  sensors->setChecked(false);
  whitePencil->setChecked(false);
  blackPencil->setChecked(false);
  grayPencil->setChecked(false);
  rotateMap->setChecked(false);
  rulerTool->setChecked(false);

  emit activateZoneTool(zone->isChecked());
}

void MapWidget::pointEdition()
{
  zone->setChecked(false);
  sensors->setChecked(false);
  whitePencil->setChecked(false);
  blackPencil->setChecked(false);
  grayPencil->setChecked(false);
  rotateMap->setChecked(false);
  rulerTool->setChecked(false);

  emit activatePointTool(point->isChecked());
}

void MapWidget::sensorsEdition()
{
  zone->setChecked(false);
  point->setChecked(false);
  whitePencil->setChecked(false);
  blackPencil->setChecked(false);
  grayPencil->setChecked(false);
  rotateMap->setChecked(false);
  rulerTool->setChecked(false);

  emit activateSensorsTool(sensors->isChecked());
}

void MapWidget::calendarEdition()
{
  emit activateCalendarTool(calendar->isChecked());
}

void MapWidget::whitePencilEdition()
{
    zone->setChecked(false);
    sensors->setChecked(false);
    point->setChecked(false);
    blackPencil->setChecked(false);
    grayPencil->setChecked(false);
    rotateMap->setChecked(false);
    rulerTool->setChecked(false);

    rotationVerticalSliderAction->setVisible(false);
    rotationSpinBoxAction->setVisible(false);

    view_->setMapEditColor(Qt::white);
    emit activateWhitePencilTool(whitePencil->isChecked());
}

void MapWidget::blackPencilEdition()
{
    zone->setChecked(false);
    sensors->setChecked(false);
    point->setChecked(false);
    whitePencil->setChecked(false);
    grayPencil->setChecked(false);
    rotateMap->setChecked(false);
    rulerTool->setChecked(false);

    rotationVerticalSliderAction->setVisible(false);
    rotationSpinBoxAction->setVisible(false);

    view_->setMapEditColor(Qt::black);
    emit activateBlackPencilTool(blackPencil->isChecked());
}

void MapWidget::grayPencilEdition()
{
    zone->setChecked(false);
    sensors->setChecked(false);
    point->setChecked(false);
    blackPencil->setChecked(false);
    whitePencil->setChecked(false);
    rotateMap->setChecked(false);
    rulerTool->setChecked(false);

    rotationVerticalSliderAction->setVisible(false);
    rotationSpinBoxAction->setVisible(false);

    view_->setMapEditColor(Qt::darkGray);
    emit activateGrayPencilTool(grayPencil->isChecked());
}

void MapWidget::undoPencilEdition()
{
    view_->undoLastEditToMap();
}

void MapWidget::rotateMapEdition()
{
    zone->setChecked(false);
    sensors->setChecked(false);
    point->setChecked(false);
    blackPencil->setChecked(false);
    whitePencil->setChecked(false);
    grayPencil->setChecked(false);
    rulerTool->setChecked(false);

    rotationVerticalSliderAction->setVisible(rotateMap->isChecked());
    rotationSpinBoxAction->setVisible(rotateMap->isChecked());

    emit activateMapRotationTool(rotateMap->isChecked());
}

void MapWidget::doMapRotation(int iDegree)
{
    rotationVerticalSlider->setValue(iDegree);

    view_->rotateMap(iDegree);
}

void MapWidget::activateDistanceMeasurementMode()
{
    zone->setChecked(false);
    sensors->setChecked(false);
    point->setChecked(false);
    blackPencil->setChecked(false);
    whitePencil->setChecked(false);
    grayPencil->setChecked(false);
    rotateMap->setChecked(false);

    rotationVerticalSliderAction->setVisible(false);
    rotationSpinBoxAction->setVisible(false);

    emit activateRulerTool(rulerTool->isChecked());
}

void MapWidget::activateEditMode()
{
    editModeStatus = mapEdit_->isChecked();

    if (whitePencil->isChecked())
        whitePencil->trigger();
    if (blackPencil->isChecked())
        blackPencil->trigger();
    if (grayPencil->isChecked())
        grayPencil->trigger();
    if (rotateMap->isChecked())
        rotateMap->trigger();

    if (zone->isChecked())
        zone->trigger();
    if (point->isChecked())
        point->trigger();
    if (sensors->isChecked())
        sensors->trigger();
    if (calendar->isChecked())
        calendar->trigger();

    zone->setDisabled(editModeStatus);
    point->setDisabled(editModeStatus);
    sensors->setDisabled(editModeStatus);
    calendar->setDisabled(editModeStatus);

    whitePencil->setVisible(editModeStatus);
    blackPencil->setVisible(editModeStatus);
    grayPencil->setVisible(editModeStatus);
    undoPencil->setVisible(editModeStatus);
    rotateMap->setVisible(editModeStatus);

    if (!editModeStatus && view_->isMapEdited())
    {
        QMessageBox::StandardButton response = QMessageBox::question(0, tr("Edition Mode"), tr("Do you want to apply your changes?"));

        if ( response == QMessageBox::Yes ) // Apply changes
        {
            qApp->setOverrideCursor(Qt::WaitCursor);
            qApp->processEvents();

            view_->ApplyEditChangesToMap();

            eos::MapSharedPointer map = instance_.configuration().base_map();

            QImage img = view_->map()->pixmap().toImage();

            eos::helper::fromQImage(img, *map);

            if ( rotationSpinBox->value() != 0 )
                resetRotationControls();

            qApp->restoreOverrideCursor();
        }
        else // Cancel changes
        {
            view_->CancelEditChangesToMap();

            if ( rotationSpinBox->value() != 0 )
                resetRotationControls();
        }
    }

    emit editModeTriggered(editModeStatus);
}

void MapWidget::resetRotationControls()
{
    QObject::disconnect(rotationVerticalSlider, SIGNAL(valueChanged(int)), rotationSpinBox, SLOT(setValue(int)));
    QObject::disconnect(rotationSpinBox, SIGNAL(valueChanged(int)), this, SLOT(doMapRotation(int)));

    rotationSpinBox->setValue(0);
    rotationVerticalSlider->setValue(0);

    QObject::connect(rotationVerticalSlider, SIGNAL(valueChanged(int)), rotationSpinBox, SLOT(setValue(int)), Qt::UniqueConnection);
    QObject::connect(rotationSpinBox, SIGNAL(valueChanged(int)), this, SLOT(doMapRotation(int)), Qt::UniqueConnection);
}

void MapWidget::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
    case Qt::Key_Escape :
    {
        if(calendar->isChecked())
            calendar->trigger();
    }
    }
    view_->keyPressEvent(e);
}
