#include "dialogcreateplannification.h"
#include "ui_dialogcreateplannification.h"

#include "contants_calendar.h"

DialogCreatePlannification::DialogCreatePlannification(core::AppInstance &instance, QVector<QString> strings, QWidget *parent)
    : QDialog(parent)
    , instance_(instance)
    , ui(new Ui::DialogCreatePlannification)
{
    ui->setupUi(this);
    setupLabel();
    ui->comboBox_patrol_name_->addItems(strings.toList());


    ui->dateEdit_start_->setDate(QDate().currentDate());
    int minute = (QTime().currentTime().minute()/10 + 1)*10;
    if(minute < 60)
    {
        ui->timeEdit_start_->setTime(QTime(QTime().currentTime().hour(),minute));
    }
    else
    {
        ui->timeEdit_start_->setTime(QTime(QTime().currentTime().hour()+1,0));
    }
    setWindowFlags( Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint );
}

DialogCreatePlannification::DialogCreatePlannification(core::AppInstance &instance, QVector<QString> strings, QDateTime start, QDateTime end , QWidget *parent) :
    QDialog(parent),
    instance_(instance),
    ui(new Ui::DialogCreatePlannification)
{
    ui->setupUi(this);
    setupLabel();
    ui->comboBox_patrol_name_->addItems(strings.toList());

    ui->dateEdit_start_->setDate(start.date());
    ui->timeEdit_start_->setTime(start.time());
    ui->dateEdit_end_->setDate(end.date());
    ui->timeEdit_end_->setTime(end.time());

}


DialogCreatePlannification::DialogCreatePlannification(core::AppInstance &instance, QVector<QString> strings,TaskScheduled task ,QWidget *parent) :
    QDialog(parent),
    instance_(instance),
    ui(new Ui::DialogCreatePlannification)
{
    ui->setupUi(this);
    setupLabel();
    ui->comboBox_patrol_name_->addItems(strings.toList());

    ui->comboBox_patrol_name_->setCurrentIndex(strings.indexOf(task.getPatrolName()));

    ui->dateEdit_start_->setDate(task.getStartingTime().date());
    ui->timeEdit_start_->setTime(task.getStartingTime().time());
    ui->dateEdit_end_->setDate(task.getEndingTime().date());
    ui->timeEdit_end_->setTime(task.getEndingTime().time());


    ui->radioButton_repeted_->setEnabled(false);
    ui->radioButton_repeted_no_->setEnabled(false);
    ui->label_repetition_question_->setEnabled(false);
}

void DialogCreatePlannification::setupLabel()
{
    ui->label_m->setText(instance_.read_only().getLocaleDate(QDate(2016,2,1),"dddd"));
    ui->label_tu->setText(instance_.read_only().getLocaleDate(QDate(2016,2,2),"dddd"));
    ui->label_w->setText(instance_.read_only().getLocaleDate(QDate(2016,2,3),"dddd"));
    ui->label_th->setText(instance_.read_only().getLocaleDate(QDate(2016,2,4),"dddd"));
    ui->label_f->setText(instance_.read_only().getLocaleDate(QDate(2016,2,5),"dddd"));
    ui->label_sa->setText(instance_.read_only().getLocaleDate(QDate(2016,2,6),"dddd"));
    ui->label_su->setText(instance_.read_only().getLocaleDate(QDate(2016,2,7),"dddd"));

    ui->groupBox_repetition_->setEnabled(false);
    start_ = true;

    on_radioButton_every_week__clicked();

    ui->label_summary_repetition->setHidden(true);
    ui->label_summary_repetition_2->setHidden(true);
    ui->label_kind_of_repetion_->setHidden(true);
    ui->label_repetition_years_->setHidden(true);

    current_repetition_ = NO_REPETITION;
}

DialogCreatePlannification::~DialogCreatePlannification()
{
    delete ui;
}


void DialogCreatePlannification::on_radioButton_repeted_no__clicked()
{
    ui->groupBox_repetition_->setEnabled(false);
    current_repetition_ = NO_REPETITION;
    if(date_time_schedule_mutex.tryLock())
        setMinimumTime();
}

