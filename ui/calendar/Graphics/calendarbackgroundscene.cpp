#include "calendarbackgroundscene.h"
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>

CalendarBackgroundScene::CalendarBackgroundScene(VISUAL_NEEDED visual_needed, core::AppInstance &instance, QObject* parent): QGraphicsScene(parent), instance_(instance)
{
    time_line_ = new TimeLineItem();
    connect(time_line_,SIGNAL(removeFromScene(QGraphicsItem*)),this,SLOT(removeSceneItem(QGraphicsItem*)));
    this->addItem(time_line_);
    prepareBackgroundItem(visual_needed);
    current_date_ = QDate::currentDate();
    current_scene_item_->setBackgound(this->sceneRect(),current_date_);
    this->addItem(current_scene_item_);
}

CalendarBackgroundScene::~CalendarBackgroundScene()
{

}

void CalendarBackgroundScene::resize(QRectF rect)
{
    current_scene_item_->setBackgound(rect,current_date_);
}


void CalendarBackgroundScene::switchVisual(VISUAL_SELECTED visual)
{
    visual_ = visual;
    QRectF rect =this->views().first()->geometry();
    current_scene_item_->removeHeader();
    current_scene_item_->removeBackground();
    if(current_scene_item_->scene() == this)
        this->removeItem(current_scene_item_);
    switch (visual_) {
    case DAY:
        current_scene_item_ = day_item_.get();
        break;
    case WEEK:
        current_scene_item_ = week_item_.get();
        break;
    case MONTH:
        current_scene_item_ = month_item_.get();
        break;
    case LIST:
        current_scene_item_ = list_item_.get();
        break;
    default:
        break;
    }

    current_scene_item_->setBackgound(rect,current_date_);
    current_scene_item_->clearPatrolOffset();
    this->addItem(current_scene_item_);
}

void CalendarBackgroundScene::shiftDate(SHIFT shift, int how_many)
{
    if(shift == TODAY)
    {
        current_date_ = QDate::currentDate();
        shift = STAND;
        // Clear selected date on month item
        month_item_->setSelectedDate( QDate() );
    }
    switch(visual_)
    {
    case NOTHING:
        break;
    case DAY :
        current_date_ = current_date_.addDays(shift*how_many*1);
        current_scene_item_->setBackgound(current_date_);

        break;
    case LIST :
    case WEEK :
        current_date_ = current_date_.addDays(shift*how_many*constants::calendar::DAYS_IN_WEEK_);
        current_scene_item_->setBackgound(current_date_);

        break;
    case MONTH :
        current_scene_item_->clearPatrolOffset();
        current_date_ = current_date_.addMonths(shift*how_many*1);
        current_scene_item_->setBackgound(current_date_);
        break;
    }
}

void CalendarBackgroundScene::changeCurrentDate(QDate date)
{
    current_date_ = date;
    current_scene_item_->setBackgound(current_date_);
}

void CalendarBackgroundScene::removeSceneItem(QGraphicsItem* item)
{
    if(item->scene() == this)
        this->removeItem(item);
    delete item;
}

QDateTime CalendarBackgroundScene::getDateTimeFromMousePosition(QPointF pos)
{
    return current_scene_item_->getDateTimeFromMousePosition(pos);
}

void CalendarBackgroundScene::prepareBackgroundItem(VISUAL_NEEDED visual_needed)
{
    if (visual_needed == NO_VISUAL)
        return;
    if(visual_needed == ALL_VISUAL || visual_needed == LIST_VISUAL)
    {
        visual_ = LIST;
        list_item_.reset(new ListBackgroundItem(instance_));
        connect(list_item_.get(),SIGNAL(removeFromScene(QGraphicsItem*)),this,SLOT(removeSceneItem(QGraphicsItem*)));
        connect(list_item_.get(),SIGNAL(newDisplayedDate(QVector<QDate>)),this,SLOT(updateBackgroundDisplayedDate(QVector<QDate>)));
        current_scene_item_ = list_item_.get();
    }
    if(visual_needed == ALL_VISUAL || visual_needed == DAY_VISUAL)
    {
        visual_ = DAY;
        day_item_.reset(new DayBackgroundItem(instance_));
        connect(day_item_.get(),SIGNAL(removeFromScene(QGraphicsItem*)),this,SLOT(removeSceneItem(QGraphicsItem*)));
        connect(day_item_.get(),SIGNAL(displayLineTime(QLine)),time_line_,SLOT(setLine(QLine)));
        connect(day_item_.get(),SIGNAL(newDisplayedDate(QVector<QDate>)),this,SLOT(updateBackgroundDisplayedDate(QVector<QDate>)));
        current_scene_item_ = day_item_.get();
    }
    if(visual_needed == ALL_VISUAL || visual_needed == MONTH_VISUAL)
    {
        visual_ = MONTH;
        month_item_.reset(new MonthBackgroundItem(instance_));
        connect(month_item_.get(),SIGNAL(removeFromScene(QGraphicsItem*)),this,SLOT(removeSceneItem(QGraphicsItem*)));
        connect(month_item_.get(),SIGNAL(displayLineTime(QLine)),time_line_,SLOT(setLine(QLine)));
        connect(month_item_.get(),SIGNAL(newDisplayedDate(QVector<QDate>)),this,SLOT(updateBackgroundDisplayedDate(QVector<QDate>)));
        current_scene_item_ = month_item_.get();
    }
    if(visual_needed == ALL_VISUAL || visual_needed == WEEK_VISUAL)
    {
        visual_ = WEEK;
        week_item_.reset(new WeekBackgroundItem(instance_));
        connect(week_item_.get(),SIGNAL(removeFromScene(QGraphicsItem*)),this,SLOT(removeSceneItem(QGraphicsItem*)));
        connect(week_item_.get(),SIGNAL(displayLineTime(QLine)),time_line_,SLOT(setLine(QLine)));
        connect(week_item_.get(),SIGNAL(newDisplayedDate(QVector<QDate>)),this,SLOT(updateBackgroundDisplayedDate(QVector<QDate>)));
        current_scene_item_ = week_item_.get();
    }
}

void CalendarBackgroundScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * ){}
void CalendarBackgroundScene::mouseMoveEvent(QGraphicsSceneMouseEvent * ){}

void CalendarBackgroundScene::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    QDate selectedDate = getDateTimeFromMousePosition(mouseEvent->scenePos()).date();
    if (selectedDate.isValid())
    {
        month_item_->setSelectedDate( getDateTimeFromMousePosition(mouseEvent->scenePos()).date() );
        current_scene_item_->setBackgound(current_date_);
    }

    emit transmitQDateTime(getDateTimeFromMousePosition(mouseEvent->scenePos()));
}

void CalendarBackgroundScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * ){}

int CalendarBackgroundScene::getRequiredHeight()
{
    return current_scene_item_->getRequiredHeight();
}

void CalendarBackgroundScene::displayBackgroundUnderDisplayedDate(QVector<QDate> date_vector)
{
    current_scene_item_->setBackgroundOnSelectedDate(date_vector);
}
