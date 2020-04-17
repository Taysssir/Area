#include "catch.hpp"
#include "eos/configuration/configuration.hpp"
#include "resource_manager/file.hpp"


TEST_CASE("settings : create, open functions", "[settings]")
{
    eos::Configuration config;

    eos::file::remove("settings.eos");
    config.open("settings.eos");

    eos::SettingsSharedPointer settings = config.settings();
    REQUIRE(settings);

    REQUIRE(settings->settings().size() == 0);

    ::eos::JSONSettings* setting = settings->add_settings();
    setting->set_keys("antenna_ip");
    setting->set_value("127.0.0.1,123.123.123.123");

    REQUIRE(settings->settings().size() == 1);
    REQUIRE(settings->settings(0).keys() == "antenna_ip");
    REQUIRE(settings->settings(0).value() == "127.0.0.1,123.123.123.123");

    setting = settings->add_settings();
    setting->set_keys("siadc09");
    setting->set_value("test");

    config.save(eos::Configuration::Settings);

    eos::file::copy("settings.eos", "another_settings.eos");

    //eos::file::remove("settings.eos");

    config.open("another_settings.eos");
    settings = config.settings();
    REQUIRE(settings->settings().size() == 2);
    REQUIRE(settings->settings(0).keys() == "antenna_ip");
    REQUIRE(settings->settings(0).value() == "127.0.0.1,123.123.123.123");
    REQUIRE(settings->settings(1).keys() == "siadc09");
    REQUIRE(settings->settings(1).value() == "test");

    settings->mutable_settings(1)->set_value("test4");

    config.save(eos::Configuration::Settings);

    eos::file::copy("another_settings.eos","settings.eos");

    eos::file::remove("another_settings.eos");

    config.open("settings.eos");

    settings = config.settings();
    REQUIRE(settings->settings().size() == 2);
    REQUIRE(settings->settings(0).keys() == "antenna_ip");
    REQUIRE(settings->settings(0).value() == "127.0.0.1,123.123.123.123");
    REQUIRE(settings->settings(1).keys() == "siadc09");
    REQUIRE(settings->settings(1).value() == "test4");

    config.export_model(eos::Configuration::Settings, "settings_export");
}

TEST_CASE("settings : import", "[settings]")
{
    eos::Configuration config;
    eos::file::remove("settings.eos");
    config.open("settings.eos");
    config.import_model(eos::Configuration::Settings, "settings_export");
    eos::SettingsSharedPointer settings = config.settings();
    REQUIRE(settings->settings().size() == 2);
    REQUIRE(settings->settings(0).keys() == "antenna_ip");
    REQUIRE(settings->settings(0).value() == "127.0.0.1,123.123.123.123");
    REQUIRE(settings->settings(1).keys() == "siadc09");
    REQUIRE(settings->settings(1).value() == "test4");
}