void DialogCreatePlannification::on_radioButton_repeted__clicked()
{
    ui->groupBox_repetition_->setEnabled(true);
    if(ui->radioButton_every_day_->isChecked())
    {
        current_repetition_ = DAY_REPETITION;
    }
    else if(ui->radioButton_every_week_->isChecked())
    {
        manageCheckBoxWeekDisplay();
    }
    else if(ui->radioButton_every_month_->isChecked())
    {
        current_repetition_ = MONTH_REPETITION;
    }
    else if(ui->radioButton_every_year_->isChecked())
    {
        current_repetition_ = YEAR_REPETITION;
    }
    if(date_time_schedule_mutex.tryLock())
        setMinimumTime();
}

void DialogCreatePlannification::on_radioButton_every_day__clicked()
{
    ui->groupBox_day_repetition_->setEnabled(false);
    current_repetition_ = DAY_REPETITION;
    if(date_time_schedule_mutex.tryLock())
        setMinimumTime();
}


void DialogCreatePlannification::on_radioButton_every_week__clicked()
{
    manageCheckBoxWeekDisplay();
    if(date_time_schedule_mutex.tryLock())
        setMinimumTime();
}

void DialogCreatePlannification::manageCheckBoxWeekDisplay()
{
    if(ui->radioButton_repeted_->isChecked())
    {
        QDateTime date_time_start = QDateTime(ui->dateEdit_start_->date(),ui->timeEdit_start_->time());
        QDateTime date_time_end = QDateTime(ui->dateEdit_end_->date(),ui->timeEdit_end_->time());
        int duration = abs(date_time_start.secsTo(date_time_end));

        if(duration > constants::calendar::SECONDE_IN_DAY)
        {
            ui->groupBox_day_repetition_->setEnabled(false);
            current_repetition_ = WEEK_REPETITION;
        }
        else
        {
            ui->groupBox_day_repetition_->setEnabled(true);
            current_repetition_ = PSEUDO_DAY_REPETITION;
        }
    }
}

void DialogCreatePlannification::manageCheckBoxWeekUsage()
{
    QDateTime date_time_start = QDateTime(ui->dateEdit_start_->date(),ui->timeEdit_start_->time());
    QDateTime date_time_end = QDateTime(ui->dateEdit_end_->date(),ui->timeEdit_end_->time());
    int duration = abs(date_time_start.secsTo(date_time_end));

    if(duration <= constants::calendar::SECONDE_IN_DAY)
    {
        switch (ui->dateEdit_start_->date().dayOfWeek())
        {
        case 1:
            ui->checkBox_monday_->setChecked(true);
            break;
        case 2:
            ui->checkBox_tuesday_->setChecked(true);
            break;
        case 3:
            ui->checkBox_wednesday_->setChecked(true);
            break;
        case 4:
            ui->checkBox_thursday_->setChecked(true);
            break;
        case 5:
            ui->checkBox_friday_->setChecked(true);
            break;
        case 6:
            ui->checkBox_saturday_->setChecked(true);
            break;
        case 7:
            ui->checkBox_sunday_->setChecked(true);
            break;
        default:
            break;
        }
        if(last_day_selected_ != ui->dateEdit_start_->date())
        {
            switch (last_day_selected_.dayOfWeek())
            {
            case 1:
                ui->checkBox_monday_->setChecked(false);
                break;
            case 2:
                ui->checkBox_tuesday_->setChecked(false);
                break;
            case 3:
                ui->checkBox_wednesday_->setChecked(false);
                break;
            case 4:
                ui->checkBox_thursday_->setChecked(false);
                break;
            case 5:
                ui->checkBox_friday_->setChecked(false);
                break;
            case 6:
                ui->checkBox_saturday_->setChecked(false);
                break;
            case 7:
                ui->checkBox_sunday_->setChecked(false);
                break;
            default:
                break;
            }
            last_day_selected_ = ui->dateEdit_start_->date();
        }
    }
}

void DialogCreatePlannification::on_radioButton_every_month__clicked()
{
    ui->groupBox_day_repetition_->setEnabled(false);
    current_repetition_ = MONTH_REPETITION;
    if(date_time_schedule_mutex.tryLock())
        setMinimumTime();
}

void DialogCreatePlannification::on_radioButton_every_year__clicked()
{
    ui->groupBox_day_repetition_->setEnabled(false);
    current_repetition_ = YEAR_REPETITION;
    if(date_time_schedule_mutex.tryLock())
        setMinimumTime();
}

void DialogCreatePlannification::on_dateEdit_start__dateChanged(const QDate &)
{
    if(ui->radioButton_every_week_->isChecked())
    {
        manageCheckBoxWeekDisplay();
        manageCheckBoxWeekUsage();
    }
    if(date_time_schedule_mutex.tryLock())
        setMinimumTime();
}

