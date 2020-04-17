#include "minicalendarwidget.h"
#include <QGridLayout>

MiniCalendarWidget::MiniCalendarWidget(core::AppInstance& instance, QWidget *parent) : QWidget(parent)
{
    layout_ = new QGridLayout();
    layout_->setSpacing(1);
    view_ = new QGraphicsView(this);
    view_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scene_ = new CalendarBackgroundScene(CalendarBackgroundScene::MONTH_VISUAL,instance);

    previous_ = new QPushButton(QIcon(":/calendar/Resources/calendar/previous.png"),"",this);
    previous_->setMaximumWidth(30);
    connect(previous_,SIGNAL(clicked()),this,SLOT(previous_clicked()));

    next_ = new QPushButton(QIcon(":/calendar/Resources/calendar/next.png"),"",this);
    next_->setMaximumWidth(30);
    connect(next_,SIGNAL(clicked()),this,SLOT(next_clicked()));
    month_box_ = new QComboBox(this);
    connect(month_box_,SIGNAL(currentIndexChanged(int)),this,SLOT(updateMonth(int)));
    year_box_ = new QSpinBox(this);
    connect(year_box_,SIGNAL(valueChanged(int)),this,SLOT(updateYear(int)));
    year_box_->setMaximum(QDate::currentDate().year()+20);
    year_box_->setMinimum(QDate::currentDate().year()-5);
    year_box_->setValue(QDate::currentDate().year());

    QStringList month;
    for(int i(1);i< 13;++i)
    {
        month.append(instance.getLocaleDate(QDate(QDate::currentDate().year(),i,1),"MMMM"));
    }
    month_box_->addItems(month);
    month_box_->setCurrentIndex(QDate::currentDate().month() - 1 );
    this->setLayout(layout_);

    layout_->setMargin(0);
    layout_->addWidget(previous_,0,0);
    layout_->addWidget(month_box_,0,1);
    layout_->addWidget(year_box_,0,2);
    layout_->addWidget(next_,0,3);
    layout_->addWidget(view_,1,0,1,-1);

    view_->setScene(scene_);
    connect(scene_,SIGNAL(transmitQDateTime(QDateTime)),this,SLOT(updateCalendarViewEvent(QDateTime)));
}

MiniCalendarWidget::~MiniCalendarWidget()
{

}

void MiniCalendarWidget::resizeEvent(QResizeEvent * )
{
    sceneFitInView();
}

void MiniCalendarWidget::previous_clicked()
{
    scene_->shiftDate(CalendarBackgroundScene::PREVIOUS);
    updateMonthAndYear(scene_->getFirstDisplayedDate().addDays(7));
    updateDisplayedDateFromBiggerCalendar(background_date_vector_);
}

void MiniCalendarWidget::next_clicked()
{
    scene_->shiftDate(CalendarBackgroundScene::NEXT);
    updateMonthAndYear(scene_->getFirstDisplayedDate().addDays(7));
    updateDisplayedDateFromBiggerCalendar(background_date_vector_);
}

void MiniCalendarWidget::display_today()
{
    scene_->shiftDate(CalendarBackgroundScene::TODAY);
    updateMonthAndYear( QDate::currentDate() );
    updateDisplayedDateFromBiggerCalendar(background_date_vector_);
}

void MiniCalendarWidget::updateMonthAndYear(QDate const& date)
{
    month_box_->setCurrentIndex(date.month()-1);
    year_box_->setValue(date.year());
    sceneFitInView();
}

void MiniCalendarWidget::updateMonth(int index)
{
    int month =scene_->getFirstDisplayedDate().addDays(7).month();
    ++index;
    if(index < month)
        scene_->shiftDate(CalendarBackgroundScene::PREVIOUS,month - index);
    else if (index > month)
        scene_->shiftDate(CalendarBackgroundScene::NEXT, index - month);
    sceneFitInView();
    updateDisplayedDateFromBiggerCalendar(background_date_vector_);
}

void MiniCalendarWidget::updateYear(int value)
{
    int year =scene_->getFirstDisplayedDate().addDays(7).year();
    if(value < year)
        scene_->shiftDate(CalendarBackgroundScene::PREVIOUS,12*(year - value));
    else if (value > year)
        scene_->shiftDate(CalendarBackgroundScene::NEXT, 12*(value - year));
    sceneFitInView();
    updateDisplayedDateFromBiggerCalendar(background_date_vector_);
}

void MiniCalendarWidget::sceneFitInView()
{
    QRectF rect = view_->rect();
    rect = QRectF(rect.x(),rect.y(),
                  rect.width(),
                  rect.height());
    scene_->setSceneRect(rect);
    scene_->resize(rect);
    view_->centerOn(0,0);
}
void MiniCalendarWidget::updateDisplayedDateFromBiggerCalendar(QVector<QDate> const& date_vector)
{
    background_date_vector_ = date_vector;
    scene_->displayBackgroundUnderDisplayedDate(date_vector);
}
