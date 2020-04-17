#include <QRegExp>

#include "helper.h"

namespace core
{

    bool isValidIP(QString const& ip)
    {
        return QRegExp("^(?:(?:[1-9]?[0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\\.){3}(?:[1-9]?[0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$").exactMatch(ip);
    }

}
