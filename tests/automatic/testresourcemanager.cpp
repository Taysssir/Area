#include <QImage>
#include <QPainter>
#include <QtGlobal>
#include <QDir>
#include <limits>

#include "testresourcemanager.h"

void TestResourceManager::initTestCase()
{
}

//namespace
//{
//    void compute_fake_map(QString const& filename)
//    {
//        QImage image(200, 200, QImage::Format_Mono);
//        image.fill(Qt::white);
//        QPainter painter;
//        painter.begin(&image);
//        //painter.setPen(QPen(QColor(Qt::black)));
//        painter.setBrush(QBrush(QColor(4278190080LL)));
//        for(int i=0;i<3;++i)
//        {
//            painter.drawRect(QRect(qrand()%200,
//                                   qrand()%200,
//                                   qrand()%200,
//                                   qrand()%200));
//        }
//        painter.end();
//        image.save( filename );
//    }
//
//}
//
//void TestResourceManager::testMap()
//{
//    Map::Ptr map = rm::ResourceManager::instance()->load<Map>("default.pgm");
//    QVERIFY( !map );
//    // create a random map
//    compute_fake_map("test.ppm");
//    QFile::rename("test.ppm", "test.pgm");
//
//    map = rm::ResourceManager::instance()->load<Map>("test.pgm");
//
//    QVERIFY( map );
//
//    QBENCHMARK(rm::ResourceManager::instance()->save<Map>("another.pgm", map));
//
//    Map::Ptr nmap = rm::ResourceManager::instance()->load<Map>("another.pgm");
//
//    QBENCHMARK(rm::ResourceManager::instance()->save<Map>("another2.pgm", nmap));
//
//    QVERIFY( utils::diff("test.pgm", "another.pgm") );
//    QVERIFY( !utils::diff("another2.pgm", "another.pgm") );
//}
//
//void TestResourceManager::testCropMap()
//{
//    Map::Ptr map = rm::ResourceManager::instance()->load<Map>("obstacle_map.pgm");
//    QVERIFY(map);
//    Map::Ptr map2 = rm::ResourceManager::instance()->load<Map>("obstacle_map2.pgm");
//    QVERIFY(map2);
//    map->miniMap().save("map1.png");
//    map2->miniMap().save("map2.png");
//    QVERIFY( !utils::diff("reference_map1.png", "map1.png") );
//    QVERIFY( !utils::diff("reference_map2.png", "map2.png") );
//}
//
//void TestResourceManager::testZone()
//{
//    {
//        Zones::Ptr zones = Zones::Ptr(new Zones());
//        Zone z;
//        zones->data.append(z);
//        zones->data.append(z);
//        zones->data.append(z);
//        zones->data.append(z);
//        rm::ResourceManager::instance()->save<Zones>("detection.zones", zones);
//        Zones::Ptr zones2 = rm::ResourceManager::instance()->reload<Zones>("detection.zones");
//        QVERIFY(zones2);
//        QCOMPARE(zones2->data.size(), zones->data.size());
//        for(int i=0;i<zones->data.size();++i)
//        {
//            QCOMPARE(zones->data[i].getZoneLeftUpPoint(), zones2->data[i].getZoneLeftUpPoint());
//            QCOMPARE(zones->data[i].getZoneWidth(), zones2->data[i].getZoneWidth());
//            QCOMPARE(zones->data[i].getZoneHeight(), zones2->data[i].getZoneHeight());
//            QCOMPARE(zones->data[i].getPermission(), zones2->data[i].getPermission());
//        }
//    }
//    {
//        Zones::Ptr zones = Zones::Ptr(new Zones());
//        Zone z;
//        for(int i=0;i<100;++i)
//        {
//            z.setZoneHeight(qrand()%200);
//            z.setZoneWidth(qrand()%200);
//            z.setZoneLeftUpPoint(QPoint(qrand()%200, qrand()%200));
//            z.setPermission(qrand() % Zone::Statup);
//            zones->data.append(z);
//        }
//        rm::ResourceManager::instance()->save<Zones>("detection.zones", zones);
//        Zones::Ptr zones2 = rm::ResourceManager::instance()->reload<Zones>("detection.zones");
//        QVERIFY(zones2);
//        QCOMPARE(zones2->data.size(), zones->data.size());
//        for(int i=0;i<zones->data.size();++i)
//        {
//            QCOMPARE(zones->data[i].getZoneLeftUpPoint(), zones2->data[i].getZoneLeftUpPoint());
//            QCOMPARE(zones->data[i].getZoneWidth(), zones2->data[i].getZoneWidth());
//            QCOMPARE(zones->data[i].getZoneHeight(), zones2->data[i].getZoneHeight());
//        }
//
//        // try to export data for the robot :
//        ZonesExporter::Data data;
//        QVERIFY(ZonesExporter::process(*zones, data));
//        for(QStringList::iterator it=data.files.begin();it!=data.files.end();++it)
//        {
//            QVERIFY( QFile::exists( *it ) );
//        }
//    }
//}
//
//void TestResourceManager::testPatrol()
//{
//    Patrols::Ptr patrols = Patrols::Ptr(new Patrols());
//    Patrol patrol;
//    patrol.setWayPoint(0, QPoint(2, 4), 0);
//    patrol.setWayPoint(1, QPoint(8, 7), 0);
//
//    patrol.setNumberOfWayPoint(2);
//    patrols->data.append(patrol);
//    patrol.setName("test");
//    patrol.setHeureDep("18:00", 1);
//    patrol.setHeureFin("19:00", 1);
//    patrol.setJourChecked(1, 1);
//    patrols->data.append(patrol);
//    rm::ResourceManager::instance()->save<Patrols>("origin.patrols", patrols);
//    Patrols::Ptr another = rm::ResourceManager::instance()->reload<Patrols>("origin.patrols");
//    rm::ResourceManager::instance()->save<Patrols>("origin2.patrols", another);
//    QVERIFY( !utils::diff("origin.patrols", "origin2.patrols") );
//
//    PatrolsExporter::Data data;
//    // try to generate patrols for the robot
//    QVERIFY(PatrolsExporter::process(*patrols, data));
//
//    for(QStringList::iterator it=data.dirs.begin();it!=data.dirs.end();++it)
//    {
//        QVERIFY( QFile::exists( utils::join(*it, constants::patrol::FILE_PARAMETER) ) );
//        QVERIFY( QFile::exists( utils::join(*it, constants::patrol::FILE_POINT) ) );
//    }
//}
//
//void TestResourceManager::testRobotSettings()
//{
//    RobotSettings::Ptr settings = RobotSettings::Ptr(new RobotSettings());
//    rm::ResourceManager::instance()->save<RobotSettings>("default.robotsettings", settings);
//    RobotSettings::Ptr settings2 = rm::ResourceManager::instance()->reload<RobotSettings>("default.robotsettings");
//    rm::ResourceManager::instance()->save<RobotSettings>("default2.robotsettings", settings2);
//
//    QVERIFY( !utils::diff("default.robotsettings", "default2.robotsettings") );
//    RobotSettingsExporter::Data data;
//    QVERIFY(RobotSettingsExporter::process(*settings2, data));
//}
//
//void TestResourceManager::testSettings()
//{
//    Settings::Ptr settings = Settings::Ptr(new Settings());
//    settings->setValue("Test.De.Jerome", true);
//    QVERIFY(settings->getValue("Test.De") == QVariant());
//    QVERIFY(settings->getValue("Test.De.Jerome") == true);
//    QVERIFY(settings->getValue("Test.De.jerome") == QVariant());
//    rm::ResourceManager::instance()->save<Settings>("default.settings", settings);
//    Settings::Ptr another = rm::ResourceManager::instance()->reload<Settings>("default.settings");
//
//    QVERIFY(another->getValue(constants::settings::global::TIMEOUT_GOBACK_PATROL)
//            ==
//            settings->getValue(constants::settings::global::TIMEOUT_GOBACK_PATROL));
//
//    QVERIFY(another->getValue("Test.De.Jerome")
//            ==
//            settings->getValue("Test.De.Jerome"));
//
//    rm::ResourceManager::instance()->save<Settings>("default2.settings", another);
//    QVERIFY( !utils::diff("default.settings", "default2.settings") );
//}
//
//void TestResourceManager::testSensors()
//{
//    Sensors::Ptr sensors = Sensors::Ptr(new Sensors());
//    Sensor sensor;
//    sensors->data.append(sensor);
//    rm::ResourceManager::instance()->save<Sensors>("default.sensors", sensors);
//    Sensors::Ptr another = rm::ResourceManager::instance()->reload<Sensors>("default.sensors");
//    rm::ResourceManager::instance()->save<Sensors>("default2.sensors", another);
//    QVERIFY( !utils::diff("default.sensors", "default2.sensors") );
//}
