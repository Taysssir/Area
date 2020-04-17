#include <QImage>

#include "patrolcolor.h"

QColor PatrolColor::index(int id)
{
    static QImage image(":/calendar/colors");
    if(id < image.width())
        return image.pixel(id, 0);
    return Qt::red;
}
