#ifndef TESTPLANIFICATION_H
#define TESTPLANIFICATION_H

#include "autorun.h"

class testPlanification
        :public QObject
{
    Q_OBJECT

private slots:
    void loadFromZip();

public:
    testPlanification();
};
DECLARE_TEST(TestPlanification)

#endif // TESTPLANIFICATION_H
