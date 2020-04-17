#ifndef TESTRESOURCEMANAGER_H
#define TESTRESOURCEMANAGER_H

#include "autorun.h"

class TestResourceManager
        : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    //void testMap();
    //void testCropMap();
    //void testZone();
    //void testPatrol();
    //void testRobotSettings();
    //void testSettings();
    //void testSensors();
};

DECLARE_TEST(TestResourceManager)

#endif // TESTRESOURCEMANAGER_H
