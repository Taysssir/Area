#include <QMap>
#include <QPainter>
#include <QDebug>
#include <math.h>
#include <QFont>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsScene>

#include "ui/dialognewsensor.h"
#include "sensorpointgraphicsitem.h"
#include "core/constants.h"
#include "sentinel-admin_core/constants.h"

#define M_PI1 3.141592

QString iconResource(eos::Sensor::Type type)
{
  static QMap<eos::Sensor::Type, QString> sensor_images;

  if (sensor_images.size() == 0)
  {
    sensor_images[eos::Sensor::RobotDock] = ":/sensors/robotdock";
    sensor_images[eos::Sensor::DoorSwitch] = ":/sensors/door";
    sensor_images[eos::Sensor::Radar] = ":/sensors/radar";
    sensor_images[eos::Sensor::InfraredBarrier] = ":/sensors/barrier";
    sensor_images[eos::Sensor::Rack] = ":/sensors/rack";
  }

  return sensor_images[type];
}

SensorPointGraphicsItem::SensorPointGraphicsItem(eos::Sensor const &sensor, SensorModifier *modifPane)
    : QGraphicsSvgs(iconResource(sensor.type()))
    , selected_(false)
    , mSensorIdx(-1)
    , mIconImage_(QImage(iconResource(sensor.type())))
    , mType_(sensor.type())
    , isize_(mIconImage_.size())
    , mModifier_(modifPane)
    , mCurrentScale_(1.0)
    , mName_(sensor.name().c_str())
    , isMovingIcon_(false)
    , isChangeName_(false)
    , isMovingRobPos_(false)

{
  setPosition(QPointF(sensor.position().x(), sensor.position().y()));
  setZValue(constants::SensorZValue); //to make it display at background
  setAcceptHoverEvents(true);
  //Setup the position
  updateElementPosition(sensor);
}

QRectF SensorPointGraphicsItem::boundingRect() const
{
  //Add space for the image and the text and the delete sign
  QRectF gr = mIconImage_.rect();
  gr.setWidth(gr.width() / mCurrentScale_);
  gr.setHeight(gr.height() / mCurrentScale_);
  QRectF rect(-gr.width() / 2, -gr.height() / 2, gr.width(), gr.height());

  rect = rect.united(mNameRect_);

  //Add space for the delete sign
  rect = rect.united(QRectF(mDelPos_.x() - 10, mDelPos_.y(), 10, 10));

  //add space for robotPoint
  //If the user is moving the robotPoint, return a bigger rectangle for a proper painting
  QRectF roboRect(mRoboPos_.x() - 6, mRoboPos_.y() - 6, 12, 12);
  if (isMovingRobPos_)
  {
    roboRect.adjust(-isize_.width() * 5, -isize_.height() * 5, isize_.width() * 5, isize_.height() * 5);
  }
  rect = rect.united(roboRect);
  return rect;
}

void SensorPointGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  if (!acceptHoverEvents())
    return;

  QPoint ppos = QPoint(event->pos().x(), event->pos().y());

  //If user clicks on the robotPoint, activate the tracking for robotPoint
  if (QRectF(mRoboPos_.x() - 4, mRoboPos_.y() - 4, 9, 9).contains(event->pos()))
  {
    isMovingRobPos_ = true;
    isChangeName_ = false;
    isMovingIcon_ = false;
    return;
  }

  //If user clicks on the icon, activate the tracking for icon
  QRectF gr = mIconImage_.rect();
  gr.translate(-gr.width() / 2, -gr.height() / 2);
  if (gr.contains(ppos))
  {
    isChangeName_ = true;
    isMovingIcon_ = false;
    isMovingRobPos_ = false;
    return;
  }

  //doing nothing when user clicks on regions outside of interest zones
  isChangeName_ = false;
  isMovingIcon_ = false;
  isMovingRobPos_ = false;
  return;
}

void SensorPointGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
  if (!acceptHoverEvents())
    return;

  //Is user click and move the mouse, change to movingIcon mode
  if (isChangeName_)
  {
    isChangeName_ = false;
    isMovingIcon_ = true;
    isMovingRobPos_ = false;
    return;
  }

  //Is user drags the sensor icon, update the whole item
  if (isMovingIcon_)
  {
    qreal x = event->scenePos().x();
    qreal y = constants::map::height() - event->scenePos().y();
    setPosition(QPoint(x, y));
    prepareGeometryChange();
    update();
    return;
  }
  //If user drags the robotPoint, update the point appropriately
  if (isMovingRobPos_)
  {
    //if the point is too close to the sensor, then do nothing
    if (event->pos().manhattanLength() < (30/mCurrentScale_))
      return;
    mRoboPos_ = QPoint(event->pos().x(), event->pos().y());
    prepareGeometryChange();
    update();
    return;
  }
}

void SensorPointGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  if (!acceptHoverEvents())
    return;

  //Is user finish dragging the sensor icon, update the whole position
  if (isMovingIcon_)
  {
    qreal x = event->scenePos().x();
    qreal y = constants::map::height() - event->scenePos().y();
    setPosition(QPoint(x, y));
    mModifier_->updateSensor(this);
    isMovingIcon_ = false;
    prepareGeometryChange();
    update();
    return;
  }

  //If user finish dragging the robotPoint, update the sensor appropriately
  if (isMovingRobPos_)
  {
    mModifier_->updateSensor(this);
    isMovingRobPos_ = false;
    prepareGeometryChange();
    update();
    return;
  }
}

void SensorPointGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent *)
{
  setSelected(true);
  mModifier_->setIsActive(true);
}

void SensorPointGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *)
{
  setSelected(false);
  mModifier_->setIsActive(false);
}

void SensorPointGraphicsItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu *sensorContextMenu = new QMenu();

    QAction *actionEditSensor = sensorContextMenu->addAction(QIcon(":/context_menus/edit"), tr("Edit sensor"));
    QAction *actionDeleteSensor = sensorContextMenu->addAction(QIcon(":/context_menus/delete"), tr("Delete sensor"));

    QAction *selectedAction = sensorContextMenu->exec(event->screenPos());

    if (selectedAction != 0)
    {
      if (selectedAction == actionEditSensor) // Pupup sensor edition dialog
      {
          DialogNewSensor dialogNewSensor(mName_, mType_);
          if (dialogNewSensor.exec() != 0)
          {
              mName_ = dialogNewSensor.getMName();
              mType_ = dialogNewSensor.getMType();
              mIconImage_ = QImage(iconResource((eos::Sensor::Type)mType_));
              isize_ = mIconImage_.size();
              mModifier_->updateSensor(this);
              changePicture(iconResource((eos::Sensor::Type)mType_));
              prepareGeometryChange();
              update();
              updateElementPosition(getConstSensor()); // Just to refresh the name display
          }
          isChangeName_ = false;
      }
      else if (selectedAction == actionDeleteSensor) // Delete the sensor
      {
          isChangeName_ = false;
          isMovingIcon_ = false;
          isMovingRobPos_ = false;
          setAcceptHoverEvents(false);
          mModifier_->deleteSensor(this);
      }
    }

    if (sensorContextMenu != Q_NULLPTR)
        delete sensorContextMenu;
}

bool SensorPointGraphicsItem::getSelected() const
{
  return selected_;
}

void SensorPointGraphicsItem::setSelected(bool value)
{
  selected_ = value;
  prepareGeometryChange();
  update();
}

eos::Sensor SensorPointGraphicsItem::getConstSensor() const
{
  eos::Sensor s;
  s.set_type((eos::Sensor::Type)mType_);
  s.set_name(mName_.toStdString());
  s.mutable_position()->set_x(pos().x());
  s.mutable_position()->set_y(constants::map::height() - pos().y());
  s.mutable_robot_point()->set_x(pos().x() + mRoboPos_.x());
  s.mutable_robot_point()->set_y(constants::map::height() - pos().y() - mRoboPos_.y());
  return s;
}

void SensorPointGraphicsItem::updateElementPosition(eos::Sensor const &sensor)
{
  mDelPos_ = QPoint(isize_.width() / 2 + 25, -15);
  QFont font("Century Gothic", 20.0 / mCurrentScale_);
  QFontMetrics fm(font);
  int pixelsWide = fm.width(mName_);
  int pixelsHigh = fm.height();
  mNameRect_ = QRectF((-isize_.width() / 2 - 10) / mCurrentScale_, (-isize_.height() / 2 - 20) / mCurrentScale_, pixelsWide, pixelsHigh);
  ////mNameRect_ = QRectF(-isize_.width()/2 - 10, -isize_.height()/2 - 25, 2*isize_.width()+10, 50);
  QPointF robotMapPoint = QPointF(sensor.robot_point().x(),sensor.robot_point().y()) - QPointF(sensor.position().x(), sensor.position().y());
  mRoboPos_ = QPoint(robotMapPoint.x(), -robotMapPoint.y());
}
QString SensorPointGraphicsItem::sensor_name() const
{
    return mName_;
}

eos::Sensor::Type SensorPointGraphicsItem::sensor_type() const
{
    return (eos::Sensor::Type)mType_;
}

void SensorPointGraphicsItem::setCurrentScale(double value)
{
    mCurrentScale_ = value;
    //Update the element size
    QRectF gr = mIconImage_.rect();
  gr.setWidth(gr.width() / mCurrentScale_);
  gr.setHeight(gr.height() / mCurrentScale_);
  QRectF rect(-gr.width() / 2, -gr.height() / 2, gr.width(), gr.height());

  mDelPos_ = QPoint(rect.width() / 2 + 25, -15);
  QFont font("Century Gothic", 20.0 / mCurrentScale_);
  QFontMetrics fm(font);
  int pixelsWide = fm.width(mName_);
  int pixelsHigh = fm.height();
  mNameRect_ = QRectF((-isize_.width() / 2 - 10) / mCurrentScale_, (-isize_.height() / 2 - 20) / mCurrentScale_, pixelsWide, pixelsHigh);
}

