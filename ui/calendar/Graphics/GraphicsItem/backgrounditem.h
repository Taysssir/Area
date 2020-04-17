#ifndef BACKGROUNDITEM_H
#define BACKGROUNDITEM_H

#include <QGraphicsObject>
#include <QGraphicsItem>
#include <QColor>
#include <QObject>
#include <QSize>
#include <QFont>

#include "core/appinstance.h"
#include "../../contants_calendar.h"

class BackgroundItem : public QGraphicsObject
{
    Q_OBJECT
public:
    BackgroundItem(core::AppInstance &instance, QGraphicsItem *parent  = 0);
    ~BackgroundItem();

    /**
     * @brief setBackgound
     * This method is used to call the method to draw the complete background (Header, Line, Rect) with the right color at the right Place
     * @param rect - the float Rectangle from the visual. If you have a small place, the rect will be smaller and if you have a big place, the rect will be bigger.
     * @param date - the choosen date to draw. this date will be contained in the semantics_date_ after the call of this method.
     */
    virtual void setBackgound(QRectF rect = QRect(), QDate date = QDate());
    /**
     * @brief setBackgound
     * This method is an ovewrite of the setBackgound(QRectF rect = QRect(), QDate date = QDate());. It will call it after
     * @param date - the choosen date to draw. this date will be contained in the semantics_date_ after the call of this method.
     */
    virtual void setBackgound(QDate date)
    {
        setBackgound(QRect(),date);
    }

    /**
     * @brief getFisrtSemanticDate
     * semantics_date_ is a vector of all the displayed date.
     * @return the first semantics_date_ displayed.
     */
    QDate const& getFisrtSemanticDate() const
    {
        return semantics_date_.first();
    }

    /**
     * @brief getLastSemanticsDate
     * semantics_date_ is a vector of all the displayed date.
     * @return the last semantics_date_ displayed.
     */
    QDate const& getLastSemanticsDate() const
    {
        return semantics_date_.last();
    }

    /**
     * @brief removeHeader
     * this method remove header child item from this backgroundItem and emit a signal : removeFromScene(item) to ask to clear the item from scene.
     * From Qt doc : If you remove a child from his parent. The child will not be removed from the scene. So you have to clear it manually.
     */
    virtual void removeHeader();

    /**
     * @brief removeBackground
     * this method remove line and rect child item from this backgroundItem and emit a signal : removeFromScene(item) to ask to clear the item from scene.
     * From Qt doc : If you remove a child from his parent. The child will not be removed from the scene. So you have to clear it manually.
     */
    virtual void removeBackground();

    /**
     * @brief getDateTimeFromMousePosition
     * this method return a QDateTime from the mouse position.
     * @param pos - is a QPointF from a scene position not a application position.
     * @return the QDateTime associate to the scene position. /!\ It can return a QDateTime() if the position is not in the scene.
     */
    virtual QDateTime getDateTimeFromMousePosition(QPointF pos) =0;

    /**
     * @brief getRectFromQDateTimes
     * this method have to return all the QRectF in a vector between to QDateTime. The aim of the method is to give back the task rectangles to draw ONLY ON the CURRENT DISPLAY VIEW.
     * Exemple: if we are in a week view and the 2 QDateTime are note in the same day, you will have more than one rectangle to draw to draw your patrol.
     * @param start - QDateTime( start_date, start_time ) of the event. Exemple :QDateTime( QDate(2,1,2016), QTime(16:00))
     * @param stop - QDateTime( stop_date, stop_time ) of the event. Exemple : QDateTime( QDate(2,3,2016), QTime(12:00))
     * @return QVector<QRectF> containing all the part of the task to draw. Exemeple : In a Weekview, that will return 3 rectangles.
     *          /!\ the method can return a empty vector if the task have no need to be displayed.
     */
    virtual QVector<QRectF> getRectFromQDateTimes(QDateTime start, QDateTime stop) = 0;

    /**
     * @brief getRequiredHeight
     * This method return the minimum height to respect to fix the size of the scene.
     * @return  return the minimum height to apply the constraint
     */
    virtual int getRequiredHeight() = 0;

    /**
     * @brief clearPatrolOffset
     * this method is used by the month view, in order to not display two patrol One on another.
     * Furthermore, If we go in the principe that you can more than One event at one time. This method is the right one to developpe in the day and week scene.
     */
    virtual void clearPatrolOffset(){}

    /**
     * @brief setBackgroundOnSelectedDate
     * this method is used to add a visual colorful hightlight on the display about the given dates in the QVector.
     * Actually the Current Day is not display by this way. But we can imagine to modify this method and do it at place of the setBackgroundDesign() method.
     * In addition, this method is at this moment (12/02/2016) only done for the month view. If you need for another view, don't hesitate to dev. it.
     * @param date_vector : all the date where you want your hightlight.
     */
    virtual void setBackgroundOnSelectedDate(QVector<QDate> /*date_vector*/){}
signals :
    /**
     * @brief removeFromScene
     * this signal is emit when this class need to delete graphicsItem/graphicsObject from the scene.
     * @param item
     */
    void removeFromScene(QGraphicsItem * item);

    /**
     * @brief newDisplayedDate
     * This signal is emit when the semantics_date_ is update. You have to catch it when you need to add highlight on some dates.
     * cf : setBackgroundOnSelectedDate(QVector<QDate> date_vector)
     */
    void newDisplayedDate(QVector<QDate>);
    void displayLineTime(QLine);
protected :

    typedef enum {BEGIN, END}FITIN_OPTION;

    virtual QRectF boundingRect() const;
    virtual void paint (QPainter */*painter*/, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/) {}
    /**
     * @brief setBackgroundDesign
     * This method will add to the parent, all the line and rect item necessary to display a day/week/month calendar
     * @param rect -  the float Rectangle from the visual. If you have a small place, the rect will be smaller and if you have a big place, the rect will be bigger.
     */
    virtual void setBackgroundDesign(QRectF rect) = 0;

    /**
     * @brief setHeader
     * this method will add to the parent, all the text header as date or time.
     * @param date - the choosen date to draw. this date will be contained in the semantics_date_ after the call of this method.
     */
    virtual void setHeader(QDate date) = 0;

    /**
     * @brief getSemanticsDisplayed
     * prepare all the display date from one date.
     * @param date - current date
     */
    virtual void getSemanticsDisplayed(QDate const& date) = 0;

    /**
     * @brief fitinCalendarDisplayed
     * this method is used in order to fit a given date in the current displayed (semantics_date_) dates
     * /!\ this method work only when you have already set the semantics_date_;
     * @param date : the given date to fit in the display
     * @param option : FITIN_OPTION if you want to verify the first or the last displayed date.
     * @param special_selected_day  : If you want to use another day than the semantics_date_
     * @return QDateTime of the first displayed date, or the given date or the last displayed date.
     * /!\ If the given date isn't valid that return a QDateTime();
     */
    QDateTime fitinCalendarDisplayed(QDateTime date, FITIN_OPTION option, QDateTime special_selected_day = QDateTime());



    QSize time_column_size_, column_size_, header_size_;
    QVector<QGraphicsTextItem*> text_header_;
    QVector<QGraphicsItem*> background_item_;
    QVector<QDate> semantics_date_;
    core::AppInstance &background_instance_;
    QRectF bounding_rect_;
    QGraphicsLineItem* time_line_item_;
};

#endif // BACKGROUNDITEM_H
