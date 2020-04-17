#include "autorun.h"

int main(int argc, char *argv[])
{
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    return AutoTest::run(argc, argv);
}
