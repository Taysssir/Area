#include <QGraphicsScene>
#include <QDebug>
#include <QMessageBox>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include <QTextStream>
#include <QMouseEvent>
#include <QScrollBar>
#include <QMouseEvent>
#include <QWheelEvent>
#include <cmath>
#include <QPushButton>
#include <QFormLayout>
#include <QGridLayout>
#include <QKeyEvent>
#include <QObject>
#include <QTimer>
#include <QGraphicsPixmapItem>

#include "patrolgraphicsitem.h"
#include "mapgraphicsview.h"
#include "zonegraphicsitem.h"
#include "sensorpointgraphicsitem.h"
#include "core/constants.h"
#include "sentinel-admin_core/constants.h"
#include "core/sentinel_logger.h"
#include "sentinel-admin_core/qt_utils.h"
#include "ui/dialognewsensor.h"

MapGraphicsView::MapGraphicsView(core::AppInstance &instance, QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent)
    , core::EventClient<MapGraphicsView>(instance)
    , scene_ (scene)
    , mode_(constants::Normal)
    , previousMode_(constants::Normal)
    , dummyZone_(nullptr)
    , patrolsItem_(0)
    , iGroupedEditPointsCounter(0)
{
    setCacheMode(QGraphicsView::CacheBackground); // Speed up the total rendering time for areas that are slow to render
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate); // More responsive viewport update mode
    setRenderHint(QPainter::SmoothPixmapTransform);
    setRenderHint(QPainter::Antialiasing, false);

    setOptimizationFlags(QGraphicsView::DontClipPainter);
    setOptimizationFlags(QGraphicsView::DontSavePainterState);
    setOptimizationFlags(QGraphicsView::DontAdjustForAntialiasing);

    setDragMode(ScrollHandDrag);

    dummyZone_ = new QGraphicsRectItem();
    dummyZone_->setPen(QColor(236, 136, 6));
    dummyZone_->setBrush(QColor(236, 136, 6, 30));
    dummyZone_->setZValue(constants::ZoneZValue);
    dummyZone_->hide();

    startCreateZone_.setX(-1);
    scene_->addItem(dummyZone_);

    patrolsItem_ = new PatrolGraphicsItem(instance,
                                          QSize(constants::map::width(),
                                                constants::map::height()),
                                          this, 0);

    scene_->addItem(patrolsItem_);

    subscribe<core::DeleteZone>(*this);
    subscribe<core::ZonesUpdate>(*this);
    subscribe<core::SensorsUpdate>(*this);
    subscribe<core::MapUpdate>(*this);

    mSensorModifier_ = new SensorModifier();
    connect(mSensorModifier_, SIGNAL(deleteSensor(SensorPointGraphicsItem *)), this, SLOT(removeSensor(SensorPointGraphicsItem *)));
    connect(mSensorModifier_, SIGNAL(updateSensor(SensorPointGraphicsItem *)), this, SLOT(updateSensor(SensorPointGraphicsItem *)));
    size_ = QSize(constants::map::width(), constants::map::height());

    map_ = new editablePixmapItem();
    map_->setZValue(constants::MapZValue);
    scene_->addItem(map_);
}

MapGraphicsView::~MapGraphicsView()
{

}

void MapGraphicsView::resize(QSize const &size)
{
  if (patrolsItem_)
    patrolsItem_->changeSize(size);
  size_ = size;
}

void MapGraphicsView::wheelEvent(QWheelEvent *e)
{
  setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
  static constexpr auto scaleFactor = 1.10;
  static constexpr auto scaleMin = 0.10;
  static constexpr auto scaleMax = 6.0;

  if (e->delta() > 0)
  {
    if (currentScale < scaleMax)
    {
      scale(scaleFactor, scaleFactor);
      currentScale *= scaleFactor;
    }
  }
  else if (currentScale > scaleMin)
  {
    scale(1 / scaleFactor, 1 / scaleFactor);
    currentScale /= scaleFactor;
  }

  if (currentScale > 2.0)
      setViewportUpdateMode(QGraphicsView::FullViewportUpdate); // FullViewportUpdate to cure zone movement glitches on high zoom
  else
      setViewportUpdateMode(QGraphicsView::SmartViewportUpdate); // More responsiveness on low zoom

  if ( currentScale > 0.5 )
      patrolsItem_->scalePoints(currentScale);

  updateSensorsScale();
  map_->setCurrentScale(currentScale);
}

