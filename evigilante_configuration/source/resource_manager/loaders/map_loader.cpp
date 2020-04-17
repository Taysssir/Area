#include "map_loader.hpp"
#include "eos/configuration/models.pb.h"
#include "resource_manager/resource.hpp"

namespace eos
{
  namespace eosconfig
  {
    BaseResource::Ptr MapLoader::load(RawData const &data) const
    {
      std::string magicNum;
      std::stringstream ss(data);
      int width, height, maxColVal;
      double resolution;
      int64_t timestamp;
      ss >> magicNum;
      std::string comment_carac;
      ss >> comment_carac;
      ss >> timestamp;
      ss >> comment_carac;
      ss >> resolution;
      ss >> width >> height >> maxColVal;
      ss.get();
      size_t size = data.size() - ss.tellg();
      auto map = rsc::alloc<Map>();
      map->set_timestamp(timestamp);
      map->set_width(width);
      map->set_height(height);
      map->set_resolution(resolution);
      map->mutable_content()->resize(size);
      std::string *content = map->mutable_content();
      char *p = &(*content)[0];
      ss.read(p, size);
      return map;
    }

    bool MapLoader::save(RawData &data, BaseResource::Ptr const &rsc) const
    {
      std::stringstream ss;
      auto map = rsc::as<Map>(rsc);
      ss << "EOS" << std::endl;
      ss << "# " << map->timestamp() << std::endl;
      ss << "# " << map->resolution() << std::endl;
      ss << map->width() << " " << map->height() << std::endl
         << 255 << std::endl;
      ss.write(map->content().c_str(), map->content().size());
      data = ss.str();
      return true;
    }
  }
}
