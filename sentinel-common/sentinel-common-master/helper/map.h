#ifndef SENTINEL_COMMON_HELPER_MAP_H
#define SENTINEL_COMMON_HELPER_MAP_H

#include <QImage>
#include <QPoint>
#include <QProgressDialog>

#include "evigilante_configuration/include/eos/configuration/models.pb.h"

namespace eos
{
  namespace helper
  {
    QImage fromMap(eos::Map const& map);
    void fromQImage(QImage const& image, eos::Map & map);
    QPoint center(eos::Map const &map);
  } // helper
} // eos

#endif // SENTINEL_COMMON_HELPER_MAP_H
