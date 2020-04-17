#ifndef CONTANTS_CALENDAR_H
#define CONTANTS_CALENDAR_H

#include <QObject>
#include <QColor>
namespace constants
{
namespace calendar {
const int DAYS_IN_WEEK_ = 7;
const QColor LINE_STRONG_COLOR = QColor(130,130,130);//lightColor(230,230,230),strongColor(130,130,130)
const QColor LINE_LIGHT_COLOR = QColor(230,230,230);
const int GRAY_THRESHOLD = 100;
const QColor BACKGROUND_CURRENT_DAY_COLOR = QColor(200,200,200,40);
const QColor LINE_CURRENT_DAY_COLOR = QColor(204,26,26);
const QColor LINE_SELECTED_DAY_COLOR = QColor(236, 136, 6);
const QColor BACKGROUND_SELECTED_DAY_COLOR = QColor(200,200,200,20);
const int SECONDE_IN_DAY = 86400;
}
namespace tasks {
const int MAX_TASKS_PER_DAY_ = 5;
}
}
#endif // CONTANTS_CALENDAR_H