void DialogCreatePlannification::on_timeEdit_start__timeChanged(const QTime &)
{
    if(ui->radioButton_every_week_->isChecked())
    {
        manageCheckBoxWeekDisplay();
        manageCheckBoxWeekUsage();
    }
    if(date_time_schedule_mutex.tryLock())
        setMinimumTime();
}

void DialogCreatePlannification::on_dateEdit_end__dateChanged(const QDate &)
{
    if(ui->radioButton_every_week_->isChecked())
    {
        manageCheckBoxWeekDisplay();
        manageCheckBoxWeekUsage();
    }
    if(date_time_schedule_mutex.tryLock())
        setMinimumTime();
}

void DialogCreatePlannification::on_timeEdit_end__timeChanged(const QTime &)
{
    if(ui->radioButton_every_week_->isChecked())
    {
        manageCheckBoxWeekDisplay();
        manageCheckBoxWeekUsage();
    }
    if(date_time_schedule_mutex.tryLock())
        setMinimumTime();
}

void DialogCreatePlannification::setMinimumTime()
{
    QDateTime date_time_start = QDateTime(ui->dateEdit_start_->date(),ui->timeEdit_start_->time());
    QDateTime date_time_end = QDateTime(ui->dateEdit_end_->date(),ui->timeEdit_end_->time());

    QDate max_date = QDate(), min_date = QDate();
    QDate date = ui->dateEdit_end_->date();
    QTime time = ui->timeEdit_end_->time();
    QTime max_time= QTime(23,59), min_time = QTime(0,0);
    QTime time_start = ui->timeEdit_start_->time();

    if(date_time_start  > date_time_end )
    {
        date = (ui->dateEdit_start_->date());
        time = (ui->timeEdit_start_->time().addSecs(60*60));
    }

    min_date = (ui->dateEdit_start_->date());

    if(ui->dateEdit_start_->date() == ui->dateEdit_end_->date() && ui->timeEdit_end_->time() == ui->timeEdit_start_->time().addSecs(60*60))
    {
        min_time = (ui->timeEdit_start_->time().addSecs(60*60));
    }

    switch(current_repetition_)
    {
    case NO_REPETITION:
    {
        break;
    }
    case DAY_REPETITION :
    {
        if(time_start == QTime(0,0))
            max_date = ui->dateEdit_start_->date();
        else
        {
            max_date = ui->dateEdit_start_->date().addDays(1);
            max_time = time_start.addSecs(-60);
        }
        break;
    }
    case PSEUDO_DAY_REPETITION:
    {
        if(time_start == QTime(0,0))
            max_date = ui->dateEdit_start_->date();
        else
        {
            max_date = ui->dateEdit_start_->date().addDays(1);
        }
        break;
    }
    case WEEK_REPETITION :
    {
        if(time_start == QTime(0,0))
            max_date = ui->dateEdit_start_->date().addDays(constants::calendar::DAYS_IN_WEEK_ - 1);
        else
        {
            max_date = ui->dateEdit_start_->date().addDays(constants::calendar::DAYS_IN_WEEK_);
        }

        if(max_date == ui->dateEdit_end_->date())
        {
            max_time = time_start.addSecs(-60);
        }
        break;
    }
    case MONTH_REPETITION :
    {
        if(time_start == QTime(0,0))
            max_date = ui->dateEdit_start_->date().addDays(ui->dateEdit_start_->date().daysInMonth() - 1);
        else
        {
            max_date = ui->dateEdit_start_->date().addDays(ui->dateEdit_start_->date().daysInMonth());
        }

        if(max_date == ui->dateEdit_end_->date())
        {
            max_time = time_start.addSecs(-60);
        }
        break;
    }
    case YEAR_REPETITION :
    {
        if(time_start == QTime(0,0))
            max_date = ui->dateEdit_start_->date().addDays(ui->dateEdit_start_->date().daysInYear());
        else
        {
            max_date = ui->dateEdit_start_->date().addDays(ui->dateEdit_start_->date().daysInYear() + 1);
        }

        if(max_date == ui->dateEdit_end_->date())
        {
            max_time = time_start.addSecs(-60);
        }
        break;
    }
    }
    if(ui->dateEdit_start_->date() == ui->dateEdit_end_->date())
    {
        min_time = ui->timeEdit_start_->time().addSecs(60*60);
    }

    if(date.isValid())
        ui->dateEdit_end_->setDate(date);
    if(max_date.isValid())
        ui->dateEdit_end_->setMaximumDate(max_date);
    if(min_date.isValid())
        ui->dateEdit_end_->setMinimumDate(min_date);
    ui->timeEdit_end_->setTime(time);
    ui->timeEdit_end_->setMaximumTime(max_time);
    ui->timeEdit_end_->setMinimumTime(min_time);

    updateWhatsUpText();
    date_time_schedule_mutex.unlock();
}

