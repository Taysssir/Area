#include <sstream>
#include <QDebug>
#include <cstring>
#include <QBuffer>

#include "core/sentinel_logger.h"
#include "map.h"
//#include "sentinel-admin_core/constants.h"

namespace eos
{
  namespace helper
  {
    QImage fromMap(eos::Map const& map)
    {
      if(!map.content().size())
        return QImage();
      return QImage::fromData((const uchar*)&map.content()[0], map.content().size(), "PNG");
    }

    void fromQImage(QImage const& image, eos::Map & map)
    {
      QByteArray bytes;
      QBuffer buffer( &bytes );
      buffer.open( QIODevice::WriteOnly );
      image.save( &buffer, "PNG" );

      map.set_width(image.width());
      map.set_height(image.height());
      map.mutable_content()->resize(buffer.size());
      std::string *content = map.mutable_content();
      char *p = &(*content)[0];
      std::memcpy(p, buffer.data(), buffer.size());
    }

    QPoint center(eos::Map const &map)
    {
      return QPoint(map.width() / 2, map.height() / 2);
    }
  } // helper
} // eos
