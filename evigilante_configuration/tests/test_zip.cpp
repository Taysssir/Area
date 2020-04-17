#include "catch.hpp"

#include "resource_manager/ziparchive.hpp"
#include "resource_manager/file.hpp"

namespace
{
  static constexpr char kTempZipName[] = "TempZip";
  eos::eosconfig::ZipArchive za(kTempZipName);
}

using namespace eos;

TEST_CASE("zip : open file", "[zip]")
{
  REQUIRE_FALSE(file::exist(kTempZipName));
  REQUIRE(za.open(eosconfig::ZipArchive::New));
  REQUIRE_FALSE(file::exist(kTempZipName));
}

TEST_CASE("zip : append data", "[zip]")
{
  REQUIRE_FALSE(file::exist(kTempZipName));
  REQUIRE(za.open(eosconfig::ZipArchive::New));
  za.addData("test", "test", 4);
  za.close();
  REQUIRE(file::exist(kTempZipName));
}

TEST_CASE("zip : list data", "[zip]")
{
  REQUIRE(za.open(eosconfig::ZipArchive::Write));
  REQUIRE(za.isOpen());
  REQUIRE(file::exist(kTempZipName));
  REQUIRE(za.getNbEntries() == 1);
  za.close();
}

TEST_CASE("zip : remove file", "[zip]")
{
  REQUIRE_FALSE(za.isOpen());
  REQUIRE(file::remove(kTempZipName));
}