void MapGraphicsView::translateView(qreal x, qreal y)
{
  this->translate(x, y);
}

void MapGraphicsView::scaleView(qreal scaleFactor)
{
  qreal f = std::sqrt(matrix().determinant());

  if (scaleFactor * f > 8.0)
    scaleFactor = 8.0 / f;
  if (scaleFactor * f < 0.05)
    scaleFactor = 0.05 / f;

  scale(scaleFactor, scaleFactor);
}

void MapGraphicsView::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);

    if (space_) return;
    if (event->button() == Qt::LeftButton)
    {
        if ( ( mode_ == constants::AddDetails ) && (map_->boundingRect().contains(map_->mapFromScene(mapToScene(event->pos())))) )
        {
            iGroupedEditPointsCounter = 1;
            map_->setEditionStatus(true);
            map_->addEditPoint(map_->mapFromScene(mapToScene(event->pos())), currentScale, 0);
        }
        else if (mode_ == constants::CreateZone)
        {
            startCreateZone_ = mapToScene(event->pos());
            if (ZoneGraphicsItem::hover() == false)
            {
                creating_ = true;
                dummyZone_->setPos(startCreateZone_);
                dummyZone_->setRect(0, 0, 0, 0);
                event->setAccepted(true);
            }
            return;
        }
    }
}

void MapGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
  QGraphicsView::mouseMoveEvent(event);

  if (space_) return;

  if ((mode_ == constants::DistanceMeasurement) && (map_->boundingRect().contains(map_->mapFromScene(mapToScene(event->pos())))) )
  {
      if (map_->getRulerStartPoint() != QPointF(0,0))
      {
          map_->setRulerEndPoint(map_->mapFromScene(mapToScene(event->pos())));
          map_->update();
      }
  }
  else if ((mode_ == constants::AddDetails) && (event->buttons() == Qt::LeftButton) && (map_->boundingRect().contains(map_->mapFromScene(mapToScene(event->pos())))) )
  {
      iGroupedEditPointsCounter++;
      map_->addEditPoint(map_->mapFromScene(mapToScene(event->pos())), currentScale, 0);

      // Optimization : update only the edited area
      map_->update( QRectF( mapToScene(event->pos()) - QPointF(15/currentScale,15/currentScale),
                            mapToScene(event->pos()) + QPointF(15/currentScale,15/currentScale) ) );
  }
  else if (mode_ == constants::CreateZone && startCreateZone_.x() != -1 && creating_) // && event->button() == Qt::LeftButton)
  {
    QPointF pos = mapToScene(event->pos());

    dummyZone_->setPos(qMin(startCreateZone_.x(), pos.x()),qMin(startCreateZone_.y(), pos.y()));
    dummyZone_->setRect(0, 0, qAbs(startCreateZone_.x()-pos.x()), qAbs(startCreateZone_.y()-pos.y()) );
    dummyZone_->show();

    event->setAccepted(true);
  }
  else if (mode_ == constants::CreatePoint)
  {
    patrolsItem_->setMousePosition(mapToScene(event->pos()));
  }
  /*
    else if (mode_ == constants::CreateSensors)
    {
        //To update the state of the modifier when a sensor is just deleted.
        //Because when the sensor currently attached to this modifier is deleted,
        //the modifier stays active to avoid adding new sensor immediately.
        //This control ensures that after deleting a sensor, user needs to move
        //the mouse to indicate new interaction on the scene
        if (mSensorModifier_->isHovered() == false)
        {
            mSensorModifier_->setIsActive(false);
            update();
        }
        return;
    }*/
  else
  {
    event->setAccepted(false);
  }
}

void MapGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
  constexpr int kMinimumSize = 0;
  QGraphicsView::mouseReleaseEvent(event);
  if (space_) return;

  if ((mode_ == constants::DistanceMeasurement) && (map_->boundingRect().contains(map_->mapFromScene(mapToScene(event->pos())))) )
  {
      if ( map_->getRulerStartPoint() != map_->getRulerEndPoint() )
      {
          map_->setRulerStartPoint(QPointF(0,0));
          map_->setRulerEndPoint(QPointF(0,0));
      }
      else
      {
          map_->setRulerStartPoint(map_->mapFromScene(mapToScene(event->pos())));
          map_->setRulerEndPoint(map_->mapFromScene(mapToScene(event->pos())));
      }
      map_->update();
  }
  else if ((mode_ == constants::RotateMap) && (event->button() == Qt::LeftButton) && (event->modifiers() == Qt::ControlModifier) && (map_->boundingRect().contains(map_->mapFromScene(mapToScene(event->pos())))) )
  {
      map_->setRotateCenter(map_->mapFromScene(mapToScene(event->pos())));
      map_->update();
  }
  else if ((mode_ == constants::AddDetails) && (event->button() == Qt::LeftButton))
  {
      if ( iGroupedEditPointsCounter == 0 )
          map_->addEditPoint(map_->mapFromScene(mapToScene(event->pos())), currentScale, 1);
      else
          map_->addEditPoint(map_->mapFromScene(mapToScene(event->pos())), currentScale, iGroupedEditPointsCounter); //Just to update grouped edit points

      iGroupedEditPointsCounter = 0; // Reset grouped points counter

      // Optimization : update only the edited area
      map_->update( QRectF( mapToScene(event->pos()) - QPointF(15/currentScale,15/currentScale),
                            mapToScene(event->pos()) + QPointF(15/currentScale,15/currentScale) ) );
  }
  else if (mode_ == constants::CreateZone && creating_)
  {
    if (dummyZone_->rect().width() > kMinimumSize
        && dummyZone_->rect().height() > kMinimumSize)
    {
      //qDebug() << dummyZone_->pos().x() << " " << dummyZone_->pos().y();
      //qDebug() << dummyZone_->rect().width() << " " << dummyZone_->rect().height();
      eos::Zone *zone = instance_.zones()->mutable_zones()->Add();
      zone->mutable_top_left()->set_x(dummyZone_->pos().x());
      zone->mutable_top_left()->set_y(constants::map::height() - dummyZone_->pos().y());
      zone->mutable_bottom_right()->set_x(zone->top_left().x() + dummyZone_->rect().width());
      zone->mutable_bottom_right()->set_y(constants::map::height() - dummyZone_->pos().y() - dummyZone_->rect().height());
      zone->set_permissions(eos::Zone::NoMovement);
      emitEvent<core::ZonesUpdate>();
      //qDebug() << constants::map::height();
      //qDebug() << zone->top_left().x() << " " << zone->top_left().y();
      //qDebug() << zone->bottom_right().x() << " " << zone->bottom_right().y();
    }
    dummyZone_->hide();
    startCreateZone_.setX(-1);
    event->setAccepted(true);
  }
  else
    event->setAccepted(false);
  creating_ = false;
}

void MapGraphicsView::ApplyEditChangesToMap()
{
    map_->writeChanges();
}

void MapGraphicsView::CancelEditChangesToMap()
{
    map_->resetChanges();
}

void MapGraphicsView::undoLastEditToMap()
{
    map_->undoLastEdit();
}

bool MapGraphicsView::isMapEdited()
{
    return map_->isEdited();
}

void MapGraphicsView::rotateMap(int iDegree)
{
    QPointF currentPos(map_->mapToScene(map_->rotateCenter()));

    map_->rotate(iDegree);

    QPointF rotatedPos(map_->mapToScene(map_->rotateCenter()));

    QPointF p = (currentPos - rotatedPos);
    map_->moveBy(p.x(), p.y());
    update();
}

