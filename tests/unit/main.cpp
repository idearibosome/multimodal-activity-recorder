#include <QtTest>

#include "shared/mmrwsrequestresponsetest.h"

int main(int argc, char *argv[]) {
    int ret = 0;

    ret += QTest::qExec(&MMRWSRequestResponseTest(), argc, argv);

    return ret;
}
