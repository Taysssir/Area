#include <QDebug>

#include "resource_manager/resource_manager.h"
#include "testzone.h"
#include "models/zone.h"

void TestZone::defaultValues()
{
    Zone zone;
    QCOMPARE(zone.permissions(), Zone::Nothing);
    QCOMPARE(zone.topLeft(), QPoint(0, 0));
    QCOMPARE(zone.bottomRight(), QPoint(0, 0));
}

void TestZone::testPermissions()
{
    Zone zone;
    QVERIFY( !(zone.permissions() & Zone::NoDetection) );
    QVERIFY( !(zone.permissions() & Zone::NoMovement) );
    zone.addPermission(Zone::NoDetection);
    QVERIFY(  (zone.permissions() & Zone::NoDetection) );
    QVERIFY( !(zone.permissions() & Zone::NoMovement) );
    zone.addPermission(Zone::NoMovement);
    QVERIFY(  (zone.permissions() & Zone::NoDetection) );
    QVERIFY(  (zone.permissions() & Zone::NoMovement) );
    zone.setPermissions(Zone::Nothing);
    QVERIFY( !(zone.permissions() & Zone::NoDetection) );
    QVERIFY( !(zone.permissions() & Zone::NoMovement) );
}

void TestZone::testPoints()
{
    Zone zone;
    zone.setTopLeft(QPoint(10, 12));
    QCOMPARE(zone.topLeft(), QPoint(10, 12));
    zone.setBottomRight(QPoint(12, 19));
    QCOMPARE(zone.bottomRight(), QPoint(12, 19));
}

void TestZone::testSave()
{
    using namespace rm;
    Zones::Ptr zones = Zones::Ptr(new Zones());
    Zone zone;
    zones->add(zone);
    zone.setPermissions(Zone::NoDetection|Zone::NoMovement);
    zones->add(zone);
    zone.setTopLeft(QPoint(100, 2034));
    zones->add(zone);
    ResourceManager::instance()->save<Zones>(".", "patrols", zones);

    Zones::Ptr zones2 = ResourceManager::instance()->load<Zones>(".", "patrols");
    ResourceManager::instance()->save<Zones>(".", "patrols2", zones2);

    //  Patrols::Ptr pat2 = ResourceManager::instance()->load<Patrols>("/home/jerome/", "patrols");
}

void TestZone::testLoad()
{
}