void MapGraphicsView::setMap(const QImage &img, const qreal &resolution)
{
    originalMap_ = img;

    map_->setOriginImage(originalMap_);
    map_->setResolution(resolution);

    // center the map
    scene_->setSceneRect(map_->boundingRect());
}

void MapGraphicsView::mouseDoubleClickEvent(QMouseEvent *event)
{
  QGraphicsView::mouseDoubleClickEvent(event);
  if (mode_ == constants::CreatePoint)
  {
    event->setAccepted(true);
    createPoint(event->pos());
  }
  else if (mode_ == constants::CreateZone)
  {
    event->setAccepted(true);
  }
  else if ((mode_ == constants::CreateSensors) && (event->buttons() & Qt::LeftButton))
  {
    addSensor(event);
    event->setAccepted(true);
  }
}

void MapGraphicsView::createPoint(QPoint const &point)
{
  if (patrolsItem_->isHovered() == false)
  {
    QPointF pt = mapToScene(point);
    //if (pt.x() < 0 || pt.y() < 0
    //    || pt.x() > size_.width() || pt.y() > size_.height())
    //{
    //  core::alert(tr("Point creation error : out og the map"));
    //  return;
    //}
    if (patrolsItem_->countPoints() >= constants::kMaxPatrolPoint)
    {
      //core::alert(tr("Erreur : Une ronde ne peut contenir que %1 points !")
      //            .arg(constants::kMaxPatrolPoint));
      QMessageBox dlg(QMessageBox::Critical, tr("Error point creation"),
                      tr("Maximum number of points reached %1")
                          .arg(constants::kMaxPatrolPoint));
      dlg.setWindowFlags(dlg.windowFlags() | Qt::WindowStaysOnTopHint);
      dlg.exec();
      return;
    }
    // else if( instance_.map()->isAuthorized( pt ) == false )
    // {
    //     alert(tr("Erreur création de point : Le point est hors zone !"));
    //     return ;
    // }
    int current = patrolsItem_->addPoint(pt);
    int prev = patrolsItem_->closestPoint();
    if (prev != -1) // Inserting between two other points
    {
        for ( int i=1; i <= (current-prev); i++) // Do a series of swaps to maintain the right index order
            patrolsItem_->changeIndex(current, prev + i );
    }

    //emitEvent<core::PatrolsUpdate>();
  }
}
void MapGraphicsView::addSensor(QMouseEvent *event)
{
  QPoint point = event->pos();

  if (!mSensorModifier_->contains(mapToScene(point))) // && !mSensorModifier_->getIsActive())
  {
    QPointF pt = mapToScene(point);
    if (pt.x() < 0 || pt.y() < 0
        || pt.x() > size_.width() || pt.y() > size_.height())
    {
      core::alert(tr("error_sensor_creation__position_is_out_of_zone"));
      return;
    }
    else
    {
      eos::Sensor *new_sensor = instance_.sensors()->mutable_sensors()->Add();
      new_sensor->set_name("default");
      new_sensor->set_orientation(0);
      new_sensor->mutable_position_second_element()->set_x(0);
      new_sensor->mutable_position_second_element()->set_y(0);
      new_sensor->mutable_distance()->set_min(0);
      new_sensor->mutable_distance()->set_max(0);
      new_sensor->mutable_angle()->set_min(0);
      new_sensor->mutable_angle()->set_max(0);
      new_sensor->set_type(eos::Sensor::RobotDock);
      if (mSensorModifier_->mSensorGItem()->size() > 0) //Copy name&type of the lastest sensor
      {
          SensorPointGraphicsItem *it = mSensorModifier_->mSensorGItem()->back();
          new_sensor->set_type(it->sensor_type());
          new_sensor->set_name("default");
      }
      new_sensor->mutable_position()->set_x(pt.x());
      new_sensor->mutable_position()->set_y(constants::map::height() - pt.y());
      new_sensor->mutable_robot_point()->set_x(new_sensor->position().x() - 40);
      new_sensor->mutable_robot_point()->set_y(new_sensor->position().y() - 40);

      SensorPointGraphicsItem *item = new SensorPointGraphicsItem(*new_sensor, mSensorModifier_);
      item->setCurrentScale(currentScale);
      item->setZValue(400); //to appear in front of all
      item->setMSensorIdx(instance_.sensors()->sensors().size() - 1);
      scene()->addItem(item);
      mSensorModifier_->appendGItem(item);
      update();
    }
    emitEvent<core::SensorsUpdate>();
  }
}

