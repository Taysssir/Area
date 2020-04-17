#include "catch.hpp"
#include "eos/configuration/configuration.hpp"
#include "resource_manager/file.hpp"

TEST_CASE("patrols : create, open functions", "[patrols]")
{
  eos::Configuration config;

  REQUIRE_FALSE(config.isOpened());

  REQUIRE(config.open("patrols.eos"));
  REQUIRE(eos::file::exist("patrols.eos"));
  REQUIRE(config.patrols());
  eos::PatrolsSharedPointer patrols = config.patrols();
  REQUIRE(patrols->patrols().size() == 0);
  eos::Patrol *p = patrols->add_patrols();
  p->mutable_points()->Add();
  // p.addPoint(eos::Patrol::Point());
  p->set_name("Patrol Uno");
  //patrols->push_back(p);
  config.save(eos::Configuration::Patrols);
  {
    eos::PatrolsSharedPointer patrols = config.patrols();
    REQUIRE(patrols->patrols().size() == 1);
  }
}

TEST_CASE("patrols : read again", "[patrols]")
{
  eos::Configuration config;
  REQUIRE(config.open("patrols.eos"));
  eos::PatrolsSharedPointer patrols = config.patrols();
  REQUIRE(patrols->patrols().size() == 1);
  if (patrols->patrols().size())
  {
    eos::Patrol patrol = patrols->patrols().Get(0);
    REQUIRE(patrol.name() == "Patrol Uno");
  }
  REQUIRE(eos::file::remove("patrols.eos"));
}

TEST_CASE("patrols : create some point", "[patrols]")
{
  eos::Configuration config;
  REQUIRE(config.open("test_patrols.eos"));
  eos::PatrolsSharedPointer patrols = config.patrols();
  eos::Patrol *p = patrols->mutable_patrols()->Add();
  p->set_name("Test Patrol 0");
  {
    eos::Patrol::Point *point = p->mutable_points()->Add();
    point->mutable_position()->set_x(2);
    point->mutable_position()->set_y(3);
  }
  {
    eos::Patrol::Point *point = p->mutable_points()->Add();
    point->mutable_position()->set_x(8);
    point->mutable_position()->set_y(8);
  }
  config.save(eos::Configuration::Patrols);
  eos::file::copy("test_patrols.eos", "new_patrols.eos");
  REQUIRE(eos::file::remove("test_patrols.eos"));
  REQUIRE(config.open("new_patrols.eos"));
  eos::Patrols npatrols = *config.patrols();
  REQUIRE(npatrols.patrols().size() == 1);
  if (npatrols.patrols().size())
  {
    eos::Patrol pl = npatrols.patrols().Get(0);
    REQUIRE(pl.points().size() == 2);
    REQUIRE(pl.points().Get(0).position().x() == 2);
    REQUIRE(pl.points().Get(0).position().y() == 3);
    REQUIRE(pl.points().Get(0).priority() == eos::Patrol::Priority::Patrol_Priority_Standard);
    REQUIRE(pl.points().Get(1).position().x() == 8);
    REQUIRE(pl.points().Get(1).position().y() == 8);
    REQUIRE(pl.points().Get(1).priority() == eos::Patrol::Priority::Patrol_Priority_Standard);
  }
  config.export_model(eos::Configuration::Patrols, "othertest");
  eos::PatrolsSharedPointer patrols2 = config.patrols();
  eos::Patrol *p2 = patrols2->mutable_patrols()->Add();
  p2->set_name("Nouvel patrol");
  {
    eos::Patrol::Point *point = p2->mutable_points()->Add();
    point->mutable_position()->set_x(8);
    point->mutable_position()->set_y(8);
  }
  config.export_model(eos::Configuration::Patrols, "othertest2");
  REQUIRE_FALSE(eos::file::compare("othertest", "othertest2"));
}

TEST_CASE("patrols : import", "[patrols]")
{
  eos::Configuration config;
  eos::file::remove("test_patrols2.eos");
  REQUIRE(config.open("test_patrols2.eos"));
  config.import_model(eos::Configuration::Patrols, "othertest2");
  eos::Patrols npatrols = *config.patrols();
  REQUIRE(npatrols.patrols().size() > 0);
  eos::Patrol pl = npatrols.patrols().Get(0);
  REQUIRE(pl.points().size() == 2);
  REQUIRE(pl.points().Get(0).position().x() == 2);
  REQUIRE(pl.points().Get(0).position().y() == 3);
  REQUIRE(pl.points().Get(0).priority() == eos::Patrol::Priority::Patrol_Priority_Standard);
  REQUIRE(pl.points().Get(1).position().x() == 8);
  REQUIRE(pl.points().Get(1).position().y() == 8);
  REQUIRE(pl.points().Get(1).priority() == eos::Patrol::Priority::Patrol_Priority_Standard);
}
