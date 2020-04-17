#include "catch.hpp"
#include "eos/configuration/configuration.hpp"
#include "resource_manager/file.hpp"

TEST_CASE("sensors : create, open functions", "[sensors]")
{
  eos::Configuration config;

  REQUIRE_FALSE(config.isOpened());

  eos::file::remove("sensors.eos");

  REQUIRE(config.open("sensors.eos"));
  REQUIRE(eos::file::exist("sensors.eos"));
  REQUIRE(config.sensors());
  eos::SensorsSharedPointer sensors = config.sensors();
  REQUIRE(sensors->sensors().size() == 0);
  sensors->mutable_sensors()->Add();
  config.save(eos::Configuration::Sensors);
  REQUIRE(sensors->sensors().size() == 1);
  REQUIRE(config.export_model(eos::Configuration::Sensors, "sensors_export"));
}

TEST_CASE("sensors : import sensors", "[sensors]")
{
  eos::Configuration config;
  eos::file::remove("sensors.eos");
  REQUIRE(config.open("sensors.eos"));
  REQUIRE(config.import_model(eos::Configuration::Sensors, "sensors_export"));
  eos::SensorsSharedPointer sensors = config.sensors();
  REQUIRE(sensors->sensors().size() == 1);
}