void MapGraphicsView::keyPressEvent(QKeyEvent *event)
{
  switch (event->key())
  {
  case Qt::Key_Plus:
    scaleView(1.2);
    break;
  case Qt::Key_Minus:
    scaleView(1.0 / 1.2);
    break;
  case Qt::Key_Space:
    if (event->isAutoRepeat() == false)
    {
      event->accept();
      space_ = true;
      setDragMode(QGraphicsView::ScrollHandDrag);
      //setCursor(QCursor(Qt::OpenHandCursor));
    }
    break;
  //If user presses F5 when in sensor editing mode, update the sensors
  //Usually used for debug when things go wrong during sensor editing task
  case Qt::Key_F5:
    if (mode_ == constants::CreateSensors)
    {
      loadSensors();
    }
    break;
  default:
    break;
  }
  QWidget::keyPressEvent(event);
}

void MapGraphicsView::keyReleaseEvent(QKeyEvent *event)
{
  if (event->key() == Qt::Key_Space && space_)
  {
    if (event->isAutoRepeat() == false)
    {
      event->accept();
      space_ = false;
      setMode(mode_);
    }
  }
  QWidget::keyReleaseEvent(event);
}

void MapGraphicsView::setMode(constants::Mode mode)
{
  previousMode_ = mode_;
  mode_ = mode;

  if ((previousMode_ == constants::DistanceMeasurement) && (map_->getRulerStartPoint() != map_->getRulerEndPoint()) )
  {
      map_->setRulerStartPoint(QPointF(0,0));
      map_->setRulerEndPoint(QPointF(0,0));
      map_->update();
  }

  map_->setIsBeingRotated(mode_ == constants::RotateMap);

  if (( mode_ == constants::AddDetails ) || ( mode_ == constants::RotateMap )  || ( mode_ == constants::DistanceMeasurement ))
  {
      showHideItemsForEditMode(false);
  }
  else if (( previousMode_ == constants::AddDetails ) || ( previousMode_ == constants::RotateMap ) || ( previousMode_ == constants::DistanceMeasurement ))
  {
      showHideItemsForEditMode(true);
  }

  setDragMode(mode_ != constants::CreateZone ? QGraphicsView::ScrollHandDrag : QGraphicsView::NoDrag);

  setDragMode(mode_ != constants::AddDetails ? this->dragMode() : QGraphicsView::NoDrag);
  setDragMode(mode_ != constants::RotateMap ? this->dragMode() : QGraphicsView::NoDrag);
  setDragMode(mode_ != constants::DistanceMeasurement ? this->dragMode() : QGraphicsView::NoDrag);

  setBackgroundBrush( (( mode_ == constants::AddDetails ) || ( mode_ == constants::RotateMap )) ?
                         QBrush(Qt::darkCyan, Qt::SolidPattern) : QBrush(Qt::gray, Qt::SolidPattern) );

  setForegroundBrush(mode_ == constants::RotateMap ? QBrush(QColor(208, 208, 208, 100), Qt::CrossPattern) : Qt::NoBrush);

  if (mode_ == constants::AddDetails)
  {
      if (mMapEditColor_ == Qt::black)
          setCursor(QCursor(QPixmap(":/icons/toolbar/square_black"))); // "circle_black" to draw round points
      else if (mMapEditColor_ == Qt::white)
          setCursor(QCursor(QPixmap(":/icons/toolbar/square_white"))); // "circle_white" to draw round points
      else if (mMapEditColor_ == Qt::darkGray)
          setCursor(QCursor(QPixmap(":/icons/toolbar/square_gray"))); // "circle_gray" to draw round points
  }
  else if (mode_ == constants::RotateMap)
      setCursor(QCursor(QPixmap(":/icons/toolbar/map_rotation")));
  else
      setCursor(Qt::ArrowCursor);

  //setCursor(mode_ == constants::Normal ? QCursor(Qt::OpenHandCursor) : QCursor(Qt::CrossCursor));
  // if(mode_ == constants::CreateZone)
  ZoneGraphicsItem::setHover(false);
  //else
  ZoneGraphicsItem::edit(mode_ == constants::CreateZone);
  PatrolPointGraphicsItem::edit(mode_ == constants::CreatePoint);
  mSensorModifier_->toggleModifer(mode_ == constants::CreateSensors);
  mSensorModifier_->setIsActive(false);
}

