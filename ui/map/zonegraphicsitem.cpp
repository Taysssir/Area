#include <QCursor>
#include <QDebug>
#include <QMenu>
#include <memory>

#include "zonegraphicsitem.h"
#include "mapgraphicsview.h"

Snap::Snap(QGraphicsItem *parent, Snap::Type type)
    : QGraphicsItem(parent)
    , type_(type)
{
  setAcceptHoverEvents(true);
}

Snap::Type Snap::getType() const
{
  return type_;
}

void Snap::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  event->setAccepted(true);
  mouseDown_ = event->pos();
}

void Snap::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
  dynamic_cast<ZoneGraphicsItem *>(parentItem())->mouseMove(event->pos(), type_, mouseDown_);
  event->setAccepted(true);
}

void Snap::hoverEnterEvent(QGraphicsSceneHoverEvent *)
{
  // if(type_ == TopLeft || type_ == BottomRight)
  //     setCursor(QCursor(Qt::SizeFDiagCursor));
  // else
  //     setCursor(QCursor(Qt::SizeBDiagCursor));
}

QRectF Snap::boundingRect() const
{
  return QRectF(0, 0, kSize, kSize);
}

void Snap::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
  QPointF topLeft(0, 0);
  QPointF bottomRight(kSize, kSize);

  QRectF rect(topLeft, bottomRight);

  QBrush brush(QColor(20, 20, 20, 80));
  painter->fillRect(rect, brush);
}

bool ZoneGraphicsItem::edit_ = false;
bool ZoneGraphicsItem::hover_ = false;
int ZoneGraphicsItem::selectedZone_ = -1;

ZoneGraphicsItem::ZoneGraphicsItem(core::AppInstance &instance, int zone, QSize const &mapSize, QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , core::EventClient<ZoneGraphicsItem>(instance)
    , dragStart_(0, 0)
    , width_(0)
    , height_(0)
    , mapSize_(mapSize)
    , zone_(zone)
    , nomovement_ (":/map/nomovement", this)
    , nodetection_(":/map/nodetection", this)
    , noremapping_(":/map/noremapping", this)
    , datacenter_ (":/map/datacenter", this)
{
    setAcceptHoverEvents(true);

    corners_[0] = new Snap(this, Snap::TopLeft);
    corners_[1] = new Snap(this, Snap::TopRight);
    corners_[2] = new Snap(this, Snap::BottomRight);
    corners_[3] = new Snap(this, Snap::BottomLeft);

    for (int i = 0; i < 4; i++)
        corners_[i]->hide();

    setCornerPositions();
    //updateZone();
    setZValue(constants::ZoneZValue);

    eos::Zone const &the_zone = instance_.zones()->zones().Get(zone_);

    nomovement_.hide();
    nodetection_.hide();
    noremapping_.hide();

    if (the_zone.permissions() == eos::Zone::NoMovement)
        nomovement_.show();
    else if (the_zone.permissions() == eos::Zone::NoDetection)
        nodetection_.show();
    else if (the_zone.permissions() == eos::Zone::NoRemapping)
        noremapping_.show();
    else if (the_zone.permissions() == eos::Zone::Nothing)
        datacenter_.show();
    else if (the_zone.permissions() == eos::Zone::Map)
        map_.show();
}

void ZoneGraphicsItem::adjustSize(int x, int y)
{
  width_ += x;
  height_ += y;
  updateZone();
}

void ZoneGraphicsItem::setSize(int x, int y)
{
  width_ = x;
  height_ = y;

  setCornerPositions();

  updateZone();
}

void ZoneGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  //event->setAccepted(false);
  QGraphicsItem::mouseReleaseEvent(event);
}

void ZoneGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  QGraphicsItem::mousePressEvent(event);
  if (edit_ == false) return;
  if (event->buttons() & Qt::LeftButton)
  {
    event->setAccepted(true);
    dragStart_ = event->pos();
  }
  else
    event->setAccepted(false);
}

void ZoneGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
  QGraphicsItem::mouseMoveEvent(event);
  if (edit_ == false) return;
  QPointF newPos = event->pos();
  QPointF l = pos() + (newPos - dragStart_);
  setPos(l);
  updateZone();
}

void ZoneGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
  QGraphicsItem::hoverLeaveEvent(event);
  hover_ = false;
  selectedZone_ = -1;
  if (edit_ == false) return;
  event->setAccepted(true);
  for (int i = 0; i < 4; i++)
    corners_[i]->hide();
}

void ZoneGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
  QGraphicsItem::hoverEnterEvent(event);
  hover_ = true;
  selectedZone_ = zone_;
  if (edit_ == false) return;
  event->setAccepted(true);
  //setCursor(QCursor(Qt::SizeAllCursor));
  for (int i = 0; i < 4; i++)
    corners_[i]->show();
}

void ZoneGraphicsItem::setCornerPositions()
{
  corners_[0]->setPos(0, 0);
  corners_[1]->setPos(width_ - Snap::kSize, 0);
  corners_[3]->setPos(0, height_ - Snap::kSize);
  corners_[2]->setPos(width_ - Snap::kSize, height_ - Snap::kSize);

  nodetection_.setPos(width_ / 2 - nodetection_.boundingRect().width() - kPadding,
                      height_ / 2 - nodetection_.boundingRect().height() / 2);
  nomovement_.setPos(width_ / 2 + kPadding,
                     height_ / 2 - nomovement_.boundingRect().height() / 2);
  noremapping_.setPos(width_ / 2 - noremapping_.boundingRect().width() / 2,
                       height_ / 2 - noremapping_.boundingRect().height() / 2);

  datacenter_.setPos(width_ / 2 - noremapping_.boundingRect().width() / 2,
                       height_ / 2 - noremapping_.boundingRect().height() / 2);

  map_.setPos(width_ / 2 - noremapping_.boundingRect().width() / 2,
                       height_ / 2 - noremapping_.boundingRect().height() / 2);


}

QRectF ZoneGraphicsItem::boundingRect() const
{
  return QRectF(0, 0, width_, height_);
}

void ZoneGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
  //Q_ASSERT(zone_ >= 0);
  //Zone zone = instance_.zones()->const_at(zone_);

  painter->setPen(QColor(236, 136, 6));

  QColor color(236, 106, 16, 55);

  QRectF rect(0, 0, width_, height_);
  painter->setBrush(color);
  painter->drawRect(rect);
  //if(zone.permissions() & Zone::NoMovement)
  //    painter->drawText( boundingRect(), QObject::tr("Zone de non mouvement"));
  //rect.setY(rect.y() + 20);
  //if(zone.permissions() & Zone::NoDetection)
  //    painter->drawText( rect, QObject::tr("Zone de non détection"));
}

void ZoneGraphicsItem::mouseMove(QPointF const &point, Snap::Type type, QPointF const &mouseDown)
{
  qreal x = point.x();
  qreal y = point.y();

  int XaxisSign = 0;
  int YaxisSign = 0;

  if (type == Snap::TopLeft || type == Snap::BottomLeft)
    XaxisSign = 1;
  else
    XaxisSign = -1;

  if (type == Snap::TopLeft || type == Snap::TopRight)
    YaxisSign = 1;
  else
    YaxisSign = -1;

  int xMoved = mouseDown.x() - x;
  int yMoved = mouseDown.y() - y;

  int newWidth = width_ + (XaxisSign * xMoved);
  if (newWidth < kMinSizeSide) newWidth = kMinSizeSide;

  int newHeight = height_ + (YaxisSign * yMoved);
  if (newHeight < kMinSizeSide) newHeight = kMinSizeSide;

  int deltaWidth = newWidth - width_;
  int deltaHeight = newHeight - height_;

  adjustSize(deltaWidth, deltaHeight);

  deltaWidth *= (-1);
  deltaHeight *= (-1);

  if (type == Snap::TopLeft)
  {
    int newXpos = this->pos().x() + deltaWidth;
    int newYpos = this->pos().y() + deltaHeight;
    this->setPos(newXpos, newYpos);
  }
  else if (type == Snap::TopRight)
  {
    int newYpos = this->pos().y() + deltaHeight;
    this->setPos(this->pos().x(), newYpos);
  }
  else if (type == Snap::BottomLeft)
  {
    int newXpos = this->pos().x() + deltaWidth;
    this->setPos(newXpos, this->pos().y());
  }

  setCornerPositions();

  updateZone();
  scene()->update();
}

void ZoneGraphicsItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if (edit_ == false)
    {
        event->setAccepted(false);
        return;
    }
    Q_ASSERT(zone_ >= 0);
    Q_ASSERT(zone_ < instance_.zones()->zones().size());
    eos::Zone *zone = instance_.zones()->mutable_zones()->Mutable(zone_);
    // this is the main menu
    std::shared_ptr<QMenu> menu(new QMenu(0));

    //// the last on is simple : remove the point !
    QAction *noMovementAction = menu->addAction(QIcon(":/map/nomovement"), QObject::tr("No movement"));
    noMovementAction->setCheckable(true);
    noMovementAction->setChecked(zone->permissions() == eos::Zone::NoMovement);

    QAction *noDetectionAction = menu->addAction(QIcon(":/map/nodetection"), QObject::tr("No detection"));
    noDetectionAction->setCheckable(true);
    noDetectionAction->setChecked(zone->permissions() == eos::Zone::NoDetection);

    QAction *noRemappingAction = menu->addAction(QIcon(":/map/noremapping"), QObject::tr("No remapping"));
    noRemappingAction->setCheckable(true);
    noRemappingAction->setChecked(zone->permissions() == eos::Zone::NoRemapping);

    QAction *datacenterZone = menu->addAction(QIcon(":/map/noremapping"), QObject::tr("datacenter zone"));
    datacenterZone->setCheckable(true);
    datacenterZone->setChecked(zone->permissions() == eos::Zone::Nothing);

    QAction *mapZone = menu->addAction(QIcon(":/map/noremapping"), QObject::tr("map zone"));
    mapZone->setCheckable(true);
    mapZone->setChecked(zone->permissions() == eos::Zone::Map);

    QAction *removeAction = menu->addAction(QIcon(":/context_menus/delete"), QObject::tr("Delete"));

    // and run the menu !
    QAction *selectedAction = menu->exec(event->screenPos());

    if (selectedAction != 0)
    {
        if (selectedAction == removeAction)
            emitEvent<core::DeleteZone>(zone_);
        else if (selectedAction == noMovementAction)
            zone->set_permissions(eos::Zone::NoMovement);
        else if (selectedAction == noDetectionAction)
            zone->set_permissions(eos::Zone::NoDetection);
        else if (selectedAction == noRemappingAction)
            zone->set_permissions(eos::Zone::NoRemapping);
        else if (selectedAction == datacenterZone)
            zone->set_permissions(eos::Zone::Nothing);
        else if (selectedAction == mapZone)
            zone->set_permissions(eos::Zone::Map);
    }

    nodetection_.hide();
    nomovement_.hide();
    noremapping_.hide();
    datacenter_.hide();
    map_.hide();

    if (zone->permissions() == eos::Zone::NoDetection)
        nodetection_.show();
    else if (zone->permissions() == eos::Zone::NoMovement)
        nomovement_.show();
    else if (zone->permissions() == eos::Zone::NoRemapping)
        noremapping_.show();
    else if (zone->permissions() == eos::Zone::Map)
        map_.show();
    else if (zone->permissions() == eos::Zone::Nothing)
        datacenter_.show();
}

void ZoneGraphicsItem::updateZone()
{
  // two place to setup the zone : not good idea
  eos::Zone *zone = instance_.zones()->mutable_zones()->Mutable(zone_);
  if (zone)
  {
    zone->mutable_top_left()->set_x(pos().x());
    zone->mutable_top_left()->set_y(constants::map::height() - pos().y());
    zone->mutable_bottom_right()->set_x(zone->top_left().x() + width_);
    zone->mutable_bottom_right()->set_y(constants::map::height() - pos().y() - height_);
  }
}
