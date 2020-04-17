#include <QDir>
#include <QDebug>

#include "testworkspace.h"
#include "core/workspace.h"

void TestWorkspace::testBase()
{
  {
      QDir dir(core::Workspace::getLocation(core::Workspace::Configurations));
      qDebug() << dir.path();
      QVERIFY( dir.path().size() );
      QVERIFY( dir.exists() );
  }
  {
      QDir dir(core::Workspace::getLocation(core::Workspace::Data));
      qDebug() << dir.path();
      QVERIFY( dir.path().size() );
      QVERIFY( dir.exists() );
  }
  {
      QDir dir(core::Workspace::getLocation(core::Workspace::Configurations));
      qDebug() << dir.path();
      QVERIFY( dir.path().size() );
      QVERIFY( dir.exists() );
  }
  {
      QDir dir(core::Workspace::getLocation(core::Workspace::Videos));
      qDebug() << dir.path();
      QVERIFY( dir.path().size() );
      QVERIFY( dir.exists() );
  }
  {
      QDir dir(core::Workspace::getLocation(core::Workspace::VideoDay));
      qDebug() << dir.path();
      QVERIFY( dir.path().size() );
      QVERIFY( dir.exists() );
  }
  {
      QDir dir(core::Workspace::getLocation(core::Workspace::Logs));
      qDebug() << dir.path();
      QVERIFY( dir.path().size() );
      QVERIFY( dir.exists() );
  }
  {
      QDir dir(core::Workspace::getLocation(core::Workspace::LogDay));
      qDebug() << dir.path();
      QVERIFY( dir.path().size() );
      QVERIFY( dir.exists() );
  }
}