void MapGraphicsView::showHideItemsForEditMode(bool bVisibility)
{
    patrolsItem_->setVisible(bVisibility);

    QList<QGraphicsItem *> allGraphicsItems = scene()->items();
    for (auto item : allGraphicsItems)
    {
      if ( ( item->type() == QGraphicsItem::UserType + 1) || ( item->type() == QGraphicsSvgs::UserType + 2) )
      {
        item->setVisible(bVisibility);
      }
    }
}

void MapGraphicsView::clearZones()
{
    QList<QGraphicsItem *> zoneItems;
    for (auto item : scene()->items())
    {
      if (qgraphicsitem_cast<ZoneGraphicsItem*>(item))
      {
        scene()->removeItem(item);
        zoneItems.append(item);
      }
    }
    scene()->update();
    qDeleteAll(zoneItems);
}

void MapGraphicsView::receive(core::ZonesUpdate const &)
{
  clearZones();
  loadZones();
}

void MapGraphicsView::receive(core::SensorsUpdate const &)
{
  loadSensors();
}

void MapGraphicsView::receive(const core::MapUpdate &)
{
  size_ = QSize(constants::map::width(), constants::map::height());
  patrolsItem_->changeSize(size_);

  map_->clearEditedPoints();
}

void MapGraphicsView::updateSensorsScale()
{
    if ( currentScale < 0.5 )
        return;

    QVector<SensorPointGraphicsItem *> *v = mSensorModifier_->mSensorGItem();
    for (int i = 0; i < v->size(); i++)
    {
        v->at(i)->setCurrentScale(currentScale);
    }
}

void MapGraphicsView::removeSensor(SensorPointGraphicsItem *item)
{
  int idx = item->getMSensorIdx();
  eos::SensorsSharedPointer sensors = instance_.sensors();
  if (idx < sensors->sensors_size())
  {
    sensors->mutable_sensors()->SwapElements(idx, sensors->sensors_size() - 1);
    sensors->mutable_sensors()->RemoveLast();
  }

  ////Remove the graphic item in the modifier
  mSensorModifier_->mSensorGItem()->removeAt(idx);
  ////update the index of graphic items after the to-be-deleted item
  for (int i = idx; i < mSensorModifier_->mSensorGItem()->size(); i++)
  {
    mSensorModifier_->mSensorGItem()->at(i)->setMSensorIdx(i);
  }
  delete item;
  update();
}

void MapGraphicsView::updateSensor(SensorPointGraphicsItem *item)
{
  eos::Sensor* s = instance_.sensors()->mutable_sensors(item->getMSensorIdx());
  const eos::Sensor& other = item->getConstSensor();
  s->set_type(other.type());
  s->set_name(other.name());
  s->mutable_position()->set_x(other.position().x());
  s->mutable_position()->set_y(other.position().y());
  s->mutable_robot_point()->set_x(other.robot_point().x());
  s->mutable_robot_point()->set_y(other.robot_point().y());
  update();
}

void MapGraphicsView::receive(core::DeleteZone const &event)
{
  eos::ZonesSharedPointer zones = instance_.zones();
  if (event.index < zones->zones().size())
  {
    zones->mutable_zones()->SwapElements(event.index, zones->zones().size() - 1);
    zones->mutable_zones()->RemoveLast();
    QTimer::singleShot(0, this, SLOT(redrawZones()));
  }
}

