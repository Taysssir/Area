#ifndef TESTZONE_H
#define TESTZONE_H

#include "autorun.h"

class TestZone
        : public QObject
{
    Q_OBJECT
public:
private slots:
    void defaultValues();
    void testPermissions();
    void testPoints();
    void testSave();
    void testLoad();
};

DECLARE_TEST(TestZone)

#endif // TESTZONE_H
