#ifndef PATROLGRAPHICSITEM_H
#define PATROLGRAPHICSITEM_H

#include <QGraphicsItem>
#include <QVector>
#include <QGraphicsObject>
#include <QGraphicsSvgItem>
#include <QSvgRenderer>

#include "evigilante_configuration/include/eos/configuration/configuration.hpp"
#include "core/appinstance.h"
#include "mapgraphicsview.h"

class QGraphicsTextItem;

class ImageRenderer
{
public:
  ImageRenderer();
  bool isInitialized() const;
  void init(QString const &image_src);
  QImage const &image() const;

private:
  QSvgRenderer *renderer_;
  QImage *image_;
};

class PatrolPointGraphicsItem
    : public QGraphicsItem
{
public:
  explicit PatrolPointGraphicsItem(QGraphicsItem *parent = 0);

  inline static void resetID()
  {
    index_ = 0;
  }

  inline void setPriority(eos::Patrol::Priority level)
  {
      priority_ = level;
  }

  bool isHovered() const
  {
    return hover_;
  }

  int id() const
  {
    return id_;
  }

  static void edit(bool e)
  {
    edition_ = e;
  }

  bool wrong_place = false;

private:
  void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
  virtual QRectF boundingRect() const;
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
  virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
  virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);

  qreal width_, height_;
  bool hover_;
  eos::Patrol::Priority priority_;
  int id_;
  static int index_;
  QPointF diff_;
  QSize size_;
  static bool edition_;
  //eos::MapSharedPointer const &map_;
  QGraphicsSvgItem *item_;
  QSvgRenderer *renderer_;
  static ImageRenderer image_normal_;
  static ImageRenderer image_point_incorrect_;
  static ImageRenderer image_high_priority_;
};

class PatrolGraphicsItem
    : public QGraphicsObject
{
  Q_OBJECT
public:
  PatrolGraphicsItem(core::AppInstance & instance, QSize const &size, MapGraphicsView * view, QGraphicsItem *parent = 0);
  ~PatrolGraphicsItem();

  int countPoints() const;
  int addPoint(QPointF const &point, int priority = eos::Patrol::Priority_MIN, bool makeEvent = true);

  // works like a broker !
  void pointMove(int index, const QPoint &position, PatrolPointGraphicsItem *item);
  void showLabelPosition(const QPoint &position);
  void releasePoint();
  void removePoint(int index);
  void changeIndex(int old, int index);
  void changePriority(int index, int priority);

  void setPatrols(eos::PatrolsSharedPointer patrols);

  void changeSize(QSize const &size);
  QSize size() const;

  bool isHovered() const;

  void removeSelected();

  void setMousePosition(QPointF const &position)
  {
    if (position != mousePosition_)
    {
      mousePosition_ = position;
      update();
    }
  }
  int const &closestPoint() const
  {
    return closestPoint_;
  }

  void clean();

  void resize(QSize const& size);

  void scalePoints(qreal scaleFactor);

signals:
  void sig_newPoint(QPoint const &point);
  void sig_updatePoint(int index, QPoint const &point);
  void sig_removePoint(int index);
  void sig_removePatrol();
  void sig_changeIndex(int old, int index);
  void sig_changePriority(int index, int priority);

public slots:
  void removePatrol();
  void changePatrol(int index);

private:
  void treatLine(QPainter *painter, QPointF const &p1,
                 QPointF const &p2, int index);
  void updatePath();
  virtual QRectF boundingRect() const;
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
  bool validatePoint(QPoint const &point);

  core::AppInstance & instance_;
  QSize size_;
  MapGraphicsView * view_;
  QVector<PatrolPointGraphicsItem *> points_;
  QPointF location_;
  QPointF dragStart_;
  QGraphicsTextItem *position_;
  eos::Patrol::Type currentTypeEditing_;
  QPointF mousePosition_;
  int closestPoint_ = -1;

  qreal currentPointScale_;
};

inline QSize PatrolGraphicsItem::size() const
{
  return size_;
}

inline void PatrolGraphicsItem::changeSize(QSize const &size)
{
  size_ = size;
}

#endif // PATROLGRAPHICSITEM_H
