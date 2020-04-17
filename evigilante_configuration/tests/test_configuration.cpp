#include "catch.hpp"

#include "eos/configuration/configuration.hpp"
#include "resource_manager/file.hpp"

TEST_CASE("configuration : create, open file", "[configuration]")
{
    eos::Configuration config;

    REQUIRE_FALSE(config.isOpened());

    SECTION("create a new eos configuration")
    {
        REQUIRE_FALSE(eos::file::exist("test.eos"));
        REQUIRE(config.open("test.eos"));
        REQUIRE(eos::file::exist("test.eos"));
    }

    CHECK(eos::file::remove("test.eos"));
}

TEST_CASE("configuration : create, open file2", "[configuration2]")
{
    eos::Configuration config;

    REQUIRE_FALSE(config.isOpened());

    REQUIRE(config.patrols());

    eos::PatrolsSharedPointer patrols = config.patrols();

    eos::Patrol *p = patrols->add_patrols();

    p->mutable_points()->Add();
    p->mutable_points()->Add();
    p->set_name("Patrol Uno");

    //REQUIRE_FALSE(config.save(eos::Configuration::Patrols));
    REQUIRE(config.save("the_test.eos"));
    //CHECK(eos::file::remove("the_test.eos"));
}

TEST_CASE("configuration : new config, saveAs, open another one, save, open last one")
{
    //// prepare the tests
    //if(eos::file::exist("testSaveAs.eos"))
    //    eos::file::remove("testSaveAs.eos");
    //if(eos::file::exist("config2.eos"))
    //    eos::file::remove("config2.eos");

    //eos::Configuration config;
    //eos::Patrols npatrols;
    //eos::ZonesSharedPointer zones;

    //// Prepare the second configuration with only 1 patrol
    //REQUIRE(config.open("config2.eos"));
    //eos::PatrolsSharedPointer patrols = config.patrols();
    //eos::Patrol *p = patrols->mutable_patrols()->Add();
    //p->set_name("Test Patrol 0");
    //{
    //    eos::Patrol::Point *point = p->mutable_points()->Add();
    //    point->mutable_position()->set_x(2);
    //    point->mutable_position()->set_y(3);
    //}
    //{
    //    eos::Patrol::Point *point = p->mutable_points()->Add();
    //    point->mutable_position()->set_x(8);
    //    point->mutable_position()->set_y(8);
    //}
    //REQUIRE(config.save() == true);

    //// open a new configuration and add it only 1 zone

    //REQUIRE(config.zones());
    //zones = config.zones();

    //eos::Zone *zone = zones->mutable_zones()->Add();
    //zone->mutable_top_left()->set_x(1);
    //zone->mutable_top_left()->set_y(2);
    //zone->mutable_bottom_right()->set_x(3);
    //zone->mutable_bottom_right()->set_y(4);
    //zone->set_permissions(eos::Zone::NoDetection);

    //// save on a new vonfiguration file
    //REQUIRE_FALSE(eos::file::exist("testSaveAs.eos"));
    //config.save("testSaveAs.eos");

    //// open the second configuration with the patrol without the zone
    //REQUIRE(config.open("config2.eos"));
    //zones = config.zones();
    //REQUIRE_FALSE(zones->zones().size() == 1);
    //npatrols = *config.patrols();
    //REQUIRE(npatrols.patrols().size() == 1);
    //if (npatrols.patrols().size())
    //{
    //    eos::Patrol pl = npatrols.patrols().Get(0);
    //    REQUIRE(pl.points().size() == 2);
    //    REQUIRE(pl.points().Get(0).position().x() == 2);
    //    REQUIRE(pl.points().Get(0).position().y() == 3);
    //    REQUIRE(pl.points().Get(0).priority() == eos::Patrol::Priority::Patrol_Priority_Standard);
    //    REQUIRE(pl.points().Get(1).position().x() == 8);
    //    REQUIRE(pl.points().Get(1).position().y() == 8);
    //    REQUIRE(pl.points().Get(1).priority() == eos::Patrol::Priority::Patrol_Priority_Standard);
    //}

    //REQUIRE(config.save() == true);

    //// Open again the saved as configuration and checked if it have 1 zone and no patrols
    //REQUIRE(config.open("testSaveAs.eos"));
    //npatrols = *config.patrols();
    //REQUIRE_FALSE(npatrols.patrols().size() == 1);
    //if (npatrols.patrols().size())
    //{
    //    eos::Patrol pl = npatrols.patrols().Get(0);
    //    REQUIRE_FALSE(pl.points().size() == 2);
    //    REQUIRE_FALSE(pl.points().Get(0).position().x() == 2);
    //    REQUIRE_FALSE(pl.points().Get(0).position().y() == 3);
    //    REQUIRE_FALSE(pl.points().Get(0).priority() == eos::Patrol::Priority::Patrol_Priority_Standard);
    //    REQUIRE_FALSE(pl.points().Get(1).position().x() == 8);
    //    REQUIRE_FALSE(pl.points().Get(1).position().y() == 8);
    //    REQUIRE_FALSE(pl.points().Get(1).priority() == eos::Patrol::Priority::Patrol_Priority_Standard);
    //}
    //zones = config.zones();
    //REQUIRE(zones->zones().size() == 1);
    //if (zones->zones().size())
    //{
    //    eos::Zone zone2 = zones->zones().Get(0);
    //    REQUIRE(zone2.top_left().x() == 1);
    //    REQUIRE(zone2.top_left().y() == 2);
    //    REQUIRE(zone2.bottom_right().x() == 3);
    //    REQUIRE(zone2.bottom_right().y() == 4);
    //    REQUIRE(zone2.permissions() == eos::Zone::NoDetection);
    //}
}
