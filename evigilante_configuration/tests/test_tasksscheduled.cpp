#include "catch.hpp"
#include "eos/configuration/configuration.hpp"
#include "resource_manager/file.hpp"

TEST_CASE("test tasks scheduled", "[Tasks]")
{
  {
    eos::Configuration config;
    REQUIRE(config.open("test_tasks.eos"));

    eos::TasksScheduledSharedPointer ts = config.tasksScheduled();

    // ts is not null
    REQUIRE(ts);

    REQUIRE(ts->tasks().size() == 0);

    {
      eos::TaskScheduled *task = ts->mutable_tasks()->Add();
      task->set_patrol_name("patrol 0");
      task->set_start(1024);
      task->set_stop(2048);
      task->set_group_id(1111);
    }

    {
      eos::TaskScheduled *task = ts->mutable_tasks()->Add();
      task->set_patrol_name("patrol 1");
      task->set_start(12345);
      task->set_stop(5467);
      task->set_group_id(0);
    }

    REQUIRE(config.save(eos::Configuration::TasksScheduled));
  }
  {
    eos::Configuration config;
    REQUIRE(config.open("test_tasks.eos"));

    eos::TasksScheduledSharedPointer ts = config.tasksScheduled();

    // ts is not null
    REQUIRE(ts);

    REQUIRE(ts->tasks().size() == 2);

    eos::TaskScheduled const &task = ts->tasks().Get(0);

    REQUIRE(task.patrol_name() == "patrol 0");
    REQUIRE(task.start() == 1024);
    REQUIRE(task.stop() == 2048);
    REQUIRE(task.group_id() == 1111);

    eos::TaskScheduled const &task2 = ts->tasks().Get(1);

    REQUIRE(task2.patrol_name() == "patrol 1");
    REQUIRE(task2.start() == 12345);
    REQUIRE(task2.stop() == 5467);
    REQUIRE(task2.group_id() == 0);
    config.export_model(eos::Configuration::TasksScheduled, "tasks_schedule_export");

    REQUIRE(eos::file::remove("test_tasks.eos"));
  }
}

TEST_CASE("import ts", "[Tasks]")
{
    eos::Configuration config;
    eos::file::remove("tasks.eos");
    REQUIRE(config.open("tasks.eos"));
    config.import_model(eos::Configuration::TasksScheduled, "tasks_schedule_export");
    eos::TasksScheduledSharedPointer ts = config.tasksScheduled();

    // ts is not null
    REQUIRE(ts);

    REQUIRE(ts->tasks().size() == 2);

    eos::TaskScheduled const &task = ts->tasks().Get(0);

    REQUIRE(task.patrol_name() == "patrol 0");
    REQUIRE(task.start() == 1024);
    REQUIRE(task.stop() == 2048);
    REQUIRE(task.group_id() == 1111);

    eos::TaskScheduled const &task2 = ts->tasks().Get(1);

    REQUIRE(task2.patrol_name() == "patrol 1");
    REQUIRE(task2.start() == 12345);
    REQUIRE(task2.stop() == 5467);
    REQUIRE(task2.group_id() == 0);
}
