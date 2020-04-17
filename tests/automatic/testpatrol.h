#ifndef TESTPATROL_H
#define TESTPATROL_H

#include "autorun.h"

class TestPatrol
        : public QObject
{
    Q_OBJECT

private slots:
    void basic();
    void basicAccessors();
    void testPoints();
    void suggestNameTest();
    void loadFromZip();
};

DECLARE_TEST(TestPatrol)

#endif // TESTPATROL_H