void MapGraphicsView::redrawZones()
{
  clearZones();
  loadZones();
}

void MapGraphicsView::loadSensors()
{
  if (!instance_.read_only().sensors())
    return;

  //Clear the graphic items if any
  QVector<SensorPointGraphicsItem *> *v = mSensorModifier_->mSensorGItem();
  for (int i = 0; i < v->size(); i++)
  {
    if (v->at(i)->scene() == scene())
    {
      scene()->removeItem(v->at(i));
    }
    delete v->at(i);
  }
  v->clear();
  int i = 0;
  for (int index = 0; index < instance_.read_only().sensors()->sensors().size(); ++index)
  {
    eos::Sensor const &sensor = instance_.sensors()->sensors().Get(index);
    SensorPointGraphicsItem *item = new SensorPointGraphicsItem(sensor, mSensorModifier_);
    item->setMSensorIdx(i);
    item->setAcceptHoverEvents(mode_ == constants::CreateSensors);
    scene()->addItem(item);
    mSensorModifier_->appendGItem(item);
    i++;
  }
  updateSensorsScale();
  update();
}

void MapGraphicsView::loadZones()
{
  if (!instance_.read_only().zones())
  {
   EOS_Error() << "No zones instances ?" << std::endl;
    return;
  }

  for (int index = 0; index < instance_.read_only().zones()->zones().size(); ++index)
  {
    eos::Zone const &zone = instance_.zones()->zones().Get(index);
    QPoint topLeft(
        zone.top_left().x(),
        constants::map::height() - zone.top_left().y());
    QSize size(
        zone.bottom_right().x() - zone.top_left().x(),
        zone.top_left().y() - zone.bottom_right().y());

    //qDebug() << zone.top_left().x() << " " << zone.top_left().y();
    //qDebug() << zone.bottom_right().x() << " " << zone.bottom_right().y();

    ZoneGraphicsItem *zoneItem = new ZoneGraphicsItem(instance_,
                                                      index,
                                                      QSize(constants::map::width(),
                                                            constants::map::height()),
                                                      0);
    zoneItem->setPos(topLeft.x(), topLeft.y());
    zoneItem->setSize(size.width(), size.height());

    scene()->addItem(zoneItem);
  }
}

void MapGraphicsView::contextMenuEvent(QContextMenuEvent *event)
{
  QGraphicsView::contextMenuEvent(event);

  if (mode_ == constants::DistanceMeasurement || constants::AddDetails)
      return;

  if (event->isAccepted() == false)
  {
    std::shared_ptr<QMenu> menu(new QMenu(0));

    QAction *removePoints = 0, *removeZones = 0, *removeSensors = 0;
    if (patrolsItem()->countPoints())
    {
      removePoints = menu->addAction(QObject::tr("Delete all points"));
    }
    if (instance_.zones()->zones_size())
    {
      removeZones = menu->addAction(QObject::tr("Delete all zones"));
    }
    if (instance_.sensors()->sensors_size())
    {
      removeSensors = menu->addAction(QObject::tr("Delete all sensors"));
    }

    // and run the menu !
    QAction *selectedAction = menu->exec(event->globalPos());

    if (selectedAction != 0)
    {
      if (selectedAction == removePoints)
      {
        if (core::question(tr("Be sure to delete all points ?")))
        {
          patrolsItem()->clean();
          emitEvent<core::PatrolsUpdate>();
        }
      }
      if (selectedAction == removeZones)
      {
        if (core::question(tr("Be sure to delete all zones ?")))
        {
          instance_.zones()->Clear();
          instance_.zones()->set_timestamp(0);
          emitEvent<core::ZonesUpdate>();
        }
      }
      else if (selectedAction == removeSensors)
      {
        if (core::question(tr("Be sure to delete all sensors ?")))
        {
          while (instance_.sensors()->sensors_size())
            removeSensor(mSensorModifier_->mSensorGItem()->first());
        }
      }
    }
  }
}
