#include <QBrush>
#include <QDebug>
#include <QMenu>
#include <QAction>
#include <QPainter>
#include <QGraphicsItem>
#include <QBitmap>
#include <QFont>
#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QCursor>
#include <memory>
#include <QStyleOptionGraphicsItem>

#include "core/helper.h"
#include "patrolgraphicsitem.h"
#include "core/constants.h"
#include "sentinel-admin_core/constants.h"
#include "sentinel-common/helper/map.h"

int PatrolPointGraphicsItem::index_ = 0;
bool PatrolPointGraphicsItem::edition_ = false;

ImageRenderer::ImageRenderer()
    : renderer_(0)
    , image_(0)
{
}

bool ImageRenderer::isInitialized() const
{
  return static_cast<bool>(image_);
}

void ImageRenderer::init(QString const &image_src)
{
  if (isInitialized() == false)
  {
    renderer_ = new QSvgRenderer(QString(image_src));
    image_ = new QImage(50, 50, QImage::Format_ARGB32);
    image_->fill(0x00000000);
    QPainter painter(image_);
    renderer_->render(&painter);
  }
}

QImage const &ImageRenderer::image() const
{
  return *image_;
}

ImageRenderer PatrolPointGraphicsItem::image_normal_;
ImageRenderer PatrolPointGraphicsItem::image_point_incorrect_;
ImageRenderer PatrolPointGraphicsItem::image_high_priority_;

PatrolPointGraphicsItem::PatrolPointGraphicsItem(QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , width_(40)
    , height_(20)
    , hover_(false)
    , priority_(eos::Patrol::Standard)
{
  image_normal_.init(":/map/point");
  image_point_incorrect_.init(":/map/point_incorrect");
  image_high_priority_.init(":/map/point_high");

  setCacheMode(QGraphicsItem::NoCache);
  setAcceptHoverEvents(true);
  setParentItem(parent);
  setZValue(constants::PatrolZValue);
  id_ = index_++;
  size_ = dynamic_cast<PatrolGraphicsItem *>(parentItem())->size();
}

void PatrolGraphicsItem::resize(QSize const& size)
{
    size_ = size;
}

void PatrolGraphicsItem::scalePoints(qreal scaleFactor)
{
    currentPointScale_ = 1/scaleFactor;

    for (int i = 0; i < points_.size(); i++)
    {
        points_.at(i)->setScale(currentPointScale_);
    }
}

PatrolGraphicsItem::~PatrolGraphicsItem()
{
}

QRectF PatrolPointGraphicsItem::boundingRect() const
{
  return QRectF(-width_, -height_ * 2, width_, height_ * 2);
}

void PatrolPointGraphicsItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
  if (edition_ == false) return;
  // this is the main menu
  std::shared_ptr<QMenu> menu(new QMenu(0));

  // this part is to manage priority
  std::shared_ptr<QMenu> subPrio(new QMenu(0));
  QAction *priority = menu->addAction(QIcon(":/context_menus/change_priority"), QObject::tr("Choose priority"));
  QActionGroup actionGroup(subPrio.get());
  actionGroup.setExclusive(true);
  // for(int i=1;i<=Patrol::kNumberOfPriority;i++)
  //{
  QAction *action_priority_normal = subPrio->addAction(QIcon(":/context_menus/normal_priority"), QObject::tr("Normal priority"));
  action_priority_normal->setCheckable(true);
  action_priority_normal->setData(eos::Patrol::Standard);
  action_priority_normal->setChecked(priority_ == eos::Patrol::Standard);

  QAction *action_priority_high = subPrio->addAction(QIcon(":/context_menus/high_priority"), QObject::tr("High priority"));
  action_priority_high->setCheckable(true);
  action_priority_high->setData(eos::Patrol::High);
  action_priority_high->setChecked(priority_ == eos::Patrol::High);
  //}
  priority->setMenu(subPrio.get());

  // and this one to change the ID
  QVector<QAction *> IDS;
  QAction *changeID = nullptr;
  std::shared_ptr<QMenu> subID(new QMenu(0));
  if (index_ > 1)
  {
    changeID = menu->addAction(QIcon(":/context_menus/change_index"), QObject::tr("change_index"));
    for (int i = 0; i < index_; i++)
    {
      if (i == id_) continue;
      QAction *action = subID->addAction(QString::number(i));
      action->setData(i);
      IDS.push_back(action);
    }
    changeID->setMenu(subID.get());
  }

  // the last on is simple : remove the point !
  QAction *removeAction = menu->addAction(QIcon(":/context_menus/delete"), QObject::tr("delete"));

  // and run the menu !
  QAction *selectedAction = menu->exec(event->screenPos());

  if (selectedAction != 0)
  {
    if (selectedAction == removeAction)
    {
      // remove point
      dynamic_cast<PatrolGraphicsItem *>(parentItem())->removePoint(id_);
    }
    else if (selectedAction == changeID)
    {
    }
    else if (selectedAction == priority)
    {
    }
    else if (IDS.contains(selectedAction))
    {
      int index = selectedAction->data().toInt();
      if (index >= 0 && index < index_)
      {
        // change index
        dynamic_cast<PatrolGraphicsItem *>(parentItem())->changeIndex(id_, index);
      }
    }
    else
    {
      // change priority
      priority_ = static_cast<eos::Patrol::Priority>(selectedAction->data().toInt());
      dynamic_cast<PatrolGraphicsItem *>(parentItem())->changePriority(id_, priority_);
    }
  }
}

void PatrolPointGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
  Q_UNUSED(widget);
  Q_UNUSED(option);
  QRectF rect(QPointF(-width_, -height_), QPointF(width_, height_));

  QBrush brush(Qt::SolidPattern);

  QColor color;

  //    static QColor colors[3] = {
  //        QColor(98, 229, 26, 255),
  //        QColor(255, 114, 0, 255),
  //        QColor(244, 50, 30, 255)
  //    };

  static QColor colors[3] = {
    QColor(255, 255, 255, 255),
    QColor(255, 114, 0, 255),
    QColor(244, 50, 30, 255)
  };

  color = colors[priority_ - 1];

  if (hover_)
    color = color.lighter();

  brush.setColor(color);

  painter->setBrush(brush);
  QPen pen;
  pen.setColor(Qt::red);
  pen.setWidth(4);
  painter->setPen(pen);
  //painter->drawRoundedRect(rect, 10, 10);
  //painter->drawEllipse(QRect(0,0,2,2));
  QFont font = painter->font();
  font.setPointSize(font.pointSize() * 1.7);
  font.setFamily("Century Gothic");
  // if(map_->isAuthorized( pos() ) == false)
  // pen.setColor(QColor(255, 50, 50, 200));
  // else
  pen.setColor(Qt::white);
  painter->setPen(pen);
  painter->setFont(font);
  //rect.adjust(15, 15, 15, 15);
  //painter.drawText(5, 50 - (5*(i+1)) - 10, 20, 30, Qt::AlignHCenter | Qt::AlignVCenter, strNumber);
  //renderer_->render(painter);
  painter->translate(-20, 4);

   if(wrong_place)
    painter->drawImage(-image_point_incorrect_.image().width() / 2,
                       -image_point_incorrect_.image().height(),
                       image_point_incorrect_.image());
   else if (priority_ == eos::Patrol::Priority_MAX)
    painter->drawImage(-image_high_priority_.image().width() / 2,
                       -image_high_priority_.image().height(),
                       image_high_priority_.image());
   else
    painter->drawImage(-image_normal_.image().width() / 2,
                       -image_normal_.image().height(),
                       image_normal_.image());

  if (id_ < 10)
    painter->translate(-2, -27);
  else
    painter->translate(-2, -27);

  if (wrong_place)
  {
    pen.setColor(Qt::white);
    painter->setPen(pen);
  }
  painter->drawText(rect, QString::number(id_), Qt::AlignHCenter | Qt::AlignVCenter);
}

void PatrolPointGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
  Q_UNUSED(event);
  hover_ = true;
  //update();
  if (!edition_) return;

  QPoint position(pos().x(), pos().y());
  dynamic_cast<PatrolGraphicsItem *>(parentItem())->showLabelPosition(position);

  QGraphicsItem::hoverEnterEvent(event);
}

void PatrolPointGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
  Q_UNUSED(event);
  hover_ = false;
  //update();
  //parentItem()->update();
  if (!edition_) return;

  dynamic_cast<PatrolGraphicsItem *>(parentItem())->releasePoint();
  QGraphicsItem::hoverLeaveEvent(event);
}

void PatrolPointGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
  if (!edition_) return;
  if (event->buttons() & Qt::LeftButton)
  {
    event->setAccepted(true);
    QPointF pt = mapToScene(event->pos());
    QPointF tempPosition = pt - diff_;
    //        if(map_->isAuthorized( tempPosition ) == true)
    {
      if (tempPosition.x() > 0
          && tempPosition.y() > 0
          && tempPosition.x() < size_.width()
          && tempPosition.y() < size_.height())
      {
        setPos(tempPosition);
        QPoint position(pos().x(), pos().y());
        dynamic_cast<PatrolGraphicsItem *>(parentItem())->pointMove(id_, position, this);
      }
    }
  }
  else
    event->setAccepted(false);
}

void PatrolPointGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  if (!edition_) return;
  QPointF pt = mapToScene(event->pos());
  diff_ = pt - pos();
  event->setAccepted(true);
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

PatrolGraphicsItem::PatrolGraphicsItem(core::AppInstance & instance, const QSize &size, MapGraphicsView *view, QGraphicsItem *parent)
    : QGraphicsObject(parent)
    , instance_(instance)
    , size_(size)
    , view_(view)
    , location_(0, 0)
    , dragStart_(0, 0)
    , currentPointScale_ (1.0)
{
  setCacheMode(QGraphicsItem::NoCache);
  setAcceptHoverEvents(false);
  position_ = new QGraphicsTextItem(this);
  position_->setDefaultTextColor(QColor(240,127,16));
  setZValue(constants::PatrolZValue);
  position_->hide();
  position_->setFlag(QGraphicsItem::ItemIgnoresTransformations);
}

QRectF PatrolGraphicsItem::boundingRect() const
{
  return QRectF(0, 0, size_.width(), size_.height());
}

namespace
{
  bool hitTest(QLineF const &baseLine, const QPointF &point, qreal margin = 10)
  {
    QPointF intersectPnt;
    QLineF line(point.x() - margin, point.y() - margin, point.x() + margin, point.y() + margin);

    return (baseLine.intersect(line, &intersectPnt) == QLineF::BoundedIntersection);
  }
}

void PatrolGraphicsItem::treatLine(QPainter *painter, QPointF const &p1,
                                   QPointF const &p2, int index)
{
  // it's not clean to run an algo directly in paint method
  // but for today it would be ok
  painter->setPen(QPen(QColor(120, 120, 120), 5*currentPointScale_, Qt::DashLine, Qt::FlatCap, Qt::MiterJoin));
  if (closestPoint_ == -1)
  {
    if (hitTest(QLineF(p1, p2), mousePosition_))
    {
      painter->setPen(QPen(QColor(255, 138, 0), 5*currentPointScale_, Qt::DashLine, Qt::FlatCap, Qt::MiterJoin));
      closestPoint_ = index;
    }
  }
  painter->drawLine(p1, p2);
}

void PatrolGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
  closestPoint_ = -1;
  painter->setClipRect(option->exposedRect);
  painter->setPen(QPen(QColor(120, 120, 120), 5*currentPointScale_, Qt::DashLine, Qt::FlatCap, Qt::MiterJoin));
  int index = -1;
  if (points_.size())
  {
    QPointF u = points_[0]->pos();
    for (auto const &point : points_)
    {
      treatLine(painter, u, point->pos(), index);
      u = point->pos();
      index++;
    }

    if (currentTypeEditing_ == eos::Patrol::Normal && points_.size() > 2)
    {
      treatLine(painter,
                points_[points_.size() - 1]->pos(),
                points_[0]->pos(), index);
    }
  }
}

bool PatrolGraphicsItem::validatePoint(const QPoint &point)
{
    const QImage &img = view_->map()->pixmap().toImage();

    if ( img.valid(point) )
        return (img.pixel(point) == QColor(Qt::white).rgb());
    return false;
}

int PatrolGraphicsItem::countPoints() const
{
  return points_.size();
}

void PatrolGraphicsItem::removePatrol()
{
  for (auto point : points_)
  {
    delete point;
  }
  points_.clear();

  PatrolPointGraphicsItem::resetID();
  update();
}

void PatrolGraphicsItem::changePatrol(int index)
{
  removePatrol();
  if (index < instance_.patrols()->patrols().size())
  {
    eos::Patrol patrol = instance_.patrols()->patrols().Get(index);
    currentTypeEditing_ = patrol.type();
    for (int i = 0; i < patrol.points().size(); ++i)
    {
      eos::Patrol::Point point = patrol.points().Get(i);
      addPoint(QPoint(point.position().x(),
                      constants::map::height() - point.position().y()),
               point.priority(), false);
    }
  }
}

void PatrolGraphicsItem::showLabelPosition(const QPoint &position)
{
  // set the position of the x, y label
  position_->setPos(position );
  QString posTxt = tr("Position : (%1,%2)").arg(position.x()).arg(position.y());
  position_->setHtml("<div style='background-color:#262626;'><b>" + posTxt + "</b></div>");
  position_->setZValue(constants::PositionStringZValue);
  position_->show();
  update();
}

void PatrolGraphicsItem::pointMove(int index, QPoint const &position, PatrolPointGraphicsItem * item)
{
  if(instance_.configuration().base_map() && item)
    item->wrong_place = !validatePoint(position);
  showLabelPosition(position);
  emit sig_updatePoint(index, QPoint(position.x(), size_.height() - position.y()));
  //update();
}

void PatrolGraphicsItem::releasePoint()
{
  position_->hide();
  update();
}

int PatrolGraphicsItem::addPoint(QPointF const &point, int priority, bool makeEvent)
{
  if (makeEvent)
    emit sig_newPoint(QPoint(point.x(), constants::map::height() - point.y()));
  PatrolPointGraphicsItem *ppgi = new PatrolPointGraphicsItem(this);
  points_.push_back(ppgi);
  ppgi->setPos(point);
  ppgi->setScale(currentPointScale_);
  ppgi->setPriority(static_cast<eos::Patrol::Priority>(priority));
  if(instance_.configuration().base_map() && ppgi)
    ppgi->wrong_place = !validatePoint(point.toPoint());
  update();
  return ppgi->id();
}

void PatrolGraphicsItem::removePoint(int index)
{
  emit sig_removePoint(index);
  update();
}

void PatrolGraphicsItem::changeIndex(int old, int index)
{
  emit sig_changeIndex(old, index);
  update();
}

void PatrolGraphicsItem::changePriority(int index, int priority)
{
  emit sig_changePriority(index, priority);
  update();
}

bool PatrolGraphicsItem::isHovered() const
{
  for (auto const &point : points_)
  {
    if (point->isHovered())
      return true;
  }
  return false;
}

void PatrolGraphicsItem::removeSelected()
{
  for (auto const &point : points_)
  {
    if (point->isHovered())
    {
      removePoint(point->id());
      return;
    }
  }
}

void PatrolGraphicsItem::clean()
{
    for (int i = points_.size()-1; i >= 0; i--)
    {
        emit sig_removePoint(points_.at(i)->id());
    }
    update();
}
