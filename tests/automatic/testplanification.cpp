#include "testplanification.h"
#include "resource_manager/resource_manager.h"
#include "models/planification.h"
#include "resource_manager/ziparchive.h"
#include "models/taskschedule.h"


testPlanification::testPlanification()
{
}

void testPlanification::loadFromZip()
{
    using namespace rm;
    Planification::Ptr planification = Planification::Ptr(new Planification());
    Planification testPlanif();
    TaskSchedule testTask();
    testTask.setPatrolName("patrole name");
    testTask().setPatrolIndex(0);
    testTask().setPatrolId(0);
    testTask().setDateBegin(0, QTime(12, 0, 0));
    testTask().setDateEnd(0, QTime(20, 0, 0));
    testTask().setEnable(true);
    testPlanif().addNewTaskSchedule(testTask());
    ResourceManager::instance()->save<Planification>("/home/fariza/Robot/Planification/", "planification");

}