void DialogCreatePlannification::updateWhatsUpText()
{
    if(current_repetition_ != NO_REPETITION )
    {
        ui->label_summary_repetition->setHidden(false);
        ui->label_summary_repetition_2->setHidden(false);
        ui->label_kind_of_repetion_->setHidden(false);
        ui->label_repetition_years_->setHidden(false);
    }
    QString kind_of_repetion ="", until ="";
    switch(current_repetition_)
    {
    case NO_REPETITION:
    {
        ui->label_summary_repetition->setHidden(true);
        ui->label_summary_repetition_2->setHidden(true);
        ui->label_kind_of_repetion_->setHidden(true);
        ui->label_repetition_years_->setHidden(true);
        break;
    }
    case DAY_REPETITION :
    {
        kind_of_repetion = tr("daily");
        break;
    }
    case PSEUDO_DAY_REPETITION :
    {
        kind_of_repetion = tr("weekly on ");
        QDate temp;
        temp = temp.currentDate().addDays(-temp.dayOfWeek() +1);
        int array[constants::calendar::DAYS_IN_WEEK_] = {ui->checkBox_monday_->isChecked(),
                                                        ui->checkBox_tuesday_->isChecked(),
                                                        ui->checkBox_wednesday_->isChecked(),
                                                        ui->checkBox_thursday_->isChecked(),
                                                        ui->checkBox_friday_->isChecked(),
                                                        ui->checkBox_saturday_->isChecked(),
                                                        ui->checkBox_sunday_->isChecked()};
        for(int i(0);i<constants::calendar::DAYS_IN_WEEK_;++i)
        {
            //2016 february start a monday that's why : QDate(2016,2,i+1)
            if(array[i])
                kind_of_repetion += instance_.getLocaleDate(QDate(2016,2,i+1),"dddd") + ", ";
        }
        break;
    }
    case WEEK_REPETITION :
    {
        kind_of_repetion = tr("weekly");
        break;
    }
    case MONTH_REPETITION :
    {
        kind_of_repetion = tr("monthly on day ");
        kind_of_repetion += instance_.getLocaleDate(ui->dateEdit_start_->date(),"d");
        break;
    }
    case YEAR_REPETITION :
    {
        kind_of_repetion = tr("yearly");
        break;
    }
    }

    ui->label_repetition_years_->setText(instance_.getLocaleDate(ui->dateEdit_start_->date().addYears(2),"MMMM yyyy"));
    ui->label_date_->setText(ui->dateEdit_start_->date().toString());

    QDateTime start_datetime_(ui->dateEdit_start_->date(),ui->timeEdit_start_->time());
    QDateTime end_datetime_(ui->dateEdit_end_->date(),ui->timeEdit_end_->time());

    QVector<QString> time_elapsed = timeSpan(start_datetime_,end_datetime_);

    ui->label_duration_->setText(time_elapsed[2] + tr(" day ")+
            time_elapsed[1]+ tr(" hour ") +
            time_elapsed[0]+ tr(" minute"));


    ui->label_kind_of_repetion_->setText(kind_of_repetion);
}

QVector<QString> DialogCreatePlannification::timeSpan(QDateTime time_left, QDateTime time_right)
{
    QVector<QString> time_elapsed;

    int secondes = time_left.secsTo(time_right);

    int days = time_left.daysTo(time_right);
    secondes -= days*constants::calendar::SECONDE_IN_DAY;

    int minutes = secondes/60;

    int hours = minutes/60;
    minutes -= hours*60;

    if(hours < 0 || minutes < 0)
    {
        if (minutes < 0)
        {
            minutes = 60 + minutes;
            --hours;
        }

        if( hours < 0)
        {
            --days;
            hours = 24 + hours;
        }
    }

    time_elapsed.append(QString::number(minutes));
    time_elapsed.append(QString::number(hours));
    time_elapsed.append(QString::number(days));
    return time_elapsed;
}

QString DialogCreatePlannification::getPatrolName()
{
    return ui->comboBox_patrol_name_->currentText();
}

