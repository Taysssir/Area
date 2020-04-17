#include "catch.hpp"
#include "eos/configuration/configuration.hpp"
#include "resource_manager/file.hpp"

namespace
{
  void createFakeMap(eos::MapSharedPointer map, int width, int height, int value)
  {
    map->set_width(width);
    map->set_height(height);
    size_t size = width * height * 3;
    map->mutable_content()->resize(size);
    std::fill(map->mutable_content()->begin(), map->mutable_content()->end(), value);
  }
}

TEST_CASE("test loading map", "[map]")
{
  eos::Configuration config;
  REQUIRE(config.open("test_map.eos"));
  createFakeMap(config.base_map(), 100, 100, 200);
  config.save(eos::Configuration::BaseMap);
  eos::file::copy("test_map.eos", "new_map.eos");
  REQUIRE(eos::file::remove("test_map.eos"));

  eos::Configuration config2;
  REQUIRE(config2.open("new_map.eos"));
  eos::MapSharedPointer new_map = config2.base_map();
  REQUIRE(new_map->width() == config.base_map()->width());
  REQUIRE(new_map->height() == config.base_map()->height());
  REQUIRE(new_map->content() == config.base_map()->content());

  config2.export_model(eos::Configuration::BaseMap, "base_map_export");
  config2.import_model(eos::Configuration::StaticMap, "base_map_export");
  config2.export_model(eos::Configuration::StaticMap, "base_map_export2");
  REQUIRE(eos::file::compare("base_map_export", "base_map_export2"));
}