int SensorPointGraphicsItem::getMSensorIdx() const
{
  return mSensorIdx;
}

void SensorPointGraphicsItem::setMSensorIdx(int value)
{
  mSensorIdx = value;
}

bool SensorPointGraphicsItem::isChangeName() const
{
  return isChangeName_;
}

void SensorPointGraphicsItem::setIsChangeName(bool isChangeName)
{
  isChangeName_ = isChangeName;
}

void SensorPointGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
  //Prepare the painter
  QBrush brush(Qt::red);
  QPen pen;
  if (selected_)
  {
    pen.setColor(QColor(255, 153, 0));
    brush.setColor(QColor(255, 153, 0));
  }
  else
  {
    pen.setColor(Qt::black);
    brush.setColor(Qt::black);
  }
  pen.setWidth(ceil(2.0 / mCurrentScale_));
  pen.setCapStyle(Qt::RoundCap);
  pen.setJoinStyle(Qt::RoundJoin);
  painter->setPen(pen);

  //Draw the sensor's name
  painter->setBrush(brush);
  painter->setPen(pen);
  painter->setFont(QFont("Times", round(11/mCurrentScale_)));
  painter->drawText(mNameRect_, Qt::TextSingleLine, mName_);

  //Draw robotPoint
  painter->setBrush(brush);
  int robotPointSize = ceil(6/mCurrentScale_);
  painter->drawEllipse(mRoboPos_, robotPointSize, robotPointSize);

  //Draw the arrow head
  double deg = atan2(mRoboPos_.y(), mRoboPos_.x()) * 180 / M_PI1;
  QTransform tf;
  tf.rotate(deg);
  QPoint head = tf.map(QPoint(20.0 / mCurrentScale_, 0));
  QLineF cLine = QLineF(head, mRoboPos_);
  painter->drawLine(cLine);
  //Draw arrow mini lines
  const qreal cLineAngle = cLine.angle();
  QLineF miniArrowLine1 = cLine;
  QLineF miniArrowLine2 = cLine;
  miniArrowLine1.setLength( 12 / mCurrentScale_ );
  miniArrowLine1.setAngle( cLineAngle+-32 );
  miniArrowLine2.setLength( 12 / mCurrentScale_ );
  miniArrowLine2.setAngle( cLineAngle+32 );
  painter->drawLine( miniArrowLine1 );
  painter->drawLine( miniArrowLine2 );

  //Draw the sensor image
  QRectF gr = mIconImage_.rect();
  gr.setWidth(gr.width() / mCurrentScale_);
  gr.setHeight(gr.height() / mCurrentScale_);
  gr.translate(-gr.width() / 2, -gr.height() / 2);
  painter->setClipRect(gr);
  QTransform tf1;
  tf1.scale(1.0 / mCurrentScale_, 1.0 / mCurrentScale_);
  painter->setTransform(tf1, true);
  QGraphicsSvgs::paint(painter, option, widget);
}

//*************************************************
//*************************************************
//****** Class SensorModifPanelView ***************
//*************************************************
//*************************************************

SensorModifier::SensorModifier()
    : QObject()
    , isActive_(false)
    , mSensorGItem_(new QVector<SensorPointGraphicsItem *>())

{
}

bool SensorModifier::getIsActive() const
{
  return isActive_;
}

void SensorModifier::setIsActive(bool value)
{
  isActive_ = value;
}

void SensorModifier::appendGItem(SensorPointGraphicsItem *item)
{
  mSensorGItem_->push_back(item);
}

bool SensorModifier::removeGItem(int idx)
{
  Q_ASSERT((idx >= 0) && (idx < mSensorGItem_->size()));
  mSensorGItem_->remove(idx);
  return true;
}

void SensorModifier::toggleModifer(bool isEditing)
{
  for (int i = 0; i < mSensorGItem_->size(); i++)
  {
    mSensorGItem_->at(i)->setAcceptHoverEvents(isEditing);
    mSensorGItem_->at(i)->setZValue(isEditing ? 400 : 40);
  }
}

bool SensorModifier::isHovered()
{
  bool hover = false;
  for (int i = 0; i < mSensorGItem_->size(); i++)
  {
    hover = hover || (mSensorGItem_->at(i)->getSelected() && mSensorGItem_->at(i)->scene() != 0);
  }
  return hover;
}

bool SensorModifier::contains(const QPointF pos)
{
  for (int i = 0; i < mSensorGItem_->size(); i++)
  {
    if (mSensorGItem_->at(i)->scene() == 0)
      continue;
    if (mSensorGItem_->at(i)->contains(pos - mSensorGItem_->at(i)->pos()))
      return true;
  }
  return false;
}
QVector<SensorPointGraphicsItem *> *SensorModifier::mSensorGItem() const
{
  return mSensorGItem_;
}

void SensorModifier::setMSensorGItem(QVector<SensorPointGraphicsItem *> *mSensorGItem)
{
  mSensorGItem_ = mSensorGItem;
}
