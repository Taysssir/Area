#include "catch.hpp"
#include "eos/configuration/configuration.hpp"
#include "eos/configuration/models.pb.h"
#include "resource_manager/file.hpp"

TEST_CASE("zones : load", "[zones]")
{
  eos::Configuration config;

  REQUIRE(config.open("zones.eos"));
  REQUIRE(eos::file::exist("zones.eos"));
  REQUIRE(config.zones());
  eos::ZonesSharedPointer zones = config.zones();

  eos::Zone *zone = zones->mutable_zones()->Add();
  zone->mutable_top_left()->set_x(1);
  zone->mutable_top_left()->set_y(2);
  zone->mutable_bottom_right()->set_x(3);
  zone->mutable_bottom_right()->set_y(4);
  zone->set_permissions(eos::Zone::NoDetection);
  config.save(eos::Configuration::Zones);

  eos::file::copy("zones.eos", "zones_test.eos");

  REQUIRE(eos::file::remove("zones.eos"));

  eos::Configuration config2;
  config2.open("zones_test.eos");
  zones = config.zones();
  REQUIRE(zones->zones().size() == 1);
  if (zones->zones().size())
  {
    eos::Zone zone2 = zones->zones().Get(0);
    REQUIRE(zone2.top_left().x() == 1);
    REQUIRE(zone2.top_left().y() == 2);
    REQUIRE(zone2.bottom_right().x() == 3);
    REQUIRE(zone2.bottom_right().y() == 4);
    REQUIRE(zone2.permissions() == eos::Zone::NoDetection);
  }
  config2.export_model(eos::Configuration::Zones, "zones_export");
}

TEST_CASE("import zones", "[zones]")
{
  eos::Configuration config2;
  eos::file::remove("zones.eos");
  config2.open("zones.eos");
  REQUIRE(config2.import_model(eos::Configuration::Zones, "zones_export"));
  eos::ZonesSharedPointer zones = config2.zones();
  eos::Zone zone2 = zones->zones().Get(0);
  REQUIRE(zone2.top_left().x() == 1);
  REQUIRE(zone2.top_left().y() == 2);
  REQUIRE(zone2.bottom_right().x() == 3);
  REQUIRE(zone2.bottom_right().y() == 4);
  REQUIRE(zone2.permissions() == eos::Zone::NoDetection);
}
