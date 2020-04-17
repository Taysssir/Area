#ifndef TESTWORKSPACE_H
#define TESTWORKSPACE_H

#include "autorun.h"

class TestWorkspace
        : public QObject
{
public:
    Q_OBJECT
    
private slots:
    void testBase();
};

DECLARE_TEST(TestWorkspace)

#endif // TESTWORKSPACE_H
