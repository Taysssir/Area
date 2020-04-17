#ifndef CALENDARBACKGROUNDSCENE_H
#define CALENDARBACKGROUNDSCENE_H

#include <QObject>
#include <QGraphicsScene>

#include "core/appinstance.h"
#include "GraphicsItem/backgrounditem.h"
#include "GraphicsItem/daybackgrounditem.h"
#include "GraphicsItem/weekbackgrounditem.h"
#include "GraphicsItem/monthbackgrounditem.h"
#include "GraphicsItem/listbackgrounditem.h"
#include "GraphicsItem/linetimeitem.h"

class CalendarBackgroundScene : public QGraphicsScene
{
    Q_OBJECT
public:
    typedef enum {ALL_VISUAL,
                  DAY_VISUAL,
                  WEEK_VISUAL,
                  MONTH_VISUAL,
                  LIST_VISUAL,
                  NO_VISUAL
                 }VISUAL_NEEDED;

    typedef enum {DAY,
                  WEEK,
                  MONTH,
                  LIST,
                  NOTHING
                 }VISUAL_SELECTED;

    typedef enum {NEXT = 1,
                  PREVIOUS = -1,
                  STAND = 0,
                  TODAY = 2
                 }SHIFT;

    CalendarBackgroundScene(VISUAL_NEEDED visual_needed ,core::AppInstance &instance, QObject *parent = 0);
    ~CalendarBackgroundScene();

    VISUAL_SELECTED getVisual() const
    {
        return visual_;
    }

    QDate const& getFirstDisplayedDate() const
    {
        return current_scene_item_->getFisrtSemanticDate();
    }

    QDate const& getLastDisplayedDate() const
    {
        return current_scene_item_->getLastSemanticsDate();
    }

    void resize(QRectF rect);
    void switchVisual(VISUAL_SELECTED visual = NOTHING);
    void shiftDate(SHIFT shift, int how_many = 1);
    void changeCurrentDate(QDate date);
    int getRequiredHeight();
    void displayBackgroundUnderDisplayedDate(QVector<QDate> date_vector);
signals:
    void transmitQDateTime(QDateTime);
    void signalUpdateBackgroundDisplayedDate(QVector<QDate>);
protected slots :
    void removeSceneItem(QGraphicsItem* item);
    void updateBackgroundDisplayedDate(QVector<QDate> date_vector){
        emit signalUpdateBackgroundDisplayedDate(date_vector);
    }

protected:
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * mouseEvent);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent);

    virtual QDateTime getDateTimeFromMousePosition(QPointF pos);
    void prepareBackgroundItem(VISUAL_NEEDED visual_needed);

    core::AppInstance &instance_;

    BackgroundItem* current_scene_item_;
    TimeLineItem* time_line_;
    std::shared_ptr <DayBackgroundItem> day_item_;
    std::shared_ptr <WeekBackgroundItem> week_item_;
    std::shared_ptr <MonthBackgroundItem> month_item_;
    std::shared_ptr <ListBackgroundItem> list_item_;
    VISUAL_SELECTED visual_;
    QDate current_date_;
};

#endif // CALENDARBACKGROUNDSCENE_H
