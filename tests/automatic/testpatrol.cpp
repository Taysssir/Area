#include <QDebug>

#include "resource_manager/resource_manager.h"
#include "testpatrol.h"
#include "models/patrol.h"

void TestPatrol::basic()
{
    Patrol patrol;
    QVERIFY(patrol.name().size() == 0);
    QVERIFY(patrol.count() == 0);
    QVERIFY(patrol.remove(QPoint()) == false);
    patrol.clear();
    QVERIFY(patrol.count() == 0);
    QVERIFY(patrol.index(QPoint()) == -1);
    QCOMPARE(patrol.find(QPoint()).position, QPoint(-1, -1));
    QCOMPARE(patrol.type(), Patrol::Normal);
}

void TestPatrol::basicAccessors()
{
    Patrol patrol("Ronde 0");
    QCOMPARE(patrol.name(), QString("Ronde 0"));
    patrol.setName("Ronde 1");
    QCOMPARE(patrol.name(), QString("Ronde 1"));
    patrol.setType(Patrol::PopOut);
    QCOMPARE(patrol.type(), Patrol::PopOut);
}

void TestPatrol::testPoints()
{
    Patrol patrol;
    QVERIFY(patrol.add(QPoint(100, 10)));
    QVERIFY(!patrol.add(QPoint(100, 10)));
    QVERIFY(!patrol.add(100, 10));
    QCOMPARE(patrol.at(0).position.x(), 100);
    QCOMPARE(patrol.at(0).position.y(), 10);
    for(int i=0;i<10;++i)
        patrol.add(i, i-10, 1);
    QCOMPARE(patrol.count(), 11);
    QCOMPARE(patrol.at(2).priority, 1);
    QCOMPARE(patrol.at(20).priority, -1);
}

void TestPatrol::suggestNameTest()
{
    Patrols patrols;
    patrols.add(Patrol("Ronde 1"));
    patrols.add(Patrol("Ronde 3"));
    patrols.add(Patrol("Ronde 4"));
    QCOMPARE(patrols.suggestName(), QString("Ronde 5"));
}

#include "resource_manager/ziparchive.h"

void TestPatrol::loadFromZip()
{
    using namespace rm;
    Patrols::Ptr patrols = Patrols::Ptr(new Patrols());
    Patrol test("ronde");
    test.add(100, 100);
    test.add(10, 100, 3);
    test.add(10, 1, 3);
    patrols->add(test);
    //ResourceManager::instance()->addPath("archive.zip");
    //ResourceManager::instance()->load<Patrols>("/home/jerome/test/", "patrols");
    //ResourceManager::instance()->save<Patrols>("/home/jerome/", "patrols", patrols);
    
    Patrols::Ptr pat2 = ResourceManager::instance()->load<Patrols>("/home/jerome/", "patrols");
    
    //ResourceManager::instance()->save<Patrols>("/home/jerome/temp.zip", "patrols2", pat2);
}