int DialogCreatePlannification::getPatrolNumber()
{
    return ui->comboBox_patrol_name_->currentIndex()+1;
}

QVector<QDateTime> DialogCreatePlannification::getStartDates()
{
    QVector<QDateTime> start_dates;
    int array[constants::calendar::DAYS_IN_WEEK_] = {ui->checkBox_monday_->isChecked(),
                                                    ui->checkBox_tuesday_->isChecked(),
                                                    ui->checkBox_wednesday_->isChecked(),
                                                    ui->checkBox_thursday_->isChecked(),
                                                    ui->checkBox_friday_->isChecked(),
                                                    ui->checkBox_saturday_->isChecked(),
                                                    ui->checkBox_sunday_->isChecked()};
    if(ui->radioButton_repeted_no_->isChecked())
    {
        start_dates.append(QDateTime(ui->dateEdit_start_->date(),ui->timeEdit_start_->time()));
        return start_dates;
    }
    else
    {
        QDateTime cursor_date = QDateTime(ui->dateEdit_start_->date(),ui->timeEdit_start_->time());
        QDateTime two_years_more = cursor_date.addYears(2);
        int day_to_add = 1;
        while(cursor_date < two_years_more)
        {
            if(ui->radioButton_every_day_->isChecked())
            {
                day_to_add = 1;
            }
            if(ui->radioButton_every_week_->isChecked())
            {
                for(int i(1);i < constants::calendar::DAYS_IN_WEEK_;++i)
                {
                    if(array[((cursor_date.date().dayOfWeek()-1)+i)%constants::calendar::DAYS_IN_WEEK_])
                    {
                        day_to_add = i;
                        break;
                    }
                    else
                    {
                        day_to_add = constants::calendar::DAYS_IN_WEEK_;
                    }
                }
            }
            if(ui->radioButton_every_month_->isChecked())
            {
                day_to_add = cursor_date.date().daysInMonth();
            }
            if(ui->radioButton_every_year_->isChecked())
            {
                day_to_add = cursor_date.date().daysInYear();
            }
            start_dates.append(cursor_date);
            cursor_date = cursor_date.addDays(day_to_add);
        }
    }
    return start_dates;
}

QVector<QDateTime> DialogCreatePlannification::getEndDates()
{
    QVector<QDateTime> end_dates;
    int array[constants::calendar::DAYS_IN_WEEK_] = {ui->checkBox_monday_->isChecked(),
                                                    ui->checkBox_tuesday_->isChecked(),
                                                    ui->checkBox_wednesday_->isChecked(),
                                                    ui->checkBox_thursday_->isChecked(),
                                                    ui->checkBox_friday_->isChecked(),
                                                    ui->checkBox_saturday_->isChecked(),
                                                    ui->checkBox_sunday_->isChecked()};

    if(ui->radioButton_repeted_no_->isChecked())
    {
        end_dates.append(QDateTime(ui->dateEdit_end_->date(),ui->timeEdit_end_->time()));
        return end_dates;
    }
    else
    {
        QDateTime cursor_date = QDateTime(ui->dateEdit_end_->date(),ui->timeEdit_end_->time());
        QDateTime cursor_date_start = QDateTime(ui->dateEdit_start_->date(),ui->timeEdit_end_->time());
        QDateTime two_years_more = cursor_date.addYears(2);
        int day_to_add = 1;
        while(cursor_date < two_years_more)
        {
            if(ui->radioButton_every_day_->isChecked())
            {
                day_to_add = 1;
            }
            if(ui->radioButton_every_week_->isChecked())
            {
                for(int i(1);i < constants::calendar::DAYS_IN_WEEK_;++i)
                {
                    if(array[((cursor_date_start.date().dayOfWeek()-1)+i)%constants::calendar::DAYS_IN_WEEK_])
                    {
                        day_to_add = i;
                        break;
                    }
                    else
                    {
                        day_to_add = constants::calendar::DAYS_IN_WEEK_;
                    }
                }
            }
            if(ui->radioButton_every_month_->isChecked())
            {
                day_to_add = cursor_date.date().daysInMonth();
            }
            if(ui->radioButton_every_year_->isChecked())
            {
                day_to_add = cursor_date.date().daysInYear();
            }
            end_dates.append(cursor_date);
            cursor_date = cursor_date.addDays(day_to_add);
            cursor_date_start = cursor_date_start.addDays(day_to_add);
        }
    }
    return end_dates;
}

