#ifndef QT_UTILS_H
#define QT_UTILS_H

#include <QString>

class QWidget;

namespace core
{
    void alert(QString const& message, QWidget * parent = 0);
    bool question(QString const& message);

    // used in sentinel-admin
    bool removeDirContent(QString const& dirName, bool first = true);
}

#endif // QT_UTILS_H
