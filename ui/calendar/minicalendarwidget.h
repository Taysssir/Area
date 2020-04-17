#ifndef MINICALENDARWIDGET_H
#define MINICALENDARWIDGET_H

#include <QWidget>
#include <QGraphicsView>
#include <QLayout>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include "Graphics/calendarbackgroundscene.h"
#include "core/appinstance.h"
class MiniCalendarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MiniCalendarWidget(core::AppInstance &instance, QWidget *parent = 0);
    ~MiniCalendarWidget();

signals:
    void updateCalendarView(QDateTime const&);
public slots:
    void previous_clicked();
    void next_clicked();
    void display_today();
    void updateMonthAndYear(QDate const& date);
    void updateMonth(int index);
    void updateYear(int value);
    void updateCalendarViewEvent(QDateTime const& date);
    void updateDisplayedDateFromBiggerCalendar(QVector<QDate> const& date_vector);

protected slots:
    void resizeEvent(QResizeEvent * event);

private :
    void sceneFitInView();

    QGraphicsView* view_;
    QComboBox* month_box_;
    QSpinBox* year_box_;
    QGridLayout* layout_;
    CalendarBackgroundScene* scene_;
    QPushButton* previous_, *next_;
    QVector<QDate> background_date_vector_;
};

inline void MiniCalendarWidget::updateCalendarViewEvent(QDateTime const& date)
{
    emit updateCalendarView(date);
}

#endif // MINICALENDARWIDGET_H
