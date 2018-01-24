#ifndef MMRWSREQUESTRESPONSETEST_H
#define MMRWSREQUESTRESPONSETEST_H

#include <QObject>
#include <QtTest>

class MMRWSRequestResponseTest : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void testRequestSerialization();
    void testResponseSerialization();
};

#endif // MMRWSREQUESTRESPONSETEST_H
